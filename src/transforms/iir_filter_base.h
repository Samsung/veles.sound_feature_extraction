/*! @file iir_filter_base.h
 *  @brief Base class for all IIR filters.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_IIR_FILTER_BASE_H_
#define SRC_TRANSFORMS_IIR_FILTER_BASE_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wsequence-point"
#include <DspFilters/Dsp.h>
#pragma GCC diagnostic pop
#include "src/transforms/filter_common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

enum IIRFilterType {
  kIIRFilterTypeBessel,
  kIIRFilterTypeButterworth,
  kIIRFilterTypeChebyshevI,
  kIIRFilterTypeChebyshevII,
  kIIRFilterTypeElliptic,
  kIIRFilterTypeLegendre
};

typedef Dsp::PoleFilterBase2 IIRFilter;

class IIRFilterBase : public FilterBase<IIRFilter> {
 public:
  IIRFilterBase() noexcept;

  IIRFilterType type() const;
  float ripple() const;
  float rolloff() const;

  static const std::unordered_map<std::string, IIRFilterType> kFilterTypeMap;

 protected:
  template <class F>
  void Execute(const std::shared_ptr<F>& exec, const float* in, float* out) const {
    memcpy(out, in, inputFormat_->UnalignedSizeInBytes());
    auto ptr = std::const_pointer_cast<F>(exec);
    ptr->reset();
    ptr->process(inputFormat_->Size(), &out);
  }

  static const IIRFilterType kDefaultIIRFilterType = kIIRFilterTypeChebyshevII;
  static constexpr float kDefaultIIRFilterRipple = 1;
  static constexpr float kDefaultIIRFilterRolloff = 0;

 private:
  IIRFilterType type_;
  float ripple_;
  float rolloff_;
};

#define IIR_FILTER_PARAMETERS(init) FILTER_PARAMETERS( \
  FORWARD_MACROS( \
      TP("type", "IIR filter type (bessel, butterworth, etc.).", \
         "butterworth") \
      TP("ripple", "Ripple level in dB (used by a subset of filter types).", \
         std::to_string(kDefaultIIRFilterRipple)) \
      TP("rolloff", "Rolloff level in dB (used by a subset of filter types).", \
         std::to_string(kDefaultIIRFilterRolloff)) \
      init) \
)

}  // namespace Formats
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_IIR_FILTER_BASE_H_
