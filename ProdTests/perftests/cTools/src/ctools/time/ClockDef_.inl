/**
 *@file ClockDef.inl
 *@brief
 *
 *@author Raffaele Mele <rmele@progesi.it>
 */

#ifndef CTOOLS_CLOCKDEF_INL
#define CTOOLS_CLOCKDEF_INL

namespace ctools { namespace localclock {
    
typedef unsigned long long TimeT;

#ifdef  WIN32
  //  To construct the current gregorian time from Windows time we need to add the
  //  difference of days between 15th October 1582  and 1th Jan 1601 .
  #define FIX_ULL(a) a
  const TimeT base_time_ = (TimeT)FIX_ULL(0x146BF33E42C000);
#else
  //  To construct the current gregorian time from UNIX time we need to add the
  //  difference of days between 15th October 1582 and 1st Jan 1970.
  //  This difference is 141427 days (0x2D8539C80 secs, 0x1B21DD213814000 100*nsec).
  #define FIX_ULL(a) a ## ULL
  const TimeT base_time_ = (TimeT)FIX_ULL(0x1B21DD213814000);
#endif // WIN32
    
} /* localclock */ } /* ctools */

#endif // CTOOLS_CLOCKDEF_INL
