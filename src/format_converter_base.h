/*! @file format_converter_base.h
 *  @brief Base class of all converters between formats.
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

#ifndef SRC_FORMAT_CONVERTER_BASE_H_
#define SRC_FORMAT_CONVERTER_BASE_H_

#include "src/format_converter.h"
#include "src/omp_transform_base.h"

namespace sound_feature_extraction {

template <typename FIN, typename FOUT>
class FormatConverterBase
    : public virtual FormatConverter,
      public virtual OmpTransformBase<FIN, FOUT> {
 public:
  virtual size_t SetInputFormat(const std::shared_ptr<BufferFormat>& format,
                                size_t buffersCount)
      override final {
    return TransformBase<FIN, FOUT>::SetInputFormat(format, buffersCount);
  }

  virtual const SupportedParametersMap&
  SupportedParameters() const noexcept override final {
    return FormatConverter::SupportedParameters();
  }

  virtual const ParametersMap&
  GetParameters() const noexcept override final {
    return FormatConverter::GetParameters();
  }

  virtual void SetParameters(
      const ParametersMap&) override final {
  }

  virtual void Initialize() const override final {
  }

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) = 0;
};

}  // namespace sound_feature_extraction


#endif  // SRC_FORMAT_CONVERTER_BASE_H_
