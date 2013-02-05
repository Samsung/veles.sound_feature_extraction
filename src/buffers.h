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

#include <vector>

namespace SpeechFeatureExtraction {

class Buffers {
 public:
  Buffers(int size);
  virtual ~Buffers() {}

  int Size() const;

  void* operator[](int index);
  const void* operator[](int index) const;

  void Set(int index, void* buffer);
  void CopyPointersFrom(const void *const *array);

  const void *const *Data() const;

 private:
  std::vector<void*> buffers_;
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_BUFFERS_H_
