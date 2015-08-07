/*! @file buffer_format.h
 *  @brief Buffer format interface class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#ifndef SRC_BUFFER_FORMAT_H_
#define SRC_BUFFER_FORMAT_H_

#include <functional>
#include <set>
#include "src/exceptions.h"

namespace sound_feature_extraction {

namespace formats {

class InvalidSamplingRateException : public ExceptionBase {
 public:
  explicit InvalidSamplingRateException(int samplingRate)
  : ExceptionBase("Sampling rate " + std::to_string(samplingRate) +
                  " is not supported or invalid.") {}
};

}  // namespace formats

class InvalidFormatException : public ExceptionBase {
 public:
  InvalidFormatException(const std::string& idMine,
                         const std::string& idYours)
  : ExceptionBase("Attempted to cast to format \"" + idMine +
                  "\" from format \"" + idYours + "\".") {}
};

class Buffers;

class BufferFormat {
 public:
  explicit BufferFormat(const std::string& id);
  BufferFormat(const std::string& id, int samplingRate);
  virtual ~BufferFormat() = default;

  bool operator==(const BufferFormat& other) const noexcept;
  bool operator!=(const BufferFormat& other) const noexcept;

  size_t SizeInBytes() const noexcept;
  virtual size_t UnalignedSizeInBytes() const noexcept = 0;

  int SamplingRate() const noexcept;
  void SetSamplingRate(int value);
  void CopySourceDetailsFrom(const BufferFormat& format) noexcept;

  virtual const std::string& Id() const noexcept;

  virtual void Validate(const Buffers& buffers) const = 0;

  virtual std::string Dump(const Buffers& buffers, size_t index) const = 0;
  virtual std::string ToString() const noexcept = 0;

  static void ValidateSamplingRate(int value);

  template <typename T>
  static T Aligned(T value) noexcept {
    return (value & (0x80 - 1)) == 0? value : (value & ~(0x80 - 1)) + 0x80;
  }

 protected:
  static constexpr const char* kIdentityID = "identity";

 private:
  static constexpr int kMinSamplingRate = 2000;
  static constexpr int kMaxSamplingRate = 48000;

  std::string id_;
  int samplingRate_;
};

class IdentityFormat : public BufferFormat {
 public:
  IdentityFormat();
  explicit IdentityFormat(int samplingRate);

  virtual size_t UnalignedSizeInBytes() const noexcept override final;

  virtual void Validate(const Buffers& buffers) const override final;

  virtual std::string Dump(const Buffers& buffers, size_t index)
      const override final;
  virtual std::string ToString() const noexcept override final;
};

}  // namespace sound_feature_extraction
#endif  // SRC_BUFFER_FORMAT_H_
