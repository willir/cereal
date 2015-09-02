#ifndef CEREAL_TYPES_JSON_NULL_HPP_
#define CEREAL_TYPES_JSON_NULL_HPP_

#include <cstddef>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>

namespace cereal {

// ######################################################################
//! Prologue for std::unique_ptr for JSON Output archive
template <class T, class D> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONOutputArchive & ar, const std::unique_ptr<T, D> & )
{
    ar.writeName();
}

// ######################################################################
//! Epilogue for std::unique_ptr for JSON Output archive
template <class T, class D> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONOutputArchive &, const std::unique_ptr<T, D> & )
{ }

//! Saving std::unique_ptr for non polymorphic types
template <class T, class D> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME( JSONOutputArchive & ar, std::unique_ptr<T, D> const & ptr )
{
    if(ptr) {
        CEREAL_SAVE_FUNCTION_NAME(ar, *ptr);
    } else {
        ar.saveValue(nullptr);
    }
}

// ######################################################################
//! Prologue for std::unique_ptr for JSON Input archive
template <class T, class D> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
prologue( JSONInputArchive &, const std::unique_ptr<T, D> & )
{ }

// ######################################################################
//! Epilogue for std::unique_ptr for JSON Input archive
template <class T, class D> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
epilogue( JSONInputArchive &, const std::unique_ptr<T, D> & )
{ }

//! Loading std::unique_ptr for non polymorphic types
template <class T, class D> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME( JSONInputArchive & ar, std::unique_ptr<T, D> & ptr )
{
    if(!ar.isNull()) {
        if(!ptr) ptr.reset(new T());
        CEREAL_LOAD_FUNCTION_NAME(ar, *ptr);
    } else {
        ptr.reset(nullptr);
    }
}

} // namespace cereal

#endif //CEREAL_TYPES_JSON_NULL_HPP_
