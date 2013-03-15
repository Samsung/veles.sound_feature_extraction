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

struct ParentChunk {
};

class ParentTestFormat : public BufferFormatBase<ParentChunk> {
 public:
  BufferFormat& operator=(const BufferFormat&) {
    return *this;
  }
};

class ParentTestTransform
    : public TransformBase<RawFormat16, ParentTestFormat> {
 public:
  ParentTestTransform() : TransformBase(SupportedParameters()) {}

  TRANSFORM_INTRO("ParentTest", "")

  TRANSFORM_PARAMETERS(_TP_("AmplifyFactor", "Volume amplification factor",
                            "1"))

 protected:
  virtual void SetParameter(const std::string&, const std::string&) {
  }

  virtual void TypeSafeInitializeBuffers(const BuffersBase<Raw16>&,
                                        BuffersBase<ParentChunk>*)
  const noexcept {
  }

  virtual void TypeSafeDo(const BuffersBase<Raw16>&, BuffersBase<ParentChunk> *)
  const noexcept {
  }
};

struct ChildChunk {
};

class ChildTestFormat : public BufferFormatBase<ChildChunk> {
 public:
  BufferFormat& operator=(const BufferFormat&) {
    return *this;
  }
};

class ChildTestTransform
    : public TransformBase<ParentTestFormat, ChildTestFormat> {
 public:
  ChildTestTransform() : TransformBase(SupportedParameters()) {}

  TRANSFORM_INTRO("ChildTest", "")

  TRANSFORM_PARAMETERS(
      _TP_("AnalysisLength", "Length of the array with analyzed results",
           "128"))

 protected:
  virtual void SetParameter(const std::string&, const std::string&) {
  }

  virtual void TypeSafeInitializeBuffers(const BuffersBase<ParentChunk>&,
                                         BuffersBase<ChildChunk>*)
  const noexcept {
  }

  virtual void TypeSafeDo(const BuffersBase<ParentChunk>&,
                          BuffersBase<ChildChunk> *) const noexcept {
  }
};

REGISTER_TRANSFORM(ParentTestTransform);
REGISTER_TRANSFORM(ChildTestTransform);

class TransformTreeTest : public TransformTree, public testing::Test {
 public:
  TransformTreeTest() : TransformTree({ 4096, 20000 }) {
  }

  virtual ~TransformTreeTest() noexcept {
  }
};

TEST_F(TransformTreeTest, AddChain) {
  ASSERT_NO_THROW(
      AddChain("One", { {"ParentTest", "" }, { "ChildTest", "" } }));

  ASSERT_THROW({
    AddChain("One", { {"ParentTest", "" }, { "ChildTest", "" } });
  }, ChainNameAlreadyExistsException);

  ASSERT_THROW({
    AddChain("Two", { {"ParentTest", "" }, { "ChildTest", "" } });
  }, ChainAlreadyExistsException);

  ASSERT_NO_THROW({
    AddChain("Two", { {"ParentTest", "" }, { "ChildTest",
                                           "AnalysisLength=256" } });
    PrepareForExecution();
  });
}

TEST_F(TransformTreeTest, Dump) {
  ASSERT_NO_THROW({
    AddChain("One", { {"ParentTest", "AmplifyFactor=1" },
                      { "ChildTest", "" } });
    AddChain("Two", { {"ParentTest", "AmplifyFactor=2" },
                      { "ChildTest", "AnalysisLength=256" } });
    Dump("/tmp/ttdump.dot");
  });
}

#include "tests/google/src/gtest_main.cc"
