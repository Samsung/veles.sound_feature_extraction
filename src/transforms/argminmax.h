/*! @file argminmax.h
 *  @brief Arithmetic and geometric argminmaxs calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_ARGMINMAX
#define SRC_TRANSFORMS_ARGMINMAX

#include <set>
#include "src/formats/window_format.h"
#include "src/formats/single_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

typedef std::tuple<int, float> ArgMinMaxResult;

class ArgMinMax
    : public OmpTransformBase<Formats::WindowFormatF,
                              Formats::SingleFormat<ArgMinMaxResult>> {
 public:
  ArgMinMax();

  TRANSFORM_INTRO("ArgMinMax", "Calculate argmin or argmax of the window.")

  OMP_TRANSFORM_PARAMETERS(
      TP("extremum", "Extremum type to find: min or max.",
         kDefaultExtremum? "min" : "max")
  )

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<ArgMinMaxResult>* buffers)
  const noexcept;

  virtual void Do(const Formats::WindowF& in,
                  ArgMinMaxResult *out)
  const noexcept;

  static ArgMinMaxResult Do(bool simd, const float* input, size_t length,
                            bool min) noexcept;

private:
  static const bool kDefaultExtremum;

  bool extremum_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_ARGMINMAX
