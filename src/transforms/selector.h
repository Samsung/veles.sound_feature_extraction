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

#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Selector
    : public OmpUniformFormatTransform<Formats::ArrayFormatF> {
 public:
  Selector();

  TRANSFORM_INTRO("Selector", "Selects the specified part of input.")

  OMP_TRANSFORM_PARAMETERS(
      TP("length", "The number of values to pass through.",
         std::to_string(kDefaultLength))
      TP("from", "The anchor of the selection. Can be either """
                 "\"left\" or \"right\".",
         kDefaultAnchor == ANCHOR_LEFT? "left" : "right")
  )

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

 private:
  typedef enum {
    ANCHOR_LEFT,
    ANCHOR_RIGHT
  } Anchor;

  static const int kDefaultLength;
  static const Anchor kDefaultAnchor;

  size_t length_;
  Anchor from_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_SELECTOR_H_
