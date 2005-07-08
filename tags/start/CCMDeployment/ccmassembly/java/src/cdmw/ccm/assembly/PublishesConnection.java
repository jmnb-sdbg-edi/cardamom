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
 * with an publishesport child XML element.
 */
class PublishesConnection
    extends EventConnection
{

     /**
     * Identifier of the publishes port into the component.
     */
    private String publishesId;
    
    /**
     * Reference to the component with the publishes port.
     */
    private ObjectRef publishesComponent;
    
    /**
     * Events interface of the component with the published port.
     */
    private org.omg.Components.Events publishesEvents;
    
    /**
     * Cookie returned after connection establishment.
     */
    private org.omg.Components.Cookie cookie;

    /**
     * Constructor.
     *
     * @param id identifier of the connection.
     * @param consumesId identifier of the consumer port into the component.
     * @param consumesComponent reference to the component with consumer port.
     * @param publishesId identifier of the publishes port into the component.
     * @param publishesComponent reference to the component with publishes port.
     */
    public PublishesConnection(String id,
                               String consumesId,
                               ObjectRef consumesComponent,
                               String publishesId,
                               ObjectRef publishesComponent)
    {
        super(id, consumesId, consumesComponent);

        System.out.println("   PublishesConnection " +  publishesId + "@" +
                           publishesComponent.toString() + " -o >- " +
                           consumesId + "@" + consumesComponent.toString());

        if (publishesId == null) {
            throw new NullPointerException("publishesId is null");
        }
        if (publishesComponent == null) {
            throw new NullPointerException("publishesComponent is null");
        }

        this.publishesId = publishesId;
        this.publishesComponent = publishesComponent;
        this.publishesEvents = null;
        this.cookie = null;
    }

    /**
     * Establishes the connection between publisher and consumer.
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
        
        // retrieve publishesComponent
        org.omg.CORBA.Object publishesObj;
        try {
            publishesObj = publishesComponent.retrieveObject();
            
        } catch (ObjectRef.RetrieveException e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "specified component for publishesport not found (" +
                e.getMessage() + ").",
                PORT_NOT_FOUND.value);
        }
        
        // get Events interface of publishesComponent
        try {
            publishesEvents =
                org.omg.Components.EventsHelper.narrow(publishesObj);

        } catch (org.omg.CORBA.SystemException e) {
            // narrow failed
            throw new CreateFailure(
                "Specified component for publishesport in connectevent " +
                getId() + " is not a component.",
                NOT_A_COMPONENT.value);
        }
        
        // connect consumesport to publishesport
        try {
            cookie = publishesEvents.subscribe(publishesId, consumesPort);
        
        } catch (org.omg.Components.InvalidName e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "publishesport " + publishesId + 
                " doesn't exists in specified component.",
                PORT_NOT_FOUND.value);
        } catch (org.omg.Components.InvalidConnection e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "publishesport " + publishesId + 
                " doesn't support the declared consumesport.",
                CONNECTION_FAILED.value);
        } catch (org.omg.Components.ExceededConnectionLimit e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "publishesport " + publishesId + 
                " has reached maximum connections.",
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

        // disconnect consumesport from publishesport
        try {
            publishesEvents.unsubscribe(publishesId, cookie);

        } catch(org.omg.Components.InvalidName e) {
            throw new RemoveFailure(
                "Disconnection of " + getId() + " failed:\n" +
                "publishesport " + publishesId + " is not valid.",
                PORT_NOT_FOUND.value);
        } catch (org.omg.Components.InvalidConnection e) {
            throw new RemoveFailure(
                "Disconnection of " + getId() + " failed:\n" +
                "connection not found for this cookie.",
                DISCONNECTION_FAILED.value);
        }

        // clear cookie and publishesEvents
        cookie = null;
        publishesEvents = null;

        // set as deactivated
        isActivated = false;    
    }
    

}



