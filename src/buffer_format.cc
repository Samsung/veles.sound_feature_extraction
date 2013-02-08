/*! @file buffer_format.cc
 *  @brief Buffer format interface class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/buffer_format.h"

namespace SpeechFeatureExtraction {

BufferFormat::BufferFormat(const std::string& id) noexcept
: id_(id) {
}

BufferFormat::BufferFormat(const BufferFormat& other) noexcept
: id_(other.id_) {
}

BufferFormat& BufferFormat::operator=(const BufferFormat& other) noexcept {
  id_ = other.id_;
  return *this;
}

const std::string& BufferFormat::Id() const noexcept {
  return id_;
}

bool BufferFormat::operator==(const BufferFormat& other) const noexcept {
  return this->id_ == other.id_;
}

}  // namespace SpeechFeatureExtraction
