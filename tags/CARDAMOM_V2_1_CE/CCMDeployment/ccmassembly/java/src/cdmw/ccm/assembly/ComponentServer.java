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
import org.omg.Components.ConfigValue;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.INVALID_CONFIGURATION_FOR_COMPONENT_SERVER;
import com.thalesgroup.CdmwDeployment.PROCESS_DESTINATION;
import com.thalesgroup.CdmwDeployment.DESTINATION_NOT_CONFORM;
import com.thalesgroup.CdmwDeployment.ILLEGAL_STATE;

import cdmw.ccm.deployment.ConfigValueImpl;
import cdmw.ccm.deployment.ConfigValues;


/**
 * Class representing a ComponentServer.
 */
class ComponentServer {

    /**
     * Identifier of the ComponentServer, as specified into
     * the processcollocation XML element of the .cad file.
     */
    private String id;
    
    /**
     * Parent HostCollocation.
     */
    private HostCollocation parentHost;
    
    /**
     * Map of Containers which are into this ComponentServer.
     * They are indexed by their identifier.
     */
    private cdmw.ccm.deployment.OrderedMap containers;
    
    /**
     * The ComponentServer CORBA object this class represent.
     */
    private org.omg.Components.Deployment.ComponentServer componentServer;
    
    /**
     * Boolean set to true if ComponentServer has been activated.
     */
    private boolean isActivated;

    /**
     * Destination of the ComponentServer, as specified into
     * the processcollocation XML element of the .cad file.
     */
    private String processDestination;


    /**
     * Constructor.
     * It automatically add the new ComponentServer to its parent 
     * HostCollocation and to its parent assembly.
     *
     * @param id Identifier of the ComponentServer (empty string accepted)
     * @param parentHost parent HostCollocation of the ComponentServer.
     * @param destination process destination of ComponentServer. 
     *        This destination will be used by the ServerActivator to return the ComponentServer reference.
     * @exception CreateFailure if the destination is not conform to the pattern:
     *            'processname'@'applicationname'
     */
    public ComponentServer (String id,
                            HostCollocation parentHost,
                            String processDestination)
        throws CreateFailure
    {
        System.out.println("      ComponentServer " + id);
        if (id == null) {
            throw new NullPointerException("id is null");
        }
        if (parentHost == null) {
            throw new NullPointerException("parentHost is null");
        }
        if (processDestination == null) {
            throw new NullPointerException("processDestination is null");
        }
        int index = processDestination.indexOf("@");
        if (index <= 0 || index >= (processDestination.length()-1)) {
            throw new CreateFailure(
                "Error in destination of process " + id + 
                "(" + processDestination + "): " +
                "It have the format 'processname@applicationname'.",
                DESTINATION_NOT_CONFORM.value);
        }

        this.id = id;
        this.parentHost = parentHost;
        this.processDestination = processDestination;
        containers = new cdmw.ccm.deployment.OrderedMap();
        componentServer = null;
        isActivated = false;
        
        parentHost.addComponentServer(this);
        parentHost.getParentAssembly().addComponentServer(this);
    }

    /**
     * Returns the identifier of this ComponentServer.
     *
     * @return the identifier of this ComponentServer.
     */
    public String getId() {
        return id;
    }

    /**
     * Returns the parent HostCollocation of this ComponentServer.
     *
     * @return the parent HostCollocation of this ComponentServer.
     */
    public HostCollocation getParentHost() {
        return parentHost;
    }
    
    /**
     * Returns the parent Assembly of this ComponentServer.
     *
     * @return the parent Assembly of this ComponentServer.
     */
    public AssemblyImpl getParentAssembly() {
        return parentHost.getParentAssembly();
    }

    /**
     * Returns the destination of this ComponentServer.
     *
     * @return the destination of this ComponentServer.
     */
    public String getProcessDestination() {
        return processDestination;
    }

    /**
     * Returns the application name of this ComponentServer, as specified
     * in the first part of its destination (processname@applicationname)
     *
     * @return the application name of this ComponentServer
     */
    public String getApplicationName() {
        try {
            return processDestination.substring(processDestination.indexOf("@")+1);
        } catch (IndexOutOfBoundsException e) {
            // shouldn't happen since the validity of '@' position 
            // in processDestination have been checked in constructor
            cdmw.common.Assert.check(false);
            // for compiler (unused)
            return null;
        }
    }
    
    /**
     * Returns the process name of this ComponentServer, as specified
     * in the first part of its destination (processname@applicationname)
     *
     * @return the process name of this ComponentServer
     */
    public String getProcessName() {
        try {
            return processDestination.substring(0, processDestination.indexOf("@"));
        } catch (IndexOutOfBoundsException e) {
            // shouldn't happen since the validity of '@' position 
            // in processDestination have been checked in constructor
            cdmw.common.Assert.check(false);
            // for compiler (unused)
            return null;
        }
    }

    /**
     * Returns true is the ComponentServer has been activated.
     *
     * @return true is the ComponentServer has been activated.
     */
    public boolean isActivated() {
        return isActivated;
    }

    /**
     * Returns the ComponentServer CORBA object corresponding 
     * to this ComponentServer.
     *
     * @return the ComponentServer CORBA object.
     *         (null is the ComponentServer is not active)
     */
   public org.omg.Components.Deployment.ComponentServer getComponentServer() {
        return componentServer;
    }

    /**
     * Adds a Container to this ComponentServer.
     *
     * @param container the Container to be added.
     */
    void addContainer(Container container) {
        containers.put(container.getId(), container);
    }

    /**
     * Returns the Container of this ComponentServer with the
     * specified identifier.
     *
     * @param containerId the Container's identifier
     * @return the Container with the specified identifier.
     *         (null is the ComponentServer doesn't have such Container)
     */
    public Container getContainer(String containerId) {
        return (Container) containers.get(containerId);
    }

    /**
     * Finds a Container with the specified ComponentKindValue 
     * in the ComponentServer's Containers list.
     * Returns null if no such Container was found.
     * 
     * @param componentKind the component kind of the Container 
     *        to be found.
     * @return a Container with the specified ComponentKindValue
     *        or null if no such Container was found.
     */
    public Container findContainer(ComponentKindValue componentKind) {
        java.util.Iterator it = containers.values().iterator();

        while(it.hasNext()) {
            Container container = (Container) it.next();
            if (container.getComponentKind() == componentKind) {
                System.out.println("         Container " + container.getId() + " re-used !");
                return container;
            }
        }
        
        return null;
    }

    /**
     * Returns the config values to be used for Container CORBA object creation.
     *
     * @return the Container's config values.
     * @exception if an error is detected in Container configuration.
     */
    protected ConfigValue[] getConfig()
        throws CreateFailure
    {
        ConfigValues config = new ConfigValues();

        // add PROCESS_DESTINATION config
        if (processDestination != null) {
            ConfigValue c = new ConfigValueImpl();
            c.name = PROCESS_DESTINATION.value;
            c.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
            c.value.insert_string(processDestination);
            config.setConfigValue(c);
        }

        return config.getConfigValueArray();
    }


    /**
     * Activates the ComponentServer.
     *  - contacts the ServerActivator of the host
     *  - requests it to create the ComponentServer
     *  - for each Container of this ComponentServer, activates it.
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

        System.out.println("ASSEMBLY INFO: create component server " + id);

        if (! parentHost.isActivated()) {
            throw new CreateFailure(
                "The parent HostCollocation ("+ parentHost.getId() + 
                ") of ComponentServer " + id + " is not activated",
                ILLEGAL_STATE.value);
        }
        
        // create ComponentServer
        try {
            org.omg.Components.Deployment.ServerActivator serverActivator = 
                parentHost.getServerActivator();
            componentServer =
                serverActivator.create_component_server(getConfig());
        } catch (org.omg.Components.Deployment.InvalidConfiguration e) {
            throw new CreateFailure(
                "The ComponentServer " + id + " has an invalid configuration:\n" + 
                invalidConfigurationToString(e),
                INVALID_CONFIGURATION_FOR_COMPONENT_SERVER.value);
        }

        // set as activated
        isActivated = true;

        // for each container
        java.util.Iterator it = containers.values().iterator();
        while(it.hasNext()) {
            Container container = (Container) it.next();

            // activate it
            container.activate();
        }
    }

    /**
     * Converts an InvalidConfiguration exception into a String.
     *
     * @param e an InvalidConfiguration exception.
     * @return a String corresponding to the InvalidConfiguration exception.
     */
    private String invalidConfigurationToString(
        org.omg.Components.Deployment.InvalidConfiguration e)
    {
        switch (e.reason) {
            case org.omg.Components.Deployment.UnknownConfigValueName.value:
                return "   UnknownConfigValueName : " + e.name;
            case org.omg.Components.Deployment.InvalidConfigValueType.value:
                return "   InvalidConfigValueType : " + e.name;        
            case org.omg.Components.Deployment.ConfigValueRequired.value:
                return "   ConfigValueRequired : " + e.name;
            case org.omg.Components.Deployment.ConfigValueNotExpected.value:
                return "   ConfigValueNotExpected : " + e.name; 
            default:
                return "   Unkwown reason (" + e.reason + ") : " + e.name;
        }
    }

    
    /**
     * Dectivates the ComponentServer.
     *  - deactivates all child Containers
     *  - requests the ServerActivator to remove the ComponentServer.
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

        // for each container (backward order)
        java.util.ListIterator it = containers.valuesListIterator(containers.size());
        while(it.hasPrevious()) {
            Container container = (Container) it.previous();

            // deactivate it
            container.deactivate();
        }

        System.out.println("ASSEMBLY INFO: remove component server " + id);

        // remove componentServer
        try {
            org.omg.Components.Deployment.ServerActivator serverActivator = 
                parentHost.getServerActivator();
            serverActivator.remove_component_server(componentServer);
            componentServer = null;
        } catch (CreateFailure e) {
            // convert CreateFailure in RemoveFailure
            throw new RemoveFailure(e.getMessage(),
                                    e.reason);
        }

        // set as deactivated
        isActivated = false;
    }

}


