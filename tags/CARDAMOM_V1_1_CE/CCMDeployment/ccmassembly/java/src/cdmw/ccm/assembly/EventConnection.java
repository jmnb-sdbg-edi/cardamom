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
import org.omg.Components.RemoveFailure;

import com.thalesgroup.CdmwDeployment.PORT_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.NOT_A_COMPONENT;


/**
 * Abstract class representing a connectevent XML element
 * from a .cad file.
 */
abstract class EventConnection
    extends Connection
{
    
    /**
     * Identifier of the consumer port into the component.
     */
    protected String consumesId;
    
    /**
     * Reference to the component with the consumer port.
     */
    protected ObjectRef consumesComponent;
    
    
    /**
     * Constructor.
     * 
     * @param id identifier of the connection.
     * @param consumesId identifier of the consumer port into the component.
     * @param consumesComponent reference to the component with consumer port.
     */
    public EventConnection(String id,
                           String consumesId,
                           ObjectRef consumesComponent)
    {
        super(id);
        
        if (consumesId == null) {
            throw new NullPointerException("consumesId is null");
        }
        if (consumesComponent == null) {
            throw new NullPointerException("consumesComponent is null");
        }
        
        this.consumesId = consumesId;
        this.consumesComponent = consumesComponent;
    }


    /**
     * Returns the consumer CORBA object corresponding to the consumer identifier.
     *
     * @return the consumer CORBA object.
     */
    protected org.omg.Components.EventConsumerBase getConsumesPort() 
        throws CreateFailure
    {
        // retrieve consumesComponent
        org.omg.CORBA.Object consumesObj;
        try {
            consumesObj = consumesComponent.retrieveObject();
            
        } catch (ObjectRef.RetrieveException e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "specified component for consumesport not found (" +
                e.getMessage() + ").",
                PORT_NOT_FOUND.value);
        }

        // If consumesComponent is a ComponentGroupRef, the ComponentGroup inherits
        // from the consumer (component should be a monolithic servant).
        // In this case, the consumer Object is the ComponentGroup object.
        if (consumesComponent instanceof ComponentGroupRef) {
            ComponentGroupRef compGroupRef = (ComponentGroupRef) consumesComponent;
            System.out.println("ASSEMBLY INFO: use reference of ComponentGroup " + 
                               compGroupRef.getComponent().getId() + 
                               " as reference of consumer " + consumesId);

            org.omg.Components.EventConsumerBase consumesPort = null;
            try {
                consumesPort = org.omg.Components.EventConsumerBaseHelper.narrow(consumesObj);
            } catch (org.omg.CORBA.BAD_PARAM e) {
                throw new CreateFailure(
                    "ComponentGroup " + compGroupRef.getComponent().getId() +
                    " doesn't inherit from org.omg.Components.EventConsumerBase " +
                    " despite it should have a consumer port named " + consumesId,
                    PORT_NOT_FOUND.value);
            }
            return consumesPort;
        }

        
        // get Events interface of consumesComponent
        org.omg.Components.Events consumesEvents;
        try {
            consumesEvents =
                org.omg.Components.EventsHelper.narrow(consumesObj);

        } catch (org.omg.CORBA.SystemException e) {
            // narrow failed
            throw new CreateFailure(
                "Specified component for consumesport in connectevent " +
                getId() + " is not a component.",
                NOT_A_COMPONENT.value);
        }
        
        // retrieve consumesPort from Events interface
        org.omg.Components.EventConsumerBase consumesPort;
        try {
            consumesPort = consumesEvents.get_consumer(consumesId);
            
        } catch (org.omg.Components.InvalidName e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "consumesport " + consumesId + 
                " doesn't exist in specified component.",
                PORT_NOT_FOUND.value);
        }
        
        return consumesPort;
    }
       
    
}



