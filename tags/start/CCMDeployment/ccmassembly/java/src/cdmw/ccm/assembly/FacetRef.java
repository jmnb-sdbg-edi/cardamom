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


package cdmw.ccm.assembly;


import org.omg.Components.CreateFailure;

import com.thalesgroup.CdmwDeployment.PORT_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.NOT_A_COMPONENT;


/**
 * Class representing a reference to a facet.
 * This class is used for connections description.
 */
class FacetRef
    implements ObjectRef
{
    /**
     * Identifier of the facet into the component.
     */
    private String providesId;
    
    /**
     * Reference of the component with referenced facet.
     */
    private ObjectRef componentRef;

    /**
     * Constructor.
     *
     * @param providesId identifier of the facet port into the component.
     * @param componentRef reference to the component with the facet port.
     */
    public FacetRef(String providesId,
                    ObjectRef componentRef)
        throws cdmw.common.BadParameterException
    {
        if (providesId == null) {
            throw new NullPointerException("providesId is null");
        }
        if (componentRef == null) {
            throw new NullPointerException("componentRef is null");
        }

        if ((componentRef instanceof HomePlacementRef) ||
            (componentRef instanceof FindbyHomeFinder)) {
            throw new cdmw.common.BadParameterException(
                "A home doesn't provide facet. It cannot be used for a facet reference.");
        }

        this.providesId = providesId;
        this.componentRef = componentRef;
    }

    /**
     * Returns the String representation of this FacetRef.
     *
     * @return the String representation of this FacetRef.
     */
    public String toString() {
        return providesId + "@" + componentRef.toString();
    }

    /**
     * Returns the identifier of the facet.
     *
     * @return the facet's identifier.
     */
    String getProvidesId() {
        return providesId;
    }

    /**
     * Returns the reference to the component providing this facet.
     *
     * @return the reference to the component providing this facet.
     */
    ObjectRef getComponentRef() {
        return componentRef;
    }

    /**
     * Gets the CORBA object corresponding to the referenced
     * facet and returns it.
     *
     * @return the CORBA object corresponding to the referenced facet.
     * 
     * @exception ObjectRef.RetrieveException if the CORBA object cannot
     *            be retrieved.
     */
    public org.omg.CORBA.Object retrieveObject()
        throws ObjectRef.RetrieveException
    {
        org.omg.CORBA.Object compObj = componentRef.retrieveObject();

        System.out.println("ASSEMBLY INFO: get facet " + providesId);

        // If componentRef is a ComponentGroupRef, the ComponentGroup inherits
        // from the facet (component should be a monolithic servant).
        // In this case, the facet Object is the ComponentGroup object.
        if (componentRef instanceof ComponentGroupRef) {
            ComponentGroupRef compGroupRef = (ComponentGroupRef) componentRef;
            System.out.println("ASSEMBLY INFO: use reference of ComponentGroup " + 
                               compGroupRef.getComponent().getId() + 
                               " as reference of facet " + providesId);
            return compObj;
        }
        
        // compObj must be a CCMObject, and therefore
        // inherit from Navigation interface
        org.omg.Components.Navigation navig = null;
        try {
            navig = org.omg.Components.NavigationHelper.narrow(compObj);

        } catch (org.omg.CORBA.SystemException e) {
            // narrow failed
            throw new ObjectRef.RetrieveException(
                "Specified object for providesport " + providesId +
                " is not a component.");
        }

        // get facet object from Navigation interface
        org.omg.CORBA.Object facetObj;
        try {
            facetObj = navig.provide_facet(providesId);
        } catch (org.omg.Components.InvalidName e) {
            throw new ObjectRef.RetrieveException(
                "providesport " + providesId +
                " not found on specified component.");
        }

        return facetObj;
    }

}

