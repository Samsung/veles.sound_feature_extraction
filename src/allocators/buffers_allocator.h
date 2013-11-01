/*! @file buffers_allocator.h
 *  @brief Definition of a buffers allocator interface.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_ALLOCATORS_BUFFERS_ALLOCATOR_H_
#define SRC_ALLOCATORS_BUFFERS_ALLOCATOR_H_

#include <stddef.h>
#include <limits>
#include <vector>
#include "src/logger.h"
#include "src/exceptions.h"

namespace sound_feature_extraction {
namespace MemoryAllocation {

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

}  // namespace MemoryAllocation
}  // namespace sound_feature_extraction
#endif  // SRC_ALLOCATORS_BUFFERS_ALLOCATOR_H_
