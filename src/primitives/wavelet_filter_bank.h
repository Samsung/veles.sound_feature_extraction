/*! @file wavelet_filter_bank.h
 *  @brief Wavelet filter bank tree (mid level).
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#ifndef SRC_PRIMITIVES_WAVELET_FILTER_BANK_H_
#define SRC_PRIMITIVES_WAVELET_FILTER_BANK_H_

#include <stddef.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <simd/wavelet_types.h>
#include "src/parameterizable_base.h"

namespace sound_feature_extraction {

typedef std::vector<int> TreeFingerprint;

constexpr const char* WAVELET_TYPE_DAUBECHIES_STR = "daub";
constexpr const char* WAVELET_TYPE_COIFLET_STR = "coif";
constexpr const char* WAVELET_TYPE_SYMLET_STR = "sym";

WaveletType Parse(const std::string& value, identity<WaveletType>);
TreeFingerprint Parse(const std::string& str, identity<TreeFingerprint>);

class WaveletTreeDescriptionParseException : public ExceptionBase {
 public:
  explicit WaveletTreeDescriptionParseException(
      const std::string& treeDescription)
  : ExceptionBase("Failed to parse \"" + treeDescription + "\".") {
  }
};

class WaveletTreeWaveletTypeParseException : public ExceptionBase {
 public:
  explicit WaveletTreeWaveletTypeParseException(
      const std::string& value)
  : ExceptionBase("\"" + value + "\" is an unknown wavelet type.") {
  }
};

namespace primitives {

class WaveletFilterBank {
 public:
  explicit WaveletFilterBank(WaveletType type, int order,
                             const TreeFingerprint& treeDescription);
  explicit WaveletFilterBank(WaveletType type, int order,
                             TreeFingerprint&& treeDescription);

  void Apply(const float* source, size_t length, float *result) noexcept;

  static void ValidateWavelet(WaveletType type, int order);
  static void ValidateDescription(const TreeFingerprint& treeDescription);
  static void ValidateLength(const TreeFingerprint& tree, size_t length);

 private:
  WaveletType type_;
  int order_;
  TreeFingerprint tree_;

  static void RecursivelyIterate(WaveletType type, int order,
                                 size_t length, TreeFingerprint* tree,
                                 TreeFingerprint* workingTree,
                                 float* source, float* desthi, float* destlo,
                                 float** result) noexcept;

  void ApplyInternal(float* source, size_t length, float *result) noexcept;
};

}  // namespace primitives
}  // namespace sound_feature_extraction

namespace std {
  inline string to_string(WaveletType type) noexcept {
    switch (type) {
      case WAVELET_TYPE_DAUBECHIES:
        return sound_feature_extraction::WAVELET_TYPE_DAUBECHIES_STR;
      case WAVELET_TYPE_COIFLET:
        return sound_feature_extraction::WAVELET_TYPE_COIFLET_STR;
      case WAVELET_TYPE_SYMLET:
        return sound_feature_extraction::WAVELET_TYPE_SYMLET_STR;
    }
    return "";
  }

  inline string to_string(
      const sound_feature_extraction::TreeFingerprint& tf) noexcept {
    std::string str;
    for (int i : tf) {
      str += std::to_string(i) + " ";
    }
    if (!str.empty()) {
      str.resize(str.size() - 1);
    }
    return str;
  }
}  // namespace std

namespace sound_feature_extraction {
namespace primitives {

class WaveletTreeInvalidOrderException : public ExceptionBase {
 public:
  explicit WaveletTreeInvalidOrderException(
      WaveletType type, int order)
  : ExceptionBase("Order " + std::to_string(order) + "is invalid for "
                  "wavelet type \"" + std::to_string(type) + "\".") {
  }
};

class WaveletTreeInvalidDescriptionException : public ExceptionBase {
 public:
  explicit WaveletTreeInvalidDescriptionException(
      const TreeFingerprint& treeDescription)
  : ExceptionBase("Wavelet tree description \"" +
                  std::to_string(treeDescription) +
                  "\" is invalid.") {
  }
};

class WaveletTreeInvalidSourceLengthException : public ExceptionBase {
 public:
  explicit WaveletTreeInvalidSourceLengthException(
      const TreeFingerprint& treeDescription, size_t length)
  : ExceptionBase("Input length " + std::to_string(length) +
                  " is incompatible with tree \"" +
                  std::to_string(treeDescription) +
                  "\".") {
  }
};

}  // namespace primitives
}  // namespace sound_feature_extraction

#endif  // SRC_PRIMITIVES_WAVELET_FILTER_BANK_H_
