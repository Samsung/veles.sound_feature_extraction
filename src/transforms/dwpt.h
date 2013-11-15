/*! @file dwpt.h
 *  @brief Discrete Wavelet Packet Transform (see http://en.wikipedia.org/wiki/Wavelet_packet_decomposition).
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_DWPT_H_
#define SRC_TRANSFORMS_DWPT_H_

#include <vector>
#include <simd/wavelet_types.h>
#include "src/transforms/common.h"
#include "src/primitives/wavelet_filter_bank.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief Discrete Wavelet Packet Transform.
/// @details Here is an example of the "tree" parameter:
///
///             ------ 3
///             |
///          ---|
///          |  |
///          |  ------ 3
///       ---|
///       |  |  ------ 3
///       |  |  |
///       |  ---|
///       |     |  --- 4
///     --|     ---|               "3, 3, 3, 4, 4, 2, 4, 4, 3"
///       |        --- 4
///       |  --------- 2
///       |  |
///       |  |
///       ---|     --- 4
///          |  ---|
///          |  |  --- 4
///          ---|
///             |
///             ------ 3
///
class DWPT : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  DWPT();

  TRANSFORM_INTRO("DWPT", "Discrete Wavelet Packet Transform", DWPT)

  TP(tree, TreeFingerprint, kDefaultTreeFingerprint(),
     "The wavelet packet binary tree fingerprint.")
  TP(type, WaveletType, kDefaultWaveletType,
     "The type of wavelet to apply. Supported values are "
     "daub (Daubechies), coif (Coiflet) and sym (Symlet).")
  TP(order, int, kDefaultWaveletOrder,
     "The number of coefficients in the wavelet.")

  virtual void Initialize() const override;

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

  static TreeFingerprint kDefaultTreeFingerprint() noexcept {
    return TreeFingerprint {
      3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      6, 6, 6, 6, 6, 6, 6, 6
    };
  };
  static constexpr WaveletType kDefaultWaveletType = WAVELET_TYPE_DAUBECHIES;
  static constexpr int kDefaultWaveletOrder = 8;

 private:
  mutable std::unique_ptr<primitives::WaveletFilterBank> filter_bank_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORMS_DWPT_H_
