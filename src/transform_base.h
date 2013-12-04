/*! @file transform_base.h
 *  @brief Transform template base class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORM_BASE_H_
#define SRC_TRANSFORM_BASE_H_

#include <assert.h>
#include <string>
#ifndef __clang__
#include <tr2/type_traits>
#endif
#include "src/buffers_base.h"
#include "src/logger.h"
#include "src/parameterizable_base.h"
#include "src/simd_aware.h"
#include "src/transform_registry.h"

namespace sound_feature_extraction {

template <typename F, typename S>
struct TypePair {
  typedef F First;
  typedef S Second;
};

template <typename FIN, typename FOUT>
class TransformBase : public virtual Transform,
                      public virtual SimdAware,
                      public virtual ParameterizableBase {
 public:
  typedef FIN InFormat;
  typedef FOUT OutFormat;
  typedef TypePair<TransformBase<FIN, FOUT>, std::nullptr_t> Family;

  TransformBase() noexcept
      : input_format_(std::make_shared<FIN>()),
        output_format_(std::make_shared<FOUT>()) {
  }

  virtual const std::shared_ptr<BufferFormat> InputFormat()
      const noexcept override final {
    return std::static_pointer_cast<BufferFormat>(input_format_);
  }

  virtual size_t SetInputFormat(const std::shared_ptr<BufferFormat>& format,
                                size_t buffersCount) override {
    input_format_ = std::static_pointer_cast<FIN>(format);
    output_format_->CopySourceDetailsFrom(*input_format_);
    return OnInputFormatChanged(buffersCount);
  }

  virtual const std::shared_ptr<BufferFormat> OutputFormat()
      const noexcept override final {
    return std::static_pointer_cast<BufferFormat>(output_format_);
  }

  /// @brief Empty stub. Implement your own, if necessary.
  virtual void Initialize() const override {
  }

  virtual void Do(const Buffers& in, Buffers* out)
      const noexcept override final {
    assert(&in != nullptr);  // yes, this may happen with shared_ptr-s
    assert(out != nullptr);
    assert(*in.Format() == *input_format_);
    assert(*out->Format() == *output_format_);
    this->Do(reinterpret_cast<const InBuffers&>(in),
             reinterpret_cast<OutBuffers*>(out));
  }

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      size_t count, void* reusedMemory = nullptr)
      const noexcept override final {
    return std::make_shared<OutBuffers>(output_format_, count, reusedMemory);
  }


 protected:
  std::shared_ptr<FIN> input_format_;
  std::shared_ptr<FOUT> output_format_;

  typedef typename FIN::BufferType InElement;
  typedef typename FOUT::BufferType OutElement;
  typedef BuffersBase<InElement> InBuffers;
  typedef BuffersBase<OutElement> OutBuffers;

  virtual size_t OnInputFormatChanged(size_t buffersCount) {
    return buffersCount;
  }

  virtual size_t OnOutputFormatChanged(size_t buffersCount) {
    return buffersCount;
  }

  virtual void Do(const InBuffers& in, OutBuffers* out) const noexcept = 0;

  virtual std::string HostName() const noexcept {
    return std::string("transform ") + Name();
  }

  virtual const SupportedParametersMap&
  SupportedParameters() const noexcept override {
    return *SupportedParametersPtr();
  }

  virtual const ParameterSettersMap&
  ParameterSetters() const noexcept override {
    return *ParameterSettersPtr();
  }

  static std::set<SupportedParametersMap*>* RegisterParameters(
      SupportedParametersMap* map = nullptr) noexcept {
    static std::set<SupportedParametersMap*> params({SupportedParametersPtr()});
    if (map != nullptr) {
      params.insert(map);
      map->insert(SupportedParametersPtr()->begin(),
                  SupportedParametersPtr()->end());
    }
    return &params;
  }

  static std::set<ParameterSettersMap*>* RegisterParameterSetters(
      ParameterSettersMap* map = nullptr) noexcept {
    static ParameterSettersMap own;
    static std::set<ParameterSettersMap*> params({&own});
    if (map != nullptr) {
      params.insert(map);
      map->insert(own.begin(), own.end());
    }
    return &params;
  }

  static SupportedParametersMap* SupportedParametersPtr() noexcept {
    static SupportedParametersMap own;
    return &own;
  }

  static ParameterSettersMap* ParameterSettersPtr() noexcept {
    static ParameterSettersMap own;
    return &own;
  }
};

/// @brief Common base of transforms which do not change the buffer format.
template <typename F>
class UniformFormatTransform : public virtual TransformBase<F, F> {
 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override final {
    this->output_format_ = std::make_shared<F>(*this->input_format_);
    return OnFormatChanged(buffersCount);
  }

  virtual size_t OnOutputFormatChanged(size_t buffersCount) override final {
    this->input_format_ = std::make_shared<F>(*this->output_format_);
    return OnFormatChanged(buffersCount);
  }

  virtual size_t OnFormatChanged(size_t buffersCount) {
    return buffersCount;
  }
};


template <typename T>
class InverseTransformBase
    : public virtual TransformBase<typename T::OutFormat,
                                   typename T::InFormat> {
 public:
  virtual const std::string& Name() const noexcept override final {
    static const std::string str = std::string("I") + T().Name();
    return str;
  }

  virtual const std::string& Description() const noexcept override final {
    static const std::string str = std::string("Inverse of \"") +
        static_cast<char>(std::tolower(T().Description()[0])) +
        T().Description().substr(1, T().Description().size() - 1) + "\".";
    return str;
  }
};

template <typename T>
class InverseUniformFormatTransform
    : public virtual InverseTransformBase<T>,
      public virtual UniformFormatTransform<typename T::InFormat> {
};

template<class T>
class TransformLogger : public Logger {
 public:
  TransformLogger() noexcept
      : Logger(RemoveRootNamespace(std::demangle(typeid(T).name())),
               EINA_COLOR_LIGHTBLUE) {
  }

 private:
  static std::string RemoveRootNamespace(std::string&& name) noexcept {
    const std::string root_ns = "sound_feature_extraction::";
    if (name.size() > root_ns.size() &&
        name.substr(0, root_ns.size()) == root_ns) {
      return name.substr(root_ns.size(), name.size() - root_ns.size());
    }
    return name;
  }
};

#define FORWARD_MACROS(...) __VA_ARGS__

// Because of http://gcc.gnu.org/bugzilla/show_bug.cgi?id=59383
// we cannot use the same method for both gcc and clang
#ifndef __clang__
#define DECLARE_PARENT_TYPE(self)                                              \
public:                                                                        \
  typedef self SelfType;                                                       \
  typedef typename std::tr2::direct_bases<self>::type::first::type ParentType;
#else
#define DECLARE_PARENT_TYPE(self)                                              \
public:                                                                        \
  typedef self SelfType;                                                       \
  typedef TypePair<self, typename self::Family::First> Family;                 \
  typedef typename Family::Second ParentType;
#endif

/// @brief Internal macros to implement service functions to support
/// parameters introspection.
#define TRANSFORM_PARAMETERS_SUPPORT(self)                                     \
  DECLARE_PARENT_TYPE(FORWARD_MACROS(self))                                    \
  static_assert(!std::is_same<self, ParentType>::value,                        \
                "http://gcc.gnu.org/bugzilla/show_bug.cgi?id=59383");          \
                                                                               \
protected:                                                                     \
  virtual const SupportedParametersMap&                                        \
  SupportedParameters() const noexcept override {                              \
    return SupportedParametersPtr()->empty()?                                  \
        *ParentType::SupportedParametersPtr() : *SupportedParametersPtr();     \
  }                                                                            \
                                                                               \
  virtual const ParameterSettersMap&                                           \
  ParameterSetters() const noexcept override {                                 \
    return ParameterSettersPtr()->empty()?                                     \
        *ParentType::ParameterSettersPtr() : *ParameterSettersPtr();           \
  }                                                                            \
                                                                               \
  static SupportedParametersMap* SupportedParametersPtr() noexcept {           \
    static SupportedParametersMap own;                                         \
    return &own;                                                               \
  }                                                                            \
                                                                               \
  static ParameterSettersMap* ParameterSettersPtr() noexcept {                 \
    static ParameterSettersMap own;                                            \
    return &own;                                                               \
  }                                                                            \
                                                                               \
  static std::set<SupportedParametersMap*>* RegisterParameters(                \
      SupportedParametersMap* map = nullptr) noexcept {                        \
    static std::set<SupportedParametersMap*> params(                           \
        { SupportedParametersPtr() });                                         \
    if (map != nullptr) {                                                      \
      params.insert(map);                                                      \
      map->insert(SupportedParametersPtr()->begin(),                           \
                  SupportedParametersPtr()->end());                            \
      ParentType::RegisterParameters(map);                                     \
    } else {                                                                   \
      ParentType::RegisterParameters(SupportedParametersPtr());                \
    }                                                                          \
    return &params;                                                            \
  }                                                                            \
                                                                               \
  static std::set<ParameterSettersMap*>* RegisterParameterSetters(             \
      ParameterSettersMap* map = nullptr) noexcept {                           \
    static std::set<ParameterSettersMap*> params({ ParameterSettersPtr() });   \
    if (map != nullptr) {                                                      \
      params.insert(map);                                                      \
      map->insert(ParameterSettersPtr()->begin(),                              \
                  ParameterSettersPtr()->end());                               \
      ParentType::RegisterParameterSetters(map);                               \
    } else {                                                                   \
      ParentType::RegisterParameterSetters(ParameterSettersPtr());             \
    }                                                                          \
    return &params;                                                            \
  }                                                                            \
                                                                               \
private:                                                                       \
  static void RegisterParameter(                                               \
      const std::string& pname, const std::string& desc,                       \
      const std::string& defv, ParameterSetter&& setter) {                     \
    for (auto ptr : *RegisterParameters()) {                                   \
      ptr->insert({ pname, { desc, defv } });                                  \
    }                                                                          \
     for (auto ptr : *RegisterParameterSetters()) {                            \
      ptr->insert({ pname, setter });                                          \
    }                                                                          \
  }                                                                            \
                                                                               \
 public:
/// @brief Adds Name() and Description() implementations, service functions.
/// @param name The name of the transform (as returned by Name()).
/// @param description The description of the transform (as returned
/// by Description()).
/// @param self The corresponding class type.
#define TRANSFORM_INTRO(name, description, self)                               \
 public:                                                                       \
  virtual const std::string& Name() const noexcept override {                  \
    static const std::string str(name);                                        \
    return str;                                                                \
  }                                                                            \
                                                                               \
  virtual const std::string& Description() const noexcept override {           \
    static const std::string str(description);                                 \
    return str;                                                                \
  }                                                                            \
                                                                               \
  TRANSFORM_PARAMETERS_SUPPORT(FORWARD_MACROS(self))

/// @brief Adds a new transform parameter.
/// @brief name The name of the parameter.
/// @brief type The parameter's type.
/// @brief defval The default value of the parameter. Only static constexpr or
/// constant values are allowed.
/// @brief descr The description of the parameter.
/// @note This macro demands the definition of
/// static bool validate_##name(const type& value) noexcept.
#define TP(name, type, defv, desc)                                             \
private:                                                                       \
  static bool validate_##name(const type& value) noexcept;                     \
  static void set_##name##_raw(void* pbthis, const std::string& value) {       \
    type parsed_value =                                                        \
        sound_feature_extraction::Parse<decltype(name##_)>(value);             \
    auto self = dynamic_cast<SelfType*>(reinterpret_cast<ParameterizableBase*>(\
        pbthis));                                                              \
    self->set_##name(parsed_value);                                            \
  }                                                                            \
                                                                               \
  class name##_registration_class {                                            \
  public:                                                                      \
    name##_registration_class() {                                              \
      SelfType::RegisterParameter(#name, desc, std::to_string(defv),           \
                                  &SelfType::set_##name##_raw);                \
    }                                                                          \
    void Ref() const {}                                                        \
  };                                                                           \
  friend class name##_registration_class;                                      \
                                                                               \
  static const name##_registration_class name##_registration_;                 \
  type name##_;                                                                \
                                                                               \
public:                                                                        \
  __attribute__((used)) type name() const noexcept {                           \
    name##_registration_.Ref();                                                \
    return name##_;                                                            \
  }                                                                            \
                                                                               \
  void set_##name(const type& value) {                                         \
    if (!validate_##name(value)) {                                             \
      throw InvalidParameterValueException(#name, std::to_string(value),       \
                                           this->HostName());                  \
    }                                                                          \
    this->UpdateParameter(#name, std::to_string(value));                       \
    name##_ = value;                                                           \
  }

#define ALWAYS_VALID_TP(cname, name)                                           \
  bool cname::validate_##name(const decltype(cname::name##_)&) noexcept {      \
    return true;                                                               \
  }

#define RTP(cname, name)                                                       \
    const typename cname::name##_registration_class cname::name##_registration_;

}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORM_BASE_H_
