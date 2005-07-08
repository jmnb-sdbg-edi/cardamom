/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ========================================================================= */

#ifndef DATE_HPP
#define DATE_HPP
/**
 * @file
 * @brief  Date utily functions.
 * 
 * @author Fabrizio Morciano <fmorciano@amsjv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 */

#include <stdexcept>
#include <iosfwd>
#include <tao/corba.h>
#include <Time/clockservice/config.hpp>


/**
 * @namespace Date
 * @brief This namespace contains functions and classes to managed data
 *  changes.
 */
namespace Date  
{  
   /**
    * @fn int leap_years( int from ,int to )
    * @brief  Find how many leap day are present between
    *         two years.
    * @param from start year.
    * @param to end year.
    * @return leap day between years.
    */
    CLOCK_API 
    unsigned int leap_years(int from ,
                            int to );
    
    /**
     * @fn int month2day( int month );
     * @brief Find how many day for selected month.
     * @param month month selected in [1,12].
     * @return day for month.
     */
    CLOCK_API 
    unsigned int month2day(int month );
    
    /**
     * @brief Simple date structure.
     */
    struct  CLOCK_API  DateT
    {
        unsigned int month;
        unsigned int day;
        unsigned int year;
        
        DateT()
            :month(10)
            ,day  (15)
            ,year (1582)
        {
        }
        
        DateT(unsigned int m,
            unsigned int d,
            unsigned int y )
            :month(m)
            ,day  (d)
            ,year (y)
        {
        }    
    };
    
    /**
     * @fn int date2days( const DateT& to, const DateT& from );
     * @brief  Find how many day are present between 'from' and 'to' date.
     * @param from  initial day of date interval (Default to 10/15/1582).
     * @param to final day of date interval.
     * @exception  range_error if not valid date.
     * @note Notation is MM/DD/YYYY.
     */    
    CLOCK_API 
    unsigned int date2days(const DateT& to,
                           const DateT& from=DateT(10,15,1582))
        throw(std::range_error);
    
    /**
     * @fn int date2days( int month, int day, int year );
     * @brief  Find how many day are present between 10/15/1582 and date.
     * @param month  month of date.
     * @param day day of date.
     * @param year year of date.
     * @return day between 10(October)/15/1582 and date.
     * @exception  range_error if not valid date.
     * @note Notation is MM/DD/YYYY.
    */
    CLOCK_API 
    unsigned int date2days(int month,
                           int day,
                           int year )
        throw(std::range_error);

} /* Date */

#endif /* DATE_HPP */
