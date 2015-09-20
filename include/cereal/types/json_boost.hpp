#ifndef CEREAL_TYPES_JSON_OPTIONAL_HPP_
#define CEREAL_TYPES_JSON_OPTIONAL_HPP_

#include <cstddef>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include <boost/optional.hpp>

namespace cereal {

//! Object can be null, for non polymorphic types
template<class T>
class JsonNullable : public boost::optional<T> {
  public:
    using boost::optional<T>::optional;
    using boost::optional<T>::operator=;
};

//! Object need not to be present, for non polymorphic types
template<class T>
class JsonOptional : public boost::optional<T> {
public:
    using boost::optional<T>::optional;
    using boost::optional<T>::operator=;
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

} // namespace cereal

#endif //CEREAL_TYPES_JSON_OPTIONAL_HPP_
