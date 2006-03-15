/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */

/**
 * @author Fabrizio Lorenna <f.lorenna@vitrociset.it>
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include <ctime>
#include <cmath>
#include <iostream>
#include "Time/clockservice/Time.hpp"
#include "Time/clockservice/config.hpp"


/**
 * @struct  timeval
 * @brief forward-declaration for struct timeval, (see sys/time.h ...).
 * 
 * This structure is used in the functions: TimeT2timeval, 
 * asctime, asctime_r, and timeval2TimeT, to retrieve a string or a
 * TimeBase::TimeT.
 * 
 * @code 
 * tv_sec  : The number of seconds since the Epoch 
 *             (Thu 1 Jan 1970 at 00:00:00,000,000)
 * tv_usec : The number of microseconds since the 
 *              Epoch (Thu 1 Jan 1970 at 00:00:00,000,000)
 * tm_sec  : The number of seconds after the minute, normally in the range 0 
 *            to 59 but can be up to 61 to allow for leap seconds.
 * tm_min  : The number of minutes after the hour, in the range 0 to 59.
 * tm_hour : The number of hours past midnight, in the range 0 to 23.
 * tm_mday : The day of the month, in the range 1 to 31.
 * tm_mon  : The number of months since January, in the range 0 to 11.
 * tm_year : The year since the year 1900.
 * tm_wday : The number of days since Sunday, in the range 0 to 6.
 * tm_yday : The number of days since January 1-st, in the range 0 to 365.
 * tm_isdst: A flag that indicates whether daylight saving time is in effect
 *            at the time described. The value is positive if daylight saving
 *            time is in effect, zero if it is not, and negative if 
 *            the information is not available.
 * @endcode 
 */



namespace
{
  // microseconds in a day
  const unsigned long long USEC_IN_A_DAY  = 86400000000ULL;

  // microseconds in a hour
  const unsigned long long USEC_IN_A_HOUR = 3600000000ULL;

  // microseconds in a minute
  const unsigned long long USEC_IN_A_MIN  = 60000000ULL;

  // microseconds in a second
  const unsigned long long USEC_IN_A_SEC  = 1000000ULL;

  // microseconds in a millisecond
  const unsigned long long USEC_IN_A_MSEC = 1000ULL;
}

////////////////////////////////////////////////////////////////////////////////

cdmw_tm::cdmw_tm()
{
    std::memset(this, 0, sizeof(cdmw_tm));
}

////////////////////////////////////////////////////////////////////////////////

cdmw_tm::cdmw_tm(const tm& par)
{
    std::memset(this, 0, sizeof(cdmw_tm));
    std::memcpy(this, &par, sizeof(tm));
}

////////////////////////////////////////////////////////////////////////////////

cdmw_tm& 
cdmw_tm::operator=(const tm& par)
{
    memset(this, 0, sizeof(cdmw_tm));
    memcpy(this, &par, sizeof(tm));
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

int Cdmw::clock::util::TimeT2cdmw_tm(const TimeBase::TimeT& input_time, 
                                     cdmw_tm& tm)
{
    /*
       Array of months, from October, to September.
       In this case the February month is the array element at index 4.
       indexes:          0   1   2   3   4   5   6   7   8   9  10  11
       months :        oct nov dec jan feb mar apr may jun jul aug sep
    */
    int months[] = { 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30 };

    // assign tm_isdst (daylight saving time in Coordinated Universal Time (UTC))
    TimeBase::TimeT input_time_sec_fra = input_time % 10000000; // note the 100 nanosec factor
    TimeBase::TimeT input_time_sec_rem = input_time - input_time_sec_fra;
    TimeBase::TimeT input_time_sec_epc = input_time_sec_rem - Cdmw::clock::TIME_BASE;
    TimeBase::TimeT input_time_sec_num = input_time_sec_epc / 10000000;
    if (input_time_sec_num < 0 || input_time_sec_num > (TimeBase::TimeT) std::numeric_limits<time_t>::max())
    {
        tm.tm_isdst = -1;
    }
    else
    {
        struct tm linux_tm;
        time_t linux_time = std::numeric_limits<time_t>::max() & input_time_sec_num;
        if (gmtime_r(&linux_time, &linux_tm) != 0)
            tm.tm_isdst = linux_tm.tm_isdst;
        else
            tm.tm_isdst = -1;
    }

    // compute the integral number of microseconds contained in input_time
    TimeBase::TimeT input_time_usec_num = (input_time - (input_time % 10)) / 10;

    // take into account the case of daylight saving time in effect, (Coordinated Universal Time (UTC))
    if (tm.tm_isdst > 0)
    {
        if ((input_time_usec_num * 10) > (std::numeric_limits<TimeBase::TimeT>::max() - (USEC_IN_A_HOUR * 10)))
        {   // cannot add an extra hour to take into account that the daylight saving time is in effect
            return (-1);
        }
        input_time_usec_num += USEC_IN_A_HOUR;
    }

    // compute the integral number of days contained in input_time
    TimeBase::TimeT input_time_num_days = (input_time_usec_num - (input_time_usec_num % USEC_IN_A_DAY)) / USEC_IN_A_DAY;

    // assign tm_wday from input_time_num_days
    tm.tm_wday = ((input_time_num_days % 7) + 5) % 7;

    // calculate current_year, current_yday, current_mday and current_mon
    int current_year = 1582;
    int current_yday = 287;
    int current_mday = 15;
    int current_mon  = 0;
    for (TimeBase::TimeT i = 0; i < input_time_num_days; i++)
    {
        current_mday++;
        current_yday++;

        // check for leap year
        if ((current_mday > 28) && (current_mon == 4))
	{
	  if (!((current_year % 4) == 0))
            {   // do nothing, it is not a leap year
	      months[4] = 28;
	    }
	    else if (((current_year % 100) == 0) && (!((current_year % 400) == 0)))
            {   // do nothing, it is not a leap year
	      months[4] = 28;
	    }
	  else
            {   // case of leap year
	      months[4] = 29;
	    }
	}
        if (current_mday > months[current_mon])
	    {
	    current_mday = 1;
	    current_mon++;
	    if (current_mon == 3)
            {   // case of january
	    current_year++;
	        current_yday=0;
	    }
	    if (current_mon > 11)
	        current_mon = 0;
	}
    }

    // assign tm_mday from current_mday
    tm.tm_mday = current_mday;

    // assign tm_mon from current_mon
    tm.tm_mon = (current_mon + 9) % 12;

    // assign tm_year from current_year
  tm.tm_year = current_year - 1900;

    // assign tm_yday from current_yday
    tm.tm_yday = current_yday;

    // calculate the remainder of the last day in microseconds
    TimeBase::TimeT remainder_day_in_usec = input_time_usec_num % USEC_IN_A_DAY;

    // assign tm_hour
    TimeBase::TimeT remainder_hour_in_usec = remainder_day_in_usec % USEC_IN_A_HOUR;
    tm.tm_hour = (remainder_day_in_usec - remainder_hour_in_usec) / USEC_IN_A_HOUR;

    // assign tm_min
    TimeBase::TimeT remainder_min_in_usec = remainder_hour_in_usec % USEC_IN_A_MIN;
    tm.tm_min  = (remainder_hour_in_usec - remainder_min_in_usec) / USEC_IN_A_MIN;

    // assign tm_sec
    TimeBase::TimeT remainder_sec_in_usec = remainder_min_in_usec % USEC_IN_A_SEC;
    tm.tm_sec  = (remainder_min_in_usec - remainder_sec_in_usec) / USEC_IN_A_SEC;

    // assign tm_msec
    TimeBase::TimeT remainder_msec_in_usec = remainder_sec_in_usec % USEC_IN_A_MSEC;
    tm.tm_msec = (remainder_sec_in_usec - remainder_msec_in_usec) / USEC_IN_A_MSEC;

    // assign tm_usec
    tm.tm_usec = remainder_msec_in_usec;

    // normal exit
    return 0;
}

char* Cdmw::clock::util::asctime_r(const cdmw_tm& tm, char *buf)
{
  if (buf != 0)
  {
        char* day_week [] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
      char* asc_month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

        sprintf(buf, "%.3s %.3s %2d %02d:%02d:%02d,%03d,%03d %d\n",
	           day_week [(tm.tm_wday)],
	           asc_month[(tm.tm_mon) ],
	           tm.tm_mday,
                   tm.tm_hour,
                   tm.tm_min,
                   tm.tm_sec,
                     tm.tm_msec,
                     tm.tm_usec,
                   tm.tm_year + 1900);
  }
  return (buf);
};

char* Cdmw::clock::util::asctime(const cdmw_tm& tm)
{
    static char asctime_buffer[80];

    char* day_week [] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    char* asc_month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    sprintf(asctime_buffer, "%.3s %.3s %2d %02d:%02d:%02d,%03d,%03d %d\n",
                            day_week [(tm.tm_wday)],
                            asc_month[(tm.tm_mon) ],
                            tm.tm_mday,
                            tm.tm_hour,
                            tm.tm_min,
                            tm.tm_sec,
                            tm.tm_msec,
                            tm.tm_usec,
                            tm.tm_year + 1900);

    return &asctime_buffer[0];
};

int Cdmw::clock::util::cdmw_tm2TimeT(const cdmw_tm& tm, TimeBase::TimeT& out_time)
{

    /*
       Array of months, from October, to September.
       In this case the February month is the array element at index 4.
       indexes:          0   1   2   3   4   5   6   7   8   9  10  11
       months :        oct nov dec jan feb mar apr may jun jul aug sep
    */
    int months[] = { 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30 };

    // compute input year from tm_year
    int input_year = tm.tm_year + 1900;

    // check if the date is before 15/10/1582
  if ((input_year <= 1582) && (tm.tm_mon <= 9) && (tm.tm_mday < 15))
    {
        // return with error
      return (-1);
    }

    int current_year = 1582;
    int current_mday = 15;
    int current_mon  = 0;

    // compute the correct day (variable 'num_days')
    unsigned long int num_days = 0; // total number of days
  for (;;)
    {
        // check for leap year
        if ((current_mday > 28) && (current_mon == 4))
	{
	  if (!((current_year % 4) == 0))
            {   // do nothing, it is not a leap year
	      months[4] = 28;
	    }
	    else if (((current_year % 100) == 0) && (!((current_year % 400) == 0)))
            {   // do nothing, it is not a leap year
	      months[4] = 28;
	    }
	  else
            {   // case of leap year
	      months[4] = 29;
	    }
	}
        if (current_mday > months[current_mon])
	{
	    current_mday = 1;
	    current_mon = (current_mon + 1) % 12;

	    if (current_mon == 3) // case of january
	    current_year++;
        }
        if ((tm.tm_mday == current_mday)             &&
            (tm.tm_mon  == ((current_mon + 9) % 12)) &&
            (input_year == current_year))
            break;

        current_mday++;
        num_days++;
    }

    /*
       Now in num_days I have the exact number of days
       since Fri 15 Oct 1582 (at 00:00:00,000).
       Now do the part for HH:MM:SS.ttt.
    */

    // calculate the TimeT in usec for the number of days
    TimeBase::TimeT days_time_t = num_days * USEC_IN_A_DAY;

    // calculate the TimeT in usec for the current day
    TimeBase::TimeT cur_day_time_t = (tm.tm_hour * USEC_IN_A_HOUR) +
                                     (tm.tm_min  * USEC_IN_A_MIN ) +
                                     (tm.tm_sec  * USEC_IN_A_SEC ) +
                                     (tm.tm_msec * USEC_IN_A_MSEC) +
                                     (tm.tm_usec);

    // calculate the total TimeT in usec taking into account the eventual daylight saving time
    TimeBase::TimeT out_time_in_usec = days_time_t + cur_day_time_t;
    if (tm.tm_isdst > 0)
	  {
        if (out_time_in_usec - USEC_IN_A_HOUR < 0)
        {   // cannot subtract an extra hour to take into account that the daylight saving time is in effect
            return (-1);
	  }
        out_time_in_usec = out_time_in_usec - USEC_IN_A_HOUR;
	  }

    // assign out_time with the converted TimeT
    out_time = (TimeBase::TimeT) (out_time_in_usec * 10); // NB: the 100 nanosec factor

    // return normally
    return 0;
}

int Cdmw::clock::util::TimeT2timeval(const TimeBase::TimeT& input_time, timeval& tv)
{
    // compute from input_time the integral number of seconds since the Epoch
    TimeBase::TimeT input_time_sec_fra = input_time % 10000000; // note the 100 nanosec factor
    TimeBase::TimeT input_time_sec_rem = input_time - input_time_sec_fra;
    TimeBase::TimeT input_time_sec_epc = input_time_sec_rem - Cdmw::clock::TIME_BASE;
    TimeBase::TimeT input_time_sec_num = input_time_sec_epc / 10000000;

    if (input_time_sec_num < 0 || input_time_sec_num > (TimeBase::TimeT) std::numeric_limits<time_t>::max())
    {   // cannot represent current TimeT with a value of time_t inside time_t bounds
        return (-1);
    }

    // compute from input_time_sec_fra the integral number of microseconds since the Epoch
    TimeBase::TimeT input_time_usec_fra = input_time_sec_fra % 10;
    TimeBase::TimeT input_time_usec_rem = input_time_sec_fra - input_time_usec_fra;
    TimeBase::TimeT input_time_usec_num = input_time_usec_rem / 10;

    // assign tv.tv_sec
    tv.tv_sec  = static_cast<time_t>(input_time_sec_num);

    // assign tv.tv_usec
    tv.tv_usec = static_cast<suseconds_t>(input_time_usec_num);

    // return normally
    return 0;
}

char* Cdmw::clock::util::asctime_r(const timeval& tv, char *buf)
{
    if (buf != 0)
    {
        char* day_week [] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
        char* asc_month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

        TimeBase::TimeT cardamom_time;
        if (Cdmw::clock::util::timeval2TimeT(tv, cardamom_time) != 0)
        {   // cannot convert timeval to TimeT
            return (0);
        }

        struct cdmw_tm cardamom_tm;
        if (Cdmw::clock::util::TimeT2cdmw_tm(cardamom_time, cardamom_tm) != 0)
        {   // cannot convert TimeT to cdmw_tm
            return (0);
        }

        sprintf(buf, "%.3s %.3s %2d %02d:%02d:%02d,%03d,%03d %d\n",
                     day_week [(cardamom_tm.tm_wday)],
                     asc_month[(cardamom_tm.tm_mon) ],
                     cardamom_tm.tm_mday,
                     cardamom_tm.tm_hour,
                     cardamom_tm.tm_min,
                     cardamom_tm.tm_sec,
                     cardamom_tm.tm_msec,
                     cardamom_tm.tm_usec,
                     cardamom_tm.tm_year + 1900);
    }
    return (buf);
}

char* Cdmw::clock::util::asctime(const timeval& tv)
{
    static char asctime_buffer[80];

    char* day_week [] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    char* asc_month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    TimeBase::TimeT cardamom_time;
    if (Cdmw::clock::util::timeval2TimeT(tv, cardamom_time) != 0)
    {   // cannot convert timeval to TimeT
        return (0);
    }
                                                                                                                             
    struct cdmw_tm cardamom_tm;
    if (Cdmw::clock::util::TimeT2cdmw_tm(cardamom_time, cardamom_tm) != 0)
    {   // cannot convert TimeT to cdmw_tm
        return (0);
    }
                                                                                                                             
    sprintf(asctime_buffer, "%.3s %.3s %2d %02d:%02d:%02d,%03d,%03d %d\n",
                            day_week [(cardamom_tm.tm_wday)],
                            asc_month[(cardamom_tm.tm_mon) ],
                            cardamom_tm.tm_mday,
                            cardamom_tm.tm_hour,
                            cardamom_tm.tm_min,
                            cardamom_tm.tm_sec,
                            cardamom_tm.tm_msec,
                            cardamom_tm.tm_usec,
                            cardamom_tm.tm_year + 1900);

    return &asctime_buffer[0];
}

int Cdmw::clock::util::timeval2TimeT(const timeval& tv, TimeBase::TimeT& out_time)
{
    out_time = Cdmw::clock::TIME_BASE +
               static_cast<TimeBase::TimeT>(tv.tv_sec)  * 10000000 +
               static_cast<TimeBase::TimeT>(tv.tv_usec) * 10;
  return 0;
}

int Cdmw::clock::util::get_isdst()
{
  time_t t = time(0);
  if (t == (time_t) -1)
  {   // error during time computation:
      // daylight saving time information is not available.
      return -1;
  }

  struct tm *m_tm = localtime(&t);
  if (m_tm == 0)
  {   // error during struct tm evaluation:
      // daylight saving time information is not available.
      return -1;
  }

  return (m_tm->tm_isdst);
}

int Cdmw::clock::util::CdmwTimeval2TimeT(const Cdmw::OsSupport::OS::Timeval& tv, 
                                         TimeBase::TimeT& out_time)
{    
    out_time = tv.seconds;
    (out_time <<= 6)*=15625;        
    out_time += tv.microseconds;
    (out_time <<= 1)*=5;            
    out_time+=Cdmw::clock::TIME_BASE;   
    return  1;   
}
