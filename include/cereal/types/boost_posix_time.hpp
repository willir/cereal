#pragma once

#include <cereal/cereal.hpp>

#include "boost/date_time/posix_time/posix_time_types.hpp"

namespace cereal {

static int64_t bpt_tmp = 0;

inline void prologue( JSONInputArchive & ar, const boost::posix_time::ptime & )
{
  prologue(ar, bpt_tmp);
}

inline void epilogue( JSONInputArchive & ar, const boost::posix_time::ptime & )
{
  epilogue(ar, bpt_tmp);
}

inline void prologue( JSONOutputArchive & ar, const boost::posix_time::ptime & )
{
  prologue(ar, bpt_tmp);
}

inline void epilogue( JSONOutputArchive & ar, const boost::posix_time::ptime & )
{
  epilogue(ar, bpt_tmp);
}

inline void CEREAL_SAVE_FUNCTION_NAME( JSONOutputArchive & ar, const boost::posix_time::ptime & time )
{
  namespace pt = boost::posix_time;
  if(!time.is_not_a_date_time()) {
    pt::time_duration dur = time - pt::ptime(boost::gregorian::date(1970,1,1));
    ar.saveValue(std::time_t(dur.ticks() / dur.ticks_per_second()));
  } else {
    ar.saveValue(nullptr);
  }
}

inline void CEREAL_LOAD_FUNCTION_NAME( JSONInputArchive & ar, boost::posix_time::ptime & time )
{
  namespace pt = boost::posix_time;
  if(!ar.isNull()) {
    std::time_t tmp;
    ar.loadValue(tmp);
    pt::ptime start(boost::gregorian::date(1970,1,1));
    time = start + pt::seconds(static_cast<int64_t>(tmp));
  } else {
    time = pt::ptime{};  // Init to Not A Day Time
  }
}

} // namespace cereall
