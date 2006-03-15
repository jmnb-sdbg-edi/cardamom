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
 * Pure base class for all Threading Policies
 *
 */
abstract public class POAThreadingPolicy {

    /**
     * Single Thread Policy Designation
     */
    public static final String SINGLE_THREAD_DESIGNATION
        = "Single Thread Policy";

    /**
     * Thread Pool Policy Designation
     */
    public static final String THREAD_POOL_DESIGNATION
        = "Thread Pool Policy";

    /**
     * Thread Per Request Policy Designation
     */
    public static final String THREAD_PER_REQUEST_DESIGNATION
        = "Thread Per Request Policy";

    /**
     * Appends self to the CDMW Strategy list..
     *
     * @param sl CDMW Strategy List.
     */
    public abstract void appendToStrategyList(StrategyList sl)
        throws BadParameterException;

    /**
     * Dumps self to an output stream.
     *
     * @param out Output stream.
     */
    public abstract void print(java.io.PrintWriter out);

}

