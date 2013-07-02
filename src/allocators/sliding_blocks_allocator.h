/*! @file sliding_blocks_allocator.h
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

#ifndef SRC_ALLOCATORS_SLIDING_BLOCKS_ALLOCATOR_H_
#define SRC_ALLOCATORS_SLIDING_BLOCKS_ALLOCATOR_H_

#include <memory>
#include "src/allocators/buffers_allocator.h"

namespace SoundFeatureExtraction {
namespace MemoryAllocation {

class SlidingBlocksImpl;

class SlidingBlocksAllocator : public BuffersAllocator {
 public:
  SlidingBlocksAllocator() noexcept;

  virtual size_t Solve(Node* root) const noexcept override;

 private:
  std::shared_ptr<SlidingBlocksImpl> impl_;
};

}  // namespace MemoryAllocation
}  // namespace SoundFeatureExtraction
#endif  // SRC_ALLOCATORS_SLIDING_BLOCKS_ALLOCATOR_H_
