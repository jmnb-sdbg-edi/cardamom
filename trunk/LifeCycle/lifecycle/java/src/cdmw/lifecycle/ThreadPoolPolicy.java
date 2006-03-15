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


package cdmw.lifecycle;

import cdmw.orbsupport.StrategyList;
import cdmw.common.BadParameterException;

/**
 * Multithread strategies supporting ThreadPool policy.
 *
 */
public class ThreadPoolPolicy extends MultiThreadPolicy {

    /**
     * Size of the thread pool
     */
    private int threadPoolSize;

    /**
     * Constructor
     *
     * @param t size of the thread pool
     */
    public ThreadPoolPolicy(int t) {
        this.threadPoolSize = t;
    }

    /**
     * Copy constructor
     *
     * @param other A ThreadPoolPolicy object to be copied
     */
    public ThreadPoolPolicy(ThreadPoolPolicy other) {
        this.threadPoolSize =  other.getPoolSize();
    }

    /**
     * Returns the size of the Thread Pool
     *
     * @return the size
     */
    public int getPoolSize() {
        return this.threadPoolSize;
    }

    /**
     * Appends self to the CDMW Strategy list..
     *
     * @param sl CDMW Strategy List.
     */
    public void appendToStrategyList(StrategyList sl)
        throws BadParameterException {

        sl.addPOAThreadPool( getPoolSize() );

    }

    /**
     * Dumps self to an output stream.
     *
     * @param out Output stream.
     */
    public void print(java.io.PrintWriter out) {
        out.println(POAThreadingPolicy.THREAD_POOL_DESIGNATION);
    }

}

