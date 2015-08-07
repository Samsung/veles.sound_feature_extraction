/*! @file buffers.h
 *  @brief Buffers class definition.
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

#ifndef SRC_BUFFERS_H_
#define SRC_BUFFERS_H_

#include <memory>
#include "src/buffer_format.h"

namespace sound_feature_extraction {

class Buffers {
 public:
  Buffers(const std::shared_ptr<BufferFormat>& format,
          size_t count = 0, void* reusedMemory = nullptr) noexcept;
  Buffers& operator=(const Buffers& other);

  virtual ~Buffers() {
  }

  const void* Data() const noexcept;
  size_t Count() const noexcept;
  size_t SizeInBytes() const noexcept;

  void* operator[](size_t index) noexcept;
  const void* operator[](size_t index) const noexcept;
  Buffers Slice(size_t index, size_t length) const;

  std::shared_ptr<BufferFormat> Format() const noexcept;

  void Validate() const;

  std::string Dump() const noexcept;
  std::string Dump(size_t index) const noexcept;
  std::string ToString() const noexcept;


 protected:
  void* Data() noexcept;

 private:
  std::shared_ptr<BufferFormat> format_;
  std::shared_ptr<void> buffers_;
  size_t count_;
};

/// @brief This exception is thrown when an attempt is made to assign "my"
/// buffers to "yours" of a bigger size.
class InsufficientAllocatedMemoryException : public ExceptionBase {
 public:
  explicit InsufficientAllocatedMemoryException(const Buffers& my,
                                                const Buffers& yours)
  : ExceptionBase("Attempted to assign " + my.ToString() + " to " +
                  yours.ToString() + ".") {
  }
};

/// @brief This exception is thrown when the user requests a slice which goes
/// beyond the Buffers' Count().
class InvalidSliceException : public ExceptionBase {
 public:
  explicit InvalidSliceException(size_t index, size_t length, size_t allCount)
  : ExceptionBase("Requested a slice [" + std::to_string(index) + ", " +
                  std::to_string(length - 1) + "] while the buffers count is " +
                  std::to_string(allCount) + ".") {
  }
};

}  // namespace sound_feature_extraction
#endif  // SRC_BUFFERS_H_
