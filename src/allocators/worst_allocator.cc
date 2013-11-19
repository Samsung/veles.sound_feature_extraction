/*! @file worst_allocator.cc
 *  @brief The worst (and the simplest) buffers allocator.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/allocators/worst_allocator.h"

namespace sound_feature_extraction {
namespace memory_allocation {

size_t WorstAllocator::Solve(Node* root) const noexcept {
  size_t result = 0;
  VisitNode(root, &result);
  return result;
}

Node* WorstAllocator::VisitNode(Node* node, size_t* mappedSoFar) noexcept {
  node->Address = *mappedSoFar;
  if (node->Children.size() > 0) {
    node->Next = &node->Children[0];
  }
  *mappedSoFar += node->Size;
  for (size_t i = 0; i < node->Children.size(); i++) {
    Node* leaf = VisitNode(&node->Children[i], mappedSoFar);
    if (i < node->Children.size() - 1) {
      leaf->Next = &node->Children[i + 1];
    } else {
      return leaf;
    }
  }
  return node;
}

}  // namespace memory_allocation
}  // namespace sound_feature_extraction
