/*! @file transform_tree.cc
 *  @brief Creating tree of transformations.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transform_tree.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string>
#include <utility>
#include "src/allocators/sliding_blocks_allocator.h"
#include "src/formats/array_format.h"
#include "src/format_converter.h"
#include "src/transform_registry.h"
#include "src/memory_protector.h"
#include "src/transforms/identity.h"

/// @brief Temporary fix for a buggy system_clock implementation in libstdc++.
/// @see http://gcc.1065356.n5.nabble.com/patch-Default-to-enable-libstdcxx-time-auto-td940166i40.html
#define BUGGY_SYSTEM_CLOCK_FIX 1000.f

extern "C" {
extern size_t get_cpu_cache_size(void);
}

namespace sound_feature_extraction {

class RootTransform : public Transform {
 public:
  explicit RootTransform(
      const std::shared_ptr<formats::ArrayFormat16>& format) noexcept
      : format_(format) {
  }

  virtual const std::string& Name() const noexcept override {
    static const std::string name("!Root");
    return name;
  }

  virtual const std::string& Description() const noexcept override {
     static const std::string desc("The root for all other transforms.");
     return desc;
    }

  virtual const std::shared_ptr<BufferFormat> InputFormat()
      const noexcept override {
    return format_;
  }

  virtual size_t SetInputFormat(
      const std::shared_ptr<BufferFormat>& format,
      size_t buffersCount) override {
    format_ = std::static_pointer_cast<formats::ArrayFormat16>(format);
    return buffersCount;
  }

  virtual const std::shared_ptr<BufferFormat> OutputFormat()
      const noexcept override {
    return format_;
  }

  virtual const SupportedParametersMap&
  SupportedParameters() const noexcept override {
    static const SupportedParametersMap p;
    return p;
  }

  virtual const ParametersMap& GetParameters() const noexcept override {
    static const ParametersMap p;
    return p;
  }

  virtual void SetParameters(
      const ParametersMap&) override {
  }

  virtual void Initialize() const override {
  }

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      size_t count, void* reusedMemory = nullptr) const noexcept override {
    return std::make_shared<BuffersBase<int16_t*>>(format_, count,
                                                   reusedMemory);
  }

  virtual void Do(const Buffers& in, Buffers *out) const noexcept override {
    *out = in;
  }

 private:
  std::shared_ptr<formats::ArrayFormat16> format_;
};

TransformTree::Node::Node(Node* parent,
                          const std::shared_ptr<Transform>& boundTransform,
                          size_t buffersCount, TransformTree* host) noexcept
    : Logger(std::string("Node (") + boundTransform->Name() + ")",
             EINA_COLOR_GREEN),
      Host(host == nullptr? parent == nullptr? nullptr : parent->Host : host),
      Parent(parent),
      BoundTransform(boundTransform),
      BoundBuffers(nullptr),
      BuffersCount(buffersCount),
      Protection(nullptr),
      Next(nullptr),
      BelongsToSlice(false),
      HasClones(false),
      ElapsedTime(new std::chrono::high_resolution_clock::duration()) {
}

void TransformTree::Node::ActionOnEachTransformInSubtree(
    const std::function<void(const Transform&)> action) const {
  action(*BoundTransform);
  for (auto& subnodes : Children) {
    for (auto& inode : subnodes.second) {
      inode->ActionOnEachTransformInSubtree(action);
    }
  }
}

void TransformTree::Node::ActionOnSubtree(
    const std::function<void(const Node&)> action) const {
  action(*this);
  for (auto& subnodes : Children) {
    for (auto& inode : subnodes.second) {
      inode->ActionOnSubtree(action);
    }
  }
}

void TransformTree::Node::ActionOnEachImmediateChild(
    const std::function<void(Node&)> action) {
  for (auto& subnodes : Children) {
    for (auto& inode : subnodes.second) {
      action(*inode);
    }
  }
}

void TransformTree::Node::ActionOnEachImmediateChild(
    const std::function<void(const Node&)> action) const {
  for (const auto& subnodes : Children) {  // NOLINT(*)
    for (const auto& inode : subnodes.second) {  // NOLINT(*)
      action(*inode);
    }
  }
}

void TransformTree::Node::ActionOnEachParent(
    const std::function<void(const Node&)> action) const {
  if (Parent != nullptr) {
    action(*Parent);
    Parent->ActionOnEachParent(action);
  }
}

std::shared_ptr<TransformTree::Node>
TransformTree::Node::FindIdenticalChildTransform(
    const Transform& base) const noexcept {
  auto tv = Children.find(base.Name());
  if (tv == Children.end()) return nullptr;
  for (auto node : tv->second) {
    if (base == *node->BoundTransform) {
      return node;
    }
  }
  return nullptr;
}

void TransformTree::Node::BuildAllocationTree(
    memory_allocation::Node* node) const noexcept {
  node->Children.reserve(ChildrenCount());
  for (auto& subnodes : Children) {
    for (auto& inode : subnodes.second) {
      memory_allocation::Node child(
          inode->BuffersCount *
              inode->BoundTransform->OutputFormat()->SizeInBytes(),
          node,
          inode.get());
      node->Children.push_back(child);
      inode->BuildAllocationTree(&node->Children.back());
    }
  }
}

void TransformTree::Node::ApplyAllocationTree(
    const memory_allocation::Node& node,
    void* allocatedMemory) noexcept {
  auto mem_ptr = reinterpret_cast<char*>(allocatedMemory) + node.Address;
  BoundBuffers = BoundTransform->CreateOutputBuffers(
      BuffersCount, mem_ptr);
  if (node.Next != nullptr) {
    Next = reinterpret_cast<TransformTree::Node*>(node.Next->Item);
  }

  for (size_t i = 0; i < node.Children.size(); i++) {
    TransformTree::Node* child = reinterpret_cast<TransformTree::Node*>(
        node.Children[i].Item);
    child->ApplyAllocationTree(node.Children[i], allocatedMemory);
  }
}

void TransformTree::Node::Execute() noexcept {
  if (Parent != nullptr) {
    DBG("Executing %s on %zu buffers -> %zu...",
        BoundTransform->Name().c_str(),
        Parent->BoundBuffers->Count(), BoundBuffers->Count());
    auto checkPointStart = std::chrono::high_resolution_clock::now();
    std::shared_ptr<Buffers> parent_buffers;
    if (Parent->Slices.size() == 0 || !BelongsToSlice) {
      parent_buffers = Parent->BoundBuffers;
    } else {
      size_t index, length;
      std::tie(index, length) = Parent->Slices[this];
      assert(length > 0);
      parent_buffers = std::make_shared<Buffers>(
          Parent->BoundBuffers->Slice(index, length));
    }
    BoundTransform->Do(*parent_buffers, BoundBuffers.get());
    auto checkPointFinish = std::chrono::high_resolution_clock::now();
    *ElapsedTime += checkPointFinish - checkPointStart;
    Host->transforms_cache_[BoundTransform->Name()].ElapsedTime += *ElapsedTime;

    if (ChildrenCount() == 0) {
      // This is a leaf, disable any further writing to the corr. memory block
      auto ptr = std::const_pointer_cast<const Buffers>(BoundBuffers)->Data();
      DBG("Enabling write protection on %p:%zu",
          ptr, BoundBuffers->SizeInBytes());
      Protection = std::make_shared<MemoryProtector>(
          ptr, BoundBuffers->SizeInBytes());
    }

    if (Host->validate_after_each_transform()) {
      try {
        BoundBuffers->Validate();
      }
      catch(const InvalidBuffersException& e) {
#ifdef DEBUG
        if (BoundBuffers->Count() == Parent->BoundBuffers->Count()) {
          ERR("Validation failed on index %zu.\n----before----\n%s\n\n"
              "----after----\n%s\n",
              e.index(),
              parent_buffers->Dump(e.index()).c_str(),
              BoundBuffers->Dump(e.index()).c_str());
        } else {
          ERR("Validation failed.\n----Buffers before----\n%s\n\n"
              "----Buffers after----\n%s\n",
              parent_buffers->Dump().c_str(),
              BoundBuffers->Dump().c_str());
        }
#endif
        throw TransformResultedInInvalidBuffersException(BoundTransform->Name(),
                                                         e.what());
      }
    }

    if (Host->transforms_cache_[BoundTransform->Name()].Dump ||
        Host->dump_buffers_after_each_transform()) {
      INF("Buffers after %s", BoundTransform->Name().c_str());
      INF("==============%s",
          std::string(BoundTransform->Name().size(), '=').c_str());
      INF("%s", BoundBuffers->Dump().c_str());
    }
  }
  if (Next) {
    Next->Execute();
  }
}

size_t TransformTree::Node::ChildrenCount() const noexcept {
  size_t size = 0;
  for (auto& child : Children) {
    size += child.second.size();
  }
  return size;
}

std::shared_ptr<TransformTree::Node> TransformTree::Node::SelfPtr()
    const noexcept {
  for (auto& child : Parent->Children[BoundTransform->Name()]) {
    if (child.get() == this) {
      return child;
      break;
    }
  }
  return nullptr;
}

TransformTree::TransformTree(formats::ArrayFormat16&& rootFormat) noexcept
    : Logger("TransformTree", EINA_COLOR_ORANGE),
      root_(std::make_shared<Node>(
        nullptr, std::make_shared<RootTransform>(
            std::make_shared<formats::ArrayFormat16>(rootFormat)), 1, this)),
      root_format_(std::make_shared<formats::ArrayFormat16>(rootFormat)),
      tree_is_prepared_(false),
      cache_optimization_(true),
      validate_after_each_transform_(false),
      dump_buffers_after_each_transform_(false) {
}

TransformTree::TransformTree(
    const std::shared_ptr<formats::ArrayFormat16>& rootFormat) noexcept
    : Logger("TransformTree", EINA_COLOR_ORANGE),
      root_(std::make_shared<Node>(
        nullptr, std::make_shared<RootTransform>(rootFormat), 1, this)),
      root_format_(rootFormat),
      tree_is_prepared_(false),
      cache_optimization_(true),
      validate_after_each_transform_(false),
      dump_buffers_after_each_transform_(false) {
}

std::shared_ptr<formats::ArrayFormat16> TransformTree::RootFormat()
    const noexcept {
  return root_format_;
}

void TransformTree::AddTransform(const std::string& name,
                                 const std::string& parameters,
                                 const std::string& relatedFeature,
                                 std::shared_ptr<Node>* currentNode) {
  DBG("Adding \"%s\" (parameters \"%s\")", name.c_str(), parameters.c_str());
  // Search for the constructor of the transform "tname"
  auto tfit = TransformFactory::Instance().Map().find(name);
  if (tfit == TransformFactory::Instance().Map().end()) {
    throw TransformNotRegisteredException(name);
  }
  // tfit is actually a map from input format to real constructor
  TransformFactory::TransformConstructor ctor;
  auto format_id = (*currentNode)->BoundTransform->OutputFormat()->Id();
  if (name == transforms::Identity::kName) {
    format_id = IdentityFormat().Id();
  }
  auto ctorit = tfit->second.find(format_id);
  if (ctorit == tfit->second.end()) {
    DBG("Formats mismatch, iterating input formats (%zu)",
        tfit->second.size());
    // No matching format found, try to add the format converter first
    for (auto& ctoritfmt : tfit->second) {
      auto t = ctoritfmt.second();
      DBG("Probing %s -> %s",
          (*currentNode)->BoundTransform->OutputFormat()->Id().c_str(),
          t->InputFormat()->Id().c_str());
      auto convName = FormatConverter::Name(
          *(*currentNode)->BoundTransform->OutputFormat(), *t->InputFormat());
      try {
        AddTransform(convName, "", relatedFeature, currentNode);
      }
      catch(const TransformNotRegisteredException&) {
        continue;
      }
      ctor = ctoritfmt.second;
      break;
    }
  } else {
    ctor = ctorit->second;
  }
  if (ctor == nullptr) {
    throw IncompatibleTransformFormatException(
        *(*currentNode)->BoundTransform, name);
  }

  // Create the transform "name"
  auto t = ctor();
  {
    auto tparams = Transform::Parse(parameters);
    t->SetParameters(tparams);
  }
  auto reused_node = (*currentNode)->FindIdenticalChildTransform(*t);
  if (reused_node != nullptr) {
    *currentNode = reused_node;
    // If this node is the exit node for some feature, redirect that feature to
    // an appended Identity transform. This step is necessary due to the way
    // memory allocation works. Particularly, the node is considered to be
    // a leaf if and only if the number of it's children equals to 0.
    auto related_feature = std::find_if(
        features_.begin(), features_.end(),
        [reused_node](const std::pair<std::string, std::shared_ptr<Node>>& el) {
      return el.second == reused_node;
    });
    if (related_feature != features_.end()) {
      DBG("Appending an extra Identity to %s",
          reused_node->BoundTransform->Name().c_str());
      std::shared_ptr<Node> identity_node = reused_node;
      AddTransform(transforms::Identity::kName, "", related_feature->first,
                   &identity_node);
      related_feature->second = identity_node;
    }
  } else {
    // Set the new input format
    size_t buffers_count = t->SetInputFormat(
        (*currentNode)->BoundTransform->OutputFormat(),
        (*currentNode)->BuffersCount);
    // Append the newly created transform
    auto new_node = std::make_shared<Node>(currentNode->get(), t, buffers_count,
                                           this);
    (*currentNode)->Children[name].push_back(new_node);
    *currentNode = new_node;
  }
  (*currentNode)->RelatedFeatures.push_back(relatedFeature);

  // Search for the environment variable to activate dumps for this transform
  auto dump_val = std::getenv((std::string(kDumpEnvPrefix) + name).c_str());
  if (dump_val != nullptr && !strncmp(dump_val, "true", 4)) {
    DBG("Activated dump of %s", name.c_str());
    transforms_cache_[name].Dump = true;
  }
}

void TransformTree::AddFeature(
    const std::string& name,
    const std::vector<std::pair<std::string, std::string>>& transforms) {
  DBG("Adding \"%s\"", name.c_str());
  if (tree_is_prepared_) {
    throw TreeIsPreparedException();
  }
  if (features_.find(name) != features_.end()) {
    throw ChainNameAlreadyExistsException(name);
  }

  auto currentNode = root_;
  root_->RelatedFeatures.push_back(name);
  for (auto& tpair : transforms) {
    auto tname = tpair.first;
    AddTransform(tpair.first, tpair.second, name, &currentNode);
  }

  features_.insert(std::make_pair(name, currentNode));
}

int TransformTree::BuildSlicedCycles() noexcept {
  int ret = 0;  // the resulting number of built cycles
  auto node = root_.get();
  decltype(node) prev_node = nullptr;
  while (node != nullptr) {
    // Search for the next cycle entry candidate
    do {
      prev_node = node;
      node = node->Next;
    }
    while (node != nullptr && (!node->BoundTransform->BufferInvariant() ||
        node->ChildrenCount() == 0));
    if (node == nullptr) {
      break;
    }
    // Grow the cycle
    std::vector<Node*> current_cycle;
    do {
      current_cycle.push_back(node);
      node = node->Next;
    }
    while (node != nullptr && node->BoundTransform->BufferInvariant() &&
           node->ChildrenCount() > 0);
    if (node != nullptr && node->BoundTransform->BufferInvariant() &&
        node->ChildrenCount() == 0) {
      current_cycle.push_back(node);
      node = node->Next;
    }
    if (current_cycle.size() == 1) {
      continue;
    }
    ret++;
    // We have found a cache optimization friendly subpath.
    // Determine the size bottleneck and mark all the nodes in that subpath.
    size_t max_size = 0;
    size_t bufs_count = current_cycle[0]->BoundBuffers->Count();
    for (auto cn : current_cycle) {
      cn->HasClones = true;
      auto size = cn->BoundTransform->InputFormat()->SizeInBytes();
      if (size > max_size) {
        max_size = size;
      }
      assert(bufs_count == cn->BoundBuffers->Count());
    }
    assert(max_size > 0);
    size_t slice_buffers_count = get_cpu_cache_size() / max_size;
    if (slice_buffers_count == 0 || bufs_count <= slice_buffers_count) {
      continue;
    }
    // Clone those nodes, connecting each slice's end to the next slice's
    // beginning.
    Node* head = current_cycle[0]->Parent;
    assert(head != nullptr);
    Node* tail = head;
    for (size_t i = 0; i < bufs_count; i += slice_buffers_count) {
      auto my_bufs_count = std::min(slice_buffers_count, bufs_count - i);
      for (size_t j = 0; j < current_cycle.size(); j++) {
        auto cn = current_cycle[j];
        auto cloned = std::make_shared<Node>(j == 0? head : tail,
                                             cn->BoundTransform,
                                             my_bufs_count, this);
        if (j == 0) {
          head->Slices[cloned.get()] = std::make_tuple(i, my_bufs_count);
        }
        cloned->RelatedFeatures = cn->RelatedFeatures;
        cloned->BoundBuffers = std::make_shared<Buffers>(
            cn->BoundBuffers->Slice(i, my_bufs_count));
        cloned->BuffersCount = my_bufs_count;
        cloned->BelongsToSlice = true;
        cloned->ElapsedTime = cn->ElapsedTime;
        tail->Children[cloned->BoundTransform->Name()].push_back(cloned);
        if (head == tail) {
          // The very first iteration
          prev_node->Next = cloned.get();
        } else {
          tail->Next = cloned.get();
        }
        tail = cloned.get();
      }
    }
    tail->Children = current_cycle.back()->Children;
    tail->Next = current_cycle.back()->Next;
  }
  return ret;
}

void TransformTree::PrepareForExecution() {
  if (tree_is_prepared_) {
    throw TreeAlreadyPreparedException();
  }
  DBG("Initializing the transforms...");
  // Run Initialize() on all transforms
  root_->ActionOnEachTransformInSubtree([](const Transform& t) {
    t.Initialize();
  });
  DBG("Finished. Baking the allocation plan...");
  // Solve the allocation problem
  memory_allocation::Node allocation_tree_root(0, nullptr, root_.get());
  root_->BuildAllocationTree(&allocation_tree_root);
  memory_allocation::SlidingBlocksAllocator allocator;
  size_t neededMemory = allocator.Solve(&allocation_tree_root);
#if DEBUG
  allocation_tree_root.Dump("/tmp/last_allocation.dot");
  assert(allocator.Validate(allocation_tree_root));
#endif
  // Allocate the buffers
  allocated_memory_ = std::shared_ptr<void>(malloc_aligned(neededMemory),
                                           [](void* ptr) {
                                             free(ptr);
                                           });
  if (allocated_memory_.get() == nullptr) {
    throw FailedToAllocateBuffersException(std::string("Failed to allocate ") +
                                           std::to_string(neededMemory) +
                                           " bytes.");
  }
  INF("Allocated %zu bytes at %p", neededMemory, allocated_memory_.get());
  // Finally, apply the memory mapping, creating the actual buffers
  // We will overwrite root's BoundBuffers on execution stage
  root_->ApplyAllocationTree(allocation_tree_root, allocated_memory_.get());
  // Try to do CPU cache optimization by splitting the buffers into slices
  if (cache_optimization_) {
    auto cycles_count = BuildSlicedCycles();
    DBG("Built %d cycles", cycles_count);
  }
  tree_is_prepared_ = true;
  INF("Prepared to extract %zu features", features_.size());
}

std::unordered_map<std::string, std::shared_ptr<Buffers>>
TransformTree::Execute(const int16_t* in) {
  if (!tree_is_prepared_) {
    throw TreeIsNotPreparedException();
  }
  if (features_.size() == 0) {
    throw TreeIsEmptyException();
  }
#if DEBUG
  Dump("/tmp/last_nodes.dot");
#endif
  // Initialize input. We have to const_cast here, but "in" is not going
  // to be overwritten anyway.
  root_->BoundBuffers = root_->BoundTransform->CreateOutputBuffers(
      1, const_cast<int16_t*>(in));
  if (validate_after_each_transform()) {
    try {
      root_->BoundBuffers->Validate();
    }
    catch(const InvalidBuffersException& e) {
      throw InvalidInputBuffersException(e.what());
    }
  }

  DBG("Executing the tree...");
  // Run the transforms, measuring the elapsed time
  auto checkPointStart = std::chrono::high_resolution_clock::now();
  root_->Execute();
  auto checkPointFinish = std::chrono::high_resolution_clock::now();
  auto allDuration = checkPointFinish - checkPointStart;
  INF("Finished. Execution took %f s", ConvertDuration(allDuration));
  auto otherDuration = allDuration;
  for (auto& cit : transforms_cache_) {
    otherDuration -= cit.second.ElapsedTime;
  }
  transforms_cache_["All"].ElapsedTime = allDuration;
  transforms_cache_["Other"].ElapsedTime = otherDuration;

  // Populate the results
  std::unordered_map<std::string, std::shared_ptr<Buffers>> results;
  for (auto& feature : features_) {
    results[feature.first] = feature.second->BoundBuffers;
  }
  return results;
}

std::unordered_map<std::string, float>
TransformTree::ExecutionTimeReport() const noexcept {
  std::unordered_map<std::string, float> ret;
  auto allIt = transforms_cache_.find("All");
  if (allIt == transforms_cache_.end()) {
    return ret;
  }
  auto allTime = allIt->second.ElapsedTime;
  for (auto& cit : transforms_cache_) {
    if (cit.first != "All") {
      ret.insert(std::make_pair(
          cit.first, (cit.second.ElapsedTime.count() + 0.f) / allTime.count()));
    } else {
      ret.insert(std::make_pair(
          cit.first, ConvertDuration(allTime)));
    }
  }
  return ret;
}

void TransformTree::Dump(const std::string& dotFileName) const {
  // I am very sorry for such a complicated code. Please forgive me.
  // It just has to be here.
  const float redThreshold = 0.25f;

  auto timeReport = ExecutionTimeReport();
  bool includeTime = timeReport.size() > 0;
  float maxTimeRatio = 0.0f;
  if (includeTime) {
    for (auto& tr : timeReport) {
      if (tr.second > maxTimeRatio && tr.first != "All") {
        maxTimeRatio = tr.second;
      }
    }
  }
  float redShift = redThreshold * maxTimeRatio;
  const int initialLight = 0x30;
  auto allTime = timeReport["All"];
  std::ofstream fw;
  fw.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fw.open(dotFileName);
  fw << "digraph TransformsTree {" << std::endl;
  std::unordered_map<std::string, int> counters;
  std::unordered_map<const Node*, int> node_counters;
  root_->ActionOnSubtree([&](const Node& node) {
    if (node.BelongsToSlice) {
      return;
    }
    auto t = node.BoundTransform;
    node_counters[&node] = counters[t->Name()];
    fw << "\t" << t->SafeName() << counters[t->Name()]++ << " [";
    if (includeTime && timeReport[t->Name()] > redShift) {
      fw << "style=\"filled\", fillcolor=\"#";
      int light = 255 - (timeReport[t->Name()] - redShift) /
          (maxTimeRatio - redShift) * (255 - initialLight);
      // crazy printing of smth like ff4040
      fw << "ff" << std::hex << std::setw(2) << std::setfill('0') << light
         << std::setw(2) << std::setfill('0') << light << "\", ";
    }
    fw << "label=<" << t->HtmlEscapedName()
        << "<br /><font point-size=\"10\">";
    if (includeTime) {
      auto cur_percent = static_cast<int>(
          (roundf(ConvertDuration(*node.ElapsedTime) * 100.f / allTime)));
      assert(cur_percent >=0 && cur_percent <= 100);
      auto all_percent = static_cast<int>(
          roundf(timeReport[t->Name()] * 100.f));
      assert(all_percent >=0 && all_percent <= 100);
      fw << "<b>" << std::to_string(cur_percent) << "% ("
          << std::to_string(all_percent) << "%)</b>";
    }
    if (t->GetParameters().size() > 0) {
      fw << "<br /> <br />";
      for (auto& p : t->GetParameters()) {
        auto isDefault = false;
        isDefault = p.second ==
            t->SupportedParameters().find(p.first)->second.DefaultValue;
        if (isDefault) {
          fw << "<font color=\"gray\">";
        }
        fw << p.first << " = " << p.second;
        if (isDefault) {
          fw << "</font>";
        }
        fw << "<br />";
      }
    } else {
      fw << " ";
    }
    fw << "</font>>]" << std::endl;

    // If this node is a leaf, append related feature node
    if (node.Children.size() == 0) {
      std::string feature = *node.RelatedFeatures.begin();
      fw << feature << " [style=\"filled\", "
          "fillcolor=\"#85b3de\", label=<" << feature;
      if (includeTime) {
        fw << "<br /><font point-size=\"10\">";
        auto featureTime = *node.ElapsedTime;
        node.ActionOnEachParent([&](const Node& parent) {
          featureTime += *parent.ElapsedTime / parent.RelatedFeatures.size();
        });
        auto cur_percent = static_cast<int>(
            (roundf((ConvertDuration(featureTime) * 100.f) / allTime)));
        assert(cur_percent >=0 && cur_percent <= 100);
        fw << "<b>" << std::to_string(cur_percent) << "%</b></font>";
      }
      fw << ">]" << std::endl;
    }
  });
  // Output "Other"
  fw << "\tOther [";
  if (includeTime && timeReport["Other"] > redShift) {
    fw << "style=\"filled\", fillcolor=\"#";
    int light = 255 - (timeReport["Other"] - redShift) /
        (maxTimeRatio - redShift) * (255 - initialLight);
    // this is crazy printing of smth like ff4040
    fw << "ff" << std::hex << std::setw(2) << std::setfill('0') << light
       << std::setw(2) << std::setfill('0') << light << "\", ";
  }
  fw << "label=<Other";
  if (includeTime) {
    auto cur_percent = static_cast<int>(roundf(timeReport["Other"] * 100.f));
    assert(cur_percent >=0 && cur_percent <= 100);
    fw << "<br /><font point-size=\"10\"><b>"
        << std::to_string(cur_percent) << "%</b></font>";
  }
  fw << ">]" << std::endl << std::endl;
  // Output the node connections
  root_->ActionOnSubtree([&](const Node& node) {
    if (node.BelongsToSlice) {
      return;
    }
    std::string nodeName = node.BoundTransform->SafeName() +
        std::to_string(node_counters[&node]);
    node.ActionOnEachImmediateChild([&](const Node& child) {
      if (child.BelongsToSlice) {
        return;
      }
      fw << "\t" << nodeName << " -> " << child.BoundTransform->SafeName()
          << node_counters[&child];
      if (node.HasClones && child.HasClones) {
        fw << "[color=\"green\"]";
      }
      fw << std::endl;
    });
    if (node.Children.size() == 0) {
      fw << "\t" << nodeName << " -> " << *node.RelatedFeatures.begin()
          << std::endl;
    }
  });
  fw << "}" << std::endl;
}

bool TransformTree::validate_after_each_transform() const noexcept {
  return validate_after_each_transform_;
}

void TransformTree::set_validate_after_each_transform(bool value) noexcept {
  validate_after_each_transform_ = value;
}

bool TransformTree::dump_buffers_after_each_transform() const noexcept {
  return dump_buffers_after_each_transform_;
}

void TransformTree::set_dump_buffers_after_each_transform(bool value) noexcept {
  dump_buffers_after_each_transform_ = value;
}

bool TransformTree::cache_optimization() const noexcept {
  return cache_optimization_;
}

void TransformTree::set_cache_optimization(bool value) noexcept {
  cache_optimization_ = value;
}

float TransformTree::ConvertDuration(
    const std::chrono::high_resolution_clock::duration& d) noexcept {
  return (d.count() + 0.f) * BUGGY_SYSTEM_CLOCK_FIX *
      std::chrono::high_resolution_clock::period::num /
      std::chrono::high_resolution_clock::period::den;
}

}  // namespace sound_feature_extraction
