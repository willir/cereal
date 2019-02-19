#ifndef CEREAL_TYPES_STRING_VIEW_JSON_HPP_
#define CEREAL_TYPES_STRING_VIEW_JSON_HPP_

#include <cstddef>
#include <string_view>

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

namespace cereal {

namespace internal { namespace json_str_view {
static const std::string MOCK_STR;
}} // namespace internal::json_str_view

inline void CEREAL_SAVE_FUNCTION_NAME(JSONOutputArchive &ar, std::string_view obj) {
    ar.saveValue(obj.data());
}

inline void prologue(JSONOutputArchive &ar, std::string_view) {
    prologue(ar, internal::json_str_view::MOCK_STR);
}

inline void epilogue(JSONOutputArchive &ar, std::string_view) {
    epilogue(ar, internal::json_str_view::MOCK_STR);
}

} // namespace cereal

#endif // CEREAL_TYPES_STRING_VIEW_JSON_HPP_
