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
#include "src/transform_base.h"
#include "src/transform_tree.h"

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

class ParentTestTransform
    : public TransformBase<RawFormat, ParentTestFormat> {
 public:
  ParentTestTransform() : TransformBase(SupportedParameters()) {}

  TRANSFORM_NAME("ParentTest")

  TRANSFORM_PARAMETERS(_TP_("AmplifyFactor", "Volume amplification factor", "1"))

  virtual void Initialize() const noexcept {

  }

  virtual void Do(const Buffers& in, Buffers *out) const noexcept {

  }

 protected:
  virtual void SetParameter(const std::string& name,
                            const std::string& value){
    parameters_[name] = value;
  }
};

class ChildTestFormat : public BufferFormat {
 public:
  ChildTestFormat()
  : BufferFormat("ChildTestFormat")
  , analysisLength_(1024) {
  }

  ChildTestFormat(int analysisLength)
  : BufferFormat("ChildTestFormat")
  , analysisLength_(analysisLength) {
  }

 protected:
  virtual bool EqualsTo(const BufferFormat& other) const noexcept {
    CAST_FORMAT(other, ChildTestFormat, octf);
    return analysisLength_ == octf.analysisLength_;
  }

  virtual void SetParametersFrom(const BufferFormat& other) noexcept {
    CAST_FORMAT(other, ChildTestFormat, octf);
    analysisLength_ = octf.analysisLength_;
  }

 private:
  int analysisLength_;
};

class ChildTestTransform
    : public TransformBase<ParentTestFormat, ChildTestFormat> {
 public:
  ChildTestTransform() : TransformBase(SupportedParameters()) {}

  TRANSFORM_NAME("ChildTest")

  TRANSFORM_PARAMETERS(
      _TP_("AnalysisLength", "Length of the array with analyzed results",
           "128"))

  virtual void Initialize() const noexcept {

  }

  virtual void Do(const Buffers& in, Buffers *out) const noexcept {

  }

 protected:
  virtual void SetParameter(const std::string& name,
                            const std::string& value) {
    parameters_[name] = value;
  }
};

REGISTER_TRANSFORM(ParentTestTransform);
REGISTER_TRANSFORM(ChildTestTransform);

TEST(TransformTree, AddChain) {
  TransformTree tt({ 4096, 20000 });

  try {
    tt.AddChain("One", { {"ParentTest", "" }, { "ChildTest", "" } });
  }
  catch (...) {
    FAIL();
  }

  EXPECT_DEATH({
    tt.AddChain("One", { {"ParentTest", "" }, { "ChildTest", "" } });
  }, ".*SpeechFeatureExtraction::ChainNameAlreadyExistsException.*");

  EXPECT_DEATH({
    tt.AddChain("Two", { {"ParentTest", "" }, { "ChildTest", "" } });
  }, ".*SpeechFeatureExtraction::ChainAlreadyExistsException.*");

  try {
    tt.AddChain("Two", { {"ParentTest", "" }, { "ChildTest", "256" } });
    tt.PrepareForExecution();
  }
  catch (...) {
    FAIL();
  }
}

#include "tests/google/src/gtest_main.cc"
