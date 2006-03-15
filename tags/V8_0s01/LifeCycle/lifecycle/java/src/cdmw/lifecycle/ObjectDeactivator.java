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

import cdmw.common.Assert;

import org.omg.CORBA.ORB;
import org.omg.CORBA.ORBPackage.InvalidName;
import org.omg.CORBA.SystemException;

import org.omg.PortableServer.Current;
import org.omg.PortableServer.CurrentHelper;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.CurrentPackage.NoContext;

/**
 * Base class for Object Dectivators. An Object Deactivator is to be used
 * whenever there is a need to deactivate a CORBA object. It can only be called
 * within in the context of a CORBA operation invocation.
 *
 * @see RetainObjectDeactivator
 * @see NoRetainObjectDeactivator
 *
 */
public abstract class ObjectDeactivator {

    /**
     * Current reference
     */
    private Current current;

    /**
     * Constructor
     *
     * @param orb A reference to the ORB pseudo-object
     */
    public ObjectDeactivator(ORB orb) {

        try {
            Assert.check(orb != null);
            org.omg.CORBA.Object obj =
                orb.resolve_initial_references("POACurrent");
            this.current = CurrentHelper.narrow(obj);
        } catch(InvalidName in) {
            throw new org.omg.CORBA.BAD_PARAM();
        }

    }

    /**
     * This operation deactivates the object
     * on which the method was invoked.
     *
     * @exception NoContext If called outside the context of
     * a POA-dispatched operation.
     */
    public final void deactivateObject() throws NoContext {
        deactivateObject( getPOA(), getObjectId() );
    }

    /**
     * This operation returns a reference to the POA implementing
     * the object in whose context it is called.
     *
     * @return The POA dispatching the current CORBA operation invocation
     *
     * @exception NoContext If called outside the context of
     * a POA-dispatched operation.
     */
    public final POA getPOA() throws NoContext {
        return this.current.get_POA();
    }

    /**
     * This operation returns the ObjectId identifying the object
     * in whose context it is called.
     *
     * @return The ObjectId of the object used for the invocation
     *
     * @exception NoContext If called outside the context of
     * a POA-dispatched operation.
     */
    public final byte[] getObjectId() throws NoContext {
        return this.current.get_object_id();
    }

    /**
     * This method does the actual deactivation of the calling object.
     * It is an abtract method to be defined by derived classes.
     *
     * @param poa A reference to the POA pseudo-object where the object
     * is activated
     * @param oid ObjectId of the object to deactivate.
     */
    protected abstract void deactivateObject(POA poa,byte[] oid);

 }

