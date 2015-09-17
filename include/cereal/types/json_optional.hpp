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
void prologue( JSONOutputArchive & ar, const boost::optional<T> & obj )
{
  if (obj.is_initialized()) {
    prologue(ar, *obj);
  }
}

// ######################################################################
//! Epilogue for std::unique_ptr for JSON Output archive
template <class T> inline
void epilogue( JSONOutputArchive & ar, const boost::optional<T> & obj )
{
  if (obj.is_initialized()) {
    epilogue(ar, *obj);
  }
}

//! Saving std::unique_ptr for non polymorphic types
template <class T> inline
void CEREAL_SAVE_FUNCTION_NAME( JSONOutputArchive & ar, const boost::optional<T> & obj )
{
  if (obj.is_initialized()) {
    ar.serializeRaw(*obj);
  }
}

// ######################################################################
//! Prologue for std::unique_ptr for JSON Input archive
template <class T> inline
typename std::enable_if<!std::is_abstract<T>::value, void>::type
prologue( JSONInputArchive & ar, boost::optional<T> & obj )
{
  if (ar.isExist()) {
    if (!obj.is_initialized()) obj.emplace();
    prologue(ar, *obj);
  } else {
    if (obj.is_initialized()) obj = boost::none;
  }
}

// ######################################################################
//! Epilogue for std::unique_ptr for JSON Input archive
template <class T> inline
typename std::enable_if<!std::is_abstract<T>::value, void>::type
epilogue( JSONInputArchive & ar, boost::optional<T> & obj )
{
  if (obj.is_initialized()) {
    epilogue(ar, *obj);
  }
}

//! Loading std::unique_ptr for non polymorphic types
template <class T> inline
typename std::enable_if<!std::is_abstract<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME( JSONInputArchive & ar, boost::optional<T> & obj )
{
  if (obj.is_initialized()) {  // It is supposed that prologue function has been called.
    ar.serializeRaw(*obj);
  }
}

} // namespace cereal

#endif //CEREAL_TYPES_JSON_OPTIONAL_HPP_
