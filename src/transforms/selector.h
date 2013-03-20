/*! @file selector.h
 *  @brief Select the specified part of input.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SELECTOR_H_
#define SRC_TRANSFORMS_SELECTOR_H_

#include <string>
#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class Selector
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  Selector();

  TRANSFORM_INTRO("Selector", "Selects the specified part of input.")

  TRANSFORM_PARAMETERS(
      TP("length", "The number of values to pass through.", "12")
      TP("from", "The anchor of the selection. Can be either """
                 "\"left\" or \"right\".",
         "right")
  )

 protected:
  virtual void OnFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept;

 private:
  typedef enum {
    ANCHOR_LEFT,
    ANCHOR_RIGHT
  } Anchor;

  int length_;
  Anchor from_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_SELECTOR_H_
