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
  virtual ~Buffers() noexcept {}

  int Size() const noexcept;

  void* operator[](int index) noexcept;
  const void* operator[](int index) const noexcept;

  void Set(int index, void* buffer) noexcept;
  void CopyPointersFrom(const void *const *array) noexcept;

  const void *const *Data() const noexcept;

 private:
  const BufferFormat& format_;
  std::shared_ptr<std::vector<void*>> buffers_;
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_BUFFERS_H_
