/*! @file window_splitter.h
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

#ifndef SRC_TRANSFORMS_WINDOW_SPLITTER_H_
#define SRC_TRANSFORMS_WINDOW_SPLITTER_H_

#include "src/transforms/window.h"

namespace SoundFeatureExtraction {
namespace Transforms {

/// @brief Splits the raw stream into numerous small chunks aka windows.
template <class T>
class WindowSplitterTemplate
    : public UniformFormatTransform<Formats::RawFormat<T>,
                                    true>,
      public TransformLogger<WindowSplitterTemplate<T>> {
 public:
  WindowSplitterTemplate()
      : window_(nullptr, free),
        length_(kDefaultLength),
        step_(kDefaultStep),
        type_(WINDOW_TYPE_HAMMING),
        windowsCount_(0),
        inverseCount_(kDefaultInverseCount) {
    this->RegisterSetter("length", [&](const std::string& value) {
      int pv = this->template Parse<int>("length", value);
      if (pv < Formats::RawFormatF::MIN_SAMPLES ||
          pv > kMaxWindowSamples) {
        return false;
      }
      length_ = pv;
      return true;
    });
    this->RegisterSetter("step", [&](const std::string& value) {
      int pv = this->template Parse<int>("step", value);
      if (pv < 1) {
        return false;
      }
      step_ = pv;
      return true;
    });
    this->RegisterSetter("type", [&](const std::string& value) {
      auto wti = kWindowTypeMap.find(value);
      if (wti == kWindowTypeMap.end()) {
        return false;
      }
      type_ = wti->second;
      return true;
    });
    this->RegisterSetter("inverse_count", [&](const std::string& value) {
      int pv = this->template Parse<int>("inverse_count", value);
      if (pv < 1) {
        return false;
      }
      inverseCount_ = pv;
      return true;
    });
  }

  TRANSFORM_INTRO("Window", "Splits the raw input signal into numerous "
                            "windows stepping \"step\" ms with length "
                            "\"length\" ms of type \"type\".")

  TRANSFORM_PARAMETERS(
      TP("length", "Window size in samples.", std::to_string(kDefaultLength))
      TP("step", "Distance between sequential windows in samples.",
         std::to_string(kDefaultStep))
      TP("type", "Type of the window. E.g. \"rectangular\" "
                 "or \"hamming\".",
         "hamming")
      TP("inverse_count", "The resulting count of buffers in inverse mode.",
         std::to_string(kDefaultInverseCount))
  )

  virtual void Initialize() const noexcept {
    if (!this->IsInverse()) {
      int realSize = this->inputFormat_->Size() - this->outputFormat_->Size();
      int excess = realSize % step_;
      if (excess != 0) {
        WRN("(input buffer size %zu - window length %zu) = %i is not "
            "divisible by step %i. It's excess (%i samples) will not be "
            "processed.",
            this->inputFormat_->Size(), this->outputFormat_->Size(),
            realSize, step_, excess);
      }

      window_ = Window::InitializeWindow(this->outputFormat_->Size(), type_);
    }
  }

 protected:
  virtual BuffersCountChange OnFormatChanged() override final {
    if (!this->IsInverse()) {
      this->outputFormat_->SetSize(length_);
    } else {
      this->inputFormat_->SetSize(this->outputFormat_->Size() / inverseCount_);
    }
    int realSize = this->inputFormat_->Size() - this->outputFormat_->Size();
    windowsCount_ = realSize / step_ + 1;
    if (!this->IsInverse()) {
      return BuffersCountChange(windowsCount_, 1);
    } else {
      return BuffersCountChange(inverseCount_);
    }
  }

  virtual void DoInverse(const BuffersBase<T*>& in, BuffersBase<T*>* out)
      const noexcept override {
    int windowLength = this->outputFormat_->Size();
    int offset = (windowLength - step_) / 2;
    int rawIndex = 0;
    size_t skippedEndingSize = this->inputFormat_->Size() -
        step_ * (windowsCount_ - 1) - this->outputFormat_->Size();
    for (size_t i = 0; i < in.Count(); i++) {
      int windowIndex = i % windowsCount_;
      if (windowIndex == 0) {
        memcpy((*out)[rawIndex], in[i],
               (windowLength - offset) * sizeof(T));
      } else if (windowIndex < windowsCount_ - 1) {
        memcpy((*out)[rawIndex] +
                   windowLength - offset + step_ * (windowIndex - 1),
               in[i] + offset, step_ * sizeof(T));
      } else {
        memcpy((*out)[rawIndex] +
                   windowLength - offset + step_ * (windowIndex - 1),
               in[i] + offset,
               (windowLength - offset) * sizeof(T));
        memset((*out)[rawIndex] +
                   this->inputFormat_->Size() - skippedEndingSize,
               0, skippedEndingSize * sizeof(T));
        rawIndex++;
      }
    }
  }

  mutable Window::WindowContentsPtr window_;
  int length_;
  int step_;
  WindowType type_;
  mutable int windowsCount_;
  int inverseCount_;

 private:
  static const int kDefaultLength = 512;
  static const int kDefaultStep = 205;
  static const int kMaxWindowSamples = 8096;
  static const int kDefaultInverseCount = 1;
};

class WindowSplitter16 : public WindowSplitterTemplate<int16_t> {
 protected:
  virtual void Do(const BuffersBase<int16_t*>& in,
                  BuffersBase<int16_t*> *out) const noexcept override;
};

class WindowSplitterF : public WindowSplitterTemplate<float> {
 protected:
  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*> *out) const noexcept override;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_WINDOW_SPLITTER_H_
