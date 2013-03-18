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

#include <string>
#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {

namespace Primitives {

class WaveletFilterBank;

}

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
    _TP_("tree", "The wavelet packet binary tree fingerprint.",
         "3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, "
         "6, 6, 6, 6, 6, 6, 6, 6")
  )

  virtual void Initialize() const noexcept;

  virtual bool HasInverse() const noexcept;

 protected:
  virtual void SetParameter(const std::string& name, const std::string& value);

  virtual void OnFormatChanged();

  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::WindowF>& in,
                          BuffersBase<Formats::WindowF> *out) const noexcept;

 private:
  std::vector<int> treeFingerprint_;
  mutable std::shared_ptr<Primitives::WaveletFilterBank> filterBank_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_DWPT_H_
