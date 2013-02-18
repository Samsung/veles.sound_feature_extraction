/*! @file buffers.h
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

#ifndef BUFFERS_H_
#define BUFFERS_H_

#include <memory>
#include <vector>
#include "src/buffer_format.h"

namespace SpeechFeatureExtraction {

class Buffers {
 public:
  Buffers(int size, const BufferFormat& format) noexcept;
  Buffers(const Buffers& other) noexcept;
  Buffers& operator=(const Buffers& other) noexcept;

  virtual ~Buffers() noexcept {
  }

  size_t Size() const noexcept;

  void* operator[](size_t index) noexcept;
  const void* operator[](size_t index) const noexcept;

  const void *const *Data() const noexcept;

 protected:
  /// @defgroup TypeUnsafe Underlying type-unsafe operations
  /// @{
  void Set(size_t index, void* buffer) noexcept;
  void SetSize(size_t size) noexcept;
  /// @}

 private:
  const BufferFormat& format_;
  std::shared_ptr<std::vector<void*>> buffers_;
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_BUFFERS_H_
