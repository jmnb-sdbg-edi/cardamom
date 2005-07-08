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
import org.omg.Components.Deployment.ServerActivator;

import com.thalesgroup.CdmwDeployment.ComponentInstallation;
import com.thalesgroup.CdmwDeployment.SERVER_ACTIVATOR_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.COMPONENT_INSTALLATION_NOT_FOUND;

import cdmw.namingandrepository.RepositoryInterface;
import cdmw.common.Locations;

/**
 * Class representing a hostcollocation, as specified into a .cad file.
 */
class HostCollocation {

    private static final String SERVER_ACTIVATOR_NAME = "ServerActivator";
    private static final String COMPONENT_INSTALLATION_NAME = "ComponentInstallation";

    /**
     * Identifier of the HostCollocation
     */
    private String id;
    
    /**
     * Parent Assembly of this HostCollocation.
     */
    private AssemblyImpl parentAssembly;
    
    /**
     * Destination of this HostCollocation.
     * This hsould be a hostname.
     */
    private String destination;
    
    /**
     * Boolean set to true if HostCollocation has been activated.
     */
    private boolean isActivated;
    
    /**
     * List of ComponentServers which are into this HostCollocation.
     */
    private java.util.Vector servers;

    /**
     * Reference to the ServerActivator present on the host.
     */
    private org.omg.Components.Deployment.ServerActivator serverActivator;


    /**
     * Constructor.
     * It automatically add the new HostCollocation to its parent assembly.
     *
     * @param id Identifier of the host collocation (empty string accepted)
     * @param parentAssembly parent Assembly of the HostCollocation.
     * @param destination destination of the HostCollocation. This should be a hostname.
     */
    public HostCollocation(String id,
                           AssemblyImpl parentAssembly,
                           String destination)
    {
        System.out.println("   HostCollocation " + id + " -> " + destination);
        if (id == null) {
            throw new NullPointerException("id is null");
        }
        if (parentAssembly == null) {
            throw new NullPointerException("parentAssembly is null");
        }
        if (destination == null) {
            throw new NullPointerException("destination is null");
        }

        this.id = id;
        this.parentAssembly = parentAssembly;
        this.destination = destination;
        isActivated = false;
        servers = new java.util.Vector();
        serverActivator = null;

        parentAssembly.addHostCollocation(this);
    }

     /**
     * Returns the identifier of this HostCollocation.
     *
     * @return the identifier of this HostCollocation.
     */
    public String getId() {
        return id;
    }

    /**
     * Returns the parent Assembly of this HostCollocation.
     *
     * @return the parent Assembly of this HostCollocation.
     */
    public AssemblyImpl getParentAssembly() {
        return parentAssembly;
    }

    /**
     * Returns the destination of this HostCollocation.
     *
     * @return the destination of this HostCollocation.
     */
    public String getDestination() {
        return destination;
    }

    /**
     * Returns true is the HostCollocation has been activated.
     *
     * @return true is the HostCollocation has been activated.
     */
    public boolean isActivated() {
        return isActivated;
    }

     /**
     * Adds a ComponentServer to this HostCollocation.
     *
     * @param server the ComponentServer to be added.
     */
    void addComponentServer(ComponentServer server) {
        servers.add(server);
    }

    /**
     * Returns the ComponentServer of this HostCollocation 
     * with the specified identifier.
     *
     * @param compSrvId the ComponentServer's identifier
     * @return the ComponentServer with the specified identifier.
     *         (null is the HostCollocation doesn't have such ComponentServer)
     */
    public ComponentServer getComponentServer(String compSrvId) {
        if (compSrvId == null)
            return null;

        java.util.Iterator it = servers.iterator();
        while (it.hasNext()) {
            ComponentServer srv = (ComponentServer) it.next();
            if (compSrvId.equals(srv.getId()))
                return srv;
        }
        
        return null;
    }

    /**
     * Returns the ServerActivator present on this host.
     *
     * @return the ServerActivator.
     *
     * @exception if the ServerActivator cannot be retrieved.
     */
    public org.omg.Components.Deployment.ServerActivator getServerActivator() 
        throws CreateFailure
    {
        // retrieve ServerActivator on destination if not already done.
        if (serverActivator == null)
            retrieveServerActivator();
            
        return serverActivator;
    }
    
    /**
     * Activates the HostCollocation.
     *  - for each ComponentServer of this HostCollocation, activates it.
     *
     * @exception CreateFailure if activation fails.
     */
    public void activate()
        throws CreateFailure
    {
        // if already active, do nothing
        if (isActivated) {
            return;
        }
        
        // set as activated
        isActivated = true;

        // for each servers
        java.util.Iterator it = servers.iterator();
        while(it.hasNext()) {
            ComponentServer server = (ComponentServer) it.next();

            // activate it
            server.activate();
        }
    }

    /**
     * Dectivates the Container.
     *  - deactivates all child ComponentServer
     *
     * @exception RemoveFailure if deactivation fails.
     */
    public void deactivate()
        throws RemoveFailure
    {
        // if not active, do nothing
        if (!isActivated) {
            return;
        }

        // for each servers (backward order)
        java.util.ListIterator it = servers.listIterator(servers.size());
        while(it.hasPrevious()) {
            ComponentServer server = (ComponentServer) it.previous();

            // deactivate it
            server.deactivate();
        }

        // disconnect from ServerActivator
        serverActivator = null;

        // set as deactivated
        isActivated = false;
    }


    /**
     * Retrieves from Repository the ServerActivator present on this host.
     * 
     * @exception if the ServerActivator cannot be retrieved.
     */
    private void retrieveServerActivator() 
        throws CreateFailure
    {
        // Get AdminRootContext NamingInterface object from Repository
        cdmw.namingandrepository.NamingInterface adminNaming = null;
        try {
            // Get admin root naming context
            org.omg.CosNaming.NamingContext adminRoot =
                RepositoryInterface.getRepository().resolve_root_context(
                    cdmw.common.Locations.ADMIN_ROOT_CONTEXT_ID);

            // create naming interface on admin root context
            adminNaming = new cdmw.namingandrepository.NamingInterface(adminRoot);
            
        } catch (com.thalesgroup.CdmwNamingAndRepository
                                .RepositoryPackage.NoRootContext ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        }
    
        // Get ServerActivator for host "destination" from RepositoryInterface
        try {
            org.omg.CORBA.Object obj = 
                adminNaming.resolve(destination + "/" + SERVER_ACTIVATOR_NAME);
            serverActivator = 
                org.omg.Components.Deployment.ServerActivatorHelper.narrow(obj);
            
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound ex) {
            throw new CreateFailure(
                "There is no object registred with " + 
                Locations.ADMIN_ROOT_CONTEXT_ID + "/" + destination + "/" + 
                SERVER_ACTIVATOR_NAME + " in Repository.",
                SERVER_ACTIVATOR_NOT_FOUND.value);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed ex) {
            throw new CreateFailure(
                "Error while retrieving ServerActivator on host " + 
                destination + " :\n" + ex.getMessage(),
                SERVER_ACTIVATOR_NOT_FOUND.value);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName ex) {
            throw new CreateFailure(
                "Error while retrieving ServerActivator on host " +
                destination + ". " + destination + "/" + SERVER_ACTIVATOR_NAME +
                " is an invalid name.",
                SERVER_ACTIVATOR_NOT_FOUND.value);
        } catch (org.omg.CORBA.BAD_PARAM ex) {
            // narrow failed
            throw new CreateFailure(
                "Error while retrieving ServerActivator on host " +
                destination + ". The registred object is not a ServerActivator.",
                SERVER_ACTIVATOR_NOT_FOUND.value);            
        }
    
    }
    

}


