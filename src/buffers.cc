/*! @file buffers.cc
 *  @brief Buffers class definition.
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

Buffers::Buffers(size_t size,
                 const std::shared_ptr<BufferFormat>& format) noexcept
    : format_(format) {
  assert(size <= (1 << 20));
  auto buffers = new std::vector<void*>();
  buffers->resize(size);
  auto destroy = format_ != nullptr? format_->Destructor() : [](void*){};
  buffers_ = std::shared_ptr<std::vector<void*>>( // NOLINT(*)
      buffers, [=](std::vector<void*>* vec) {
    for (size_t i = 0; i < vec->size(); i++) {
      if (vec->at(i) != nullptr) {
        destroy(vec->at(i));
      }
    }
    delete vec;
  });
}

Buffers::Buffers(const Buffers& other) noexcept
    : format_(other.format_),
      buffers_(other.buffers_) {
}

Buffers::Buffers(const Buffers& other,
                 const std::shared_ptr<BufferFormat>& format) noexcept
    : format_(format),
      buffers_(other.buffers_) {
}

Buffers& Buffers::operator=(const Buffers& other) noexcept {
  assert(format_ == other.format_);
  buffers_ = other.buffers_;
  return *this;
}

size_t Buffers::Size() const noexcept {
  return buffers_->size();
}

void Buffers::SetSize(size_t size) noexcept {
  if (size < buffers_->size()) {
    auto destroy = format_->Destructor();
    for (size_t i = size; i < buffers_->size(); i++) {
      if (buffers_->at(i) != nullptr) {
        destroy(buffers_->at(i));
        buffers_->assign(i, nullptr);
      }
    }
  }
  buffers_->resize(size);
}

const std::shared_ptr<BufferFormat> Buffers::Format() const noexcept {
  return format_;
}

void* Buffers::operator[](size_t index) noexcept {
  assert(index < Size());
  return buffers_->at(index);
}

const void* Buffers::operator[](size_t index) const noexcept {
  assert(index < Size());
  return buffers_->at(index);
}

void Buffers::Set(size_t index, void* buffer) noexcept {
  assert(static_cast<size_t>(index) < buffers_->size());
  (*buffers_)[index] = buffer;
}

const void *const *Buffers::Data() const noexcept {
  return reinterpret_cast<const void *const *>(&(*buffers_)[0]);
}

void Buffers::Validate() const {
  Format()->Validate(*this);
}

std::string Buffers::Dump() const noexcept {
  return Format()->Dump(*this);
}

} /* namespace SpeechFeatureExtraction */
