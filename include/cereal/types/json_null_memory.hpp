#ifndef CEREAL_TYPES_JSON_NULL_MEMORY_HPP_
#define CEREAL_TYPES_JSON_NULL_MEMORY_HPP_

#include <cstddef>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>

namespace cereal {

// ######################################################################
//! Prologue for std::unique_ptr for JSON Output archive
template <class T, class D> inline
void prologue( JSONOutputArchive & ar, const std::unique_ptr<T, D> & ptr )
{
  if(ptr) {
    prologue(ar, *ptr);
  } else {
    ar.writeName();
  }
}

// ######################################################################
//! Epilogue for std::unique_ptr for JSON Output archive
template <class T, class D> inline
void epilogue( JSONOutputArchive & ar, const std::unique_ptr<T, D> & ptr )
{
  if(ptr) {
    epilogue(ar, *ptr);
  }
}

//! Saving std::unique_ptr for non polymorphic types
template <class T, class D> inline
void CEREAL_SAVE_FUNCTION_NAME( JSONOutputArchive & ar, const std::unique_ptr<T, D> & ptr )
{
  if (ptr) {
    ar.serializeRaw(*ptr);
  } else {
    ar.saveValue(nullptr);
  }
}

// ######################################################################
//! Prologue for std::unique_ptr for JSON Input archive
template <class T, class D> inline
typename std::enable_if<!std::is_abstract<T>::value, void>::type
prologue( JSONInputArchive & ar, std::unique_ptr<T, D> & ptr )
{
  if (!ar.isNull()) {
    if (!ptr) ptr.reset(new T());
    prologue(ar, *ptr);
  }
}

// ######################################################################
//! Epilogue for std::unique_ptr for JSON Input archive
template <class T, class D> inline
typename std::enable_if<!std::is_abstract<T>::value, void>::type
epilogue( JSONInputArchive & ar, std::unique_ptr<T, D> & ptr )
{
  if(ptr) {
    epilogue(ar, *ptr);
  }
}

//! Loading std::unique_ptr for non polymorphic types
template <class T, class D> inline
typename std::enable_if<!std::is_abstract<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME( JSONInputArchive & ar, std::unique_ptr<T, D> & ptr )
{
  if (!ar.isNull()) {
    if (!ptr) ptr.reset(new T());
    ar.serializeRaw(*ptr);
  } else {
    ptr.reset(nullptr);
  }
}

} // namespace cereal

#endif //CEREAL_TYPES_JSON_NULL_MEMORY_HPP_
