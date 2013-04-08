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

#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class Log
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  Log();

  TRANSFORM_INTRO("Log",
                  "Takes the logarithm on each real value of the signal.")

  TRANSFORM_PARAMETERS(
      TP("base", "Logarithm base (2, 10 or e).",
         LogBaseToString(kDefaultLogBase))
  )

  virtual bool HasInverse() const noexcept;

 protected:
  enum LogBase {
    LOG_BASE_2,
    LOG_BASE_10,
    LOG_BASE_E
  };

  static const std::unordered_map<std::string, LogBase> kLogBaseMap;
  static const LogBase kDefaultLogBase;

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers)
  const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept;

  void Do(bool simd, const float* input, int length,
          float* output) const noexcept;

  static std::string LogBaseToString(LogBase lb) noexcept;

  LogBase base_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_LOG_H_
