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

#ifndef TRANSFORM_TREE_H_
#define TRANSFORM_TREE_H_

#include <vector>
#include <set>
#include "src/formats/raw_format.h"
#include "src/exceptions.h"
#include "src/transform.h"

namespace SpeechFeatureExtraction {

class ChainNameAlreadyExistsException : public ExceptionBase {
 public:
  ChainNameAlreadyExistsException(const std::string& name)
  : ExceptionBase("Chain name \"" + name + "\" already exists.") {}
};

class ChainAlreadyExistsException : public ExceptionBase {
 public:
  ChainAlreadyExistsException(const std::string& nameOld,
                              const std::string& nameNew)
  : ExceptionBase("Chain \"" + nameNew + "\" is identical to previously "
                  "added \"" + nameOld + "\".") {}
};

class TransformNotRegisteredException : public ExceptionBase {
 public:
  TransformNotRegisteredException(const std::string& name)
  : ExceptionBase("Transform \"" + name + "\" is not registered.") {}
};

class IncompatibleTransformFormatException : public ExceptionBase {
 public:
  IncompatibleTransformFormatException(const Transform& parent,
                                       const Transform& child)
  : ExceptionBase("Transform \"" + child.Name() + "\" has input format \"" +
                  child.InputFormat()->Id() +
                  "\", while the previous transform \"" +
                  parent.Name() + "\" has output format \"" +
                  parent.OutputFormat().Id() + "\".") {}
};

class TreeIsEmptyException : public ExceptionBase {
 public:
  TreeIsEmptyException()
  : ExceptionBase("Transform tree is empty.") {}
};

class DependencyParameterUnknownException : public ExceptionBase {
 public:
  DependencyParameterUnknownException(const std::string& pname,
                                      const std::string& parentTransformName,
                                      const std::string& childTransformName)
  : ExceptionBase("Transform \"" + parentTransformName + "\" does not "
                  "have a registered parameter \"" + pname + "\" checked "
                  " by transform \"" + childTransformName + "\".") {}
};

class TreeIsPreparedException : public ExceptionBase {
 public:
  TreeIsPreparedException()
  : ExceptionBase("Transform tree has previously been prepared for execution "
      "and no more chains may be added.") {}
};

class TreeIsNotPreparedException : public ExceptionBase {
 public:
  TreeIsNotPreparedException()
  : ExceptionBase("Transform tree has not been prepared for execution.") {}
};

class TransformTree {
  struct Node {
    Node* Parent;
    const std::shared_ptr<Transform> BoundTransform;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Node>>>
    Children;
    std::string ChainName;

    Node(Node* parent, const std::shared_ptr<Transform>& boundTransform);

    std::shared_ptr<Node> FindIdenticalChildTransform(const Transform& base);

    void Apply(const std::function<void(const Transform&)> action);

    void ExecuteOwn(
        const Buffers& in,
        std::unordered_map<std::string, std::shared_ptr<Buffers>>* results,
        std::shared_ptr<Buffers> *out);

    void ExecuteChildren(
        const Buffers& in,
        std::unordered_map<std::string, std::shared_ptr<Buffers>>* results);

    void Execute(
        const Buffers& in,
        std::unordered_map<std::string, std::shared_ptr<Buffers>>* results);
  };

 public:
  explicit TransformTree(const Formats::RawFormat& rootFormat) noexcept;
  virtual ~TransformTree() noexcept;

  void AddChain(
      const std::string& name,
      const std::vector<std::pair<std::string, std::string>>& transforms)
  throw (ChainNameAlreadyExistsException, TransformNotRegisteredException,
         ChainAlreadyExistsException);

  void PrepareForExecution() noexcept;

  std::unordered_map<std::string, std::shared_ptr<Buffers>> Execute(
      const Buffers& in)
  throw(TreeIsNotPreparedException, TreeIsEmptyException);

 private:
  std::shared_ptr<Node> root_;
  bool treeIsPrepared_;
  std::set<std::string> chains_;
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_TRANSFORM_TREE_H_
