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
#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"
#include "src/primitives/wavelet_types.h"
#include "src/primitives/wavelet_filter_bank.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

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
class DWPT
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  DWPT();

  TRANSFORM_INTRO("DWPT", "Discrete Wavelet Packet Transform")

  TRANSFORM_PARAMETERS(
    TP("tree", "The wavelet packet binary tree fingerprint.",
       Primitives::WaveletFilterBank::DescriptionToString(
           kDefaultTreeFingerprint))
    TP("type", "The type of the wavelet to apply. Supported values are "
               "daub (Daubechies), coif (Coiflet) and sym (Symlet).",
       kDefaultWaveletType)
    TP("order", "The number of coefficients in the wavelet.",
        std::to_string(kDefaultWaveletOrder))
  )

  virtual void Initialize() const noexcept;

  virtual bool HasInverse() const noexcept;

 protected:
  virtual void OnFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept;

 private:
  static const std::vector<int> kDefaultTreeFingerprint;
  static const std::string kDefaultWaveletType;
  static const WaveletType kDefaultWaveletTypeEnum;
  static const int kDefaultWaveletOrder;

  std::vector<int> treeFingerprint_;
  WaveletType waveletType_;
  int waveletOrder_;
  mutable std::unique_ptr<Primitives::WaveletFilterBank> filterBank_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_DWPT_H_
