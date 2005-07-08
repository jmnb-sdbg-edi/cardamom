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

import cdmw.orbsupport.ExceptionMinorCodes;

import org.omg.CORBA.ORB;
import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.INTERNAL;

import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAPackage.ObjectNotActive;
import org.omg.PortableServer.POAPackage.WrongPolicy;

/**
 * Object Deactivator class for POAs with RETAIN policy
 *
 * @see NoRetainObjectDeactivator
 * @see ObjectDeactivator
 *
 */
public class RetainObjectDeactivator extends ObjectDeactivator {

    /**
     * Constructor
     *
     * @param orb A reference to the ORB pseudo-object
     */
    public RetainObjectDeactivator(org.omg.CORBA.ORB orb) {
            super(orb);
    }

    /**
     * This method does the actual deactivation of the calling object.
     * It deactivates the object with <I>oid</oid> ObjectId
     * in <I>poa</I> Portable Object Adapter.
     *
     * @param poa A reference to the POA pseudo-object
     * where the object is activated
     * @param oid ObjectId of the object to deactivate.
     */
    protected final void deactivateObject(POA poa,byte[] oid) {

        // This is used with a POA with RETAIN policy
        try {
            poa.deactivate_object(oid);
        } catch (ObjectNotActive ona) {
        	throw new
                INTERNAL(ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    CompletionStatus.COMPLETED_NO);
            // This should not happen with RETAIN policy
            // and within a request invocation
        } catch (WrongPolicy wp) {
        	throw new
                INTERNAL(ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    CompletionStatus.COMPLETED_NO);
            // This should not happen with RETAIN policy
        }

    }

}

