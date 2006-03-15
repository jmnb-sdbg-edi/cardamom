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

import cdmw.orbsupport.ExceptionMinorCodes;

import org.omg.CORBA.SystemException;
import org.omg.CORBA.CompletionStatus;
import org.omg.PortableServer.CurrentPackage.NoContext;

import org.omg.CosLifeCycle.LifeCycleObject;
import org.omg.CosLifeCycle.FactoryFinder;
import org.omg.CosLifeCycle.NVP;

import org.omg.CosLifeCycle.NoFactory;
import org.omg.CosLifeCycle.NotCopyable;
import org.omg.CosLifeCycle.NotMovable;
import org.omg.CosLifeCycle.InvalidCriteria;
import org.omg.CosLifeCycle.CannotMeetCriteria;
import org.omg.CosLifeCycle.NotRemovable;

/**
 * A base implementation to a LifeCycleObjectPOA. It is
 * abstract as there are still two methods to be supplied by derived classes.
 * It provides default implementations to IDL operations copy, move and remove.
 *
 * @see LifeCycleMultipleObject
 * @see LifeCycleSingleObject
 *
 */
public abstract class LifeCycleObjectBase 
    extends org.omg.CosLifeCycle.LifeCycleObjectPOA
    implements LifeCycleObjectBaseInterface {

    /**
     * An object deactivator
     */
    private ObjectDeactivator objectDeactivator;

    /**
     * Constructor
     *
     * @param deactivator A reference to an Object Deactivator to be used
     * whenever there is a need to deactivate a CORBA object incarnated by
     * the current servant (self).
     */
    public LifeCycleObjectBase(ObjectDeactivator deactivator) {
        this.objectDeactivator = deactivator;
    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/copy:1.0
    //
    /**
     * This makes a copy of the object. The copy is located in the scope
     * of the factory finder passed as the first parameter. It returns
     * an object reference to the new object initialised from the existing
     * object. Current implementation throws a NO_IMPLEMENT system exception.
     *
     * @param there A factory finder where the copy will be located
     * @param the_criteria Allows for a number of optional parameters to be passed
     * (to the factory used in creating the new object)
     *
     * @return Object reference of the copy of the object
     *
     * @exception NoFactory The target cannot find an appropriate factory
     * to create a copy
     * @exception NotCopyable The implementation refuses to copy itself
     * @exception InvalidCriteria The target does not understand the criteria
     * @exception CannotMeetCriteria The target understands the criteria
     * but cannot satisfy the criteria
     * @exception SystemException Any CORBA system exception
     */
    public LifeCycleObject copy(FactoryFinder there, NVP[] the_criteria)
        throws  NoFactory,
                NotCopyable,
                InvalidCriteria,
                CannotMeetCriteria,
                SystemException {

        throw new org.omg.CORBA.NO_IMPLEMENT(
                ExceptionMinorCodes.NO_IMPLEMENT,
                    CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/move:1.0
    //
    /**
     * This moves the object to the scope of the factory finder passed as
     * the first parameter. The object reference for the target object remains
     * valid after move has successfully executed.
     * Current implementation throws a NO_IMPLEMENT system exception.
     *
     * @param there A factory finder where the object will be moved
     * @param the_criteria Allows for a number of optional parameters to be passed
     * (to the factory used in migrating the new object)
     *
     * @return The object reference of the new object
     *
     * @exception NoFactory The target cannot find an appropriate factory
     * to support migration of the object
     * @exception NotMovable The implementation refuses to move itself
     * @exception InvalidCriteria The target does not understand the criteria
     * @exception CannotMeetCriteria The target understands the criteria
     * but cannot satisfy the criteria
     * @exception SystemException Any CORBA system exception
     */
    public void move(FactoryFinder there,NVP[] the_criteria)
        throws  NoFactory,
                NotMovable,
                InvalidCriteria,
                CannotMeetCriteria,
                SystemException {

        throw new org.omg.CORBA.NO_IMPLEMENT(
                ExceptionMinorCodes.NO_IMPLEMENT,
                    CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
    //
    /**
     * Remove instructs the object to cease to exist. The object reference
     * for the target is no longer valid after remove successfully completes.
     *
     * Current implementation deactivates the current object using
     * the ObjectDeactivator and registers it as removed.
     * <B>NOTA:</B> Any further call to <I>Remove</I> for this object will
     * raise a OBJECT_NOT_EXIST system exception.
     *
     * @exception NotRemovable An implementation that refuses to remove
     * itself should raise this exception
     * @exception SystemException Any CORBA system exception
     */
    public void remove() throws NotRemovable,SystemException {

        try {
            byte[] oid = this.objectDeactivator.getObjectId();
            if (isRemoved(oid)) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST,
                        CompletionStatus.COMPLETED_NO);
            }

            this.objectDeactivator.deactivateObject();
            registerObjectAsRemoved(oid);

        } catch (NoContext nc) {
            // Not called within the context of an operation invocation
            throw new org.omg.CORBA.INTERNAL(ExceptionMinorCodes.INTERNAL,
                CompletionStatus.COMPLETED_NO);
        }

    }


    /**
     * This checks whether the CORBA object incarnated by the current
     * object (this) has been removed (<I>remove</I> operation has been
     * called).
     *
     * @param oid The Object Id of the object incarnated by self
     *
     * @return <B>true</B> if the object has been removed,
     * <B>false</B> otherwise.
     */
    public abstract boolean isRemoved(byte[] oid);


    // The three operations bellow provide method implementations with access
    // to the identity of the object on which the method was invoked. These are
    // to be used within the context of POA-dispatched method invocations.

    /**
     * This operation returns a reference to the POA implementing
     * the object in whose context it is called.
     *
     * @return The POA dispatching the current CORBA operation invocation
     *
     * @exception NoContext If called outside the context of
     * a POA-dispatched operation.
     */
    protected final org.omg.PortableServer.POA getPOA() throws NoContext {
        return this.objectDeactivator.getPOA();
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
    protected final byte[] getObjectId() throws NoContext {
        return this.objectDeactivator.getObjectId();
    }

    /**
     * This operation deactivates the object on which the method
     * was invoked.
     *
     * @exception NoContext If called outside the context of
     * a POA-dispatched operation.
     */
    protected final void deactivateObject() throws NoContext {
        this.objectDeactivator.deactivateObject();
    }

    /**
     * This registers the object Id <I>oid</I> of the object incarnated by
     * self as removed.
     *
     * @param oid The Object Id of an object incarnated by self
     */
    protected abstract void registerObjectAsRemoved(byte[] oid);

}

