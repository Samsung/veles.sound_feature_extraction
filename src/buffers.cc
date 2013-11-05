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
#include <cassert>
#include <cstring>
#include <simd/memory.h>

namespace sound_feature_extraction {

Buffers::Buffers(const std::shared_ptr<BufferFormat>& format,
                 size_t count, void* reusedMemory) noexcept
    : format_(format),
      count_(count) {
  assert(count <= (1 << 30));
  count_ = count;
  size_t sizeEach = format_->SizeInBytes();
  void* memory = reusedMemory == nullptr ?
      malloc_aligned(count_ * sizeEach) : reusedMemory;
  buffers_ = std::shared_ptr<void>(
      memory,
      [=](void* buffers) {
        if (reusedMemory == nullptr) {
          free(buffers);
        }
      });
}

Buffers& Buffers::operator=(const Buffers& other) {
  if (SizeInBytes() < other.SizeInBytes()) {
    throw InsufficientAllocatedMemoryException(*this, other);
  }
  format_ = other.format_;
  std::memcpy(Data(), other.Data(), other.SizeInBytes());
  count_ = other.count_;
  return *this;
}

size_t Buffers::Count() const noexcept {
  return count_;
}

size_t Buffers::SizeInBytes() const noexcept {
  return count_ * format_->SizeInBytes();
}

std::shared_ptr<BufferFormat> Buffers::Format() const noexcept {
  return format_;
}

void* Buffers::operator[](size_t index) noexcept {
  assert(index < Count());
  return reinterpret_cast<char*>(buffers_.get()) +
      index * format_->SizeInBytes();
}

const void* Buffers::operator[](size_t index) const noexcept {
  assert(index < Count());
  return reinterpret_cast<const char*>(buffers_.get()) +
      index * format_->SizeInBytes();
}

void* Buffers::Data() noexcept {
  return buffers_.get();
}

const void* Buffers::Data() const noexcept {
  return buffers_.get();
}

void Buffers::Validate() const {
  format_->Validate(*this);
}

std::string Buffers::Dump() const noexcept {
  std::string ret("====Dump of ");
  ret += std::to_string(Count());
  ret += " buffers (format ";
  ret += format_->ToString();
  ret += ")====\n";
  for (size_t i = 0; i < Count(); i++) {
    ret += format_->Dump(*this, i);
  }
  return ret;
}

std::string Buffers::Dump(size_t index) const noexcept {
  std::string ret("====Dump of [");
  ret += std::to_string(index);
  ret += "] (format ";
  ret += format_->ToString();
  ret += ")====\n";
  return ret + format_->Dump(*this, index);
}

std::string Buffers::ToString() const noexcept {
  std::string ret("Buffers (format \"");
  ret += Format()->ToString();
  ret += "\", count ";
  ret += std::to_string(Count());
  ret += ", size ";
  ret += std::to_string(SizeInBytes());
  ret += ")";
  return ret;
}

} /* namespace sound_feature_extraction */
