#pragma once

#include <cereal/cereal.hpp>

#include "boost/date_time/posix_time/posix_time_types.hpp"

namespace cereal {

//! Saving for boost::posix_time::ptime
template <class Archive> inline
void CEREAL_SAVE_FUNCTION_NAME(Archive &ar, const boost::posix_time::ptime &time)
{
    namespace pt = boost::posix_time;
    pt::time_duration dur = time - pt::ptime(boost::gregorian::date(1970,1,1));
    ar(std::time_t(dur.total_seconds()));
}

//! Loading for boost::posix_time::ptime
template <class Archive> inline
void CEREAL_LOAD_FUNCTION_NAME(Archive &ar, boost::posix_time::ptime &time)
{
    namespace pt = boost::posix_time;
    std::time_t tmp;
    ar(tmp);
    pt::ptime start(boost::gregorian::date(1970,1,1));
    time = start + pt::seconds(static_cast<long>(tmp));
}

} // namespace cereal