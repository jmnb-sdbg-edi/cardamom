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

/**
 * This class specialises LifeCycleObjectBase for servant incarnating
 * a single CORBA object.
 *
 * @see LifeCycleMultipleObject
 * @see LifeCycleObjectBase
 *
 */
public class LifeCycleSingleObject extends LifeCycleObjectBase {

    /**
     * Indicates whether the remove operation has been called or not.
     */
    private boolean removed = false;

    /**
     * Constructor
     *
     * @param deactivator A reference to an Object Deactivator to be used
     * whenever there is a need to deactivate a CORBA object incarnated by
     * the current servant (self).
     */
    public LifeCycleSingleObject(ObjectDeactivator deactivator) {
        super(deactivator);
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
    public boolean isRemoved(byte[] oid) {
        return this.removed;
    }

    /**
     * This registers the object Id <I>oid</I> of the object incarnated by
     * self as removed.
     *
     * @param oid The Object Id of the object incarnated by self
     */
    protected void registerObjectAsRemoved(byte[] oid) {
        this.removed = true;
    }

}

