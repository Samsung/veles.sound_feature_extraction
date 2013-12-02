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

namespace sound_feature_extraction {
namespace transforms {

template <class T>
class WindowSplitterTemplateBase
    : public virtual UniformFormatTransform<formats::ArrayFormat<T>> {
 public:
  typedef UniformFormatTransform<formats::ArrayFormat<T>> Base;

  WindowSplitterTemplateBase()
      : length_(kDefaultLength),
        step_(kDefaultStep),
        interleaved_(kDefaultInterleaved),
        windows_count_(0) {
  }

  TRANSFORM_PARAMETERS_SUPPORT(WindowSplitterTemplateBase<T>)

  TP(length, int, kDefaultLength, "Window size in samples.")
  TP(step, int, kDefaultStep, "Distance between sequential windows in samples.")
  TP(interleaved, bool, kDefaultInterleaved,
     "Interleave windows from sequential input buffers. "
     "In inverse mode, treat the windows as interleaved.")

 protected:
  static constexpr int kDefaultLength = 512;
  static constexpr int kDefaultStep = 205;
  static constexpr bool kDefaultInterleaved = true;

  mutable int windows_count_;
};

template <class T>
bool WindowSplitterTemplateBase<T>::validate_length(const int& value) noexcept {
  return value >= formats::ArrayFormatF::kMinSamplesCount &&
         value <= formats::ArrayFormatF::kMaxSamplesCount;
}

template <class T>
bool WindowSplitterTemplateBase<T>::validate_step(const int& value) noexcept {
  return value >= 1;
}

template <class T>
bool WindowSplitterTemplateBase<T>::validate_interleaved(
    const bool&) noexcept {
  return true;
}

template <class T>
RTP(WindowSplitterTemplateBase<T>, length)

template <class T>
RTP(WindowSplitterTemplateBase<T>, step)

template <class T>
RTP(WindowSplitterTemplateBase<T>, interleaved)

/// @brief Splits the raw stream into numerous small chunks aka windows.
template <class T>
class WindowSplitterTemplate
    : public WindowSplitterTemplateBase<T>,
      public TransformLogger<WindowSplitterTemplate<T>> {
 public:
  WindowSplitterTemplate()
      : type_(kDefaultWindowType),
        window_(nullptr, free) {
  }

  TRANSFORM_INTRO("Window", "Splits the raw input signal into numerous "
                            "windows stepping \"step\" ms with length "
                            "\"length\" ms of type \"type\".",
                  WindowSplitterTemplate<T>)

  TP(type, WindowType, kDefaultWindowType,
     "Type of the window. E.g. \"rectangular\" or \"hamming\".")

  virtual void Initialize() const {
    int realSize = this->input_format_->Size() - this->output_format_->Size();
    int excess = realSize % this->step();
    if (excess != 0) {
      WRN("(input buffer size %zu - window length %zu) = %i is not "
          "divisible by step %i. Its excess (%i samples) will not be "
          "processed.",
          this->input_format_->Size(), this->output_format_->Size(),
          realSize, this->step(), excess);
    }

    window_ = Window::InitializeWindow(this->output_format_->Size(), type_);
  }

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override final {
    this->output_format_->SetSize(this->length());
    int realSize = this->input_format_->Size() - this->output_format_->Size();
    this->windows_count_ = realSize / this->step()+ 1;
    return this->windows_count_ * buffersCount;
  }

  static constexpr WindowType kDefaultWindowType =
      WindowType::kWindowTypeHamming;

  mutable Window::WindowContentsPtr window_;
};

template <class T>
bool WindowSplitterTemplate<T>::validate_type(const WindowType&) noexcept {
  return true;
}

template <class T>
RTP(WindowSplitterTemplate<T>, type)

/// @brief Merges chunks of data.
template <class T>
class WindowSplitterInverseTemplate
    : public virtual WindowSplitterTemplateBase<T> {
 public:
  WindowSplitterInverseTemplate()
      : count_(kDefaultInverseCount) {
  }

  TRANSFORM_PARAMETERS_SUPPORT(WindowSplitterInverseTemplate<T>)

  TP(count, int, kDefaultInverseCount, "The resulting amount of buffers.")

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override final {
    this->windows_count_ = buffersCount / count_;
    this->output_format_->SetSize(this->input_format_->Size() +
        (this->windows_count_ - 1) * this->step());
    return count_;
  }

  virtual void Do(const BuffersBase<T*>& in, BuffersBase<T*>* out)
      const noexcept override {
    int windowLength = this->input_format_->Size();
    int offset = (windowLength - this->step()) / 2;
    for (size_t i = 0; i < in.Count(); i++) {
      int outIndex, windowIndex;
      if (this->interleaved()) {
        outIndex = i % count_;
        windowIndex = (i / count_) % this->windows_count_;
      } else {
        outIndex = i / this->windows_count_;
        windowIndex = i % this->windows_count_;
      }
      if (windowIndex == 0) {
        memcpy((*out)[outIndex], in[i],
               (windowLength - offset) * sizeof(T));
      } else if (windowIndex < this->windows_count_ - 1) {
        memcpy((*out)[outIndex] +
                   windowLength - offset + this->step()* (windowIndex - 1),
               in[i] + offset, this->step()* sizeof(T));
      } else {
        memcpy((*out)[outIndex] +
                   windowLength - offset + this->step() * (windowIndex - 1),
               in[i] + offset,
               (windowLength - offset) * sizeof(T));
      }
    }
  }

  static constexpr int kDefaultInverseCount = 1;
};

template <class T>
bool WindowSplitterInverseTemplate<T>::validate_count(
    const int& value) noexcept {
  return value >= 1;
}

template <class T>
RTP(WindowSplitterInverseTemplate<T>, count)

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

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_WINDOW_SPLITTER_H_
