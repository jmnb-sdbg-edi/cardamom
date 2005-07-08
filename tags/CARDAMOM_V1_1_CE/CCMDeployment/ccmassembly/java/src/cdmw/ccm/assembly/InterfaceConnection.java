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
 * Class representing a connectinterface XML element
 * from a .cad file.
 */
class InterfaceConnection
    extends Connection
{

    /**
     * Identifier of the uses port into the component.
     */
    private String usesId;
    
    /**
     * Reference to the component with the uses port.
     */
    private ObjectRef usesComponent;
    
    /**
     * Reference to the object to be connected to the uses port.
     */
    private ObjectRef objToBeConnected;
    
    /**
     * Receptacles interface of the component with the uses port. 
     */
    private org.omg.Components.Receptacles receptacles;
    
    /**
     * Cookie returned after connection establishment.
     */
    private org.omg.Components.Cookie cookie;


    /**
     * Constructor.
     *
     * @param id identifier of the connection.
     * @param usesId identifier of the uses port into the component.
     * @param usesComponent reference to the component with uses port.
     * @param objToBeConnected reference to the object to be connected
     *        to the uses port.
     */
    public InterfaceConnection(String id,
                               String usesId,
                               ObjectRef usesComponent,
                               ObjectRef objToBeConnected)
    {
        super(id);

        System.out.println("   InterfaceConnection " +  objToBeConnected.toString() +
                           " -o >- " + usesId + "@" + usesComponent.toString());

        if (usesId == null) {
            throw new NullPointerException("usesId is null");
        }
        if (usesComponent == null) {
            throw new NullPointerException("usesComponent is null");
        }
        if (objToBeConnected == null) {
            throw new NullPointerException("objToBeConnected is null");
        }

        this.usesId = usesId;
        this.usesComponent = usesComponent;
        this.objToBeConnected = objToBeConnected;
        receptacles = null;
        cookie = null;
    }


    /**
     * Establishes the connection between uses port and the object.
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

        // retrieve object to be connected
         org.omg.CORBA.Object objConnection;
        try {
            objConnection = objToBeConnected.retrieveObject();

        } catch (ObjectRef.RetrieveException e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "object to be connected to usesport not found (" +
                e.getMessage() + ").",
                PORT_NOT_FOUND.value);
        }

        // retrieve usesComponent
        org.omg.CORBA.Object usesObj;
        try {
            usesObj = usesComponent.retrieveObject();

        } catch (ObjectRef.RetrieveException e) {
            // cannot retrieve usesComponent
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "usesport " + usesId + " not found (" + e.getMessage() + ").",
                PORT_NOT_FOUND.value);
        }

        // get Receptacles interface of usesComponent
        try {
            receptacles =
                org.omg.Components.ReceptaclesHelper.narrow(usesObj);

        } catch (org.omg.CORBA.SystemException e) {
            // narrow failed
            throw new CreateFailure(
                "Specified component for usesport in connectinterface " +
                getId() + " is not a component.",
                NOT_A_COMPONENT.value);
        }

        // connect object to be connected to receptacle 'usesName'
        try {
            cookie = receptacles.connect(usesId, objConnection);

        } catch (org.omg.Components.InvalidName e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "usesport " + usesId + 
                " doesn't exists in specified component.",
                PORT_NOT_FOUND.value);
        } catch (org.omg.Components.InvalidConnection e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "usesport " + usesId + " doesn't support the declared interface.",
                CONNECTION_FAILED.value);
        } catch (org.omg.Components.AlreadyConnected e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "usesport " + usesId + " is already connected.",
                CONNECTION_FAILED.value);
        } catch (org.omg.Components.ExceededConnectionLimit e) {
            throw new CreateFailure(
                "Connection " + getId() + " failed:\n" +
                "usesport " + usesId + " has reached maximum connections.",
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

        // disconnect object from receptacle 'usesName'
        try {
            receptacles.disconnect(usesId, cookie);

        } catch(org.omg.Components.InvalidName e) {
            throw new RemoveFailure(
                "Disconnection of " + getId() + " failed:\n" +
                "usesport " + usesId + " is not valid.",
                PORT_NOT_FOUND.value);
        } catch (org.omg.Components.InvalidConnection e) {
            throw new RemoveFailure(
                "Disconnection of " + getId() + " failed:\n" +
                "connection not found for this cookie.",
                DISCONNECTION_FAILED.value);
        } catch (org.omg.Components.CookieRequired e) {
            throw new RemoveFailure(
                "Disconnection of " + getId() + " failed:\n" +
                "a Cookie is required for disconnection.",
                DISCONNECTION_FAILED.value);
        } catch (org.omg.Components.NoConnection e) {
            throw new RemoveFailure(
                "Disconnection of " + getId() + " failed:\n" +
                "there is no connection for this usesport.",
                DISCONNECTION_FAILED.value);
        }

        // clear cookie and receptacles
        cookie = null;
        receptacles = null;

        // set as deactivated
        isActivated = false;
    }


}



