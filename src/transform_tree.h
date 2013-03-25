/*! @file transform_tree.h
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

#ifndef SRC_TRANSFORM_TREE_H_
#define SRC_TRANSFORM_TREE_H_

#include <chrono>
#include "src/formats/raw_format.h"
#include "src/exceptions.h"
#include "src/transform.h"

namespace SpeechFeatureExtraction {

class ChainNameAlreadyExistsException : public ExceptionBase {
 public:
  explicit ChainNameAlreadyExistsException(const std::string& name)
  : ExceptionBase("Chain name \"" + name + "\" already exists.") {
  }
};

class ChainAlreadyExistsException : public ExceptionBase {
 public:
  explicit ChainAlreadyExistsException(const std::string& nameOld,
                              const std::string& nameNew)
  : ExceptionBase("Chain \"" + nameNew + "\" is identical to previously "
                  "added \"" + nameOld + "\".") {
  }
};

class TransformNotRegisteredException : public ExceptionBase {
 public:
  explicit TransformNotRegisteredException(const std::string& name)
  : ExceptionBase("Transform \"" + name + "\" is not registered.") {
  }
};

class IncompatibleTransformFormatException : public ExceptionBase {
 public:
  IncompatibleTransformFormatException(const Transform& parent,
                                       const Transform& child)
  : ExceptionBase("Transform \"" + child.Name() + "\" has input format \"" +
                  child.InputFormat()->Id() +
                  "\", while the previous transform \"" +
                  parent.Name() + "\" has output format \"" +
                  parent.OutputFormat()->Id() + "\".") {
  }
};

class TreeIsEmptyException : public ExceptionBase {
 public:
  TreeIsEmptyException()
  : ExceptionBase("Transform tree is empty.") {
  }
};

class DependencyParameterUnknownException : public ExceptionBase {
 public:
  DependencyParameterUnknownException(const std::string& pname,
                                      const std::string& parentTransformName,
                                      const std::string& childTransformName)
  : ExceptionBase("Transform \"" + parentTransformName + "\" does not "
                  "have a registered parameter \"" + pname + "\" checked "
                  " by transform \"" + childTransformName + "\".") {
  }
};

class TreeIsPreparedException : public ExceptionBase {
 public:
  TreeIsPreparedException()
  : ExceptionBase("Transform tree has previously been prepared for execution "
      "and no more chains may be added.") {
  }
};

class TreeIsNotPreparedException : public ExceptionBase {
 public:
  TreeIsNotPreparedException()
  : ExceptionBase("Transform tree has not been prepared for execution.") {
  }
};

class TransformTree {
 public:
  explicit TransformTree(Formats::RawFormat16&& rootFormat) noexcept;
  explicit TransformTree(
      const std::shared_ptr<Formats::RawFormat16>& rootFormat) noexcept;
  virtual ~TransformTree() noexcept;

  std::shared_ptr<Formats::RawFormat16> RootFormat() noexcept;

  void AddChain(
      const std::string& name,
      const std::vector<std::pair<std::string, std::string>>& transforms);  // NOLINT(*)

  void PrepareForExecution() noexcept;

  std::unordered_map<std::string, std::shared_ptr<Buffers>> Execute(
      const Buffers& in);

  std::unordered_map<std::string, float> ExecutionTimeReport() const noexcept;
  void Dump(const std::string& dotFileName) const;

 private:
  struct Node {
    Node* Parent;
    const std::shared_ptr<Transform> BoundTransform;
    std::shared_ptr<Buffers> BoundBuffers;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Node>>>  // NOLINT(*)
    Children;
    std::string ChainName;
    TransformTree* Host;
    std::chrono::high_resolution_clock::duration ElapsedTime;

    Node(Node* parent, const std::shared_ptr<Transform>& boundTransform,
         TransformTree* host = nullptr);

    std::shared_ptr<Node> FindIdenticalChildTransform(const Transform& base);

    void ActionOnEachTransform(
        const std::function<void(const Transform&)> action);
    void ActionOnEachNode(
        const std::function<void(const Node&)> action);

    void Execute(
        std::unordered_map<std::string, std::shared_ptr<Buffers>>* results);  // NOLINT(*)
  };

  struct TransformCacheItem {
    std::shared_ptr<Transform> BoundTransform;
    std::chrono::high_resolution_clock::duration ElapsedTime;
  };

  void AddTransform(const std::string& name,
                    const std::string& parameters,
                    std::shared_ptr<Node>* currentNode);
  std::shared_ptr<Transform> FindIdenticalTransform(
      const Transform& base) noexcept;
  void SaveTransformToCache(const std::shared_ptr<Transform>& t) noexcept;

  std::shared_ptr<Node> root_;
  std::shared_ptr<Formats::RawFormat16> rootFormat_;
  bool treeIsPrepared_;
  std::set<std::string> chains_;
  std::unordered_map<std::string, TransformCacheItem> transformsCache_;
};

}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORM_TREE_H_
