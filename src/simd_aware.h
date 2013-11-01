/*! @file simd_aware.h
 *  @brief All classes which support SIMD operations should inherit from this.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_SIMD_AWARE_H_
#define SRC_SIMD_AWARE_H_

void set_use_simd(int);

namespace sound_feature_extraction {

class SimdAware {
  friend void ::set_use_simd(int);
 public:
  static bool UseSimd() noexcept {
    return useSimd_;
  }

 protected:
  static void SetUseSimd(bool value) noexcept {
    useSimd_ = value;
  }

  static bool useSimd_;
};

}  // namespace sound_feature_extraction

#endif  // SRC_SIMD_AWARE_H_
