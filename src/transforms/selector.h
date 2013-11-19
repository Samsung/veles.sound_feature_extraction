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

namespace sound_feature_extraction {
namespace transforms {

enum Anchor {
  kAnchorLeft,
  kAnchorRight
};

namespace internal {
constexpr const char* kAnchorLeftStr = "left";
constexpr const char* kAnchorRightStr = "right";
}

Anchor Parse(const std::string value, identity<Anchor>);

class Selector : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Selector();

  TRANSFORM_INTRO("Selector", "Selects the specified part of input.",
                  Selector)

  TP(length, int, kDefaultLength, "The number of values to pass through.")
  TP(from, Anchor, kDefaultAnchor,
     "The anchor of the selection. Can be either \"left\" or \"right\".")

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

  static constexpr int kDefaultLength = 12;
  static constexpr Anchor kDefaultAnchor = kAnchorLeft;
};

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  inline string to_string(
      sound_feature_extraction::transforms::Anchor a) noexcept {
    return a == sound_feature_extraction::transforms::kAnchorLeft?
        sound_feature_extraction::transforms::internal::kAnchorLeftStr :
        sound_feature_extraction::transforms::internal::kAnchorRightStr;
  }
}  // namespace std

#endif  // SRC_TRANSFORMS_SELECTOR_H_
