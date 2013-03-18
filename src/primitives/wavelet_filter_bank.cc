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
#include <boost/regex.hpp>
#include "src/primitives/wavelet.h"

namespace SpeechFeatureExtraction {
namespace Primitives {

WaveletFilterBank::WaveletFilterBank(const std::vector<int>& treeDescription)
    : tree_(treeDescription) {
  ValidateDescription(treeDescription);
}

WaveletFilterBank::WaveletFilterBank(std::vector<int>&& treeDescription)
    : tree_(treeDescription) {
  ValidateDescription(treeDescription);
}

void WaveletFilterBank::ValidateLength(
    const std::vector<int>& tree, size_t length) {
  int max = *std::max_element(tree.begin(), tree.end());
  // length / 2^max >= 4, since the least wavelet dest* length is 8 / 2 = 4
  if ((int)length < (1 << (max + 2)) || (int)length % (1 << max) != 0) {
    throw WaveletTreeInvalidSourceLengthException(tree, length);
  }
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
    const std::vector<int>& treeDescription) {
  std::string str;
  for (int i : treeDescription) {
    str += std::to_string(i) + " ";
  }
  if (str != "") {
    str.resize(str.size() - 1);
  }
  return std::move(str);
}

void WaveletFilterBank::Apply(const float* source, size_t length,
                              float *result) noexcept {
  assert(source && result);
  ValidateLength(tree_, length);

  std::vector<int> tree(tree_);
  std::reverse(tree.begin(), tree.end());
  std::vector<int> workingTree;
  workingTree.reserve(tree.size());

  auto lsrc = std::shared_ptr<float>(
      wavelet_prepare_array(source, length), free);
  auto ldesthi = std::shared_ptr<float>(
      wavelet_allocate_destination(length), free);
  auto ldestlo = std::shared_ptr<float>(
      wavelet_allocate_destination(length), free);
  wavelet_apply(lsrc.get(), length, ldesthi.get(), ldestlo.get());
  float *desthihi, *desthilo, *destlohi, *destlolo;
  wavelet_recycle_source(lsrc.get(), length, &desthihi, &desthilo,
                         &destlohi, &destlolo);
  workingTree.push_back(1);
  workingTree.push_back(1);

  while (!tree.empty()) {
    RecursivelyIterate(length / 2, &tree, &workingTree, ldesthi.get(),
                       desthihi, desthilo, &result);
    RecursivelyIterate(length / 2, &tree, &workingTree, ldestlo.get(),
                       destlohi, destlolo, &result);
  }
}

void WaveletFilterBank::RecursivelyIterate(
    size_t length, std::vector<int> *tree, std::vector<int>* workingTree,
    float* source, float* desthi, float* destlo, float** result) noexcept {
  if (tree->back() != workingTree->back()) {
    if (length > 8) {
      wavelet_apply(source, length, desthi, destlo);
    } else {
      wavelet_apply_na(source, 8, 8, desthi, destlo);
    }
    float *desthihi, *desthilo, *destlohi, *destlolo;
    if (length >= 16) {
      wavelet_recycle_source(source, length, &desthihi, &desthilo,
                             &destlohi, &destlolo);
    } else {
      desthihi = desthilo = destlohi = destlolo = nullptr;
    }
    int next = workingTree->back() + 1;
    workingTree->pop_back();
    workingTree->push_back(next);
    workingTree->push_back(next);
    RecursivelyIterate(length / 2, tree, workingTree, desthi,
                       desthihi, desthilo, result);
    RecursivelyIterate(length / 2, tree, workingTree, destlo,
                       destlohi, destlolo, result);
  } else {
    memcpy(*result, source, length * sizeof(float));
    *result += length;
    tree->pop_back();
    workingTree->pop_back();
  }
}

}  // namespace Primitives
}  // namespace SpeechFeatureExtraction
