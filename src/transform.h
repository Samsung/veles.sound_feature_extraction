/*! @file transform.h
 *  @brief Transform parent class declaration.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORM_H_
#define SRC_TRANSFORM_H_

#include "src/config.h"
#include "src/buffer_format.h"
#include "src/buffers.h"
#include "src/parameterizable.h"

namespace sound_feature_extraction {

/// @brief Transform abstract class.
class Transform : public virtual Parameterizable {
 public:
  virtual ~Transform() = default;

  virtual const std::string& Name() const noexcept = 0;

  virtual const std::string& Description() const noexcept = 0;

  virtual const std::shared_ptr<BufferFormat> InputFormat() const noexcept = 0;

  virtual size_t SetInputFormat(const std::shared_ptr<BufferFormat>& format,
                                size_t buffersCount) = 0;

  virtual const std::shared_ptr<BufferFormat> OutputFormat()
      const noexcept = 0;

  virtual void Initialize() const = 0;

  virtual void Do(const Buffers& in, Buffers* out) const noexcept = 0;

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      size_t count, void* reusedMemory = nullptr) const noexcept = 0;

  std::shared_ptr<Transform> Clone() const noexcept;

  std::string SafeName() const noexcept;

  std::string HtmlEscapedName() const noexcept;

  bool operator==(const Transform& other) const noexcept;
};

}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORM_H_
