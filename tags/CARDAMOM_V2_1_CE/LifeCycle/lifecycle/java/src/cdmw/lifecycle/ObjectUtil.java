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
import cdmw.orbsupport.ExceptionMinorCodes;

import org.omg.CORBA.SystemException;
import org.omg.CORBA.CompletionStatus;

import org.omg.PortableServer.POA;
import org.omg.PortableServer.Servant;
import org.omg.PortableServer.ForwardRequest;
import org.omg.PortableServer.POAPackage.WrongPolicy;
import org.omg.PortableServer.POAPackage.ObjectAlreadyActive;
import org.omg.PortableServer.POAPackage.ServantAlreadyActive;

/**
 * A utility class for use by the LifeCycle Framewotk
 * for object creation and activation.
 *
 */

public class ObjectUtil {

    /**
     * This creates a corba object without activating it. It first gets
     * the repository Id of the object from the servant provider, creates
     * the object reference and then asserts the object is really
     * of the expected type.
     *
     * @param poa The POA responsible for creating the object reference
     * @param oid The ObjectId to use for the object reference
     * @param servantProvider The Servant Provider object reference
     *
     * @return A new object reference narrowed to the appropriate type
     *
     * @exception INTERNAL Indicates that a ServantProvider or a LifeCycle
     * Framework error
     */
    public static org.omg.CORBA.Object createObjectReference(
        POA poa,byte[] oid,ServantProviderBase servantProvider)
        throws org.omg.CORBA.INTERNAL {

        // [a] Get the repository Id
        String repid = servantProvider.getSupportedInterface(oid);

        // [b] Create the object reference
        org.omg.CORBA.Object obj = null;
        try {
             obj = poa.create_reference_with_id(oid,repid);
            Assert.check( obj!=null );
        } catch (SystemException se) {
            // This should not happen at this stage since the object is just
            // about to be created and the servant provider has created
            // the object id.
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALServantProviderError,
                    CompletionStatus.COMPLETED_YES);
        } catch (Exception e /* JacORB raises WrongPolicy wp */) {
            // Thrown by activate_object_with_id.
            // This should not happen as we are using RETAIN policy.
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    CompletionStatus.COMPLETED_YES);
        }
        return obj;
    }


    /**
     * This activates an object with the specified <I>oid</I>. It first
     * acquires a servant incarnating the object from the servant provider,
     * activates it through the specified POA, retrieves its object's reference
     * and then asserts it is really of the expected type.
     *
     * @param poa The POA responsible for activating the object
     * @param oid The ObjectId of the CORBA object
     * @param servantProvider The Servant Provider object reference
     *
     * @return The object reference of the CORBA object narrowed
     * to the appropriate type
     *
     * @exception INTERNAL Indicates that a ServantProvider or a LifeCycle
     * Framework error
     */
    public static org.omg.CORBA.Object
        activateObject(POA poa, byte[] oid,ServantProviderBase servantProvider)
        throws org.omg.CORBA.INTERNAL {

        // [a] Acquire servant from user for oid
        try {
            Servant servant = servantProvider.lookupServant(oid);

            // [b] Activate object
            // We are using a servant activator
            poa.activate_object_with_id(oid,servant);
        } catch (ForwardRequest fr) {
            // Thrown by lookup_servant.
            // This should not happen at this stage since the object is just
            // about to be created and the previous call
            // to create_resource should have failed.
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALServantProviderError,
                    CompletionStatus.COMPLETED_YES);
        } catch (ServantAlreadyActive saa) {
            // Thrown by activate_object_with_id.
            // We use UNIQUE_ID policy so the servant should not be reused for
            // another ObjectId with this POA.
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALServantProviderError,
                    CompletionStatus.COMPLETED_YES);
        } catch (ObjectAlreadyActive oaa) {
            // Thrown by activate_object_with_id.
            // The object is already active!
            // Has the servant provider activated it?
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALServantProviderError,
                    CompletionStatus.COMPLETED_YES);
        } catch (WrongPolicy wp) {
            // Thrown by activate_object_with_id.
            // This should not happen as we are using RETAIN policy.
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    CompletionStatus.COMPLETED_YES);
        } catch (SystemException se) {
            // This should not happen at this stage since the object is just
            // about to be created and the previous call
            // to create_resource should have failed.
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALServantProviderError,
                    CompletionStatus.COMPLETED_YES);
        }

        // [c] Create object reference
        // This may raise ObjectNotActive,WrongPolicy, and SystemException
        org.omg.CORBA.Object obj = null;
        try {
            obj = poa.id_to_reference(oid);
            Assert.check( obj!=null );
        } catch (Exception e) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    CompletionStatus.COMPLETED_YES);
        }
        return obj;
    }

}

