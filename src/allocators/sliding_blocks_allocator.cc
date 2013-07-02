/*! @file sliding_blocks_allocator.cc
 *  @brief "Sliding blocks" buffers allocation strategy implementation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/allocators/sliding_blocks_allocator.h"
#include "src/allocators/sliding_blocks_impl.h"

namespace SoundFeatureExtraction {
namespace MemoryAllocation {

SlidingBlocksAllocator::SlidingBlocksAllocator() noexcept
    : impl_(new SlidingBlocksImpl()){
}

size_t  SlidingBlocksAllocator::Solve(Node* root) const noexcept {
  return impl_->Solve(root);
}

}  // namespace MemoryAllocation
}  // namespace SoundFeatureExtraction
