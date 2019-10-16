#ifndef CEREAL_TYPES_JSON_JSON_AS_TYPE_HPP_
#define CEREAL_TYPES_JSON_JSON_AS_TYPE_HPP_

#include <cstddef>
#include <string>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

namespace cereal {

inline void prologue( JSONOutputArchive & ar, const CEREAL_RAPIDJSON_NAMESPACE::Document & json )
{
  prologue(ar, 0);
}

inline void epilogue( JSONOutputArchive & ar, const CEREAL_RAPIDJSON_NAMESPACE::Document & json )
{
  epilogue(ar, 0);
}

inline void CEREAL_SAVE_FUNCTION_NAME(
        JSONOutputArchive & ar,
        const CEREAL_RAPIDJSON_NAMESPACE::Document & json )
{
  ar.saveValue(json);
}

// ######################################################################

inline void prologue( JSONInputArchive & ar, const CEREAL_RAPIDJSON_NAMESPACE::Document & json )
{
    prologue(ar, 0);
}

inline void epilogue( JSONInputArchive & ar, const CEREAL_RAPIDJSON_NAMESPACE::Document & json )
{
    epilogue(ar, 0);
}

inline void CEREAL_LOAD_FUNCTION_NAME(
        JSONInputArchive & ar,
        CEREAL_RAPIDJSON_NAMESPACE::Document & json )
{
  ar.loadValue(json);
}

} // namespace cereal

#endif //CEREAL_TYPES_JSON_JSON_AS_TYPE_HPP_
