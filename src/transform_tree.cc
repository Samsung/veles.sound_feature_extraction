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
#include "src/transform_registry.h"

namespace SpeechFeatureExtraction {

TransformTree::Node::Node(Node* parent,
                          const std::shared_ptr<Transform>& boundTransform)
: Parent(parent),
  BoundTransform(boundTransform) {
}

void TransformTree::Node::Apply(
    const std::function<void(const Transform&)> action) {
  action(*BoundTransform);
  for (auto subnode : Children) {
    for (auto inode : subnode.second) {
      inode->Apply(action);
    }
  }
}

std::shared_ptr<TransformTree::Node>
TransformTree::Node::FindIdenticalChildTransform(
        const Transform& base) {
  auto tv = Children.find(base.Name());
  if (tv == Children.end()) return nullptr;
  for (auto node : tv->second) {
    if (base == *node->BoundTransform.get()) {
      return node;
    }
  }
  return nullptr;
}

void TransformTree::Node::ExecuteOwn(
        const Buffers& in,
        std::unordered_map<std::string, std::shared_ptr<Buffers>>* results,
        std::shared_ptr<Buffers> *out) {
  auto myOutBuffers = std::make_shared<Buffers>(in.Size());
  BoundTransform->Do(in, myOutBuffers.get());
  if (ChainName != "") {
    (*results)[ChainName] = myOutBuffers;
  }
  *out = std::move(myOutBuffers);
}

void TransformTree::Node::ExecuteChildren(
    const Buffers& in,
    std::unordered_map<std::string, std::shared_ptr<Buffers>>* results) {
  for (auto tnodepair : Children) {
    for (auto tnode : tnodepair.second) {
      auto buf = std::make_shared<Buffers>(in.Size());
      tnode->Execute(in, results);
    }
  }
}

void TransformTree::Node::Execute(
    const Buffers& in,
    std::unordered_map<std::string, std::shared_ptr<Buffers>>* results) {
  std::shared_ptr<Buffers> myOutBuffers = nullptr;
  ExecuteOwn(in, results, &myOutBuffers);
  if (Children.size() == 1 && Children.begin()->second.size() == 1) {
    // Memory optimization - free myOutBuffers in consecutive case
    std::shared_ptr<Buffers> nextBuffers = nullptr;
    auto childNodePtr = *Children.begin()->second.begin();
    childNodePtr->ExecuteOwn(*myOutBuffers, results, &nextBuffers);
    myOutBuffers.reset();
    childNodePtr->ExecuteChildren(*nextBuffers, results);
  } else {
    ExecuteChildren(*myOutBuffers, results);
  }
}

TransformTree::TransformTree() noexcept
: treeIsPrepared_(false) {
}

TransformTree::~TransformTree() noexcept {
}

bool TransformTree::ParametersAreCompatible(const Transform& parent,
                                            const Transform& child)
noexcept {
  for (auto p : child.DependencyParametersCheck()) {
    auto ppi = parent.CurrentParameters().find(p.first);
    if (ppi == parent.CurrentParameters().end()) {
      throw new DependencyParameterUnknownException(p.first,
                                                    parent.Name(),
                                                    child.Name());
    }
    auto compatible = p.second(ppi->second);
    if (!compatible) {
      return false;
    }
  }
  return true;
}

void TransformTree::AddChain(
    const std::string& name,
    const std::vector<std::pair<std::string, std::string>>& transforms)
throw (ChainNameAlreadyExistsException, TransformNotRegisteredException,
       ChainAlreadyExistsException) {
  if (treeIsPrepared_) {
    throw new TreeIsPreparedException();
  }
  if (chains_.find(name) != chains_.end()) {
    throw new ChainNameAlreadyExistsException(name);
  }
  chains_.insert(name);

  auto currentNode = root_;/*
  for (auto tpair : transforms) {
    auto tname = tpair.first;
    // Search for the factory of transform "tname"
    auto tf = TransformFactory.find(tname);
    if (tf == TransformFactory.end()) {
      throw new TransformNotRegisteredException(tname);
    }
    // Create the transform "tname"
    auto t = tf->second();
    {
      auto tparams = Parameters::Parse(tpair.second);
      t->SetParameters(tparams);
    }
    // Try to reuse the already existing transform
    auto reusedTransform = currentNode->FindIdenticalChildTransform(*t);
    if (reusedTransform != nullptr) {
      currentNode = reusedTransform;
    } else {
      // If the parent node's transform is not compatible, duplicate parent.
      // We assume here that no side effects appear on parent's parent.
      // Otherwise, the algorithm would become more complicated.
      if (!ParametersAreCompatible(*currentNode->BoundTransform, *t)) {
        auto parentCopy = currentNode->BoundTransform->Clone();
        parentCopy->SetParameters(t->DependencyParameters());
        assert(currentNode->Parent != NULL);
        auto parentCopyNode = std::make_shared<Node>(
            currentNode->Parent, parentCopy);
        currentNode->Parent->Children[parentCopy->Name()].push_back(parentCopyNode);
        currentNode = parentCopyNode;
      }
      // Append the newly created transform
      auto newNode = std::make_shared<Node>(currentNode.get(), t);
      currentNode->Children[tname].push_back(newNode);
      currentNode = newNode;
    }
  }
  if (currentNode->ChainName != "") {
    throw new ChainAlreadyExistsException(currentNode->ChainName, name);
  }
  currentNode->ChainName = name;*/
}

void TransformTree::PrepareForExecution() noexcept {
  treeIsPrepared_ = true;
  root_->Apply([](const Transform& t) {
    t.Initialize();
  });
}

std::unordered_map<std::string, std::shared_ptr<Buffers>>
TransformTree::Execute(const Buffers& in)
throw(TreeIsNotPreparedException, TreeIsEmptyException) {
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
  root_->Execute(in, &results);
  return std::move(results);
}

}  // namespace SpeechFeatureExtraction
