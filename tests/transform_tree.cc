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

  virtual std::function<void(void*)> Destructor() const noexcept {
    return [](void*){};
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

  virtual std::function<void(void*)> Destructor() const noexcept {
    return [](void*){};
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

  tt.AddChain("One", { {"ParentTest", "" }, { "ChildTest", "" } });

  bool fail = false;
  try {
    tt.AddChain("One", { {"ParentTest", "" }, { "ChildTest", "" } });
  }
  catch (ChainNameAlreadyExistsException* e) {
    fail = true;
  }
  EXPECT_TRUE(fail);

  fail = false;
  try {
    tt.AddChain("Two", { {"ParentTest", "" }, { "ChildTest", "" } });
  }
  catch (ChainAlreadyExistsException* e) {
    fail = true;
  }
  EXPECT_TRUE(fail);

  tt.AddChain("Two", { {"ParentTest", "" }, { "ChildTest", "AnalysisLength=256" } });
  tt.PrepareForExecution();
}

#include "tests/google/src/gtest_main.cc"
