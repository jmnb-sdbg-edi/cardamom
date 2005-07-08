/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


package cdmw.lifecycle;

import org.omg.CORBA.ORB;
import org.omg.PortableServer.POA;

/**
 * Object Deactivator class for POAs with NO_RETAIN policy
 *
 * @see RetainObjectDeactivator
 * @see ObjectDeactivator
 *
 */
public class NoRetainObjectDeactivator extends ObjectDeactivator {

     /**
     * Constructor
     *
     * @param orb A reference to the ORB pseudo-object
     */
    public NoRetainObjectDeactivator(ORB orb) {
        super(orb);
    }

    /**
     * This operation does nothing for NO_RETAIN policy
     * since the POA has no Active Objects Map
     *
     * @param poa A reference to the POA pseudo-object
     * where the object is activated
     * @param oid ObjectId of the object to deactivate.
     */
    protected void deactivateObject(POA poa,byte[] oid) {

        // Does nothing for NO_RETAIN policy since the POA has
        // no Active Objects Map
        // A call to poa.deactivate_object(oid) would raise ObjectNotActive
        // exception

    }

}

