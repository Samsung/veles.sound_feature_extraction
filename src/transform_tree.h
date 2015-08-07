/*! @file transform_tree.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#ifndef SRC_TRANSFORM_TREE_H_
#define SRC_TRANSFORM_TREE_H_

#include <chrono>
#include <vector>
#include "src/formats/array_format.h"
#include "src/exceptions.h"
#include "src/logger.h"
#include "src/transform.h"
#include "src/allocators/buffers_allocator.h"

namespace sound_feature_extraction {

class ChainNameAlreadyExistsException : public ExceptionBase {
 public:
  explicit ChainNameAlreadyExistsException(const std::string& name)
  : ExceptionBase("Chain name \"" + name + "\" already exists.") {
  }
};

class ChainAlreadyExistsException : public ExceptionBase {
 public:
  ChainAlreadyExistsException(const std::string& nameOld,
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
                                       const std::string& child)
  : ExceptionBase("Transform \"" + child + "\" is incompatible with "
                  "output format \"" + parent.OutputFormat()->Id() +
                  "\" of transform \"" + parent.Name() + "\".") {
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

class TreeAlreadyPreparedException : public ExceptionBase {
 public:
  TreeAlreadyPreparedException()
  : ExceptionBase("Transform tree has already been prepared for execution.") {
  }
};

class TransformResultedInInvalidBuffersException : public ExceptionBase {
 public:
  TransformResultedInInvalidBuffersException(const std::string& transform,
                                             const std::string& inner)
  : ExceptionBase("Transform " + transform +
                  " resulted in invalid buffers. " + inner) {
  }
};

class InvalidInputBuffersException : public ExceptionBase {
 public:
  explicit InvalidInputBuffersException(const std::string& message)
  : ExceptionBase("Invalid input data. " + message) {
  }
};

class FailedToAllocateBuffersException : public std::bad_alloc {
 public:
  explicit FailedToAllocateBuffersException(const char* message) noexcept
  : message_(message) {}

  explicit FailedToAllocateBuffersException(const std::string& message) noexcept
  : message_(message) {}

  virtual const char* what() const noexcept {
    return message_.c_str();
  }

 private:
  std::string message_;
};

class MemoryProtector;

class TransformTree : public Logger {
 public:
  explicit TransformTree(formats::ArrayFormat16&& rootFormat) noexcept;
  explicit TransformTree(
      const std::shared_ptr<formats::ArrayFormat16>& rootFormat) noexcept;
  virtual ~TransformTree() = default;

  std::shared_ptr<formats::ArrayFormat16> RootFormat() const noexcept;

  void AddFeature(
      const std::string& name,
      const std::vector<std::pair<std::string, std::string>>& transforms);

  void PrepareForExecution();

  std::unordered_map<std::string, std::shared_ptr<Buffers>> Execute(
      const int16_t* in);

  std::unordered_map<std::string, float> ExecutionTimeReport() const noexcept;
  void Dump(const std::string& dotFileName) const;

  bool validate_after_each_transform() const noexcept;
  void set_validate_after_each_transform(bool value) noexcept;
  bool dump_buffers_after_each_transform() const noexcept;
  void set_dump_buffers_after_each_transform(bool value) noexcept;
  bool cache_optimization() const noexcept;
  void set_cache_optimization(bool value) noexcept;
  bool memory_protection() const noexcept;
  void set_memory_protection(bool value) noexcept;

 private:
  class Node : public Logger {
   public:
    Node(Node* parent, const std::shared_ptr<Transform>& boundTransform,
         size_t buffersCount, TransformTree* host) noexcept;

    void ActionOnEachTransformInSubtree(
        const std::function<void(const Transform&)> action) const;
    void ActionOnSubtree(
        const std::function<void(const Node&)> action) const;
    void ActionOnSubtree(const std::function<void(Node&)> action);
    void ActionOnEachImmediateChild(
        const std::function<void(Node&)> action);
    void ActionOnEachImmediateChild(
        const std::function<void(const Node&)> action) const;
    void ActionOnEachParent(
        const std::function<void(const Node&)> action) const;

    std::shared_ptr<Node> FindIdenticalChildTransform(const Transform& base)
        const noexcept;

    void BuildAllocationTree(memory_allocation::Node* node) const noexcept;

    void ApplyAllocationTree(const memory_allocation::Node& node,
                             void* allocatedMemory) noexcept;

    void Execute() noexcept;

    size_t ChildrenCount() const noexcept;
    std::shared_ptr<Node> SelfPtr() const noexcept;

    TransformTree* Host;
    Node* Parent;
    const std::shared_ptr<Transform> BoundTransform;
    std::shared_ptr<Buffers> BoundBuffers;
    size_t BuffersCount;
    std::shared_ptr<MemoryProtector> Protection;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Node>>>
    Children;
    /// @brief The next executed node in the pipeline.
    Node* Next;
    std::unordered_map<Node*, std::tuple<size_t, size_t>> Slices;
    Node* OriginalNode;
    int CycleId;
    bool HasClones;

    std::shared_ptr<std::chrono::high_resolution_clock::duration> ElapsedTime;
    std::vector<std::string> RelatedFeatures;
  };

  struct TransformCacheItem {
    TransformCacheItem() : ElapsedTime(0), Dump(false) {
    }

    std::shared_ptr<Transform> BoundTransform;
    std::chrono::high_resolution_clock::duration ElapsedTime;
    bool Dump;
  };

  static constexpr const char* kDumpEnvPrefix = "SFE_DUMP_";

  void AddTransform(const std::string& name,
                    const std::string& parameters,
                    const std::string& relatedFeature,
                    std::shared_ptr<Node>* currentNode);
  void AddIdentityTransform(const std::string& feature,
                            std::shared_ptr<Node>* currentNode);

  int BuildSlicedCycles() noexcept;

  void DismantleMemoryProtection() noexcept;
  void ResetTimers() noexcept;

  static float ConvertDuration(
      const std::chrono::high_resolution_clock::duration& d) noexcept;

  /// @brief The continuous memory block containing all the buffers. It MUST
  /// go before root_ because of the memory protection scheme (mprotect).
  std::shared_ptr<void> allocated_memory_;
  /// @brief The transform tree to extract the features.
  std::shared_ptr<Node> root_;
  std::shared_ptr<formats::ArrayFormat16> root_format_;
  bool tree_is_prepared_;
  std::unordered_map<std::string, std::shared_ptr<Node>> features_;
  std::unordered_map<std::string, TransformCacheItem> transforms_cache_;
  bool cache_optimization_;
  bool memory_protection_;
  bool validate_after_each_transform_;
  bool dump_buffers_after_each_transform_;
};

}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORM_TREE_H_
