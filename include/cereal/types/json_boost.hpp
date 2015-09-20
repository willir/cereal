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
//============================================================================

//! Object can be null, for non polymorphic types
template<class T>
class JsonNullable : public boost::optional<T> {
  public:
    JsonNullable() BOOST_NOEXCEPT { }
    JsonNullable(JsonNullT) BOOST_NOEXCEPT : boost::optional<T>(boost::none) { }
    bool isNull() const BOOST_NOEXCEPT { return !this->is_initialized(); }

    JsonNullable& operator= ( JsonNullT ) BOOST_NOEXCEPT
    {
        this->assign( boost::none ) ;
        return *this;
    }

    using boost::optional<T>::optional;
    using boost::optional<T>::operator=;
};

//! Object need not to be present, for non polymorphic types
template<class T>
class JsonOptional : public boost::optional<T> {
  public:
    JsonOptional() BOOST_NOEXCEPT { }
    JsonOptional(JsonAbsentT) BOOST_NOEXCEPT : boost::optional<T>(boost::none) { }
    bool isAbsent() const BOOST_NOEXCEPT { return !this->is_initialized(); }

    JsonOptional& operator= ( JsonAbsentT ) BOOST_NOEXCEPT
    {
        this->assign( boost::none ) ;
        return *this;
    }

    using boost::optional<T>::optional;
    using boost::optional<T>::operator=;
};

//! Object need not to be present, and can be null, for non polymorphic types
template<class T>
class JsonOptNull : public boost::optional<T> {
  public:
    JsonOptNull() BOOST_NOEXCEPT { }
    JsonOptNull(JsonNullT) BOOST_NOEXCEPT : boost::optional<T>(boost::none), mIsNull(true) { }
    JsonOptNull(JsonAbsentT) BOOST_NOEXCEPT : boost::optional<T>(boost::none), mIsNull(false) { }

    bool isNull() const BOOST_NOEXCEPT { return !this->is_initialized() && mIsNull; }
    bool isAbsent() const BOOST_NOEXCEPT { return !this->is_initialized() && !mIsNull; }

    // **DEPPRECATED**
    void reset() BOOST_NOEXCEPT { BOOST_ASSERT_MSG(false, "DEPPRECATED."); }

    JsonOptNull& operator= ( boost::none_t none_ ) BOOST_NOEXCEPT
    {
      this->assign( none_ ) ;
      mIsNull = false;
      return *this;
    }

    JsonOptNull& operator= ( JsonNullT ) BOOST_NOEXCEPT
    {
      this->assign( boost::none ) ;
      mIsNull = true;
      return *this;
    }

    JsonOptNull& operator= ( JsonAbsentT ) BOOST_NOEXCEPT
    {
      this->assign( boost::none ) ;
      mIsNull = false;
      return *this;
    }

    using boost::optional<T>::optional;
    using boost::optional<T>::operator=;
  private:
    bool mIsNull = false;
};

template<class D, template<class D> class C> inline
typename std::enable_if<std::is_base_of< boost::optional<D>, C<D> >::value, void>::type
boostOptionalToEmpty( C<D> & obj ) {
  obj = C<D>(D{});
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
