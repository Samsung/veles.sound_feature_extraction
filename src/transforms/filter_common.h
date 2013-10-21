/*! @file filter_common.h
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

#ifndef SRC_TRANSFORMS_FILTER_COMMON_H_
#define SRC_TRANSFORMS_FILTER_COMMON_H_

#include <cassert>
#include <vector>
#include <mutex>
#include "src/formats/array_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

template <class E>
class FilterBase
    : public OmpUniformFormatTransform<Formats::ArrayFormatF> {
 public:
  FilterBase() noexcept
      : length_(kDefaultFilterLength),
        max_executors_(MaxThreadsNumber()) {
    RegisterSetter("length", [&](const std::string& value) {
      int pv = Parse<int>("length", value);
      if (pv < kMinFilterLength || pv > kMaxFilterLength) {
        return false;
      }
      length_ = pv;
      return true;
    });
  }

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

  int length() const {
    return length_;
  }

  void set_length(int value) {
    assert(value >= kMinFilterLength);
    length_ = value;
  }

  int max_executors() const {
    return max_executors_;
  }

  void set_max_executors(int value) {
    assert(value > 0);
    max_executors_ = value;
  }

  static constexpr int kMinFilterLength = 8;
  static constexpr int kMaxFilterLength = 1000000;
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
  int length_;
  int max_executors_;
};

#define FILTER_PARAMETERS(init) OMP_TRANSFORM_PARAMETERS( \
  FORWARD_MACROS( \
      TP("length", "Filter size in samples (order).", \
         std::to_string(kDefaultFilterLength)) \
      init) \
)

}  // namespace Transforms
}  // namespace SoundFeatureExtraction


#endif  // SRC_TRANSFORMS_FILTER_COMMON_H_
