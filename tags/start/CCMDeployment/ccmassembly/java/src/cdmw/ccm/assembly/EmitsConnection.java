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
import com.thalesgroup.CdmwDeployment.CONNECTION_FAILED;
import com.thalesgroup.CdmwDeployment.DISCONNECTION_FAILED;


/**
 * Class representing a connectevent XML element
 * with an emitsport child XML element.
 */
class EmitsConnection
    extends EventConnection
{
    
    /**
     * Identifier of the emitter port into the component.
     */
    private String emitsId;

    /**
     * Reference to the component with the emitter port.
     */
    private ObjectRef emitsComponent;
    
    /**
     * Events interface of the component with the emitter port.
     */
    private org.omg.Components.Events emitsEvents;

    
    /**
     * Constructor.
     *
     * @param id identifier of the connection.
     * @param consumesId identifier of the consumer port into the component.
     * @param consumesComponent reference to the component with consumer port.
     * @param emitsId identifier of the emitter port into the component.
     * @param emitsComponent reference to the component with emitter port.
     */
    public EmitsConnection(String id,
                           String consumesId,
                           ObjectRef consumesComponent,
                           String emitsId,
                           ObjectRef emitsComponent)
    {
        super(id, consumesId, consumesComponent);

        System.out.println("   EmitsConnection " +  emitsId + "@" +
                           emitsComponent.toString() + " -o >- " +
                           consumesId + "@" + consumesComponent.toString());

        if (emitsId == null) {
            throw new NullPointerException("emitsId is null");
        }
        if (emitsComponent == null) {
            throw new NullPointerException("emitsComponent is null");
        }

        this.emitsId = emitsId;
        this.emitsComponent = emitsComponent;
        this.emitsEvents = null;
    }


    /**
     * Establishes the connection between emitter and consumer.
     *
     * @exception CreateFailure if the connection establishment fails.
     */
    public void activate()
        throws CreateFailure 
    {
        // if already active, do nothing
        if (isActivated) {
            return;
        }

        System.out.println("ASSEMBLY INFO: establish connection " + getId());

        // retrieve consumesPort
        org.omg.Components.EventConsumerBase consumesPort = getConsumesPort();
        
        // retrieve emitsComponent
        org.omg.CORBA.Object emitsObj;
        try {
            emitsObj = emitsComponent.retrieveObject();
            
        } catch (ObjectRef.RetrieveException e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "specified component for emitsport not found (" +
                e.getMessage() + ").",
                PORT_NOT_FOUND.value);
        }

        // get Events interface of emitsComponent
        try {
            emitsEvents =
                org.omg.Components.EventsHelper.narrow(emitsObj);

        } catch (org.omg.CORBA.SystemException e) {
            // narrow failed
            throw new CreateFailure(
                "Specified component for emitsport in connectevent " +
                getId() + " is not a component.",
                NOT_A_COMPONENT.value);
        }
        
        // connect consumesport to emitsport
        try {
            emitsEvents.connect_consumer(emitsId, consumesPort);
        
        } catch (org.omg.Components.InvalidName e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "emitsport " + emitsId + 
                " doesn't exists in specified component.",
                PORT_NOT_FOUND.value);
        } catch (org.omg.Components.InvalidConnection e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "emitsport " + emitsId + " doesn't support the declared consumesport.",
                CONNECTION_FAILED.value);
        } catch (org.omg.Components.AlreadyConnected e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "emitsport " + emitsId + " is already connected.",
                CONNECTION_FAILED.value);
        }
        
        // set as activated
        isActivated = true;
    }

    
    /**
     * Removes the connection.
     *
     * @exception RemoveFailure if the connection removal fails.
     */
    public void deactivate()
        throws RemoveFailure 
    {
        // if not active, do nothing
        if (!isActivated) {
            return;
        }

        System.out.println("ASSEMBLY INFO: remove connection " + getId());

        // disconnect consumesport from emitsport
        try {
            emitsEvents.disconnect_consumer(emitsId);

        } catch(org.omg.Components.InvalidName e) {
            throw new RemoveFailure(
                "Disconnection of " + getId() + " failed:\n" +
                "emitsport " + emitsId + " is not valid.",
                PORT_NOT_FOUND.value);
        } catch (org.omg.Components.NoConnection e) {
            throw new RemoveFailure(
                "Disconnection of " + getId() + " failed:\n" +
                "there is no connection for this emitsport.",
                DISCONNECTION_FAILED.value);
        }

        // clear emitsEvents
        emitsEvents = null;

        // set as deactivated
        isActivated = false;    
    }
    

}


