/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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


package cdmw.ossupport;

import java.util.Date;

/**
 * This class provides helper functions to get time information
 * needed by the Trace CSC
 *
 * Features:
 * Thread safe, exception safe
 *
 */
public class Timeval {

    /**
     * Date object used to store time information
     */
    private Date date;

    /**
     * Precision of time in microseconds.
     * (this attribute is < 1000)
     */
    private int microseconds;
    
    
    /**
     * Allocates a Timeval object and initializes it to represent
     * the specified number of milliseconds since the standard base time known
     * as "the epoch", namely January 1, 1970, 00:00:00 GMT.
     *
     * @param millis The number of milliseconds since "the epoch"
     */
    public Timeval(long millis) {
        this.date = new Date(millis);
        this.microseconds = 0;
    }

    /**
     * Allocates a Timeval object and initializes it to represent
     * the specified number of seconds and microseconds (< 1000000)
     * since the standard base time known as "the epoch", 
     * namely January 1, 1970, 00:00:00 GMT.
     * 
     * @param seconds the number of seconds since "the epoch"
     * @param microseconds the number of remaining microseconds since "the epoch"
     */
    public Timeval(int seconds, int microseconds) {
        int milliseconds = microseconds/1000;
        this.microseconds = microseconds % 1000;
        this.date = new Date((long) seconds*1000 + milliseconds);
    }
    
    
    
    
    /**
     * Returns a date object corresponding to the number of milliseconds
     *
     * @return A corresponding date object.
     */
    public Date toDate() {
        return this.date;
    }

    /**
     * Returns the number of microseconds remaining
     * from the number of seconds since "the epoch"
     * <p/><B>NB:</B> If the Timeval has been constructed with
     * the time in millisecond, the time precision cannot
     * exceed millisecond precision.
     *
     * @return The number of microseconds remaining
     * from the number of seconds since "the epoch"
     */
    public int getMicroseconds() {
        long millisec = (getMilliseconds() % 1000) * 1000;
        return (int) millisec + microseconds;
    }

    /**
     * Returns the number of seconds since "the epoch"
     *
     * @return The number of seconds since "the epoch"
     */
    public int getSeconds() {
        return (int)(this.date.getTime() / 1000);
    }

    /**
     * Returns the number of milliseconds since "the epoch"
     *
     * @return The number of milliseconds since "the epoch"
     */
    public long getMilliseconds() {
        return this.date.getTime();
    }

    /**
     * Returns a String representation of this Timeval object
     *
     * @return The String representation of this Timeval object
     */
    public String toString() {
        return this.date.toString();
    }

}

