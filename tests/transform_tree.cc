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

using namespace sound_feature_extraction;  // NOLINT(*)
using namespace sound_feature_extraction::formats;  // NOLINT(*)

struct ParentChunk {
};

class ParentTestFormat : public BufferFormatBase<ParentChunk> {
 public:
  virtual void Validate(const BuffersBase<ParentChunk>&) const override {
  }

  virtual std::string Dump(const BuffersBase<ParentChunk>&, size_t)
      const noexcept override {
    return "";
  }

  virtual std::string ToString() const noexcept override {
    return Id();
  }

 protected:
  virtual size_t UnalignedSizeInBytes() const noexcept override {
    return 10;
  }
};

class ParentTestTransform
    : public TransformBase<ArrayFormat16, ParentTestFormat> {
 public:
  TRANSFORM_INTRO("ParentTest", "", ParentTestTransform)

  TP(AmplifyFactor, int, 1, "Volume amplification factor")

 protected:
  virtual void InitializeBuffers(const BuffersBase<int16_t*>&,
                                 BuffersBase<ParentChunk>*) const noexcept {
  }

  virtual void Do(const BuffersBase<int16_t*>&, BuffersBase<ParentChunk> *)
      const noexcept {
  }
};

ALWAYS_VALID_TP(ParentTestTransform, AmplifyFactor)
RTP(ParentTestTransform, AmplifyFactor)

struct ChildChunk {
};

class ChildTestFormat : public BufferFormatBase<ChildChunk> {
 public:
  virtual void Validate(const BuffersBase<ChildChunk>&) const override {
  }

  virtual std::string Dump(const BuffersBase<ChildChunk>&, size_t)
      const noexcept override {
    return "";
  }

  virtual std::string ToString() const noexcept override {
    return Id();
  }

 protected:
  virtual size_t UnalignedSizeInBytes() const noexcept override {
    return 10;
  }
};

class ChildTestTransform
    : public TransformBase<ParentTestFormat, ChildTestFormat> {
 public:
  TRANSFORM_INTRO("ChildTest", "", ChildTestTransform)

  TP(AnalysisLength, int, 128, "Length of the array with analyzed results")

 protected:
  virtual void InitializeBuffers(const BuffersBase<ParentChunk>&,
                                 BuffersBase<ChildChunk>*)
  const noexcept {
  }

  virtual void Do(const BuffersBase<ParentChunk>&,
                  BuffersBase<ChildChunk> *) const noexcept {
  }
};

ALWAYS_VALID_TP(ChildTestTransform, AnalysisLength)
RTP(ChildTestTransform, AnalysisLength)
REGISTER_TRANSFORM(ParentTestTransform);
REGISTER_TRANSFORM(ChildTestTransform);

class TransformTreeTest : public TransformTree, public testing::Test {
 public:
  TransformTreeTest() : TransformTree({ 4096, 20000 }) {
  }

  virtual ~TransformTreeTest() noexcept {
  }
};

TEST_F(TransformTreeTest, AddFeature) {
  AddFeature("One", { {"ParentTest", "" }, { "ChildTest", "" } });

  ASSERT_THROW({
    AddFeature("One", { {"ParentTest", "" }, { "ChildTest", "" } });
  }, ChainNameAlreadyExistsException);

  AddFeature("Two", { {"ParentTest", "" }, { "ChildTest", "" } });

  ASSERT_THROW({
    AddFeature("Two", { {"ParentTest", "" },
        { "ChildTest", "AnalysisLength=256" } });
  }, ChainNameAlreadyExistsException);
  PrepareForExecution();
}

TEST_F(TransformTreeTest, Dump) {
  AddFeature("One", { {"ParentTest", "AmplifyFactor=1" },
                    { "ChildTest", "" } });
  AddFeature("Two", { {"ParentTest", "AmplifyFactor=2" },
                    { "ChildTest", "AnalysisLength=256" } });
  Dump("/tmp/ttdump.dot");
}

#include "tests/google/src/gtest_main.cc"
