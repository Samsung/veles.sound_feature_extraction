/*! @file filter_base.h
 *  @brief Common stuff for FIRFilterBase and IIRFilterBase.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_FILTER_BASE_H_
#define SRC_TRANSFORMS_FILTER_BASE_H_

#include <cassert>
#include <vector>
#include <mutex>
#include "src/formats/array_format.h"
#include "src/omp_transform_base.h"

namespace sound_feature_extraction {
namespace transforms {

template <class E>
class FilterBase : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  FilterBase() noexcept
      : length_(kDefaultFilterLength),
        max_executors_(threads_number()) {
  }

  TRANSFORM_PARAMETERS_SUPPORT(FilterBase)

  TP(length, int, kDefaultFilterLength, "Filter size in samples (order).")

  virtual void Initialize() const override {
    executors_.resize(max_executors_);
    for (int i = 0; i < max_executors_; i++) {
      executors_[i].executor = CreateExecutor();
      executors_[i].mutex = std::make_shared<std::mutex>();
    }
  }

  virtual void Do(const float* in, float* out) const noexcept override final {
    bool executed = false;
    while (!executed) {
      for (auto& sse : executors_) {
        if (sse.mutex->try_lock()) {
          Execute(sse.executor, in, out);
          sse.mutex->unlock();
          executed = true;
          break;
        }
      }
    }
  }

  int max_executors() const {
    return max_executors_;
  }

  void set_max_executors(int value) {
    assert(value > 0);
    max_executors_ = value;
  }

  static bool ValidateFrequency(const int& value) noexcept {
    return value >= kMinFilterFrequency && value <= kMaxFilterFrequency;
  }

  static constexpr int kMinFilterLength = 8;
  static constexpr int kMaxFilterLength = 10000;
  static constexpr int kDefaultFilterLength = 256;
  static constexpr int kMinFilterFrequency = 1;
  static constexpr int kMaxFilterFrequency = 24000;

 protected:
  virtual std::shared_ptr<E> CreateExecutor() const noexcept = 0;
  virtual void Execute(const std::shared_ptr<E>& exec, const float* in,
                       float* out) const = 0;

 private:
  struct ThreadSafeExecutor {
    std::shared_ptr<E> executor;
    std::shared_ptr<std::mutex> mutex;
  };

  mutable std::vector<ThreadSafeExecutor> executors_;
  int max_executors_;
};

template <class E>
bool FilterBase<E>::validate_length(const int& value) noexcept {
  if (value < kMinFilterLength || value > kMaxFilterLength) {
    return false;
  }
  return true;
}

template <class E>
RTP(FilterBase<E>, length)

}  // namespace transforms
}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORMS_FILTER_BASE_H_
