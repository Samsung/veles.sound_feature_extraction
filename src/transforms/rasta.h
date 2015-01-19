/*! @file rasta.h
 *  @brief RASTA IIR filtering transform definition.
 *  recursion.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2015 Samsung R&D Institute Russia
 *
 *  @section References
 *  http://labrosa.ee.columbia.edu/~dpwe/papers/HermM94-rasta.pdf
 *  http://labrosa.ee.columbia.edu/matlab/rastamat/rastafilt.m
 */

#ifndef SRC_TRANSFORMS_RASTA_H_
#define SRC_TRANSFORMS_RASTA_H_

#include "src/transforms/iir_filter_base.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief Discards the frequencies which are lower than the threshold.
class RASTA : public IIRFilterBase, public TransformLogger<RASTA> {
 public:
  RASTA() noexcept;

  TRANSFORM_INTRO("RASTA", "Perform RASTA filtering.", RASTA)

  TP(pole, float, kDefaultPole, "Pole value. It normally lies within (1, 0.9].")

  static constexpr float kDefaultPole = 0.94;

 protected:
  virtual std::shared_ptr<IIRFilter> CreateExecutor() const noexcept override;
  virtual void Execute(const std::shared_ptr<IIRFilter>& exec, const float* in,
                       float* out) const override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_RASTA_H_
