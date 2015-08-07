/*! @file transform.h
 *  @brief Transform parent class declaration.
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

#ifndef SRC_TRANSFORM_H_
#define SRC_TRANSFORM_H_

#include "src/config.h"
#include "src/buffer_format.h"
#include "src/buffers.h"
#include "src/parameterizable.h"

namespace sound_feature_extraction {

/// @brief Abstract class representing a public interface of any transform.
class Transform : public virtual Parameterizable {
 public:
  virtual ~Transform() = default;

  virtual const std::string& Name() const noexcept = 0;

  virtual const std::string& Description() const noexcept = 0;

  virtual bool BufferInvariant() const noexcept;

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
