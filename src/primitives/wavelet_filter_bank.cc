/*! @file wavelet_filter_bank.cc
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

#include "src/primitives/wavelet_filter_bank.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <list>
#include <memory>
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <boost/regex.hpp>  // NOLINT(build/include_order)
#pragma GCC diagnostic pop
#include <simd/wavelet.h>
#include "src/stoi_function.h"

namespace sound_feature_extraction {

TreeFingerprint Parse(const std::string& str, identity<TreeFingerprint>) {
  static const boost::regex all_regex("^\\s*(\\d+\\s*(\\s|$))+");
  static const boost::regex int_regex("(\\d+\\s*(\\s|$))");

  boost::smatch match;
  if (!boost::regex_match(str, match, all_regex)) {
    throw WaveletTreeDescriptionParseException(str);
  }

  TreeFingerprint res;
  try {
    std::transform(boost::sregex_token_iterator(str.begin(), str.end(),
                                                int_regex,
                                                1),
                   boost::sregex_token_iterator(),
                   std::back_inserter(res),
                   std::stoi_function);
  }
  catch(const std::invalid_argument& e) {
    throw WaveletTreeDescriptionParseException(str);
  }
  if (res.empty()) {
    throw WaveletTreeDescriptionParseException(str);
  }
  return res;
}

WaveletType Parse(const std::string& value, identity<WaveletType>) {
  static const std::unordered_map<std::string, WaveletType> map = {
    { WAVELET_TYPE_DAUBECHIES_STR, WAVELET_TYPE_DAUBECHIES },
    { WAVELET_TYPE_COIFLET_STR, WAVELET_TYPE_COIFLET },
    { WAVELET_TYPE_SYMLET_STR, WAVELET_TYPE_SYMLET }
  };
  auto it = map.find(value);
  if (it == map.end()) {
    throw WaveletTreeWaveletTypeParseException(value);
  }
  return it->second;
}

namespace primitives {

WaveletFilterBank::WaveletFilterBank(WaveletType type, int order,
                                     const TreeFingerprint& treeDescription)
    : type_(type),
      order_(order),
      tree_(treeDescription) {

  ValidateDescription(treeDescription);
}

WaveletFilterBank::WaveletFilterBank(WaveletType type, int order,
                                     TreeFingerprint&& treeDescription)
    : type_(type),
      order_(order),
      tree_(treeDescription) {
  ValidateDescription(treeDescription);
}

void WaveletFilterBank::ValidateWavelet(WaveletType type,
                                      int order) {
  if (!wavelet_validate_order(type, order)) {
    throw WaveletTreeInvalidOrderException(type, order);
  }
}

void WaveletFilterBank::ValidateDescription(
    const TreeFingerprint& treeDescription) {
  if (treeDescription.size() < 2) {
    throw WaveletTreeInvalidDescriptionException(treeDescription);
  }
  std::list<int> tree(treeDescription.begin(), treeDescription.end());
  while (!tree.empty()) {
    // Reduce the tree using a simple grammar:
    // (n) (n) -> (n - 1)
    bool reduced = false;
    for (auto it = tree.begin(); it != tree.end(); it++) {
       auto next = it;
       next++;
       if (next != tree.end() && *it == *next) {
         if (*it == 1 && tree.size() == 2) {
           tree.erase(it, tree.end());
         } else {
           *it = *it - 1;
           tree.erase(next);
         }
         reduced = true;
         break;
       }
    }
    if (!reduced) {
      throw WaveletTreeInvalidDescriptionException(treeDescription);
    }
  }
}

void WaveletFilterBank::ValidateLength(
    const TreeFingerprint& tree, size_t length) {
  int max = *std::max_element(tree.begin(), tree.end());
  // length / 2^max >= 1
  if (length == 0 || length % (1 << max) != 0) {
    throw WaveletTreeInvalidSourceLengthException(tree, length);
  }
}

void WaveletFilterBank::Apply(const float* source, size_t length,
                              float *result) noexcept {
  assert(source && result);
  ValidateLength(tree_, length);

  // Support zero-copy
  auto lsrc =
#ifndef __AVX__
      (source == result)? result :
#endif
      wavelet_prepare_array(order_, source, length);
  ApplyInternal(lsrc, length, result);
#ifndef __AVX__
  if (source != result) {
#endif
  free(lsrc);
#ifndef __AVX__
  }
#endif
}

void WaveletFilterBank::ApplyInternal(float* source, size_t length,
                                      float *result) noexcept {
  assert(source && result);
  ValidateLength(tree_, length);

  TreeFingerprint tree(tree_);
  std::reverse(tree.begin(), tree.end());
  TreeFingerprint workingTree;
  workingTree.reserve(tree.size());

  auto ldesthi = std::unique_ptr<float, decltype(&std::free)>(
      wavelet_allocate_destination(order_, length), free);
  auto ldestlo = std::unique_ptr<float, decltype(&std::free)>(
      wavelet_allocate_destination(order_, length), free);
  wavelet_apply(type_, order_, EXTENSION_TYPE_PERIODIC, source, length,
                ldesthi.get(), ldestlo.get());
  float *desthihi, *desthilo, *destlohi, *destlolo;
  wavelet_recycle_source(order_, source, length,
                         &desthihi, &desthilo,
                         &destlohi, &destlolo);
  workingTree.push_back(1);
  workingTree.push_back(1);

  while (!tree.empty()) {
    RecursivelyIterate(type_, order_, length / 2, &tree, &workingTree,
                       ldesthi.get(), desthihi, desthilo, &result);
    RecursivelyIterate(type_, order_, length / 2, &tree, &workingTree,
                       ldestlo.get(), destlohi, destlolo, &result);
  }
}

void WaveletFilterBank::RecursivelyIterate(
    WaveletType type, int order, size_t length,
    TreeFingerprint *tree, TreeFingerprint* workingTree, float* source,
    float* desthi, float* destlo, float** result) noexcept {
  if (tree->back() != workingTree->back()) {
    wavelet_apply(type, order, EXTENSION_TYPE_PERIODIC, source, length, desthi,
                  destlo);
    float *desthihi, *desthilo, *destlohi, *destlolo;
    wavelet_recycle_source(order, source, length,
                           &desthihi, &desthilo,
                           &destlohi, &destlolo);
    int next = workingTree->back() + 1;
    workingTree->pop_back();
    workingTree->push_back(next);
    workingTree->push_back(next);
    RecursivelyIterate(type, order, length / 2, tree, workingTree,
                       desthi, desthihi, desthilo, result);
    RecursivelyIterate(type, order, length / 2, tree, workingTree,
                       destlo, destlohi, destlolo, result);
  } else {
    memcpy(*result, source, length * sizeof(source[0]));
    *result += length;
    tree->pop_back();
    workingTree->pop_back();
  }
}

}  // namespace primitives
}  // namespace sound_feature_extraction
