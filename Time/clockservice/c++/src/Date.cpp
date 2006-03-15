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

#include <Time/clockservice/Date.hpp>
#include <iostream>
#include <Foundation/common/Assert.hpp>

using namespace Date;

///////////////////////////////////////////////////////////////////////////////
// Local utility function
///////////////////////////////////////////////////////////////////////////////

unsigned int 
days_count(const DateT& d);

unsigned int 
days_count_reverse(const DateT& d);

void 
checkDate(const DateT& d);

void 
dispDate(const DateT& d);

///////////////////////////////////////////////////////////////////////////////

unsigned int 
Date::leap_years(int from,
                 int to)
{
    unsigned int out = 0;
    if(from > to)
        return 0;
    
    while (from <= to)
    {
        if (!(from % 4)  &&
            (from % 100) ||
            !(from % 400))
            ++out;
        ++from;
    }
    return out;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int 
Date::month2day(int month)
{
    switch(month)
    {
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
        break;
    case 2:
        return 28;
        break;
    }
    return 31;
}

///////////////////////////////////////////////////////////////////////////////

void 
checkDate(const DateT& d)
{
    if (d.month == 0 || d.day == 0 || d.year == 0)
        throw std::range_error("not valid Date");
    if (d.month > 12 || d.day > 31)
        throw std::range_error("not valid Date");
    if ( (d.month != 2) && (d.day > Date::month2day(d.month)))
        throw std::range_error("not valid Date");
    if ((d.month == 2) && 
        (d.day > (28 + Date::leap_years(d.year,d.year))))
        throw std::range_error("not valid Date");
}

///////////////////////////////////////////////////////////////////////////////

void 
dispDate(const DateT& d)
{
    std::cerr << "-----------------------"  << std::endl;
    std::cerr << "d.day:"   << d.day        << std::endl;
    std::cerr << "d.month:" << d.month      << std::endl;
    std::cerr << "d.year:"  << d.year       << std::endl;
    std::cerr << "-----------------------"  << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int 
Date::date2days(const DateT& to, 
                const DateT& from)  
    throw(std::range_error)
{
    checkDate(to);
    checkDate(from);
    
    // chack for correct time interval
    if (to.year < from.year)
        throw std::range_error("not valid Date interval");
    else if (to.year == from.year){
        if (to.month < from.month)
            throw std::range_error("not valid Date interval");
        else if (to.month == from.month)
            if (to.day < from.day)
                throw std::range_error("not valid Date interval");
    }
    
    unsigned int counter = 0;
    int year_counter = to.year - from.year;
    
    if (year_counter == 0)// two date in the same year
        counter = days_count(to) - days_count(from) + 1;
    else {
        while (year_counter-- > 1)
            counter += 365;
        counter += days_count(to);
        counter += days_count_reverse(from);
        counter += leap_years(from.year+1,to.year-1);
    }
    return counter;    
}

///////////////////////////////////////////////////////////////////////////////

// count days in the year, starting from the days furnished in 
// the parameter, until to year begin. Include check for leap year.
unsigned int 
days_count(const DateT& d)
{
    checkDate(d);
    
    unsigned int day_counter   = d.day;
    unsigned int month_counter = d.month - 1;
    
    unsigned int counter = day_counter;
    
    while(month_counter-- > 0)
        counter+=month2day(month_counter + 1);
    
    if(d.month > 2)
        counter+=leap_years(d.year,d.year);
    
    return counter;
}

///////////////////////////////////////////////////////////////////////////////

// count remaining days in the year, starting from the next days furnished in 
// the parameter, until to year end. Include check for leap year.
unsigned int 
days_count_reverse(const DateT& d)
{
    return (days_count(DateT(12,31,d.year)) - days_count(d));
}

///////////////////////////////////////////////////////////////////////////////

unsigned int 
Date::date2days(int month, 
                    int day, 
                    int year)
    throw(std::range_error)
{
    // check for start of gregorian calendar  TO_ERASE
    const int month_offset_in_year = 0;
    const int day_offset_in_month = 0;
    
    // check year
    if(30000 < year || year < 1583)
        throw std::range_error("not valid year");
    
    // check month
    if(12 < month || month < month_offset_in_year)
        throw std::range_error("not valid month");

    // check day
    int day_in_month = (int)month2day(month);
    if(month == 2)  // if February check for leap
        day_in_month+=leap_years( year, year );
    
    if(day_in_month < day || day < day_offset_in_month)
        throw std::range_error("not valid day");
    
    const int day_2_1583 = 78;
    
    int day_counter   = day - 1;
    int month_counter = month - 1;
    int year_counter  = year - 1583;
    unsigned int counter = day_2_1583;
    
    while(year_counter-- > 0)
        counter+=365;
    
    // correction for leap year
    counter+=leap_years(1583,year-1);
    if(month_counter >= 2)
        counter+=leap_years(year,year);
    
    while(month_counter-- > 0)
        counter+=month2day(month_counter + 1);  // add 1 to offset in month2day
    
    counter+=day_counter;
    
    return counter;
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
