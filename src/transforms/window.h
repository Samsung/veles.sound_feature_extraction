/*! @file window.h
 *  @brief Windowing transform.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_WINDOW_H_
#define SRC_TRANSFORMS_WINDOW_H_

#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"
#include "src/transform_base.h"
#include "src/primitives/window.h"

namespace SoundFeatureExtraction {
namespace Transforms {

template <class T>
class WindowSplitterTemplate;

//// @brief Applies a window function to each window.
class Window
    : public UniformFormatTransform<Formats::WindowFormatF> {
  template <class T> friend class WindowSplitterTemplate;
  friend class WindowSplitter16;
  friend class WindowSplitterF;
 public:
  Window();

  TRANSFORM_INTRO("Window", "Applies a window function to each window.")

  TRANSFORM_PARAMETERS(
      TP("type", "Type of the window. E.g. \"rectangular\" "
                 "or \"hamming\".",
         kDefaultType)
      TP("predft", "Apply Discrete Fourier Transform to window function.",
         std::to_string(kDefaultPreDft))
  )

  virtual void Initialize() const noexcept override;

 protected:
  typedef std::unique_ptr<float, void(*)(void*)> WindowContentsPtr;

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept override;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF> *out) const noexcept override;

  mutable WindowContentsPtr window_;

  static void ApplyWindow(bool simd, const float* window, int length,
                          const float* input, float* output) noexcept;

 private:
  static const std::string kDefaultType;
  static const WindowType kDefaultTypeEnum;
  static const bool kDefaultPreDft;

  WindowType type_;
  bool preDft_;

  static WindowContentsPtr InitializeWindow(size_t length,
                                            WindowType type,
                                            int allocSize = -1) noexcept;
};

/// @brief Splits the raw stream into numerous small chunks aka windows.
template <class T>
class WindowSplitterTemplate
    : public TransformBase<Formats::RawFormat<T>,
                           Formats::WindowFormat<T>,
                           true>,
      public TransformLogger<WindowSplitterTemplate<T>> {
 public:
  WindowSplitterTemplate()
      : window_(nullptr, free),
        length_(kDefaultLength),
        step_(kDefaultStep),
        type_(WINDOW_TYPE_HAMMING),
        windowsCount_(0) {
    this->RegisterSetter("length", [&](const std::string& value) {
      int pv = this->template Parse<int>("length", value);
      if (pv < Formats::WindowFormatF::MIN_WINDOW_SAMPLES ||
          pv > Formats::WindowFormatF::MAX_WINDOW_SAMPLES) {
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
  )

  virtual void Initialize() const noexcept {
    int realSize = this->inputFormat_->Size() - this->outputFormat_->Size();
    windowsCount_ = realSize / step_ + 1;
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

 protected:
  virtual void OnInputFormatChanged() override final {
    // Allocate 2 extra samples to use zero-copy FFT
    this->outputFormat_->SetAllocatedSize(length_ + 2);
    this->outputFormat_->SetSize(length_);
    this->outputFormat_->SetParentRawSize(this->inputFormat_->Size());
  }

  virtual void OnOutputFormatChanged() override final {
    this->inputFormat_->SetSize(this->outputFormat_->ParentRawSize());
  }

  virtual void InitializeBuffers(const BuffersBase<Formats::Raw<T>>& in,
        BuffersBase<Formats::Window<T>>* buffers)
      const noexcept override final {
    buffers->Initialize(in.Size() * windowsCount_,
                        this->outputFormat_->AllocatedSize());
  }

  virtual void InitializeBuffers(const BuffersBase<Formats::Window<T>>& in,
        BuffersBase<Formats::Raw<T>>* buffers)
      const noexcept override final {
    buffers->Initialize(in.Size() / windowsCount_,
                        this->inputFormat_->Size(), 0);
  }

  virtual void Do(const BuffersBase<Formats::Window<T>>& in,
                  BuffersBase<Formats::Raw<T>> *out) const noexcept {
    int windowLength = this->outputFormat_->Size();
    int offset = (windowLength - step_) / 2;
    int rawIndex = 0;
    size_t skippedEndingSize = this->inputFormat_->Size() -
        step_ * (windowsCount_ - 1) - this->outputFormat_->Size();
    for (size_t i = 0; i < in.Size(); i++) {
      int windowIndex = i % windowsCount_;
      if (windowIndex == 0) {
        memcpy((*out)[rawIndex].Data.get(), in[i].Data.get(),
               (windowLength - offset) * sizeof(T));
      } else if (windowIndex < windowsCount_ - 1) {
        memcpy((*out)[rawIndex].Data.get() +
                   windowLength - offset + step_ * (windowIndex - 1),
               in[i].Data.get() + offset, step_ * sizeof(T));
      } else {
        memcpy((*out)[rawIndex].Data.get() +
                   windowLength - offset + step_ * (windowIndex - 1),
               in[i].Data.get() + offset,
               (windowLength - offset) * sizeof(T));
        memset((*out)[rawIndex].Data.get() +
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

 private:
  static const int kDefaultLength =
      Formats::WindowFormatF::DEFAULT_WINDOW_SAMPLES;
  static const int kDefaultStep =
      Formats::WindowFormatF::DEFAULT_WINDOW_STEP;
};

class WindowSplitter16 : public WindowSplitterTemplate<int16_t> {
 protected:
  virtual void Do(const BuffersBase<Formats::Raw16>& in,
                  BuffersBase<Formats::Window16> *out) const noexcept override;
};

class WindowSplitterF : public WindowSplitterTemplate<float> {
 protected:
  virtual void Do(const BuffersBase<Formats::RawF>& in,
                  BuffersBase<Formats::WindowF> *out) const noexcept override;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_WINDOW_H_
