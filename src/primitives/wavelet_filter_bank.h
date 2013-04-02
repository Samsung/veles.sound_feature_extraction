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
#include <string>
#include <unordered_map>  // NOLINT(build/include_order)
#include "src/exceptions.h"
#include "src/primitives/wavelet_types.h"

namespace SpeechFeatureExtraction {
namespace Primitives {

class WaveletFilterBank {
 public:
  explicit WaveletFilterBank(WaveletType type, int order,
                             const std::vector<int>& treeDescription);
  explicit WaveletFilterBank(WaveletType type, int order,
                             std::vector<int>&& treeDescription);

  void Apply(const float* source, size_t length, float *result) noexcept;

  static void ValidateOrder(WaveletType type, int order);
  static std::string WaveletTypeToString(WaveletType type) noexcept;
  static WaveletType ParseWaveletType(const std::string& value);
  static void ValidateDescription(const std::vector<int>& treeDescription);
  static std::vector<int> ParseDescription(const std::string& str);
  static std::string DescriptionToString(
      const std::vector<int>& treeDescription) noexcept;
  static void ValidateLength(const std::vector<int>& tree, size_t length);

 private:
  WaveletType type_;
  int order_;
  std::vector<int> tree_;

  static const std::unordered_map<std::string, WaveletType> kWaveletTypeStrMap;

  static void RecursivelyIterate(WaveletType type, int order,
                                 size_t length, std::vector<int>* tree,
                                 std::vector<int>* workingTree,
                                 float* source, float* desthi, float* destlo,
                                 float** result) noexcept;

  void ApplyInternal(float* source, size_t length, float *result) noexcept;
};

class WaveletTreeInvalidOrderException : public ExceptionBase {
 public:
  explicit WaveletTreeInvalidOrderException(
      WaveletType type, int order)
  : ExceptionBase("Order " + std::to_string(order) + "is invalid for "
                  "wavelet type \"" +
                  WaveletFilterBank::WaveletTypeToString(type) + "\".") {
  }
};

class WaveletTreeWaveletTypeParseException : public ExceptionBase {
 public:
  explicit WaveletTreeWaveletTypeParseException(
      const std::string& value)
  : ExceptionBase("\"" + value + "\" is an unknown wavelet type.") {
  }
};

class WaveletTreeInvalidDescriptionException : public ExceptionBase {
 public:
  explicit WaveletTreeInvalidDescriptionException(
      const std::vector<int>& treeDescription)
  : ExceptionBase("Wavelet tree description \"" +
                  WaveletFilterBank::DescriptionToString(treeDescription) +
                  "\" is invalid.") {
  }
};

class WaveletTreeDescriptionParseException : public ExceptionBase {
 public:
  explicit WaveletTreeDescriptionParseException(
      const std::string& treeDescription)
  : ExceptionBase("Failed to parse \"" + treeDescription + "\".") {
  }
};

class WaveletTreeInvalidSourceLengthException : public ExceptionBase {
 public:
  explicit WaveletTreeInvalidSourceLengthException(
      const std::vector<int>& treeDescription, size_t length)
  : ExceptionBase("Input length " + std::to_string(length) +
                  " is incompatible with tree \"" +
                  WaveletFilterBank::DescriptionToString(treeDescription) +
                  "\".") {
  }
};

}  // namespace Primitives
}  // namespace SpeechFeatureExtraction
#endif  // SRC_PRIMITIVES_WAVELET_FILTER_BANK_H_
