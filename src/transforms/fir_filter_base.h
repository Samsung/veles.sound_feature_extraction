/*! @file fir_filter_base.h
 *  @brief Base class for all FIR filters.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_FIR_FILTER_BASE_H_
#define SRC_TRANSFORMS_FIR_FILTER_BASE_H_

#include "src/transforms/filter_common.h"

struct ConvolutionHandle;

namespace sound_feature_extraction {
namespace transforms {

class FIRFilterBase : public FilterBase<ConvolutionHandle> {
 public:
  virtual void Initialize() const override;

 protected:
  virtual void CalculateFilter(float* filter) const noexcept = 0;
  virtual size_t OnFormatChanged(size_t buffersCount) override;
  virtual std::shared_ptr<ConvolutionHandle> CreateExecutor()
      const noexcept override final;
  virtual void Execute(const std::shared_ptr<ConvolutionHandle>& exec,
                       const float* in, float* out) const override final;

 private:
  mutable std::vector<float> filter_;
};

}  // namespace formats
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_FIR_FILTER_BASE_H_
