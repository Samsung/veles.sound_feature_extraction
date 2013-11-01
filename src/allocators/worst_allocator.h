/*! @file worst_allocator.h
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

#ifndef SRC_ALLOCATORS_WORST_ALLOCATOR_H_
#define SRC_ALLOCATORS_WORST_ALLOCATOR_H_

#include "src/allocators/buffers_allocator.h"

namespace sound_feature_extraction {
namespace MemoryAllocation {

class WorstAllocator : public BuffersAllocator {
 public:
  virtual size_t Solve(Node* root) const noexcept override;

 private:
  static Node* VisitNode(Node* node, size_t* mappedSoFar) noexcept;
};

}  // namespace MemoryAllocation
}  // namespace sound_feature_extraction
#endif  // SRC_ALLOCATORS_WORST_ALLOCATOR_H_
