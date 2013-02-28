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

#ifndef SRC_TRANSFORMS_SQUARE_H_
#define SRC_TRANSFORMS_SQUARE_H_

#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class SquareRaw
    : public TransformBase<Formats::RawFormat16, Formats::RawFormat32> {
 public:
  SquareRaw();

  TRANSFORM_INTRO("Square", "Squares the signal values.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void OnInputFormatChanged();

  virtual void TypeSafeInitializeBuffers(const BuffersBase<Formats::Raw16>& in,
        BuffersBase<Formats::Raw32>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::Raw16>& in,
                          BuffersBase<Formats::Raw32> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_SQUARE_H_
