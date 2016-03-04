#ifndef CEREAL_TYPES_JSON_OPTIONAL_HPP_
#define CEREAL_TYPES_JSON_OPTIONAL_HPP_

#include <cstddef>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include <boost/optional.hpp>

namespace cereal {


namespace detail {

// the trick here is to make cereal::jsonNull defined once as a global but in a header file
template <typename T>
struct NoneInstance
{
    static const T instance;
};

template <typename T>
const T NoneInstance<T>::instance = T(); // global, but because 'tis a template, no cpp file required

} // namespace detail

class JsonNullT {};
class JsonAbsentT {};

namespace {
// TU-local
const JsonNullT& jsonNull = detail::NoneInstance<JsonNullT>::instance;
const JsonAbsentT& jsonAbsent = detail::NoneInstance<JsonAbsentT>::instance;
}

template<class T>
class JsonNullable;

template<class T>
class JsonOptional;

template<class T>
class JsonOptNull;

//============================================================================

#define INHERIT_BOOST_OPTIONAL_CONSTRUCTOR(ClassName)                                                              \
  ClassName() BOOST_NOEXCEPT { }                                                                                   \
  ClassName( boost::none_t none_ ) BOOST_NOEXCEPT : boost::optional<T>(none_) { }                                  \
  ClassName( const T &val ) : boost::optional<T>(val) { }                                                          \
  ClassName( bool cond, const T &val ) : boost::optional<T>(cond, val) { }                                         \
  ClassName( T &&val ) : boost::optional<T>(std::forward<T>(val)) { }                                              \
  template<class U>                                                                                                \
  ClassName( const boost::optional<U> & val ) : boost::optional<T>(val) { }                                        \
  template<class U>                                                                                                \
  explicit ClassName( boost::optional<U> && rhs ) : boost::optional<T>(std::move(rhs)) { }

#define INHERIT_BOOST_OPTIONAL_ASSIGNEMENT_OPERATOR(ClassName)                                                     \
  template<class U>                                                                                                \
  ClassName &operator=(const boost::optional<U> &obj) { boost::optional<T>::operator=(obj); return *this; }        \
  template<class U>                                                                                                \
  ClassName &operator=(boost::optional<U> &&obj) { boost::optional<T>::operator=(std::move(obj)); return *this; }  \
  ClassName &operator=(const T &obj) { boost::optional<T>::operator=(obj); return *this; }                         \
  ClassName &operator=(T &&obj) { boost::optional<T>::operator=(std::move(obj)); return *this; }


//! Object can be null, for non polymorphic types
template<class T>
class JsonNullable : public boost::optional<T> {
  public:
    JsonNullable(JsonNullT) BOOST_NOEXCEPT : boost::optional<T>(boost::none) { }
    JsonNullable(const JsonNullable &obj) : boost::optional<T>(static_cast<const boost::optional<T>&>(obj)) { }
    JsonNullable(JsonNullable &&obj) : boost::optional<T>(std::move(static_cast<boost::optional<T>&>(obj))) { }
    JsonNullable(const JsonOptNull<T> &that);
    JsonNullable(JsonOptNull<T> &&that);

    INHERIT_BOOST_OPTIONAL_CONSTRUCTOR(JsonNullable)

    INHERIT_BOOST_OPTIONAL_ASSIGNEMENT_OPERATOR(JsonNullable)

    JsonNullable &operator=(const JsonNullable &obj)
    {
      boost::optional<T>::operator=(static_cast<const boost::optional<T>&>(obj));
      return *this;
    }

    JsonNullable &operator=(JsonNullable &&obj)
    {
      boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
      return *this;
    }

    JsonNullable &operator=(const JsonOptNull<T> &obj);
    JsonNullable &operator=(JsonOptNull<T> &&obj);

    bool isNull() const BOOST_NOEXCEPT { return !this->is_initialized(); }

    JsonNullable& operator= ( JsonNullT ) BOOST_NOEXCEPT
    {
        this->assign( boost::none ) ;
        return *this;
    }
    JsonNullable& operator= ( boost::none_t none_ ) BOOST_NOEXCEPT { this->assign( none_ ) ; return *this; }
};

//! Object need not to be present, for non polymorphic types
template<class T>
class JsonOptional : public boost::optional<T> {
  public:
    JsonOptional(JsonAbsentT) BOOST_NOEXCEPT : boost::optional<T>(boost::none) { }
    JsonOptional(const JsonOptional &obj) : boost::optional<T>(static_cast<const boost::optional<T>&>(obj)) { }
    JsonOptional(JsonOptional &&obj) : boost::optional<T>(std::move(static_cast<boost::optional<T>&>(obj))) { }
    JsonOptional(const JsonOptNull<T> &obj);
    JsonOptional(JsonOptNull<T> &&obj);

    INHERIT_BOOST_OPTIONAL_CONSTRUCTOR(JsonOptional)

    INHERIT_BOOST_OPTIONAL_ASSIGNEMENT_OPERATOR(JsonOptional)

    JsonOptional &operator=(const JsonOptional &obj)
    {
      boost::optional<T>::operator=(static_cast<const boost::optional<T>&>(obj));
      return *this;
    }

    JsonOptional &operator=(JsonOptional &&obj)
    {
      boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
      return *this;
    }

    JsonOptional &operator=(const JsonOptNull<T> &obj);
    JsonOptional &operator=(JsonOptNull<T> &&obj);

    bool isAbsent() const BOOST_NOEXCEPT { return !this->is_initialized(); }

    JsonOptional& operator= ( JsonAbsentT ) BOOST_NOEXCEPT
    {
        this->assign( boost::none ) ;
        return *this;
    }

    JsonOptional& operator= ( boost::none_t none_ ) BOOST_NOEXCEPT { this->assign( none_ ) ; return *this; }
};

//! Object need not to be present, and can be null, for non polymorphic types
template<class T>
class JsonOptNull : public boost::optional<T> {
  public:
    JsonOptNull(JsonNullT) BOOST_NOEXCEPT : boost::optional<T>(boost::none), mIsNull(true) { }
    JsonOptNull(JsonAbsentT) BOOST_NOEXCEPT : boost::optional<T>(boost::none), mIsNull(false) { }
    INHERIT_BOOST_OPTIONAL_CONSTRUCTOR(JsonOptNull)
    JsonOptNull(const JsonOptNull &obj)
            : boost::optional<T>(static_cast<const boost::optional<T>&>(obj)),
              mIsNull(obj.mIsNull) { }
    JsonOptNull(JsonOptNull &&obj)
            : boost::optional<T>(std::move(static_cast<boost::optional<T>&>(obj))),
              mIsNull(obj.mIsNull) { }
    JsonOptNull(const JsonNullable<T> &obj)
            : boost::optional<T>(static_cast<const boost::optional<T>&>(obj)),
              mIsNull(true) { }
    JsonOptNull(JsonNullable<T> &&obj)
            : boost::optional<T>(std::move(static_cast<boost::optional<T>&>(obj))),
              mIsNull(true) { }
    JsonOptNull(const JsonOptional<T> &obj)
            : boost::optional<T>(static_cast<const boost::optional<T>&>(obj)),
              mIsNull(false) { }
    JsonOptNull(JsonOptional<T> &&obj)
            : boost::optional<T>(std::move(static_cast<boost::optional<T>&>(obj))),
              mIsNull(false) { }

    INHERIT_BOOST_OPTIONAL_ASSIGNEMENT_OPERATOR(JsonOptNull)

    template <typename U>
    JsonOptNull &operator=(const JsonOptNull<U> &obj)
    {
      boost::optional<T>::operator=(static_cast<const boost::optional<U>&>(obj));
      mIsNull = obj.mIsNull;
      return *this;
    }

    template <typename U>
    JsonOptNull &operator=(JsonOptNull<U> &&obj)
    {
      boost::optional<T>::operator=(std::move(static_cast<boost::optional<U>&>(obj)));
      mIsNull = obj.mIsNull;
      return *this;
    }

    JsonOptNull &operator=(const JsonOptNull &obj)
    {
      boost::optional<T>::operator=(static_cast<const boost::optional<T>&>(obj));
      mIsNull = obj.mIsNull;
      return *this;
    }

    JsonOptNull &operator=(JsonOptNull &&obj)
    {
      boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
      mIsNull = obj.mIsNull;
      return *this;
    }

    JsonOptNull &operator=(const JsonNullable<T> &obj)
    {
        boost::optional<T>::operator=(static_cast<const boost::optional<T>&>(obj));
        mIsNull = true;
        return *this;
    }

    JsonOptNull &operator=(JsonNullable<T> &&obj)
    {
        boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
        mIsNull = true;
        return *this;
    }

    JsonOptNull &operator=(const JsonOptional<T> &obj)
    {
        boost::optional<T>::operator=(static_cast<const boost::optional<T>&>(obj));
        mIsNull = false;
        return *this;
    }

    JsonOptNull &operator=(JsonOptional<T> &&obj)
    {
        boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
        mIsNull = false;
        return *this;
    }

    JsonOptNull &operator=( boost::none_t none_ ) BOOST_NOEXCEPT
    {
      this->assign( none_ ) ;
      mIsNull = false;
      return *this;
    }

    JsonOptNull &operator= ( JsonNullT ) BOOST_NOEXCEPT
    {
      this->assign( boost::none ) ;
      mIsNull = true;
      return *this;
    }

    JsonOptNull &operator= ( JsonAbsentT ) BOOST_NOEXCEPT
    {
      this->assign( boost::none ) ;
      mIsNull = false;
      return *this;
    }

    bool isNull() const BOOST_NOEXCEPT { return !this->is_initialized() && mIsNull; }
    bool isAbsent() const BOOST_NOEXCEPT { return !this->is_initialized() && !mIsNull; }

    JsonNullable<T> toNullable() const { return JsonNullable<T>(*this); }
    JsonOptional<T> toOptional() const { return JsonOptional<T>(*this); }

    // **DEPPRECATED**
    void reset() BOOST_NOEXCEPT { BOOST_ASSERT_MSG(false, "DEPPRECATED."); }

  private:
    bool mIsNull = false;
};

template<typename T>
JsonNullable<T>::JsonNullable(const JsonOptNull<T> &obj)
    : boost::optional<T>(static_cast<const boost::optional<T>&>(obj)) {
  BOOST_ASSERT_MSG(!obj.isAbsent(), "Should be present.");
}

template<typename T>
JsonNullable<T>::JsonNullable(JsonOptNull<T> &&obj) {
  BOOST_ASSERT_MSG(!obj.isAbsent(), "Should be present.");
  boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
}

template<typename T>
JsonNullable<T> &JsonNullable<T>::operator=(const JsonOptNull<T> &obj) {
  BOOST_ASSERT_MSG(!obj.isAbsent(), "Should be present.");
  boost::optional<T>::operator=(static_cast<const boost::optional<T>&>(obj));
  return *this;
}

template<typename T>
JsonNullable<T> &JsonNullable<T>::operator=(JsonOptNull<T> &&obj) {
  BOOST_ASSERT_MSG(!obj.isAbsent(), "Should be present.");
  boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
  return *this;
}
//----------------------------------

template<typename T>
JsonOptional<T>::JsonOptional(const JsonOptNull<T> &obj)
        : boost::optional<T>(static_cast<const boost::optional<T>&>(obj)) {
    BOOST_ASSERT_MSG(!obj.isNull(), "Should be present.");
}

template<typename T>
JsonOptional<T>::JsonOptional(JsonOptNull<T> &&obj) {
    BOOST_ASSERT_MSG(!obj.isNull(), "Should be present.");
    boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
}

template<typename T>
JsonOptional<T> &JsonOptional<T>::operator=(const JsonOptNull<T> &obj) {
    BOOST_ASSERT_MSG(!obj.isNull(), "Should be present.");
    boost::optional<T>::operator=(static_cast<const boost::optional<T>&>(obj));
    return *this;
}

template<typename T>
JsonOptional<T> &JsonOptional<T>::operator=(JsonOptNull<T> &&obj) {
    BOOST_ASSERT_MSG(!obj.isNull(), "Should be present.");
    boost::optional<T>::operator=(std::move(static_cast<boost::optional<T>&>(obj)));
    return *this;
}
//----------------------------------

#undef INHERIT_BOOST_OPTIONAL_CONSTRUCTOR
#undef INHERIT_BOOST_OPTIONAL_ASSIGNEMENT_OPERATOR

template<class D, template<class> class C> inline
typename std::enable_if<std::is_base_of< boost::optional<D>, C<D> >::value, void>::type
boostOptionalToEmpty( C<D> & obj ) {
  obj = C<D>(D{});
}

//boost::optional it is the same as JsonNullable
//=================================================================================

// JSONOutputArchive for boost::optional:
//----------------------------------------

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONOutputArchive & ar, const boost::optional<T> & obj )
{
    if(obj.is_initialized()) {
        prologue(ar, *obj);
    } else {
        ar.writeName();
    }
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONOutputArchive & ar, const boost::optional<T> & obj )
{
    if(obj.is_initialized()) {
        epilogue(ar, *obj);
    }
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME( JSONOutputArchive & ar, const boost::optional<T> & obj )
{
    if (obj.is_initialized()) {
        ar.serializeRaw(*obj);
    } else {
        ar.saveValue(nullptr);
    }
}

// JSONInputArchive for JsonNullable:
//----------------------------------------

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONInputArchive & ar, boost::optional<T> & obj )
{
    if (!ar.isNull()) {
        if (!obj) boostOptionalToEmpty(obj);
        prologue(ar, *obj);
    }
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONInputArchive & ar, boost::optional<T> & obj )
{
    if(obj.is_initialized()) {
        epilogue(ar, *obj);
    }
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME( JSONInputArchive & ar, boost::optional<T> & obj )
{
    if (!ar.isNull()) {
        if (!obj.is_initialized()) {
            boostOptionalToEmpty(obj);
        }
        ar.serializeRaw(*obj);
    } else {
        obj = boost::none;
    }
}

//cereal::JsonNullable
//=================================================================================

// JSONOutputArchive for JsonNullable:
//----------------------------------------

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONOutputArchive & ar, const JsonNullable<T> & obj )
{
  if(obj.is_initialized()) {
    prologue(ar, *obj);
  } else {
    ar.writeName();
  }
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONOutputArchive & ar, const JsonNullable<T> & obj )
{
  if(obj.is_initialized()) {
    epilogue(ar, *obj);
  }
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME( JSONOutputArchive & ar, const JsonNullable<T> & obj )
{
  if (obj.is_initialized()) {
    ar.serializeRaw(*obj);
  } else {
    ar.saveValue(nullptr);
  }
}

// JSONInputArchive for JsonNullable:
//----------------------------------------

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONInputArchive & ar, JsonNullable<T> & obj )
{
  if (!ar.isNull()) {
    if (!obj) boostOptionalToEmpty(obj);
    prologue(ar, *obj);
  }
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONInputArchive & ar, JsonNullable<T> & obj )
{
  if(obj.is_initialized()) {
    epilogue(ar, *obj);
  }
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME( JSONInputArchive & ar, JsonNullable<T> & obj )
{
  if (!ar.isNull()) {
    if (!obj.is_initialized()) {
      boostOptionalToEmpty(obj);
    }
    ar.serializeRaw(*obj);
  } else {
    obj = boost::none;
  }
}

// JsonOptional
//=================================================================================

//! Prologue for JsonOptional for JSON Output archive
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONOutputArchive & ar, const JsonOptional<T> & obj )
{
  if (obj.is_initialized()) {
    prologue(ar, *obj);
  }
}

//! Epilogue for JsonOptional for JSON Output archive
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONOutputArchive & ar, const JsonOptional<T> & obj )
{
  if (obj.is_initialized()) {
    epilogue(ar, *obj);
  }
}

//! Saving JsonOptional for non polymorphic types
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME( JSONOutputArchive & ar, const JsonOptional<T> & obj )
{
  if (obj.is_initialized()) {
    ar.serializeRaw(*obj);
  }
}

//! Prologue for JsonOptional for JSON Input archive
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONInputArchive & ar, JsonOptional<T> & obj )
{
  if (ar.isExist()) {
    if (!obj.is_initialized()) boostOptionalToEmpty(obj);
    prologue(ar, *obj);
  } else {
    if (obj.is_initialized()) obj = boost::none;
  }
}

//! Epilogue for JsonOptional for JSON Input archive
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONInputArchive & ar, JsonOptional<T> & obj )
{
  if (obj.is_initialized()) {
    epilogue(ar, *obj);
  }
}

//! Loading JsonOptional for non polymorphic types
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME( JSONInputArchive & ar, JsonOptional<T> & obj )
{
  if (obj.is_initialized()) {  // It is supposed that prologue function has been called.
    ar.serializeRaw(*obj);
  }
}

// JsonOptNull
//=================================================================================

//! Prologue for JsonOptNull for JSON Output archive
template<class T>
inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue(JSONOutputArchive &ar, const JsonOptNull<T> &obj)
{
  if (obj.is_initialized()) {
    prologue(ar, *obj);
  } else if (obj.isNull()) {
    ar.writeName();
  }
}

//! Epilogue for JsonOptional for JSON Output archive
template<class T>
inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue(JSONOutputArchive &ar, const JsonOptNull<T> &obj)
{
  if (obj.is_initialized()) {
    epilogue(ar, *obj);
  } else if(obj.isNull()) {
    ar.saveValue(nullptr);
  }
}

//! Saving JsonOptional for non polymorphic types
template<class T>
inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME(JSONOutputArchive &ar, const JsonOptNull<T> &obj)
{
  if (obj.is_initialized()) {
    ar.serializeRaw(*obj);
  }
}

//! Prologue for JsonOptional for JSON Input archive
template<class T>
inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue(JSONInputArchive &ar, JsonOptNull<T> &obj)
{
  if (ar.isExist()) {
    if (!ar.isNull()) {
      if (!obj.is_initialized()) boostOptionalToEmpty(obj);
      prologue(ar, *obj);
    } else {
      obj = jsonNull;
    }
  } else {
    obj = jsonAbsent;
  }
}

//! Epilogue for JsonOptional for JSON Input archive
template<class T>
inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue(JSONInputArchive &ar, JsonOptNull<T> &obj)
{
  if (obj.is_initialized()) {  // It is supposed that prologue function has been called.
    epilogue(ar, *obj);
  }
}

//! Loading JsonOptional for non polymorphic types
template<class T>
inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME(JSONInputArchive &ar, JsonOptNull<T> &obj)
{
  if (obj.is_initialized()) {  // It is supposed that prologue function has been called.
    ar.serializeRaw(*obj);
  }
}

} // namespace cereal

#endif //CEREAL_TYPES_JSON_OPTIONAL_HPP_
