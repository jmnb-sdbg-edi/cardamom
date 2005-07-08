/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
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
 * =========================================================================== */


package cdmw.trace;


/**
 * This class provide simple access to trace functionnalities.
 * 
 * Features:
 * Utility class (only static methods). Thread safe.
 * 
 */
public class Tracer {
    
    
    /**
     * private constructor (this is an Utility class)
     */
    private Tracer() {
    }
    
    /**
     *  Activate trace message flushing.
     */
    public static void activeFlushing() {
        FlushAreaMngr.getInstance().activateFlushing();
    }
    
    /**
     *  Deactivate trace message flushing.
     */
    public static void deactiveFlushing() {
        FlushAreaMngr.getInstance().deactivateFlushing();
    }
    
    
    
    /**
     * Add a new trace to be send to collectors if
     * domain and level are actives.
     *
     * @param domain The domain of the trace.
     * @param level  The level of the trace.
     * @param str    The message to be traced.
     */
    public static void trace(String domain, 
                             short  level, 
                             String str) 
    {
        // Stream.getInstance() return a specific stream object 
        // for each calling thread.
        traceWithStream(Stream.getInstance(),
                        domain,
                        level,
                        str);
    }
 
 
 
    /**
     * Add a new trace to be send to collectors if
     * domain and level are actives.
     * This trace is constructed into a specified stream
     * wich must be only used by current thread.
     *
     * @param stream The stream for trace construction.
     * @param domain The domain of the trace.
     * @param level  The level of the trace.
     * @param str    The message to be traced.
     */
    public static void traceWithStream(Stream stream, 
                                       String domain, 
                                       short    level, 
                                       String str) 
    {
        if (FlushAreaMngr.isToBeTraced(domain, level)) {
            stream.setLine(0);
            stream.setFile("");
            stream.setUserDomain(domain);
            stream.setUserLevel(level);
            stream.setMessage(str); 
            stream.doFlush();           
        }
    }


}

