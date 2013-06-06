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

#include <vector>
#include "src/omp_transform_base.h"
#include "src/formats/raw_format.h"
#include "src/primitives/window.h"

struct ConvoluteHandle;

namespace std {
  class mutex;
}

namespace SoundFeatureExtraction {
namespace Transforms {

class FirFilterBase
    : public OmpUniformFormatTransform<Formats::RawFormatF> {
 public:
  FirFilterBase() noexcept;

  virtual void Initialize() const noexcept;

 protected:
  int length_;

  virtual void CalculateFilter(float* filter) const noexcept = 0;

  virtual void OnFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::RawF>& in,
      BuffersBase<Formats::RawF>* buffers) const noexcept;

  virtual void Do(const Formats::RawF& in,
                  Formats::RawF *out) const noexcept;

  static const int MIN_FILTER_LENGTH = 16;
  static const int MAX_FILTER_LENGTH = 100000;
  static const int DEFAULT_FILTER_LENGTH = 150;

  static const int MIN_FILTER_FREQUENCY = 100;
  static const int MAX_FILTER_FREQUENCY = 24000;

  static const int DEFAULT_FILTER_HIGH_FREQUENCY = 8000;
  static const int DEFAULT_FILTER_LOW_FREQUENCY = 50;

 private:
  struct SyncHandle {
    std::shared_ptr<ConvoluteHandle> handle;
    std::shared_ptr<std::mutex> mutex;
  };

  WindowType windowType_;
  mutable std::vector<float> filter_;
  mutable std::vector<SyncHandle> convolutionHandles_;
};

#define FIR_FILTER_PARAMETERS(init) OMP_TRANSFORM_PARAMETERS( \
  FORWARD_MACROS( \
      TP("length", "Window length in samples", \
         std::to_string(DEFAULT_FILTER_LENGTH)) \
      TP("window", "Type of the window. E.g. " \
         "\"rectangular\" or \"hamming\".", \
         "hamming") \
      init) \
)

}  // namespace Formats
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_FIR_FILTER_BASE_H_
