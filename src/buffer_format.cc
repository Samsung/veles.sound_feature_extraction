/*! @file buffer_format.cc
 *  @brief Buffer format base class.
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

const std::string& BufferFormat::Id() const noexcept {
  return id_;
}

BufferFormat& BufferFormat::operator=(const BufferFormat& other)
throw (InvalidFormatException) {
  if (this->id_ != other.id_) {
    throw new InvalidFormatException(this->id_, other.id_);
  }
  SetParametersFrom(other);
  return *this;
}

bool BufferFormat::operator==(const BufferFormat& other) const noexcept {
  if (this->id_ != other.id_) {
    return false;
  }
  return this->EqualsTo(other);
}

}  // namespace SpeechFeatureExtraction
