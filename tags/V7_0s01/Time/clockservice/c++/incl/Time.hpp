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

#ifndef CARDAMOM_CDMW_TIME_HPP
#define CARDAMOM_CDMW_TIME_HPP

/**
 * @author Fabrizio Lorenna <f.lorenna@vitrociset.it>
 * @author Marco Macellari <mmacellari@progesi.it>
 */

#include <cstring>
#include "Foundation/ossupport/OS.hpp"
#include "Time/clockservice/CdmwTime.skel.hpp"

struct timeval;

/**
 * @brief cdmw_tm used in the methods TimeT2cdmw_tm, 
 * asctime_r, asctime and cdmw_tm2TimeT.
 */
struct cdmw_tm : tm // inerithed from tm 
{
    /**
     * The number of milliseconds, in the range 0 to 999.
     */
    int tm_msec;

    /**
     * The number of microseconds, in the range 0 to 999.
     */
    int tm_usec;
       
    /**
     * Default Constructor, all values are 0.
     */
    cdmw_tm();

    /**
     * Conversion from a tm structure.
     * @par The tm structure.
     */
    cdmw_tm(const tm& par);

    /**
     * Conversion from a tm structure.
     * @par The tm structure.
 */
    cdmw_tm& 
    operator=(const tm& par);
};

namespace Cdmw { namespace clock { namespace util {

    /**
     * @brief function TimeT2cdmw_tm converts a TimeT to a struct cdmw_tm
     * @param input_time is the number of 10^(-7) seconds from the julian day
     *        Fri 15 Oct 1582 (at 00:00:00,000,000), Coordinated Universal Time (UTC).
     * @param tm is a cdmw_tm struct (inherited from tm struct of Unix).
     * @return the return value is 0 if all ok, -1 in case of error.
     *         Remark: if the daylight saving time is in effect, the time returned
     *         takes into account the daylight saving hour legally imposed!
     */
    int TimeT2cdmw_tm(const TimeBase::TimeT& input_time, cdmw_tm& tm);

    /**
     * @brief function asctime_r converts an input cdmw_tm struct to a suitable string.
     *        Examples of resulting strings are the following:
     *        "Thu Jan  1 00:00:00,000,000 1970\n",
     *        "Wed Feb 23 21:49:08,347,002 2005\n".
     * @param tm is the input cdmw_tm struct containing all detailed time information
     * @param buf is the output buffer that will store the TimeT-to-string conversion
     * @return this function returns a pointer to the output buffer.
     *         If buf is NULL, or in case of error, a NULL pointer is returned.
     */
    char* asctime_r(const cdmw_tm& tm, char *buf);
     
    /**
     * @brief function asctime converts an input cdmw_tm struct to a suitable string.
     *        Examples of resulting strings are the following:
     *        "Thu Jan  1 00:00:00,000,000 1970\n",
     *        "Wed Feb 23 21:49:08,347,002 2005\n".
     * @param tm is the input cdmw_tm struct containing all detailed time information
     * @return this function returns a pointer to a statically allocated string
     *         which might be overwritten by subsequent calls to this function.
     *         In case of error a NULL pointer is returned.
     */
    char* asctime(const cdmw_tm& tm);

    /**
     * @brief function cdmw_tm2TimeT converts a cdmw_tm input struct in a TimeT
     * @param tm is the cdmw_tm input struct containing all detailed time informations
     * @param out_time is the associated output TimeT expressed as the number of 10^(-7) seconds
     *        from the julian day Fri 15 Oct 1582 (at 00:00:00,000,000), Coordinated Universal Time (UTC).
     * @return the return value is 0 if all ok, -1 in case of error.
     *         Remark: if the daylight saving time is in effect, the time returned
     *         takes into account the daylight saving hour legally imposed!
     */
    int cdmw_tm2TimeT(const cdmw_tm& tm, TimeBase::TimeT& out_time);
     
    /**
     * @brief function TimeT2timeval converts a TimeT to a struct timeval
     * @param input_time is the number of 10^(-7) seconds from the julian day
     *        Fri 15 Oct 1582 (at 00:00:00,000,000), Coordinated Universal Time (UTC).
     * @param tv is a timeval struct (used inside Linux, file sys/time.h).
     * @return the return value is 0 if all ok, -1 in case of error.
     */
    int TimeT2timeval(const TimeBase::TimeT& input_time, timeval& tv);

    /**
     * @brief function asctime_r converts an input timeval struct to a suitable string.
     *        Examples of resulting strings are the following:
     *        "Thu Jan  1 00:00:00,000,000 1970\n",
     *        "Wed Feb 23 21:49:08,347,002 2005\n".
     * @param tv is the input timeval struct containing all detailed time information
     * @param buf is the output buffer that will store the TimeT-to-string conversion
     * @return this function returns a pointer to the output buffer.
     *         If buf is NULL, or in case of error, a NULL pointer is returned.
     */
    char* asctime_r(const timeval& tv, char *buf);

    /**
     * @brief function asctime converts an input timeval struct to a suitable string.
     *        Examples of resulting strings are the following:
     *        "Thu Jan  1 00:00:00,000,000 1970\n",
     *        "Wed Feb 23 21:49:08,347,002 2005\n".
     * @param tv is the input timeval struct containing all detailed time information
     * @return this function returns a pointer to a statically allocated string
     *         which might be overwritten by subsequent calls to this function.
     *         In case of error a NULL pointer is returned.
     */
    char* asctime(const timeval& tv);

    /**
     * @brief function timeval2TimeT converts a timeval input struct in a TimeT
     * @param tv is the timeval input struct containing all detailed time informations
     * @param out_time is the associated output TimeT expressed as the number of 10^(-7) seconds
     *        from the julian day Fri 15 Oct 1582 (at 00:00:00,000,000), Coordinated Universal Time (UTC).
     * @return the return value is 0 if all ok, -1 in case of error.
     */
    int timeval2TimeT(const timeval& tv, TimeBase::TimeT& out_time);

    /**
     * @brief function CdmwTimeval2TimeT converts a Cdmw Timeval input struct in a TimeT
     * @param tv is the timeval input struct containing all detailed time informations
     * @param out_time is the associated output TimeT expressed as the number of 10^(-7) seconds
     *        from the julian day Fri 15 Oct 1582 (at 00:00:00,000,000), Coordinated Universal Time (UTC).
     * @return the return value is 0 if all ok, -1 in case of error.
     */
    int CdmwTimeval2TimeT(const Cdmw::OsSupport::OS::Timeval& tv, 
                          TimeBase::TimeT& out_time);

    /**
     * @brief function get_isdst retrieves the daylight saving time
     *        flag tm_isdst of the CURRENT LOCAL TIME of the computer.
     *
     * tm_isdst: is a flag that indicates whether daylight saving time
     * is in effect at the time described.  
     * @return returns a positive value if daylight saving time is in effect;
     *         returns zero if daylight saving time is not in effect;
     *         returns a negative value if daylight saving time information is not available.
     */
     int get_isdst();

} /* util */ } /* clock */ } /* Cdmw */

#endif /* CARDAMOM_CDMW_TIME_HPP */
