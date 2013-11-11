/*! @file identity.h
 *  @brief Just copies the input.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_IDENTITY_H_
#define SRC_TRANSFORMS_IDENTITY_H_

#include "src/transform.h"

namespace sound_feature_extraction {
namespace transforms {

class Identity : public Transform {
 public:
  Identity();

  static constexpr const char* kName = "Identity";

  virtual const std::string& Name() const noexcept;

  virtual const std::string& Description() const noexcept;

  virtual const std::shared_ptr<BufferFormat> InputFormat() const noexcept;

  virtual size_t SetInputFormat(const std::shared_ptr<BufferFormat>& format,
                                size_t buffersCount);

  virtual const std::shared_ptr<BufferFormat> OutputFormat()
      const noexcept;

  virtual void Initialize() const;

  virtual void Do(const Buffers& in, Buffers* out) const noexcept;

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      size_t count, void* reusedMemory = nullptr) const noexcept;

  virtual const SupportedParametersMap&
      SupportedParameters() const noexcept override final;

  virtual const ParametersMap& GetParameters()
      const noexcept;

  virtual void SetParameters(
      const ParametersMap& params);

 protected:
  std::shared_ptr<BufferFormat> input_format_;
  std::shared_ptr<BufferFormat> output_format_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_IDENTITY_H_
