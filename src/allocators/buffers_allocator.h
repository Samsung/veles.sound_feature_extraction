/*! @file buffers_allocator.h
 *  @brief Definition of a buffers allocator interface.
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

#ifndef SRC_ALLOCATORS_BUFFERS_ALLOCATOR_H_
#define SRC_ALLOCATORS_BUFFERS_ALLOCATOR_H_

#include <stddef.h>
#include <limits>
#include <vector>
#include "src/logger.h"
#include "src/exceptions.h"

namespace sound_feature_extraction {
namespace memory_allocation {

class NodeParentIsNullException : public ExceptionBase {
 public:
  NodeParentIsNullException() : ExceptionBase("Node's parent is null.") {
  }
};

class CorruptedTreeException : public ExceptionBase {
 public:
  CorruptedTreeException()
      : ExceptionBase("The tree's parent-child relations are broken.") {
  }
};

struct Node {
  Node(size_t size, Node* parent, void* item)
      : Size(size),
        Address(UNINITIALIZED_ADDRESS),
        Parent(parent),
        Next(nullptr),
        Item(item) {
  }

  void Dump(const std::string& dotFileName) const;

  static const size_t UNINITIALIZED_ADDRESS =
      std::numeric_limits<size_t>::max();

  size_t Size;
  size_t Address;
  Node* Parent;
  Node* Next;
  void* Item;
  std::vector<Node> Children;
};

class BuffersAllocator : public Logger {
 public:
  BuffersAllocator();

  virtual ~BuffersAllocator() {
  }

  virtual size_t Solve(Node* root) const noexcept = 0;

  bool Validate(const Node& root) const;

 protected:
  size_t NodesCount(const Node& root) const noexcept;
  static bool NodesOverlap(const Node& n1, const Node& n2) noexcept;
  static bool SegmentsOverlap(size_t a1, size_t a2, size_t b1, size_t b2)
      noexcept;
};

}  // namespace memory_allocation
}  // namespace sound_feature_extraction
#endif  // SRC_ALLOCATORS_BUFFERS_ALLOCATOR_H_
