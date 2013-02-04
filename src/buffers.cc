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

Buffers::Buffers(int size) {
  buffers_.resize(size);
}

int Buffers::Size() const {
  return buffers_.size();
}

void* Buffers::operator[](int index) {
  return buffers_[index];
}

const void* Buffers::operator[](int index) const {
  return buffers_[index];
}

void Buffers::Set(int index, void* buffer) {
  assert(index >= 0 && index < buffers_.size());
  buffers_[index] = buffer;
}

void Buffers::CopyPointersFrom(const void *const *array) {
  memcpy(&buffers_[0], array, sizeof(void *) * buffers_.size());
}

} /* namespace SpeechFeatureExtraction */
