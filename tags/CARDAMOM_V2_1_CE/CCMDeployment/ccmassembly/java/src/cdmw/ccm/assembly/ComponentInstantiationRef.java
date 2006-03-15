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

package cdmw.ccm.assembly;

import org.omg.Components.CreateFailure;
import org.omg.Components.RemoveFailure;


/**
 *  Class representing a reference to a Component. Such a reference is specified
 *  into .cad files with a componentinstantiationref XML element.
 *  This class is used for connections description.
 */
class ComponentInstantiationRef
     implements ObjectRef {

    /**
     * The referenced Component.
     */
    private Component component;


    /**
     * Constructor.
     *
     * @param component the referenced Component.
     */
    public ComponentInstantiationRef(Component component) {
        if (component == null) {
            throw new NullPointerException();
        }

        this.component = component;
    }


    /**
     * Returns the String representation of this ComponentInstantiationRef object.
     *
     * @return the String representation of this ComponentInstantiationRef object.
     */
    public String toString() {
        return "component(" + component.getId() + ")";
    }


    /**
     * Returns the Component referenced by this object.
     *
     * @return The referenced Component.
     */
    Component getComponent() {
        return component;
    }


    /**
     * Gets the CORBA object corresponding to the referenced
     * component and returns it.
     *
     * @return the CORBA object corresponding to the referenced component.
     * 
     * @exception ObjectRef.RetrieveException if the CORBA object cannot
     *            be retrieved.
     */
    public org.omg.CORBA.Object retrieveObject()
        throws ObjectRef.RetrieveException {
        System.out.println("ASSEMBLY INFO: get component " + component.getId());

        if (!component.isActivated()) {
            throw new ObjectRef.RetrieveException(
                "Component " + component.getId() + " is not activated.");
        }

        return component.getComponent();
    }

}


