/*! @file transform_tree.cc
 *  @brief Tests for TransformTree class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <gtest/gtest.h>
#include "src/transform_tree.h"
#include "src/transform_registry.h"

using namespace SpeechFeatureExtraction;
using namespace SpeechFeatureExtraction::Formats;

class ParentTestFormat : public BufferFormat {
 public:
  ParentTestFormat()
  : BufferFormat("ParentTestFormat")
  , samplingRate_(16000)
  , samplesCount_(1024) {
  }

  ParentTestFormat(int samplingRate, int samplesCount)
  : BufferFormat("ParentTestFormat")
  , samplingRate_(samplingRate)
  , samplesCount_(samplesCount) {
  }

 protected:
  virtual bool EqualsTo(const BufferFormat& other) const noexcept {
    CAST_FORMAT(other, ParentTestFormat, optf);
    return samplingRate_ == optf.samplingRate_ &&
        samplesCount_ == optf.samplesCount_;
  }

  virtual void SetParametersFrom(const BufferFormat& other) noexcept {
    CAST_FORMAT(other, ParentTestFormat, optf);
    samplingRate_ = optf.samplingRate_;
    samplesCount_ = optf.samplesCount_;
  }

 private:
  int samplingRate_;
  int samplesCount_;
};

class ParentTestTransform : public Transform {
 public:
  ParentTestTransform() {
    params_.insert(std::make_pair("AmplifyFactor", "1"));
  }

  virtual const std::string& Name() const noexcept {
    static const std::string name("ParentTest");
    return name;
  }

  virtual BufferFormat* InputFormat() noexcept {
    return &inputFormat_;
  }

  virtual const BufferFormat& OutputFormat() const noexcept {
    return outputFormat_;
  }

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept {
    static const std::unordered_map<std::string, ParameterTraits> sp = {
        { "AmplifyFactor", { "Volume amplification factor", "1" } }
    };
    return sp;
  }

  virtual const std::unordered_map<std::string, std::string>&
  CurrentParameters() const noexcept {
    return params_;
  }

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>& parameters)
  throw(InvalidParameterNameException, InvalidParameterValueException) {
    for (auto p : parameters) {
      auto pvp = params_.find(p.first);
      if (pvp == params_.end()) {
        throw new InvalidParameterNameException(p.first, Name());
      }
      params_[pvp->second] = p.second;
    }
  }

  virtual void Initialize() const noexcept {

  }

  virtual void Do(const Buffers& in, Buffers *out) const noexcept {

  }

 private:
  RawFormat inputFormat_;
  ParentTestFormat outputFormat_;
  std::unordered_map<std::string, std::string> params_;
};

REGISTER_TRANSFORM(ParentTestTransform);

TEST(TransformTree, AddChain) {

}

#include "tests/google/src/gtest_main.cc"
