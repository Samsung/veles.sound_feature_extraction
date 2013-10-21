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

#define WINDOW_SPLITTER_TRANSFORM_PARAMETERS(init) \
  TRANSFORM_PARAMETERS(FORWARD_MACROS( \
   TP("length", "Window size in samples. Ignored in inverse mode.", \
         std::to_string(this->kDefaultLength)) \
   TP("step", "Distance between sequential windows in samples.", \
      std::to_string(this->kDefaultStep)) \
   TP("type", "Type of the window. E.g. \"rectangular\" " \
              "or \"hamming\".", \
      "hamming") \
   TP("interleaved", "Interleave windows from sequential input buffers. " \
                     "In inverse mode, treat the windows as interleaved.", \
      "true") \
  init) \
)

template <class T>
class WindowSplitterTemplateBase
    : public virtual UniformFormatTransform<Formats::ArrayFormat<T>> {
 public:
  WindowSplitterTemplateBase()
      : length_(kDefaultLength),
        step_(kDefaultStep),
        windows_count_(0),
        interleaved_(true) {
    this->RegisterSetter("length", [&](const std::string& value) {
      int pv = this->template Parse<int>("length", value);
      if (pv < Formats::ArrayFormatF::kMinSamplesCount ||
          pv > Formats::ArrayFormatF::kMaxSamplesCount) {
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
    this->RegisterSetter("interleaved", [&](const std::string& value) {
      interleaved_ = this->template Parse<bool>("interleaved", value);
      return true;
    });
  }

 protected:
  static constexpr int kDefaultLength = 512;
  static constexpr int kDefaultStep = 205;

  int length_;
  int step_;
  mutable int windows_count_;
  bool interleaved_;
};

/// @brief Splits the raw stream into numerous small chunks aka windows.
template <class T>
class WindowSplitterTemplate
    : public WindowSplitterTemplateBase<T>,
      public TransformLogger<WindowSplitterTemplate<T>> {
 public:
  WindowSplitterTemplate()
      : window_(nullptr, free),
        type_(WINDOW_TYPE_HAMMING) {
    this->RegisterSetter("type", [&](const std::string& value) {
      auto wti = kWindowTypeMap.find(value);
      if (wti == kWindowTypeMap.end()) {
        return false;
      }
      type_ = wti->second;
      return true;
    });
  }

  TRANSFORM_INTRO("Window", "Splits the raw input signal into numerous "
                            "windows stepping \"step\" ms with length "
                            "\"length\" ms of type \"type\".")

  WINDOW_SPLITTER_TRANSFORM_PARAMETERS()

  virtual void Initialize() const {
    int realSize = this->inputFormat_->Size() - this->outputFormat_->Size();
    int excess = realSize % this->step_;
    if (excess != 0) {
      WRN("(input buffer size %zu - window length %zu) = %i is not "
          "divisible by step %i. It's excess (%i samples) will not be "
          "processed.",
          this->inputFormat_->Size(), this->outputFormat_->Size(),
          realSize, this->step_, excess);
    }

    window_ = Window::InitializeWindow(this->outputFormat_->Size(), type_);
  }

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override final {
    this->outputFormat_->SetSize(this->length_);
    int realSize = this->inputFormat_->Size() - this->outputFormat_->Size();
    this->windows_count_ = realSize / this->step_ + 1;
    return this->windows_count_ * buffersCount;
  }

  mutable Window::WindowContentsPtr window_;
  WindowType type_;
};

/// @brief Merges chunks of data.
template <class T>
class WindowSplitterInverseTemplate
    : public virtual WindowSplitterTemplateBase<T> {
 public:
  WindowSplitterInverseTemplate()
      : inverse_count_(kDefaultInverseCount) {
    this->RegisterSetter("count", [&](const std::string& value) {
      int pv = this->template Parse<int>("inverse_count", value);
      if (pv < 1) {
        return false;
      }
      inverse_count_ = pv;
      return true;
    });
  }

  WINDOW_SPLITTER_TRANSFORM_PARAMETERS(
      TP("count", "The resulting amount of buffers.",
         std::to_string(kDefaultInverseCount)))

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override final {
    this->windows_count_ = buffersCount / inverse_count_;
    this->outputFormat_->SetSize(this->inputFormat_->Size() +
        (this->windows_count_ - 1) * this->step_);
    return inverse_count_;
  }

  virtual void Do(const BuffersBase<T*>& in, BuffersBase<T*>* out)
      const noexcept override {
    int windowLength = this->inputFormat_->Size();
    int offset = (windowLength - this->step_) / 2;
    for (size_t i = 0; i < in.Count(); i++) {
      int outIndex, windowIndex;
      if (this->interleaved_) {
        outIndex = i % inverse_count_;
        windowIndex = (i / inverse_count_) % this->windows_count_;
      } else {
        outIndex = i / this->windows_count_;
        windowIndex = i % this->windows_count_;
      }
      if (windowIndex == 0) {
        memcpy((*out)[outIndex], in[i],
               (windowLength - offset) * sizeof(T));
      } else if (windowIndex < this->windows_count_ - 1) {
        memcpy((*out)[outIndex] +
                   windowLength - offset + this->step_ * (windowIndex - 1),
               in[i] + offset, this->step_ * sizeof(T));
      } else {
        memcpy((*out)[outIndex] +
                   windowLength - offset + this->step_ * (windowIndex - 1),
               in[i] + offset,
               (windowLength - offset) * sizeof(T));
      }
    }
  }

  static constexpr int kDefaultInverseCount = 1;

  int inverse_count_;
};

class WindowSplitter16 : public WindowSplitterTemplate<int16_t> {
 protected:
  virtual void Do(const BuffersBase<int16_t*>& in,
                  BuffersBase<int16_t*> *out) const noexcept override;
};

class WindowSplitter16Inverse
    : public WindowSplitterInverseTemplate<int16_t>,
      public virtual InverseUniformFormatTransform<WindowSplitter16> {
};

class WindowSplitterF : public WindowSplitterTemplate<float> {
 protected:
  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*> *out) const noexcept override;
};

class WindowSplitterFInverse
    : public WindowSplitterInverseTemplate<float>,
      public virtual InverseUniformFormatTransform<WindowSplitterF> {
};



}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_WINDOW_SPLITTER_H_
