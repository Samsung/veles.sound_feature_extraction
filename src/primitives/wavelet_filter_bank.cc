/*! @file wavelet_filter_bank.cc
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

#include "src/primitives/wavelet_filter_bank.h"
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <boost/regex.hpp>  // NOLINT(build/include_order)
#include "src/primitives/wavelet.h"

namespace SpeechFeatureExtraction {
namespace Primitives {

const std::unordered_map<std::string, WaveletType>
    WaveletFilterBank::kWaveletTypeStrMap = {
  { "daub", WAVELET_TYPE_DAUBECHIES },
  { "coif", WAVELET_TYPE_COIFLET },
  { "sym", WAVELET_TYPE_SYMLET }
};

WaveletFilterBank::WaveletFilterBank(WaveletType type, int order,
                                     const std::vector<int>& treeDescription)
    : type_(type),
      order_(order),
      tree_(treeDescription) {

  ValidateDescription(treeDescription);
}

WaveletFilterBank::WaveletFilterBank(WaveletType type, int order,
                                     std::vector<int>&& treeDescription)
    : type_(type),
      order_(order),
      tree_(treeDescription) {
  ValidateDescription(treeDescription);
}

void WaveletFilterBank::ValidateOrder(WaveletType type,
                                      int order) {
  if (!wavelet_validate_order(type, order)) {
    throw WaveletTreeInvalidOrderException(type, order);
  }
}

std::string WaveletFilterBank::WaveletTypeToString(WaveletType type) noexcept {
  for (auto wtstrp : kWaveletTypeStrMap) {
    if (wtstrp.second == type) {
      return wtstrp.first;
    }
  }
  return "";
}

WaveletType WaveletFilterBank::ParseWaveletType(const std::string& value) {
  auto it = kWaveletTypeStrMap.find(value);
  if (it == kWaveletTypeStrMap.end()) {
    throw WaveletTreeWaveletTypeParseException(value);
  }
  return it->second;
}

void WaveletFilterBank::ValidateDescription(
    const std::vector<int>& treeDescription) {
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

std::vector<int> WaveletFilterBank::ParseDescription(const std::string& str) {
  static const boost::regex allRegex("^\\s*(\\d+\\s*(\\s|$))+");
  static const boost::regex intRegex("(\\d+\\s*(\\s|$))");
  static const boost::sregex_token_iterator empty;

  boost::smatch match;
  if (!boost::regex_match(str, match, allRegex)) {
    throw WaveletTreeDescriptionParseException(str);
  }
  boost::sregex_token_iterator intIterator(
          str.begin(), str.end(), intRegex, 1);

  std::vector<int> res;
  while (intIterator != empty) {
    std::string intstr = *intIterator++;
    int val;
    try {
      val = std::stoi(intstr);
    }
    catch(const std::invalid_argument& e) {
      throw WaveletTreeDescriptionParseException(str);
    }
    res.push_back(val);
  }
  if (res.empty()) {
    throw WaveletTreeDescriptionParseException(str);
  }
  ValidateDescription(res);
  return std::move(res);
}

std::string WaveletFilterBank::DescriptionToString(
    const std::vector<int>& treeDescription) noexcept {
  std::string str;
  for (int i : treeDescription) {
    str += std::to_string(i) + " ";
  }
  if (str != "") {
    str.resize(str.size() - 1);
  }
  return std::move(str);
}

void WaveletFilterBank::ValidateLength(
    const std::vector<int>& tree, size_t length) {
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

  std::vector<int> tree(tree_);
  std::reverse(tree.begin(), tree.end());
  std::vector<int> workingTree;
  workingTree.reserve(tree.size());

  auto ldesthi = std::shared_ptr<float>(
      wavelet_allocate_destination(order_, length), free);
  auto ldestlo = std::shared_ptr<float>(
      wavelet_allocate_destination(order_, length), free);
  wavelet_apply(type_, order_, source, length,
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
    std::vector<int> *tree, std::vector<int>* workingTree, float* source,
    float* desthi, float* destlo, float** result) noexcept {
  if (tree->back() != workingTree->back()) {
    wavelet_apply(type, order, source, length, desthi, destlo);
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

}  // namespace Primitives
}  // namespace SpeechFeatureExtraction
