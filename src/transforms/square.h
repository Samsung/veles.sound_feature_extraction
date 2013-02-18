/*! @file square.h
 *  @brief Square the input signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SQUARE_H_
#define SQUARE_H_

#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class SquareRaw
    : public TransformBase<Formats::RawFormat16, Formats::RawFormat32> {
 public:
  SquareRaw();

  TRANSFORM_NAME("Square")

  TRANSFORM_PARAMETERS()

  virtual void Initialize() const noexcept;

 protected:
  virtual void OnInputFormatChanged();

  virtual void SetParameter(const std::string& name, const std::string& value);

  virtual void TypeSafeInitializeBuffers(const BuffersBase<Formats::Raw16>& in,
        BuffersBase<Formats::Raw32>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::Raw16>& in,
                          BuffersBase<Formats::Raw32> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_SQUARE_H_
