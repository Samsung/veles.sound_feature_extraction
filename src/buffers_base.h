/*! @file buffers_base.h
 *  @brief Base class for buffer formats which implements BufferFormat.
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

#ifndef SRC_BUFFERS_BASE_H_
#define SRC_BUFFERS_BASE_H_

#include <cassert>
#include <limits>
#include "src/config.h"
#include "src/buffer_format.h"
#include "src/buffers.h"
#include "src/demangle.h"
#include "src/logger.h"

namespace sound_feature_extraction {

class InvalidBuffersException : public ExceptionBase {
 public:
  InvalidBuffersException(const std::string& format,
                          size_t index, const std::string& value)
  : ExceptionBase("Buffers[" + std::to_string(index) +
                  "] is invalid (" + value + "). Format is " +
                  format + "."),
    index_(index) {
  }

  size_t index() const noexcept {
    return index_;
  }

 private:
  size_t index_;
};

template <typename T>
class BuffersBase;

template <typename T>
class BufferFormatBase : public BufferFormat {
 public:
  typedef T BufferType;

  BufferFormatBase()
      : BufferFormat(CutNamespaces(std::demangle(typeid(T).name()))) {
  }

  explicit BufferFormatBase(int samplingRate)
      : BufferFormat(CutNamespaces(std::demangle(typeid(T).name())),
                     samplingRate) {
  }

  virtual void Validate(const Buffers& buffers) const override final {
    if (*this != *buffers.Format()) {
      throw InvalidFormatException(Id(), buffers.Format()->Id());
    }
    Validate(reinterpret_cast<const BuffersBase<T>&>(buffers));
  }

  virtual std::string Dump(const Buffers& buffers, size_t index)
      const noexcept override final {
    if (*this != *buffers.Format()) {
      throw InvalidFormatException(Id(), buffers.Format()->Id());
    }
    return Dump(reinterpret_cast<const BuffersBase<T>&>(buffers), index);
  }

 protected:
  std::string CutNamespaces(std::string&& str) {
    return str.substr(str.find_last_of(':') + 1, std::string::npos);
  }

  virtual void Validate(const BuffersBase<T>& buffers) const = 0;

  virtual std::string Dump(const BuffersBase<T>& buffers, size_t index)
      const noexcept = 0;
};

namespace validation {
  template <class TE>
  struct Validator {
    static bool Validate(const TE&) noexcept {
      return true;
    }

    static bool Validate(TE&&) noexcept {
      return true;
    }
  };

  template <>
  struct Validator<float> {
    static bool Validate(float value) noexcept {
      return value == value &&
          value != std::numeric_limits<float>::infinity() &&
          value != -std::numeric_limits<float>::infinity();
    }
  };
}  // namespace validation

template <typename T>
class BuffersBase : public Buffers {
 public:
  typedef T ElementType;

  BuffersBase(const std::shared_ptr<BufferFormatBase<T>>& format,
              size_t count, void* reusedMemory = nullptr)
      : Buffers(format, count, reusedMemory) {
  }

  T& operator[](int index) noexcept {
    return *reinterpret_cast<T*>(Buffers::operator[](index));
  }

  const T& operator[](int index) const noexcept {
    return *reinterpret_cast<const T*>(Buffers::operator[](index));
  }

  const std::shared_ptr<BufferFormatBase<T>> Format() const noexcept {
    return std::static_pointer_cast<BufferFormatBase<T>>(Buffers::Format());
  }

 private:
  BuffersBase(const BuffersBase<T>&) = delete;
  BuffersBase& operator=(const BuffersBase<T>&) = delete;

  void RunConstructors() {
    if (!std::is_pointer<T>::value) {
      for (size_t i = 0; i < this->Count(); i++) {
        auto mem = reinterpret_cast<T*>(Buffers::operator[](i));
        new(mem) T();
      }
    }
  }
};

template <typename T>
class BuffersBase<T*> : public Buffers {
 public:
  typedef T* ElementType;

  BuffersBase(const std::shared_ptr<BufferFormatBase<T*>>& format,
              size_t count, void* reusedMemory = nullptr)
      : Buffers(format, count, reusedMemory) {
  }

  T* operator[](int index) noexcept {
    return reinterpret_cast<T*>(Buffers::operator[](index));
  }

  const T* operator[](int index) const noexcept {
    return reinterpret_cast<const T*>(Buffers::operator[](index));
  }

  const std::shared_ptr<BufferFormatBase<T*>> Format() const noexcept {
    return std::static_pointer_cast<BufferFormatBase<T*>>(Buffers::Format());
  }

 private:
  explicit BuffersBase(const BuffersBase<T*>&) = delete;
  BuffersBase& operator=(const BuffersBase<T*>&) = delete;
};

template<class T>
class FormatLogger : public Logger {
 public:
  FormatLogger() : Logger(std::demangle(typeid(T).name()), EINA_COLOR_ORANGE) {
  }
};

}  // namespace sound_feature_extraction

#endif  // SRC_BUFFERS_BASE_H_
