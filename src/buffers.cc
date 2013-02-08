/*! @file buffers.cc
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

#include "src/buffers.h"
#include <assert.h>
#include <string.h>

namespace SpeechFeatureExtraction {

Buffers::Buffers(int size, const BufferFormat& format) noexcept
: format_(format) {
  assert(size > 0 && size < (1 << 20));
  auto buffers = new std::vector<void*>();
  buffers->resize(size);
  buffers_ = std::shared_ptr<std::vector<void*>>(buffers,
                                                 [&](std::vector<void*>* vec) {
    auto destroy = format_.Destructor();
    for (size_t i = 0; i < vec->size(); i++) {
      if (vec->at(i) != nullptr) {
        destroy(vec->at(i));
      }
    }
    delete vec;
  });
}

Buffers::Buffers(const Buffers& other) noexcept
: format_(other.format_)
, buffers_(other.buffers_) {
}

int Buffers::Size() const noexcept {
  return buffers_->size();
}

void* Buffers::operator[](int index) noexcept {
  assert(index >= 0 && index < Size());
  return buffers_->at(index);
}

const void* Buffers::operator[](int index) const noexcept {
  assert(index >= 0 && index < Size());
  return buffers_->at(index);
}

void Buffers::Set(int index, void* buffer) noexcept {
  assert(index >= 0 && static_cast<size_t>(index) < buffers_->size());
  (*buffers_)[index] = buffer;
}

void Buffers::CopyPointersFrom(const void *const *array) noexcept {
  assert(array != nullptr);
  memcpy(&(*buffers_)[0], array, sizeof(void *) * buffers_->size());
}

const void *const *Buffers::Data() const noexcept {
  return reinterpret_cast<const void *const *>(&(*buffers_)[0]);
}

} /* namespace SpeechFeatureExtraction */
