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
#include <assert.h>
#include <math.h>
#include <fstream>  // NOLINT(*)
#include <iomanip>
#include <string>
#include <utility>
#include "src/formats/raw_format.h"
#include "src/format_converter.h"
#include "src/transform_registry.h"

/// @brief Temporary fix for a buggy system_clock implementation in libstdc++.
/// @see http://gcc.1065356.n5.nabble.com/patch-Default-to-enable-libstdcxx-time-auto-td940166i40.html
#define BUGGY_SYSTEM_CLOCK_FIX 1000.f

namespace SoundFeatureExtraction {

class RootTransform : public Transform {
 public:
  explicit RootTransform(
      const std::shared_ptr<Formats::RawFormat16>& format) noexcept
      : format_(format) {
  }

  virtual const std::string& Name() const noexcept {
    static const std::string name("!Root");
    return name;
  }

  virtual const std::string& Description() const noexcept {
     static const std::string desc("The root for all other transforms.");
     return desc;
    }

  virtual const std::shared_ptr<BufferFormat> InputFormat() const noexcept {
    return format_;
  }

  virtual void SetInputFormat(const std::shared_ptr<BufferFormat>& format) {
    format_ = std::static_pointer_cast<Formats::RawFormat16>(format);
  }

  virtual const std::shared_ptr<BufferFormat> OutputFormat() const noexcept {
    return format_;
  }

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept {
    static const std::unordered_map<std::string, ParameterTraits> p;
    return p;
  }

  virtual const std::unordered_map<std::string, std::string>&
  GetParameters() const noexcept {
    static const std::unordered_map<std::string, std::string> p;
    return p;
  }

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>&) {
  }

  virtual void Initialize() const noexcept {
  }

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(const Buffers&)
      const noexcept {
    return nullptr;
  }

  virtual void Do(const Buffers& in, Buffers *out) const noexcept {
    *out = in;
  }

 private:
  std::shared_ptr<Formats::RawFormat16> format_;
};

TransformTree::Node::Node(Node* parent,
                          const std::shared_ptr<Transform>& boundTransform,
                          TransformTree* host) noexcept
    : Logger(std::string("Node (") + boundTransform->Name() + ")",
             EINA_COLOR_GREEN),
      Parent(parent),
      BoundTransform(boundTransform),
      BoundBuffers(nullptr),
      Host(host == nullptr? parent == nullptr? nullptr : parent->Host : host) {
}

void TransformTree::Node::ActionOnEachTransformInSubtree(
    const std::function<void(const Transform&)> action) const {
  action(*BoundTransform);
  for (auto subnodes : Children) {
    for (auto inode : subnodes.second) {
      inode->ActionOnEachTransformInSubtree(action);
    }
  }
}

void TransformTree::Node::ActionOnSubtree(
    const std::function<void(const Node&)> action) const {
  action(*this);
  for (auto subnodes : Children) {
    for (auto inode : subnodes.second) {
      inode->ActionOnSubtree(action);
    }
  }
}

void TransformTree::Node::ActionOnEachImmediateChild(
    const std::function<void(Node&)> action) {
  for (auto subnodes : Children) {
    for (auto inode : subnodes.second) {
      action(*inode);
    }
  }
}

void TransformTree::Node::ActionOnEachImmediateChild(
    const std::function<void(const Node&)> action) const {
  for (auto subnodes : Children) {
    for (auto inode : subnodes.second) {
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

void TransformTree::Node::AllocateBuffers(size_t visitedChildrenCount) noexcept {
  if (Parent != nullptr && BoundBuffers == nullptr) {
    if (!BoundTransform->OutputFormat()->MustReallocate(
            *BoundTransform->InputFormat()) &&
        Parent->Children.size() == visitedChildrenCount + 1) {
      // Recycle the parent's buffers
      BoundBuffers = std::make_shared<Buffers>(*Parent->BoundBuffers,
                                               BoundTransform->OutputFormat());
    } else {
      // Honestly allocate the buffers
      BoundBuffers = BoundTransform->CreateOutputBuffers(
          *Parent->BoundBuffers);
    }
  } else {
    // I am the root and can have only one raw buffer
    auto buffers = std::make_shared<BuffersBase<Formats::Raw16>>(
        Host->RootFormat());
    buffers->Initialize(1, nullptr);
    BoundBuffers = buffers;
  }
  size_t childrenIndex = 0;
  ActionOnEachImmediateChild([&](Node& node) {
    node.AllocateBuffers(childrenIndex++);
  });
}

void TransformTree::Node::Execute() {
  if (Parent != nullptr) {
    DBG("Executing");
    auto checkPointStart = std::chrono::high_resolution_clock::now();
    BoundTransform->Do(*Parent->BoundBuffers, BoundBuffers.get());
    auto checkPointFinish = std::chrono::high_resolution_clock::now();
    ElapsedTime = checkPointFinish - checkPointStart;
    Host->transformsCache_[BoundTransform->Name()].ElapsedTime += ElapsedTime;

    if (Host->ValidateAfterEachTransform()) {
      try {
        BoundBuffers->Validate();
      }
      catch(const InvalidBuffersException& e) {
        throw TransformResultedInInvalidBuffersException(BoundTransform->Name(),
                                                         e.what());
      }
    }

    if (Host->DumpBuffersAfterEachTransform()) {
      INF("Buffers after %s", BoundTransform->Name().c_str());
      INF("==============%s",
          std::string(BoundTransform->Name().size(), '=').c_str());
      INF("%s", BoundBuffers->Dump().c_str());
    }
  }
  ActionOnEachImmediateChild([&](Node& node) {
    node.Execute();
  });
}

size_t TransformTree::Node::ChildrenCount() const noexcept {
  size_t size = 0;
  for (auto child : Children) {
    size += child.second.size();
  }
  return size;
}

TransformTree::TransformTree(Formats::RawFormat16&& rootFormat) noexcept
    : Logger("TransformTree", EINA_COLOR_ORANGE),
      root_(std::make_shared<Node>(
        nullptr, std::make_shared<RootTransform>(
            std::make_shared<Formats::RawFormat16>(rootFormat)), this)),
      rootFormat_(std::make_shared<Formats::RawFormat16>(rootFormat)),
      treeIsPrepared_(false),
      validateAfterEachTransform_(false),
      dumpBuffersAfterEachTransform_(false) {
}

TransformTree::TransformTree(
    const std::shared_ptr<Formats::RawFormat16>& rootFormat) noexcept
    : Logger("TransformTree", EINA_COLOR_ORANGE),
      root_(std::make_shared<Node>(
        nullptr, std::make_shared<RootTransform>(rootFormat), this)),
      rootFormat_(rootFormat),
      treeIsPrepared_(false),
      validateAfterEachTransform_(false),
      dumpBuffersAfterEachTransform_(false) {
}

TransformTree::~TransformTree() noexcept {
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
  auto ctorit = tfit->second.find(
      (*currentNode)->BoundTransform->OutputFormat()->Id());
  if (ctorit == tfit->second.end()) {
    DBG("Formats mismatch, iterating input formats (%zu)",
        tfit->second.size());
    // No matching format found, try to add the format converter first
    for (auto ctoritfmt : tfit->second) {
      auto t = ctoritfmt.second();
      DBG("Probing %s -> %s",
          (*currentNode)->BoundTransform->OutputFormat()->Id().c_str(),
          t->InputFormat()->Id().c_str());
      auto convName = FormatConverter::Name(
          *(*currentNode)->BoundTransform->OutputFormat(), *t->InputFormat());
      try {
        AddTransform(convName, "", relatedFeature, currentNode);
      }
      catch (const TransformNotRegisteredException&) {
        continue;
      }
      ctor = ctoritfmt.second;
      break;
    }
  } else {
    ctor = ctorit->second;
  }
  if (ctor == nullptr) {
    throw IncompatibleTransformFormatException(*(*currentNode)->BoundTransform,
                                               name);
  }

  // Create the transform "name"
  auto t = ctor();
  {
    auto tparams = Transform::Parse(parameters);
    t->SetParameters(tparams);
  }

  // Try to reuse the already existing transform
  auto reusedNode = (*currentNode)->FindIdenticalChildTransform(*t);
  if (reusedNode != nullptr) {
    *currentNode = reusedNode;
  } else {
    auto reusedTransform = FindIdenticalTransform(*t);
    if (reusedTransform != nullptr) {
      DBG("Reusing the already existing transform");
      t = reusedTransform;
    } else {
      // Set the input format
      t->SetInputFormat((*currentNode)->BoundTransform->OutputFormat());
    }
    // Append the newly created transform
    auto newNode = std::make_shared<Node>(currentNode->get(), t);
    (*currentNode)->Children[name].push_back(newNode);
    *currentNode = newNode;
  }

  (*currentNode)->RelatedFeatures.push_back(relatedFeature);
}

std::shared_ptr<Formats::RawFormat16> TransformTree::RootFormat()
    const noexcept {
  return rootFormat_;
}

void TransformTree::AddFeature(
    const std::string& name,
    const std::vector<std::pair<std::string, std::string>>& transforms) {
  DBG("Adding \"%s\"", name.c_str());
  if (treeIsPrepared_) {
    throw TreeIsPreparedException();
  }
  if (features_.find(name) != features_.end()) {
    throw ChainNameAlreadyExistsException(name);
  }

  auto currentNode = root_;
  root_->RelatedFeatures.push_back(name);
  for (auto tpair : transforms) {
    auto tname = tpair.first;
    AddTransform(tpair.first, tpair.second, name, &currentNode);
  }

  features_.insert(std::make_pair(name, currentNode));
}

void TransformTree::PrepareForExecution() {
  DBG("Entered");
  if (treeIsPrepared_) {
    throw TreeAlreadyPreparedException();
  }
  root_->ActionOnEachTransformInSubtree([](const Transform& t) {
    t.Initialize();
  });
  root_->AllocateBuffers(0);
  treeIsPrepared_ = true;
}

std::unordered_map<std::string, std::shared_ptr<Buffers>>
TransformTree::Execute(const Formats::Raw16& in) {
  DBG("Entered");
  if (!treeIsPrepared_) {
    throw TreeIsNotPreparedException();
  }
  if (features_.size() == 0) {
    throw TreeIsEmptyException();
  }

  // Initialize input
  auto buffers = std::make_shared<BuffersBase<Formats::Raw16>>(
      rootFormat_);
  buffers->Initialize(1, in);
  root_->BoundBuffers = buffers;
  if (ValidateAfterEachTransform()) {
    try {
      root_->BoundBuffers->Validate();
    }
    catch(const InvalidBuffersException& e) {
      throw InvalidInputBuffersException(e.what());
    }
  }

  // Run the transforms, measuring the elapsed time
  auto checkPointStart = std::chrono::high_resolution_clock::now();
  root_->Execute();
  auto checkPointFinish = std::chrono::high_resolution_clock::now();
  auto allDuration = checkPointFinish - checkPointStart;
  INF("Finished. Execution took %f s", ConvertDuration(allDuration));
  auto otherDuration = allDuration;
  for (auto cit : transformsCache_) {
    otherDuration -= cit.second.ElapsedTime;
  }
  transformsCache_["All"].ElapsedTime = allDuration;
  transformsCache_["Other"].ElapsedTime = otherDuration;

  // Populate the results
  std::unordered_map<std::string, std::shared_ptr<Buffers>> results;
  for (auto feature : features_) {
    results[feature.first] = feature.second->BoundBuffers;
  }
  return std::move(results);
}

std::shared_ptr<Transform> TransformTree::FindIdenticalTransform(
    const Transform& base) noexcept {
  std::string id = base.Name();
  for (auto pp : base.GetParameters()) {
    id += pp.first;
    id += pp.second;
  }
  auto iti = transformsCache_.find(id);
  if (iti != transformsCache_.end()) {
    return iti->second.BoundTransform;
  }
  return nullptr;
}

void TransformTree::SaveTransformToCache(
    const std::shared_ptr<Transform>& transform) noexcept {
  std::string id = transform->Name();
  for (auto pp : transform->GetParameters()) {
    id += pp.first;
    id += pp.second;
  }
  transformsCache_.insert(
      std::make_pair(id,
                     TransformCacheItem {
                       transform,
                       std::chrono::high_resolution_clock::duration(0)
                     }
      )
  );
}

std::unordered_map<std::string, float>
TransformTree::ExecutionTimeReport() const noexcept {
  std::unordered_map<std::string, float> ret;
  auto allIt = transformsCache_.find("All");
  if (allIt == transformsCache_.end()) {
    return ret;
  }
  auto allTime = allIt->second.ElapsedTime;
  for (auto cit : transformsCache_) {
    if (cit.first != "All") {
      ret.insert(std::make_pair(
          cit.first, (cit.second.ElapsedTime.count() + 0.f) / allTime.count()));
    } else {
      ret.insert(std::make_pair(
          cit.first, ConvertDuration(allTime)));
    }
  }
  return std::move(ret);
}

void TransformTree::Dump(const std::string& dotFileName) const {
  // I am very sorry for such a complicated code. Please forgive me.
  // It just has to be here.
  const float redThreshold = 0.25f;

  auto timeReport = ExecutionTimeReport();
  bool includeTime = timeReport.size() > 0;
  float maxTimeRatio = 0.0f;
  if (includeTime) {
    for (auto tr : timeReport) {
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
  std::unordered_map<const Node*, int> nodeCounters;
  root_->ActionOnSubtree([&](const Node& node) {
    auto t = node.BoundTransform;
    nodeCounters[&node] = counters[t->Name()];
    fw << "\t" << t->SafeName() << counters[t->Name()]++ << " [";
    if (includeTime && timeReport[t->Name()] > redShift) {
      fw << "style=\"filled\", fillcolor=\"#";
      int light = 255 - (timeReport[t->Name()] - redShift) /
          (maxTimeRatio - redShift) * (255 - initialLight);
      // this is crazy printing of smth like ff4040
      fw << "ff" << std::hex << std::setw(2) << std::setfill('0') << light
         << std::setw(2) << std::setfill('0') << light << "\", ";
    }
    fw << "label=<" << t->HtmlEscapedName()
        << "<br /><font point-size=\"10\">";
    if (includeTime) {
      fw << "<b>"
          << std::to_string(static_cast<int>(
              (roundf((node.ElapsedTime.count() * 100.f *
                  BUGGY_SYSTEM_CLOCK_FIX ) / allTime))))
          << "% ("
          << std::to_string(static_cast<int>(roundf(
              timeReport[t->Name()] * 100.f)))
          << "%)</b>";
    }
    if (t->GetParameters().size() > 0) {
      fw << "<br /> <br />";
      for (auto p : t->GetParameters()) {
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
        auto featureTime = node.ElapsedTime;
        node.ActionOnEachParent([&](const Node& parent) {
          featureTime += parent.ElapsedTime / parent.RelatedFeatures.size();
        });
        fw << "<b>"
          << std::to_string(static_cast<int>(
              (roundf((ConvertDuration(featureTime) * 100.f) / allTime))))
          << "%</b></font>";
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
    fw << "<br /><font point-size=\"10\"><b>"
        << std::to_string(static_cast<int>(roundf(timeReport["Other"] * 100.f)))
        << "%</b></font>";
  }
  fw << ">]" << std::endl << std::endl;
  // Output the node connections
  root_->ActionOnSubtree([&](const Node& node) {
    std::string nodeName = node.BoundTransform->SafeName() +
        std::to_string(nodeCounters[&node]);
    node.ActionOnEachImmediateChild([&](const Node& child) {
      fw << "\t" << nodeName << " -> "
          << child.BoundTransform->SafeName()
          << nodeCounters[&child] << std::endl;
    });
    if (node.Children.size() == 0) {
      fw << "\t" << nodeName << " -> " << *node.RelatedFeatures.begin()
          << std::endl;
    }
  });
  fw << "}" << std::endl;
}

bool TransformTree::ValidateAfterEachTransform() const noexcept {
  return validateAfterEachTransform_;
}

void TransformTree::SetValidateAfterEachTransform(bool value) noexcept {
  validateAfterEachTransform_ = value;
}

bool TransformTree::DumpBuffersAfterEachTransform() const noexcept {
  return dumpBuffersAfterEachTransform_;
}

void TransformTree::SetDumpBuffersAfterEachTransform(bool value) noexcept {
  dumpBuffersAfterEachTransform_ = value;
}

float TransformTree::ConvertDuration(
    const std::chrono::high_resolution_clock::duration& d) noexcept {
  return (d.count() + 0.f) * BUGGY_SYSTEM_CLOCK_FIX *
      std::chrono::high_resolution_clock::period::num /
      std::chrono::high_resolution_clock::period::den;
}

}  // namespace SoundFeatureExtraction
