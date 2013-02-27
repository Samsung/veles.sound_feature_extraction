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
#include "src/formats/raw_format.h"
#include "src/format_converter.h"
#include "src/transform_registry.h"

namespace SpeechFeatureExtraction {

class RootTransform : public Transform {
 public:
  explicit RootTransform(const Formats::RawFormat16& format) noexcept
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

  virtual const BufferFormat& InputFormat() const noexcept {
    return format_;
  }

  virtual void SetInputFormat(const BufferFormat& format) {
    format_ = format;
  }

  virtual const BufferFormat& OutputFormat() const noexcept {
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

  virtual Buffers* CreateOutputBuffers(const Buffers&) const noexcept {
    return nullptr;
  }

  virtual void Do(const Buffers& in, Buffers *out) const noexcept {
    *out = in;
  }

 private:
  Formats::RawFormat16 format_;
};

TransformTree::Node::Node(Node* parent,
                          const std::shared_ptr<Transform>& boundTransform)
: Parent(parent)
, BoundTransform(boundTransform)
, BoundBuffers(nullptr)
, Host(parent == nullptr? nullptr : parent->Host) {
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
    std::unordered_map<std::string, std::shared_ptr<Buffers>>* results) {
  if (Parent != nullptr) {
    if (BoundBuffers == nullptr) {
      if (BoundTransform->OutputFormat() ==
            Parent->BoundTransform->InputFormat() &&
          Parent->Children.size() == 1) {
          BoundBuffers = Parent->BoundBuffers;
      } else {
          BoundBuffers = std::shared_ptr<Buffers>(
              BoundTransform->CreateOutputBuffers(*Parent->BoundBuffers),
              [](Buffers *ptr){ delete[] ptr; });
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

TransformTree::TransformTree(const Formats::RawFormat16& rootFormat) noexcept
: root_(std::make_shared<Node>(nullptr,
                               std::make_shared<RootTransform>(rootFormat))),
  treeIsPrepared_(false) {
  root_->Host = this;
}

TransformTree::~TransformTree() noexcept {
}

void TransformTree::AddTransform(const std::string& name,
                                 const std::string& parameters,
                                 std::shared_ptr<Node>* currentNode) {
  // Search for the constructor of the transform "tname"
  auto tfit = TransformFactory.find(name);
  if (tfit == TransformFactory.end()) {
    throw new TransformNotRegisteredException(name);
  }
  // tfit is actually a map from input format to real constructor
  auto ctorit = tfit->second.find(
      (*currentNode)->BoundTransform->OutputFormat().Id());
  if (ctorit == tfit->second.end()) {
    throw new TransformNotRegisteredException(name);
  }
  auto ctor = ctorit->second;

  // Create the transform "name"
  auto t = ctor();
  {
    auto tparams = Parameters::Parse(parameters);
    t->SetParameters(tparams);
  }

  // Add the format converter, if needed
  if (t->InputFormat() != (*currentNode)->BoundTransform->OutputFormat()) {
    auto convName = FormatConverter::Name(
        t->InputFormat(), (*currentNode)->BoundTransform->OutputFormat());
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
    const std::vector<std::pair<std::string, std::string>>& transforms) {
  if (treeIsPrepared_) {
    throw new TreeIsPreparedException();
  }
  if (chains_.find(name) != chains_.end()) {
    throw new ChainNameAlreadyExistsException(name);
  }

  auto currentNode = root_;
  for (auto tpair : transforms) {
    auto tname = tpair.first;
    AddTransform(tpair.first, tpair.second, &currentNode);
  }
  if (currentNode->ChainName != "") {
    throw new ChainAlreadyExistsException(currentNode->ChainName, name);
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
    throw new TreeIsNotPreparedException();
  }
  if (chains_.size() == 0) {
    throw new TreeIsEmptyException();
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
      }
  ));
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
      ret.insert(std::make_pair(cit.first, cit.second.ElapsedTime / allTime));
    } else {
      ret.insert(std::make_pair(cit.first, allTime.count()));
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
  std::chrono::high_resolution_clock::duration::rep rep(timeReport["All"]);
  auto allTime = std::chrono::high_resolution_clock::duration(rep);

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
      fw << std::hex << static_cast<int>(
          (timeReport[t->Name()] - redShift) / (maxTimeRatio - redShift) * 255)
         << "4040\", ";
    }
    fw << "label=<" << t->HtmlEscapedName()
        << "<br /><font point-size=\"10\">";
    if (includeTime) {
      fw << "<b>"
          << static_cast<int>((node.ElapsedTime / allTime * 100))
          << "% ("
          << static_cast<int>(timeReport[t->Name()] / timeReport["All"] * 100)
          << "%)</b><br /> <br />";
    }
    for (auto p : t->CurrentParameters()) {
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
    if (t->CurrentParameters().size() == 0) {
      fw << " ";
    }
    fw << "</font>>]" << std::endl;
  });
  fw << "\tOther [label=<Other";
  if (includeTime) {
    fw << "<br /><font point-size=\"10\"><b>"
        << timeReport["Other"] / timeReport["All"] * 100
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
