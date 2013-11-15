/*! @file convolve.h
 *  @brief Convolve a raw signal with a window function.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_CONVOLVE_H_
#define SRC_TRANSFORMS_CONVOLVE_H_

#include "src/transforms/fir_filter_base.h"
#include "src/primitives/window.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief Convolves a raw signal with a window function.
class ConvolveFilter : public FIRFilterBase {
 public:
  ConvolveFilter() noexcept;

  TRANSFORM_INTRO("Convolve", "Convolve a raw signal with a window function.",
                  ConvolveFilter)

  TP(window, WindowType, kDefaultWindowType,
     "Type of the window. E.g. \"rectangular\" or \"hamming\".")

 protected:
  static constexpr WindowType kDefaultWindowType = WINDOW_TYPE_RECTANGULAR;
  virtual void CalculateFilter(float* filter) const noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_CONVOLVE_H_
