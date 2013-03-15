/*! @file wavelet_filter_bank.h
 *  @brief Wavelet filter bank tree (mid level).
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_WAVELET_FILTER_BANK_H_
#define SRC_PRIMITIVES_WAVELET_FILTER_BANK_H_

#include <stddef.h>
#include <vector>
#include "src/exceptions.h"

namespace SpeechFeatureExtraction {
namespace Primitives {

class InvalidWaveletTreeDescriptionException : public ExceptionBase {
 public:
  explicit InvalidWaveletTreeDescriptionException(
      const std::vector<int>& treeDescription)
  : ExceptionBase("Wavelet tree description \"" +
                  DescriptionToString(treeDescription) + "\" is invalid.") {
  }

 protected:
  std::string DescriptionToString(const std::vector<int>& treeDescription) {
    std::string str;
    for (int i : treeDescription) {
      str += std::to_string(i) + " ";
    }
    if (str != "") {
      str.resize(str.size() - 1);
    }
    return std::move(str);
  }
};

class WaveletTreeDescriptionParseException : public ExceptionBase {
 public:
  explicit WaveletTreeDescriptionParseException(
      const std::string& treeDescription)
  : ExceptionBase("Failed to parse \"" + treeDescription + "\".") {
  }
};

class WaveletFilterBank {
 public:
  explicit WaveletFilterBank(const std::vector<int>& treeDescription);
  explicit WaveletFilterBank(std::vector<int>&& treeDescription);

  void Apply(const float* source, size_t length, float *result) noexcept;

  static void ValidateDescription(const std::vector<int>& treeDescription);
  static std::vector<int> ParseDescription(const std::string& str);

 private:
  std::vector<int> tree_;
  size_t minLength_;

  static void RecursivelyIterate(size_t length, std::vector<int>* tree,
                                 std::vector<int>* workingTree,
                                 float* source, float* desthi, float* destlo,
                                 float** result) noexcept;

  size_t CalculateMinLength() noexcept;
};

}  // namespace Primitives
}  // namespace SpeechFeatureExtraction
#endif  // SRC_PRIMITIVES_WAVELET_FILTER_BANK_H_
