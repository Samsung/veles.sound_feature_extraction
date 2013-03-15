/*! @file transform_tree.cc
 *  @brief New file description.
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
#include <fstream>
#include <iomanip>
#include <string>
#include <utility>
#include "src/formats/raw_format.h"
#include "src/format_converter.h"
#include "src/transform_registry.h"

namespace SpeechFeatureExtraction {

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
  CurrentParameters() const noexcept {
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
                          TransformTree* host)
: Parent(parent)
, BoundTransform(boundTransform)
, BoundBuffers(nullptr)
, Host(host == nullptr? parent == nullptr? nullptr : parent->Host : host) {
}

void TransformTree::Node::ActionOnEachTransform(
    const std::function<void(const Transform&)> action) {
  action(*BoundTransform);
  for (auto subnode : Children) {
    for (auto inode : subnode.second) {
      inode->ActionOnEachTransform(action);
    }
  }
}

void TransformTree::Node::ActionOnEachNode(
    const std::function<void(const Node&)> action) {
  action(*this);
  for (auto subnode : Children) {
    for (auto inode : subnode.second) {
      inode->ActionOnEachNode(action);
    }
  }
}

std::shared_ptr<TransformTree::Node>
TransformTree::Node::FindIdenticalChildTransform(
        const Transform& base) {
  auto tv = Children.find(base.Name());
  if (tv == Children.end()) return nullptr;
  for (auto node : tv->second) {
    if (base == *node->BoundTransform) {
      return node;
    }
  }
  return nullptr;
}

void TransformTree::Node::Execute(
    std::unordered_map<std::string, std::shared_ptr<Buffers>>* results) {  // NOLINT(*)
  if (Parent != nullptr) {
    if (BoundBuffers == nullptr) {
      if (*BoundTransform->OutputFormat() ==
            *Parent->BoundTransform->InputFormat() &&
          Parent->Children.size() == 1) {
          BoundBuffers = Parent->BoundBuffers;
      } else {
          BoundBuffers = BoundTransform->CreateOutputBuffers(
              *Parent->BoundBuffers);
      }
    }
    auto checkPointStart = std::chrono::high_resolution_clock::now();
    BoundTransform->Do(*Parent->BoundBuffers, BoundBuffers.get());
    auto checkPointFinish = std::chrono::high_resolution_clock::now();
    ElapsedTime = checkPointFinish - checkPointStart;
    Host->transformsCache_[BoundTransform->Name()].ElapsedTime += ElapsedTime;
    if (ChainName != "") {
      (*results)[ChainName] = BoundBuffers;
    }
  }
  for (auto tnodepair : Children) {
    for (auto tnode : tnodepair.second) {
      tnode->Execute(results);
    }
  }
}

TransformTree::TransformTree(Formats::RawFormat16&& rootFormat) noexcept
    : root_(std::make_shared<Node>(
        nullptr, std::make_shared<RootTransform>(
            std::make_shared<Formats::RawFormat16>(rootFormat)), this)),
      treeIsPrepared_(false) {
}

TransformTree::TransformTree(
    const std::shared_ptr<Formats::RawFormat16>& rootFormat) noexcept
    : root_(std::make_shared<Node>(
        nullptr, std::make_shared<RootTransform>(rootFormat), this)),
      treeIsPrepared_(false) {
}

TransformTree::~TransformTree() noexcept {
}

void TransformTree::AddTransform(const std::string& name,
                                 const std::string& parameters,
                                 std::shared_ptr<Node>* currentNode) {
  // Search for the constructor of the transform "tname"
  auto tfit = TransformFactory.find(name);
  if (tfit == TransformFactory.end()) {
    throw TransformNotRegisteredException(name);
  }
  // tfit is actually a map from input format to real constructor
  auto ctorit = tfit->second.find(
      (*currentNode)->BoundTransform->OutputFormat()->Id());
  if (ctorit == tfit->second.end()) {
    ctorit = tfit->second.begin();
  }
  auto ctor = ctorit->second;

  // Create the transform "name"
  auto t = ctor();
  {
    auto tparams = Parameters::Parse(parameters);
    t->SetParameters(tparams);
  }

  // Add the format converter, if needed
  if (*t->InputFormat() != *(*currentNode)->BoundTransform->OutputFormat()) {
    auto convName = FormatConverter::Name(
        *(*currentNode)->BoundTransform->OutputFormat(), *t->InputFormat());
    AddTransform(convName, "", currentNode);
  }

  // Try to reuse the already existing transform
  auto reusedNode = (*currentNode)->FindIdenticalChildTransform(*t);
  if (reusedNode != nullptr) {
    *currentNode = reusedNode;
  } else {
    auto reusedTransform = FindIdenticalTransform(*t);
    if (reusedTransform != nullptr) {
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
}

void TransformTree::AddChain(
    const std::string& name,
    const std::vector<std::pair<std::string, std::string>>& transforms) {  // NOLINT(*)
  if (treeIsPrepared_) {
    throw TreeIsPreparedException();
  }
  if (chains_.find(name) != chains_.end()) {
    throw ChainNameAlreadyExistsException(name);
  }

  auto currentNode = root_;
  for (auto tpair : transforms) {
    auto tname = tpair.first;
    AddTransform(tpair.first, tpair.second, &currentNode);
  }
  if (currentNode->ChainName != "") {
    throw ChainAlreadyExistsException(currentNode->ChainName, name);
  }
  currentNode->ChainName = name;
  chains_.insert(name);
}

void TransformTree::PrepareForExecution() noexcept {
  treeIsPrepared_ = true;
  root_->ActionOnEachTransform([](const Transform& t) {
    t.Initialize();
  });
}

std::unordered_map<std::string, std::shared_ptr<Buffers>>
TransformTree::Execute(const Buffers& in) {
  if (!treeIsPrepared_) {
    throw TreeIsNotPreparedException();
  }
  if (chains_.size() == 0) {
    throw TreeIsEmptyException();
  }
  std::unordered_map<std::string, std::shared_ptr<Buffers>> results;
  for (auto name : chains_) {
    results.insert(std::make_pair(name, nullptr));
  }
  root_->BoundBuffers = std::make_shared<Buffers>(in);
  auto checkPointStart = std::chrono::high_resolution_clock::now();
  root_->Execute(&results);
  auto checkPointFinish = std::chrono::high_resolution_clock::now();
  auto allDuration = checkPointFinish - checkPointStart;
  auto otherDuration = allDuration;
  for (auto cit : transformsCache_) {
    otherDuration -= cit.second.ElapsedTime;
  }
  transformsCache_["All"].ElapsedTime = allDuration;
  transformsCache_["Other"].ElapsedTime = otherDuration;
  return std::move(results);
}

std::shared_ptr<Transform> TransformTree::FindIdenticalTransform(
    const Transform& base) noexcept {
  std::string id = base.Name();
  for (auto pp : base.CurrentParameters()) {
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
    const std::shared_ptr<Transform>& t) noexcept {
  std::string id = t->Name();
  for (auto pp : t->CurrentParameters()) {
    id += pp.first;
    id += pp.second;
  }
  transformsCache_.insert(std::make_pair(
      id,
      TransformCacheItem {
        t, std::chrono::high_resolution_clock::duration(0)
      }));
}

std::unordered_map<std::string, float>
TransformTree::ExecutionTimeReport() const noexcept {
  std::unordered_map<std::string, float> ret;
  auto allIt = transformsCache_.find("All");
  if (allIt == transformsCache_.end()) {
    return ret;
  }
  auto allTime = allIt->second.ElapsedTime.count();
  for (auto cit : transformsCache_) {
    if (cit.first != "All") {
      ret.insert(std::make_pair(cit.first, (cit.second.ElapsedTime.count() + .0f) / allTime));
    } else {
      ret.insert(std::make_pair(cit.first, allTime));
    }
  }
  return std::move(ret);
}

void TransformTree::Dump(const std::string& dotFileName) const {
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
  root_->ActionOnEachNode([&](const Node& node) {
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
          << std::to_string(static_cast<int>(((node.ElapsedTime.count() * 100) / allTime)))
          << "% ("
          << std::to_string(static_cast<int>(timeReport[t->Name()] * 100))
          << "%)</b>";
    }
    if (t->CurrentParameters().size() > 1 ||
        (t->CurrentParameters().size() > 0 && t->HasInverse())) {
      fw << "<br /> <br />";
      for (auto p : t->CurrentParameters()) {
        if (p.first == "inverse" && !t->HasInverse()) {
          continue;
        }
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
  });
  fw << "\tOther [label=<Other";
  if (includeTime) {
    fw << "<br /><font point-size=\"10\"><b>"
        << std::to_string(static_cast<int>(timeReport["Other"]))
        << "%</b></font>";
  }
  fw << ">]" << std::endl << std::endl;
  root_->ActionOnEachNode([&](const Node& node) {
    for (auto child : node.Children) {
      for (auto childNode : child.second) {
        fw << "\t" << node.BoundTransform->SafeName()
            << nodeCounters[&node] << " -> "
            << childNode->BoundTransform->SafeName()
            << nodeCounters[childNode.get()] << std::endl;
      }
    }
  });
  fw << "}" << std::endl;
}

}  // namespace SpeechFeatureExtraction
