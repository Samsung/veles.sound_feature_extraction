/*! @file subband_energy.h
 *  @brief Calculate the subband energy.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SUBBAND_ENERGY_H_
#define SRC_TRANSFORMS_SUBBAND_ENERGY_H_

#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"
#include "src/primitives/wavelet_filter_bank.h"

namespace SoundFeatureExtraction {
namespace Transforms {

/// @brief Calculates the energy of the sub-signals for each subband.
/// @details Calculates
/// \f$\displaystyle \frac{\sum_{j\in S_i}{Value^2(j)}}{N_i}\f$, where
/// \f$S_i\f$ is subband's signal values set and \f$N_i\f$ is the corresponding
/// cardinal number. Thus, the output of this transform is a series of numbers,
/// each being equal to the corresponding subband energy. Numbers count is the
/// same as the length of the binary tree fingerprint.
class SubbandEnergy
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  SubbandEnergy();

  TRANSFORM_INTRO("SubbandEnergy",
                  "Calculates the subband energies (subbands are defined with"
                  "a binary tree fingerprint identical to used in DWPT).")

  TRANSFORM_PARAMETERS(
    TP("tree", "The subbands binary tree fingerprint.",
       Primitives::WaveletFilterBank::DescriptionToString(
           kDefaultTreeFingerprint))
  )

  virtual void Initialize() const noexcept;

 protected:
  virtual void OnFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers)
  const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept;

 private:
  static const std::vector<int> kDefaultTreeFingerprint;

  std::vector<int> treeFingerprint_;
  mutable std::vector<int> offsets_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_SUBBAND_ENERGY_H_
