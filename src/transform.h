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

namespace SoundFeatureExtraction {

class BuffersCountChange {
 public:
  BuffersCountChange() noexcept;

  BuffersCountChange(int constant) noexcept;

  BuffersCountChange(int num, int den) noexcept;

  BuffersCountChange(const std::function<size_t(size_t)>& calc) noexcept;

  bool operator==(const BuffersCountChange& other) const noexcept;

  size_t operator()(size_t count) const noexcept;

  static const BuffersCountChange Identity;

 private:
  std::function<size_t(size_t)> calc_;
};

/// @brief Transform abstract class.
class Transform : public virtual Parameterizable {
 public:
  virtual ~Transform() {
 }

  /// @defgroup PureVirt Pure virtual interface methods
  /// @{
  virtual const std::string& Name() const noexcept = 0;

  virtual const std::string& Description() const noexcept = 0;

  virtual const std::shared_ptr<BufferFormat> InputFormat() const noexcept = 0;

  virtual const std::shared_ptr<BufferFormat> OutputFormat()
      const noexcept = 0;

  virtual void Initialize() const noexcept = 0;

  virtual void Do(const Buffers& in, Buffers* out) const noexcept = 0;

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      size_t count, void* reusedMemory = nullptr) const noexcept = 0;
  /// @}

  /// @defgroup NonVirt Non-virtual methods
  /// @{
  void UpdateInputFormat(const std::shared_ptr<BufferFormat>& format);

  BuffersCountChange CalculateBuffersCountChange() const noexcept;

  std::shared_ptr<Transform> Clone() const noexcept;

  std::string SafeName() const noexcept;

  std::string HtmlEscapedName() const noexcept;

  bool operator==(const Transform& other) const noexcept;
  /// @}

protected:
  virtual BuffersCountChange SetInputFormat(
      const std::shared_ptr<BufferFormat>& format) = 0;

 private:
  BuffersCountChange buffersCountChange_;
};

}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORM_H_
