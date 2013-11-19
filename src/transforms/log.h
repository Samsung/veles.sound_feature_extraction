/*! @file log.h
 *  @brief Taking logarithm on the real values.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_LOG_H_
#define SRC_TRANSFORMS_LOG_H_

#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

enum class LogarithmBase {
  k2,
  k10,
  kE
};

namespace internal {
constexpr const char* kLogBaseEStr = "e";
constexpr const char* kLogBase2Str = "2";
constexpr const char* kLogBase10Str = "10";
}

LogarithmBase Parse(const std::string& value, identity<LogarithmBase>);

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  using sound_feature_extraction::transforms::LogarithmBase;

  inline string
  to_string(const LogarithmBase& lb) noexcept {
    switch (lb) {
      case LogarithmBase::kE:
        return sound_feature_extraction::transforms::internal::kLogBaseEStr;
      case LogarithmBase::k2:
        return sound_feature_extraction::transforms::internal::kLogBase2Str;
      case LogarithmBase::k10:
        return sound_feature_extraction::transforms::internal::kLogBase10Str;
    }
    return "";
  }
}  // namespace std

namespace sound_feature_extraction {
namespace transforms {

template <class F>
class LogBase : public virtual OmpUniformFormatTransform<F> {
 public:
  LogBase() : base_(kDefaultLogBase) {
  }
  TRANSFORM_PARAMETERS_SUPPORT(LogBase<F>)

  TP(base, LogarithmBase, kDefaultLogBase, "Logarithm base (2, 10 or e).")

 protected:
  static constexpr LogarithmBase kDefaultLogBase = LogarithmBase::kE;
};

template <class F>
constexpr LogarithmBase LogBase<F>::kDefaultLogBase;

template <class F>
bool LogBase<F>::validate_base(const LogarithmBase&) noexcept {
  return true;
}

template <class F>
RTP(LogBase<F>, base)

template <class F>
class Log : public LogBase<F> {
 public:
  TRANSFORM_INTRO("Log",
                  "Takes the logarithm from each real value of the signal.",
                  LogBase<F>)
};

class LogRaw : public Log<formats::ArrayFormatF> {
 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

  void Do(bool simd, const float* input, int length,
          float* output) const noexcept;
};

class LogRawInverse : public OmpInverseUniformFormatTransform<LogRaw>,
                      public LogBase<formats::ArrayFormatF> {
 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

  void Do(bool simd, const float* input, int length,
          float* output) const noexcept;
};

class LogSingle : public Log<formats::SingleFormatF> {
 protected:
  virtual void Do(const float& in, float* out) const noexcept override;
};

class LogSingleInverse : public OmpInverseUniformFormatTransform<LogSingle>,
                         public LogBase<formats::SingleFormatF> {
 protected:
  virtual void Do(const float& in, float* out) const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORMS_LOG_H_
