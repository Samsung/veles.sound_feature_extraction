/*! @file format_converter.h
 *  @brief Interface of a converter between formats.
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

#ifndef SRC_FORMAT_CONVERTER_H_
#define SRC_FORMAT_CONVERTER_H_

#include "src/transform.h"

namespace sound_feature_extraction {

class FormatConverter : public virtual Transform {
 public:
  virtual const std::string& Name() const noexcept;

  virtual const std::string& Description() const noexcept;

  virtual const SupportedParametersMap&
  SupportedParameters() const noexcept;

  virtual const ParametersMap&
  GetParameters() const noexcept;

  virtual void SetParameters(
      const ParametersMap& parameters);

  virtual void Initialize() const override;

  static std::string Name(const BufferFormat& in,
                          const BufferFormat& out);

 private:
  mutable std::string name_;
  mutable std::string description_;
};

}  // namespace sound_feature_extraction


#endif  // SRC_FORMAT_CONVERTER_H_
