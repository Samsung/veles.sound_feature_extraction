/*! @file beat.h
 *  @brief Beat detection.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_BEAT_H_
#define SRC_TRANSFORMS_BEAT_H_

#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Beat
    : public TransformBase<Formats::RawFormatF, Formats::SingleFormatF> {
 public:
  Beat();

  TRANSFORM_INTRO("Beat", "Find the tempo of a musical signal.")

  TRANSFORM_PARAMETERS(
      TP("bands", "The number of bands to sum.", "1"))

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Initialize() const noexcept override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float>* out) const noexcept override;

 private:
  static const float kInitialBeatsValue;
  static const float kDifference[4];
  static const float kStep[4];
  static const int kMaxFrequency;
  static const float kCoefficient;
  static const int kPulses;
  mutable std::unique_ptr<float, decltype(&std::free)> buffer_;
  int bands_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_BEAT_H_
