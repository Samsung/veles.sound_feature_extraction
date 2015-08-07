/*! @file singles_to_array.h
 *  @brief Merge all single-s to one solid array.
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

#ifndef SRC_TRANSFORMS_SINGLES_TO_ARRAY_H_
#define SRC_TRANSFORMS_SINGLES_TO_ARRAY_H_

#include "src/transform_base.h"
#include "src/formats/single_format.h"
#include "src/formats/array_format.h"

namespace sound_feature_extraction {
namespace transforms {

template <class T>
class SinglesToArray : public TransformBase<formats::SingleFormat<T>,
                                         formats::ArrayFormat<T>> {
 public:
  TRANSFORM_INTRO("Merge", "Merge all single-s to one solid array.",
                  SinglesToArray)

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override final {
    this->output_format_->SetSize(buffersCount);
    return 1;
  }

  virtual void Do(const BuffersBase<T>& in,
                  BuffersBase<T*>* out) const noexcept override {
    for (size_t i = 0; i < in.Count(); i++) {
      (*out)[0][i] = in[i];
    }
  }
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SINGLES_TO_ARRAY_H_
