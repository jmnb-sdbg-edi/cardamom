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


package cdmw.trace;

/**
 * This class defines trace level used in Cdmw to output information.
 *
 */
public class TraceLevels {

    /**
     * Information purpose
     */
    public static final short INF = 0;

    /**
     * Maybe the user has made an error, so we warn it
     */
    public static final short WRN = 1;

    /**
     * This level report an error from Cdmw.
     */
    public static final short ERR = 2;

    /**
     * The level reports an error from Cdmw, Cdmw is no more
     * able to process safely.
     */
    public static final short FTL = 3;

}

