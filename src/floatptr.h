/*! @file floatptr.h
 *  @brief Typedef of unique_ptr to aligned floating-point array and the
 *  thread-safe wrapper.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FLOATPTR_H_
#define SRC_FLOATPTR_H_

#include <cstdlib>
#include <mutex>

namespace sound_feature_extraction {

typedef std::unique_ptr<float[], decltype(&std::free)> FloatPtr;

struct ThreadSafeFloatPtr {
  ThreadSafeFloatPtr() : data(nullptr, std::free) {
  }

  ThreadSafeFloatPtr(const ThreadSafeFloatPtr&) : data(nullptr, std::free) {
  }

  FloatPtr data;
  std::mutex mutex;
};

}  // namespace sound_feature_extraction

#endif  // SRC_FLOATPTR_H_
