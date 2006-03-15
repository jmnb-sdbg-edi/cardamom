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

#include <cppunit/extensions/HelperMacros.h>
#include "testclockservice/TimeTest.hpp"
#include "Time/clockservice/test/testcommon/ClockTest.hpp"
#include "Time/clockservice/config.hpp"
#include "Time/clockservice/Util.hpp"
#include "Time/clockservice/Time.hpp"
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace CosClockService;
using namespace CosPropertyService;

CPPUNIT_TEST_SUITE_REGISTRATION (TimeTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TimeTest, "testclockservice");

void
TimeTest::setUp ()
{
}

void
TimeTest::tearDown ()
{
}

// test on the value Cdmw::clock::TIME_BASE
void TimeTest::cdmw_tm_time_base()
{
  struct cdmw_tm tm;
  char comp_str[256];
  TimeBase::TimeT out_time;

    // give in input the TIME_BASE to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm = Cdmw::clock::util::TimeT2cdmw_tm(Cdmw::clock::TIME_BASE, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm == 0);

    // see if the output tm struct is correct
    // TIME_BASE is TimeT since Fri 15-OCT-1582 00:00:00,000,000 of Thu 1-JAN-1970 00:00:00
  CPPUNIT_ASSERT ((tm.tm_year == 70) &&
                  (tm.tm_mon  ==  0) &&
                  (tm.tm_mday ==  1) &&
                  (tm.tm_hour ==  0) &&
                  (tm.tm_min  ==  0) &&
                  (tm.tm_sec  ==  0) &&
                  (tm.tm_msec ==  0) &&
                    (tm.tm_usec ==  0) &&
                  (tm.tm_yday ==  0) &&
                  (tm.tm_wday ==  4));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Thu Jan  1 00:00:00,000,000 1970\n") == 0);

    // inverse operation, give tm and the output must be TIME_BASE
    int result_cdmw_tm2TimeT = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT == 0);
    CPPUNIT_ASSERT (out_time == Cdmw::clock::TIME_BASE);
}

// test on the value Cdmw::clock::TIME_BASE
void TimeTest::timeval_time_base()
{
    struct timeval tv;
    char comp_str[256];
    TimeBase::TimeT out_time;
                                                                                                                             
    // give in input the TIME_BASE to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval = Cdmw::clock::util::TimeT2timeval(Cdmw::clock::TIME_BASE, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval == 0);
                                                                                                                             
    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Thu Jan  1 00:00:00,000,000 1970\n") == 0);
                                                                                                                             
    // inverse operation, give tv and the output must be TIME_BASE
    int result_timeval2TimeT = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT == 0);
  CPPUNIT_ASSERT (out_time == Cdmw::clock::TIME_BASE);
}

// test of 15 reference dates taken using boost library
void TimeTest::cdmw_tm_time_dates()
{
  struct cdmw_tm tm;
  char comp_str[256];
  TimeBase::TimeT out_time;

    /* ------------------------------------------- *
       date 1 : 15-OCT-1582 00:00:00,000,000 with
              - TimeT = 0
              - day of week = Friday
              - days in the year = 287
     * ------------------------------------------- */

    // give in input the TimeT of date 1 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_01 = Cdmw::clock::util::TimeT2cdmw_tm(0, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_01 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -318) &&
                  (tm.tm_mon  ==    9) &&
                  (tm.tm_mday ==   15) &&
                  (tm.tm_hour ==    0) &&
                  (tm.tm_min  ==    0) &&
                  (tm.tm_sec  ==    0) &&
                  (tm.tm_msec ==    0) &&
                    (tm.tm_usec ==    0) &&
                  (tm.tm_yday ==  287) &&
                  (tm.tm_wday ==    5));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Fri Oct 15 00:00:00,000,000 1582\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 1
    int result_cdmw_tm2TimeT_01 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_01 == 0);
  CPPUNIT_ASSERT (out_time == 0);

    /* ------------------------------------------- *
       date 2 : 15-OCT-1582 00:00:00,002,000 with
              - TimeT = 0
              - day of week = Friday
              - days in the year = 287
     * ------------------------------------------- */

    // give in input the TimeT of date 2 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_02 = Cdmw::clock::util::TimeT2cdmw_tm(20000, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_02 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -318) &&
                  (tm.tm_mon  ==    9) &&
                  (tm.tm_mday ==   15) &&
                  (tm.tm_hour ==    0) &&
                  (tm.tm_min  ==    0) &&
                  (tm.tm_sec  ==    0) &&
                  (tm.tm_msec ==    2) &&
                    (tm.tm_usec ==    0) &&
                  (tm.tm_yday ==  287) &&
                  (tm.tm_wday ==    5));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Fri Oct 15 00:00:00,002,000 1582\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 2
    int result_cdmw_tm2TimeT_02 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_02 == 0);
  CPPUNIT_ASSERT (out_time == 20000);

    /* ------------------------------------------- *
       date 3 : 30-NOV-1620 01:02:05,457,002 with
                - TimeT = 12032101254570020
              - days in the year = 334
     * ------------------------------------------- */

    // give in input the TimeT of date 3 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_03 = Cdmw::clock::util::TimeT2cdmw_tm(12032101254570020ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_03 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -280) &&
                  (tm.tm_mon  ==   10) &&
                  (tm.tm_mday ==   30) &&
                  (tm.tm_hour ==    1) &&
                  (tm.tm_min  ==    2) &&
                  (tm.tm_sec  ==    5) &&
                  (tm.tm_msec ==  457) &&
                    (tm.tm_usec ==    2) &&
                  (tm.tm_yday ==  334) &&
                  (tm.tm_wday ==    1));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Mon Nov 30 01:02:05,457,002 1620\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 3
    int result_cdmw_tm2TimeT_03 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_03 == 0);
    CPPUNIT_ASSERT (out_time == 12032101254570020ULL);

    /* ------------------------------------------- *
       date 4 : 04-DEC-1658 12:23:11,987,042 with
                - TimeT = 24027421919870420
              - days in the year = 337
     * ------------------------------------------- */

    // give in input the TimeT of date 4 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_04 = Cdmw::clock::util::TimeT2cdmw_tm(24027421919870420ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_04 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -242) &&
                  (tm.tm_mon  ==   11) &&
                  (tm.tm_mday ==    4) &&
                  (tm.tm_hour ==   12) &&
                  (tm.tm_min  ==   23) &&
                  (tm.tm_sec  ==   11) &&
                  (tm.tm_msec ==  987) &&
                    (tm.tm_usec ==   42) &&
                  (tm.tm_yday ==  337) &&
                  (tm.tm_wday ==    3));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Wed Dec  4 12:23:11,987,042 1658\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 4
    int result_cdmw_tm2TimeT_04 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_04 == 0);
    CPPUNIT_ASSERT (out_time == 24027421919870420ULL);

    /* ------------------------------------------- *
       date 5 : 12-JAN-1696 17:42:56,193,157 with
                - TImeT = 35737405761931570
              - days in the year = 11
     * ------------------------------------------- */

    // give in input the TimeT of date 5 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_05 = Cdmw::clock::util::TimeT2cdmw_tm(35737405761931570ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_05 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -204) &&
                  (tm.tm_mon  ==    0) &&
                  (tm.tm_mday ==   12) &&
                  (tm.tm_hour ==   17) &&
                  (tm.tm_min  ==   42) &&
                  (tm.tm_sec  ==   56) &&
                  (tm.tm_msec ==  193) &&
                    (tm.tm_usec ==  157) &&
                  (tm.tm_yday ==   11) &&
                  (tm.tm_wday ==    4));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Thu Jan 12 17:42:56,193,157 1696\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 5
    int result_cdmw_tm2TimeT_05 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_05 == 0);
    CPPUNIT_ASSERT (out_time == 35737405761931570ULL);

    /* ------------------------------------------- *
       date 6 : 29-FEB-1736 19:53:12,634,000 with
              - TimeT = 48401131926340000 
              - days in the year = 59
     * ------------------------------------------- */

    // give in input the TimeT of date 6 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_06 = Cdmw::clock::util::TimeT2cdmw_tm(48401131926340000ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_06 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -164) &&
                  (tm.tm_mon  ==    1) &&
                  (tm.tm_mday ==   29) &&
                  (tm.tm_hour ==   19) &&
                  (tm.tm_min  ==   53) &&
                  (tm.tm_sec  ==   12) &&
                  (tm.tm_msec ==  634) &&
                    (tm.tm_usec ==    0) &&
                  (tm.tm_yday ==   59) &&
                  (tm.tm_wday ==    3));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Wed Feb 29 19:53:12,634,000 1736\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 6
    int result_cdmw_tm2TimeT_06 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_06 == 0);
  CPPUNIT_ASSERT (out_time == 48401131926340000ULL);

    /* ------------------------------------------- *
       date 7 : 02-MAR-1772 22:32:44,001,000 with
              - TimeT = 59763691640010000
              - days in the year = 61
              - day of week = Mon
     * ------------------------------------------- */

    // give in input the TimeT of date 7 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_07 = Cdmw::clock::util::TimeT2cdmw_tm(59763691640010000ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_07 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -128) &&
                  (tm.tm_mon  ==    2) &&
                  (tm.tm_mday ==    2) &&
                  (tm.tm_hour ==   22) &&
                  (tm.tm_min  ==   32) &&
                  (tm.tm_sec  ==   44) &&
                  (tm.tm_msec ==    1) &&
                    (tm.tm_usec ==    0) &&
                  (tm.tm_yday ==   61) &&
                  (tm.tm_wday ==    1));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Mon Mar  2 22:32:44,001,000 1772\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 7
    int result_cdmw_tm2TimeT_07 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_07 == 0);
  CPPUNIT_ASSERT (out_time == 59763691640010000ULL);

    /* ------------------------------------------- *
       date 8 : 17-APR-1810 07:05:21,101,101 with
                - TimeT = 71793471211011010
              - days in the year = 106
              - day of week = Tue
     * ------------------------------------------- */

    // give in input the TimeT of date 8 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_08 = Cdmw::clock::util::TimeT2cdmw_tm(71793471211011010ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_08 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -90) &&
                  (tm.tm_mon  ==   3) &&
                  (tm.tm_mday ==  17) &&
                  (tm.tm_hour ==   7) &&
                  (tm.tm_min  ==   5) &&
                  (tm.tm_sec  ==  21) &&
                  (tm.tm_msec == 101) &&
                    (tm.tm_usec == 101) &&
                  (tm.tm_yday == 106) &&
                  (tm.tm_wday ==   2));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Tue Apr 17 07:05:21,101,101 1810\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 8
    int result_cdmw_tm2TimeT_08 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_08 == 0);
    CPPUNIT_ASSERT (out_time == 71793471211011010ULL);

    /* ------------------------------------------- *
       date 9 : 07-MAY-1839 03:12:23,342,000 with
              - TimeT = 80962099433420000 
              - days in the year = 126
              - day of week = Tue
     * ------------------------------------------- */

    // give in input the TimeT of date 9 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_09 = Cdmw::clock::util::TimeT2cdmw_tm(80962099433420000ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_09 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -61) &&
                  (tm.tm_mon  ==   4) &&
                  (tm.tm_mday ==   7) &&
                  (tm.tm_hour ==   3) &&
                  (tm.tm_min  ==  12) &&
                  (tm.tm_sec  ==  23) &&
                  (tm.tm_msec == 342) &&
                    (tm.tm_usec ==   0) &&
                  (tm.tm_yday == 126) &&
                  (tm.tm_wday ==   2));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Tue May  7 03:12:23,342,000 1839\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 9
    int result_cdmw_tm2TimeT_09 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_09 == 0);
  CPPUNIT_ASSERT (out_time == 80962099433420000ULL);

    /* ------------------------------------------- *
       date 10 : 10-JUN-1877 02:34:42,205,307 with
                 - TImeT = 92983772822053070     
               - days in the year = 160
               - day of week = Sun
     * ------------------------------------------- */

    // give in input the TimeT of date 10 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_10 = Cdmw::clock::util::TimeT2cdmw_tm(92983772822053070ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_10 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year == -23) &&
                  (tm.tm_mon  ==   5) &&
                  (tm.tm_mday ==  10) &&
                  (tm.tm_hour ==   2) &&
                  (tm.tm_min  ==  34) &&
                  (tm.tm_sec  ==  42) &&
                  (tm.tm_msec == 205) &&
                    (tm.tm_usec == 307) &&
                  (tm.tm_yday == 160) &&
                  (tm.tm_wday ==   0));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Sun Jun 10 02:34:42,205,307 1877\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 10
    int result_cdmw_tm2TimeT_10 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_10 == 0);
    CPPUNIT_ASSERT (out_time == 92983772822053070ULL);

    /* ------------------------------------------- *
       date 11 : 22-JUL-1915 06:43:21,608,000 with
               - TimeT = 105010802016080000
               - days in the year = 202
               - day of week = Thu
     * ------------------------------------------- */

    // give in input the TimeT of date 11 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_11 = Cdmw::clock::util::TimeT2cdmw_tm(105010802016080000ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_11 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year ==  15) &&
                  (tm.tm_mon  ==   6) &&
                  (tm.tm_mday ==  22) &&
                  (tm.tm_hour ==   6) &&
                  (tm.tm_min  ==  43) &&
                  (tm.tm_sec  ==  21) &&
                  (tm.tm_msec == 608) &&
                    (tm.tm_usec ==   0) &&
                  (tm.tm_yday == 202) &&
                  (tm.tm_wday ==   4));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Thu Jul 22 06:43:21,608,000 1915\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 11
    int result_cdmw_tm2TimeT_11 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_11 == 0);
  CPPUNIT_ASSERT (out_time == 105010802016080000ULL);

    /* ------------------------------------------- *
       date 12 : 15-AUG-1953 09:29:27,365,446 with
                 - TimeT = 117023957673654460
               - days in the year = 226
               - day of week = Sat
     * ------------------------------------------- */

    // give in input the TimeT of date 12 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_12 = Cdmw::clock::util::TimeT2cdmw_tm(117023957673654460ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_12 == 0);

    // see if the output tm struct is correct
  CPPUNIT_ASSERT ((tm.tm_year ==  53) &&
                  (tm.tm_mon  ==   7) &&
                  (tm.tm_mday ==  15) &&
                  (tm.tm_hour ==   9) &&
                  (tm.tm_min  ==  29) &&
                  (tm.tm_sec  ==  27) &&
                  (tm.tm_msec == 365) &&
                    (tm.tm_usec == 446) &&
                  (tm.tm_yday == 226) &&
                  (tm.tm_wday ==   6));

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Sat Aug 15 09:29:27,365,446 1953\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 12
    int result_cdmw_tm2TimeT_12 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_12 == 0);
    CPPUNIT_ASSERT (out_time == 117023957673654460ULL);

    /* ------------------------------------------- *
       date 13 : 19-SEP-1991 10:12:14,554,000 with
               - TimeT = 129045679345540000  
               - days in the year = 261
               - day of week = Thu
     * ------------------------------------------- */

    // give in input the TimeT of date 13 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_13 = Cdmw::clock::util::TimeT2cdmw_tm(129045679345540000ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_13 == 0);

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Thu Sep 19 10:12:14,554,000 1991\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 13
    int result_cdmw_tm2TimeT_13 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_13 == 0);
  CPPUNIT_ASSERT (out_time == 129045679345540000ULL);

    /* ------------------------------------------- *
       date 14 : 29-FEB-2000 02:09:03,632,741 with
                 - TimeT = 131710829436327410
               - days in the year = 59
               - day of week = Tue
     * ------------------------------------------- */

    // give in input the TimeT of date 14 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_14 = Cdmw::clock::util::TimeT2cdmw_tm(131710829436327410ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_14 == 0);

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Tue Feb 29 02:09:03,632,741 2000\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 14
    int result_cdmw_tm2TimeT_14 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_14 == 0);
    CPPUNIT_ASSERT (out_time == 131710829436327410ULL);

    /* ------------------------------------------- *
       date 15 : 04-OCT-2038 05:06:07,889,000 with
               - TimeT = 143890743678890000
               - days in the year = 276
               - day of week = Mon
     * ------------------------------------------- */

    // give in input the TimeT of date 15 to function Cdmw::clock::util::TimeT2cdmw_tm
    int result_TimeT2cdmw_tm_15 = Cdmw::clock::util::TimeT2cdmw_tm(143890743678890000ULL, tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm_15 == 0);

    // do the asctime operation to see if it is correct
  Cdmw::clock::util::asctime_r(tm, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Mon Oct  4 05:06:07,889,000 2038\n") == 0);

    // inverse operation, give tm and the output must be TimeT of date 15
    int result_cdmw_tm2TimeT_15 = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT_15 == 0);
  CPPUNIT_ASSERT (out_time == 143890743678890000ULL);
}

// test of 15 reference dates taken using boost library
void TimeTest::timeval_time_dates()
{
    struct timeval tv;
    char comp_str[256];
    TimeBase::TimeT out_time;

    /* ------------------------------------------- *
       date 1 : 15-OCT-1582 00:00:00,000,000 with
                - TimeT = 0
     * ------------------------------------------- */

    // give in input the TimeT of date 1 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_01 = Cdmw::clock::util::TimeT2timeval(0, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_01 == -1); // TimeT referes to date coming before Thu 1 Jan 1970 00:00:00

    /* ------------------------------------------- *
       date 2 : 30-NOV-1620 01:02:05,457,002 with
                - TimeT = 12032101254570020
     * ------------------------------------------- */

    // give in input the TimeT of date 2 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_02 = Cdmw::clock::util::TimeT2timeval(12032101254570020ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_02 == -1); // TimeT referes to date coming before Thu 1 Jan 1970 00:00:00

    /* ------------------------------------------- *
       date 3 : 01-JAN-1970 00:00:00,000,000 with
                - TimeT = 122192928000000000
     * ------------------------------------------- */

    // give in input the TimeT of date 3 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_03 = Cdmw::clock::util::TimeT2timeval(122192928000000000ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_03 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Thu Jan  1 00:00:00,000,000 1970\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 3
    int result_timeval2TimeT_03 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_03 == 0);
    CPPUNIT_ASSERT (out_time == 122192928000000000ULL);

    /* ------------------------------------------- *
       date 4 : 14-APR-1974 20:10:36,123,456 with
                - TimeT = 123544950361234560
     * ------------------------------------------- */

    // give in input the TimeT of date 4 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_04 = Cdmw::clock::util::TimeT2timeval(123544950361234560ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_04 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Sun Apr 14 20:10:36,123,456 1974\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 4
    int result_timeval2TimeT_04 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_04 == 0);
    CPPUNIT_ASSERT (out_time == 123544950361234560ULL);

    /* ------------------------------------------- *
       date 5 : 27-JUN-1986 15:45:27,003,999 with
                - TimeT = 127395639270039990
     * ------------------------------------------- */

    // give in input the TimeT of date 5 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_05 = Cdmw::clock::util::TimeT2timeval(127395639270039990ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_05 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Fri Jun 27 15:45:27,003,999 1986\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 5
    int result_timeval2TimeT_05 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_05 == 0);
    CPPUNIT_ASSERT (out_time == 127395639270039990ULL);

    /* ------------------------------------------- *
       date 6 : 28-FEB-1990 12:30:00,000,000 with
                - TimeT = 128555010000000000 
     * ------------------------------------------- */

    // give in input the TimeT of date 6 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_06 = Cdmw::clock::util::TimeT2timeval(128555010000000000ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_06 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Wed Feb 28 12:30:00,000,000 1990\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 6
    int result_timeval2TimeT_06 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_06 == 0);
    CPPUNIT_ASSERT (out_time == 128555010000000000ULL);

    /* ------------------------------------------- *
       date 7 : 30-JUL-1998 17:27:03,017,045 with
                - TimeT = 131211124230170450
     * ------------------------------------------- */

    // give in input the TimeT of date 7 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_07 = Cdmw::clock::util::TimeT2timeval(131211124230170450ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_07 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Thu Jul 30 17:27:03,017,045 1998\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 7
    int result_timeval2TimeT_07 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_07 == 0);
    CPPUNIT_ASSERT (out_time == 131211124230170450ULL);

    /* ------------------------------------------- *
       date 8 : 29-FEB-2000 16:34:10,368,534 with
                - TimeT = 131711348503685340
     * ------------------------------------------- */

    // give in input the TimeT of date 8 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_08 = Cdmw::clock::util::TimeT2timeval(131711348503685340ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_08 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Tue Feb 29 16:34:10,368,534 2000\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 8
    int result_timeval2TimeT_08 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_08 == 0);
    CPPUNIT_ASSERT (out_time == 131711348503685340ULL);

    /* ------------------------------------------- *
       date 9 : 23-JUL-2010 18:45:17,987,654 with
                - TimeT = 134992035179876540
     * ------------------------------------------- */

    // give in input the TimeT of date 9 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_09 = Cdmw::clock::util::TimeT2timeval(134992035179876540ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_09 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Fri Jul 23 18:45:17,987,654 2010\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 9
    int result_timeval2TimeT_09 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_09 == 0);
    CPPUNIT_ASSERT (out_time == 134992035179876540ULL);

    /* ------------------------------------------- *
       date 10 : 15-NOV-2018 01:00:05,001,000 with
                 - TimeT = 137615364050010000
     * ------------------------------------------- */

    // give in input the TimeT of date 10 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_10 = Cdmw::clock::util::TimeT2timeval(137615364050010000ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_10 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Thu Nov 15 01:00:05,001,000 2018\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 10
    int result_timeval2TimeT_10 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_10 == 0);
    CPPUNIT_ASSERT (out_time == 137615364050010000ULL);

    /* ------------------------------------------- *
       date 11 : 25-DEC-2030 23:30:45,000,000 with
                 - TimeT = 141437646450000000
     * ------------------------------------------- */

    // give in input the TimeT of date 11 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_11 = Cdmw::clock::util::TimeT2timeval(141437646450000000ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_11 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Wed Dec 25 23:30:45,000,000 2030\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 11
    int result_timeval2TimeT_11 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_11 == 0);
    CPPUNIT_ASSERT (out_time == 141437646450000000ULL);

    /* ------------------------------------------- *
       date 12 : 28-FEB-2035 19:14:56,120,489 with
                 - TimeT = 142755956961204890
     * ------------------------------------------- */

    // give in input the TimeT of date 12 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_12 = Cdmw::clock::util::TimeT2timeval(142755956961204890ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_12 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Wed Feb 28 19:14:56,120,489 2035\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 12
    int result_timeval2TimeT_12 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_12 == 0);
    CPPUNIT_ASSERT (out_time == 142755956961204890ULL);

    /* ------------------------------------------- *
       date 13 : 17-JAN-2038 10:30:25,341,450 with
                 - TimeT = 143666298253414500
     * ------------------------------------------- */

    // give in input the TimeT of date 13 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_13 = Cdmw::clock::util::TimeT2timeval(143666298253414500ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_13 == 0);

    // do the asctime operation to see if it is correct
    Cdmw::clock::util::asctime_r(tv, comp_str);
    CPPUNIT_ASSERT (strcmp (comp_str, "Sun Jan 17 10:30:25,341,450 2038\n") == 0);

    // inverse operation, give tv and the output must be TimeT of date 13
    int result_timeval2TimeT_13 = Cdmw::clock::util::timeval2TimeT(tv, out_time);
    CPPUNIT_ASSERT (result_timeval2TimeT_13 == 0);
    CPPUNIT_ASSERT (out_time == 143666298253414500ULL);

    /* ------------------------------------------- *
       date 14 : 04-OCT-2038 05:06:07,889,000 with
                 - TimeT = 143890743678890000
     * ------------------------------------------- */

    // give in input the TimeT of date 14 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_14 = Cdmw::clock::util::TimeT2timeval(143890743678890000ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_14 == -1); // TimeT referes to date coming after Tue 19 Jan 2038 04:14:07

    /* ------------------------------------------- *
       date 15 : 08-SEP-2040 08:08:08,008,008 with
                 - TimeT = 144499972880080080
     * ------------------------------------------- */

    // give in input the TimeT of date 15 to function Cdmw::clock::util::TimeT2timeval
    int result_TimeT2timeval_15 = Cdmw::clock::util::TimeT2timeval(144499972880080080ULL, tv);
    CPPUNIT_ASSERT (result_TimeT2timeval_15 == -1); // TimeT referes to date coming after Tue 19 Jan 2038 04:14:07
}

// test to verify the return of cdmw_tm2TimeT(.) 
void TimeTest::ret_values()
{
  struct cdmw_tm tm;
  TimeBase::TimeT out_time;
    int result_cdmw_tm2TimeT;

    // try a date before 15-OCT-1582
    tm.tm_usec =  999;
  tm.tm_msec =  999;
  tm.tm_sec  =   59;
  tm.tm_min  =   59;
  tm.tm_hour =   23;
  tm.tm_mday =   14;
  tm.tm_mon  =    9;
  tm.tm_year = -318;
    result_cdmw_tm2TimeT = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT == -1);

    // try a date equal to 15-OCT-1582
    tm.tm_usec =    0;
  tm.tm_msec =    0;
  tm.tm_sec  =    0;
  tm.tm_min  =    0;
  tm.tm_hour =    0;
  tm.tm_mday =   15;
  tm.tm_mon  =    9;
  tm.tm_year = -318;
    result_cdmw_tm2TimeT = Cdmw::clock::util::cdmw_tm2TimeT(tm, out_time);
    CPPUNIT_ASSERT (result_cdmw_tm2TimeT == 0);
}

// test for returned flag of daylight saving time
void TimeTest::daylight_saving()
{
    time_t t = time(0);
    CPPUNIT_ASSERT (t != (time_t) -1);

    struct tm  linux_tm;
    struct tm* linux_tm_ptr = gmtime_r(&t, &linux_tm);
    CPPUNIT_ASSERT (linux_tm_ptr != 0);

    struct cdmw_tm cardamom_tm;
    int result_TimeT2cdmw_tm = Cdmw::clock::util::TimeT2cdmw_tm(Cdmw::clock::compute_current_time(), cardamom_tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm == 0);

    CPPUNIT_ASSERT (linux_tm.tm_isdst == cardamom_tm.tm_isdst);

    //int daylight = Cdmw::clock::util::get_isdst();
}

// test if the cardamom year of the Epoch is 70
void TimeTest::cdmw_year_of_time_base()
{
    TimeBase::TimeT time_base = Cdmw::clock::TIME_BASE;

    cdmw_tm cardamom_tm;
    int result_TimeT2cdmw_tm = Cdmw::clock::util::TimeT2cdmw_tm(time_base, cardamom_tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm == 0);

    CPPUNIT_ASSERT_EQUAL(70, cardamom_tm.tm_year);
}

// test if, for the current time, the cardamom and linux years are equal
void TimeTest::cdmw_year_vs_linux_year()
{
    TimeBase::TimeT current_cardamom_time = Cdmw::clock::compute_current_time();
    time_t current_linux_time = time(0);
    CPPUNIT_ASSERT (current_linux_time != (time_t) -1);

    struct cdmw_tm cardamom_tm;
    int result_TimeT2cdmw_tm = Cdmw::clock::util::TimeT2cdmw_tm(current_cardamom_time, cardamom_tm);
    CPPUNIT_ASSERT (result_TimeT2cdmw_tm == 0);

    struct tm linux_tm;
    struct tm* tm_ptr = gmtime_r(&current_linux_time, &linux_tm);
    CPPUNIT_ASSERT (tm_ptr != 0);

    CPPUNIT_ASSERT_EQUAL(linux_tm.tm_year, cardamom_tm.tm_year);
}

