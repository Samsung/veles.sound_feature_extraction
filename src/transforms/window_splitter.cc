/*! @file window_splitter.cc
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/window_splitter.h"
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Transforms {

void WindowSplitter16::Do(const BuffersBase<int16_t*>& in,
                          BuffersBase<int16_t*> *out)
const noexcept {
#ifdef __AVX__
  int16_t intbuf[outputFormat_->Size()] __attribute__ ((aligned (32)));  // NOLINT(*)
#endif
  float fbuf[outputFormat_->Size()] __attribute__ ((aligned (64)));  // NOLINT(*)
  float* window = window_.get();

  for (size_t i = 0; i < in.Count(); i++) {
    for (int j = 0; j < windows_count_; j++) {
      auto input = in[i] + j * step_;
      auto output = interleaved_? (*out)[i * windows_count_ + j] :
                                  (*out)[j * in.Count() + i];
      if (type_ != WINDOW_TYPE_RECTANGULAR) {
#ifdef __AVX__
        if (align_complement_i16(input) != 0) {
          memcpy(intbuf, input, outputFormat_->Size() * sizeof(int16_t));
          int16_to_float(intbuf, outputFormat_->Size(), fbuf);
        } else {
          int16_to_float(input, outputFormat_->Size(), fbuf);
        }
#else
        int16_to_float(input, outputFormat_->Size(), fbuf);
#endif
        Window::ApplyWindow(UseSimd(), window, outputFormat_->Size(), fbuf, fbuf);
        float_to_int16(fbuf, outputFormat_->Size(), output);
      } else {  // type_ != WINDOW_TYPE_RECTANGULAR
        memcpy(output, input, outputFormat_->Size() * sizeof(input[0]));
      }
    }
  }
}

void WindowSplitterF::Do(const BuffersBase<float*>& in,
                         BuffersBase<float*> *out)
const noexcept {
#ifdef __AVX__
  float intbuf[outputFormat_->Size()] __attribute__ ((aligned (32)));  // NOLINT(*)
#endif
  for (size_t i = 0; i < in.Count(); i++) {
    for (int j = 0; j < windows_count_; j++) {
      auto input = in[i] + j * step_;
      auto output = (*out)[i * windows_count_ + j];
      if (type_ != WINDOW_TYPE_RECTANGULAR) {
#ifdef __AVX__
        if (align_complement_f32(input) != 0) {
          memcpy(intbuf, input, outputFormat_->Size() * sizeof(float));
          Window::ApplyWindow(UseSimd(), window_.get(), outputFormat_->Size(),
                              intbuf, output);
        } else {
#endif
        Window::ApplyWindow(UseSimd(), window_.get(), outputFormat_->Size(),
                            input, output);
#ifdef __AVX__
        }
#endif
      } else {
        memcpy(output, input, outputFormat_->Size() * sizeof(input[0]));
      }
    }
  }
}

REGISTER_TRANSFORM(WindowSplitter16);
REGISTER_TRANSFORM(WindowSplitterF);
REGISTER_TRANSFORM(WindowSplitter16Inverse);
REGISTER_TRANSFORM(WindowSplitterFInverse);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
