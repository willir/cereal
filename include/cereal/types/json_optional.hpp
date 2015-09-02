#ifndef CEREAL_TYPES_JSON_OPTIONAL_HPP_
#define CEREAL_TYPES_JSON_OPTIONAL_HPP_

#include <cstddef>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include <boost/optional.hpp>

namespace cereal {

// ######################################################################
//! Prologue for std::unique_ptr for JSON Output archive
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONOutputArchive & ar, const boost::optional<T> & obj )
{
  if (obj.is_initialized()) {
    ar.writeName();
  }
}

// ######################################################################
//! Epilogue for std::unique_ptr for JSON Output archive
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONOutputArchive &, const boost::optional<T> & )
{ }

//! Saving std::unique_ptr for non polymorphic types
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME( JSONOutputArchive & ar, const boost::optional<T> & obj )
{
  if (obj.is_initialized()) {
    CEREAL_SAVE_FUNCTION_NAME(ar, *obj);
  }
}

// ######################################################################
//! Prologue for std::unique_ptr for JSON Input archive
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONInputArchive &, const boost::optional<T> & )
{ }

// ######################################################################
//! Epilogue for std::unique_ptr for JSON Input archive
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONInputArchive &, const boost::optional<T> & )
{ }

//! Loading std::unique_ptr for non polymorphic types
template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME( JSONInputArchive & ar, boost::optional<T> & obj )
{
  if (ar.isExist()) {
    if (!obj.is_initialized()) obj = T{};
    CEREAL_LOAD_FUNCTION_NAME(ar, *obj);
  } else {
    obj = boost::none;
  }
}

} // namespace cereal

#endif //CEREAL_TYPES_JSON_OPTIONAL_HPP_
