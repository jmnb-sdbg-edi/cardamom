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
import org.omg.Components.Deployment.AssemblyState;


/**
 * Implementation of the Assembly CORBA interface.
 */
class AssemblyImpl 
    extends org.omg.Components.Deployment.AssemblyPOA
{

    /**
     * POA for Assembly objects.
     */
    private org.omg.PortableServer.POA assembliesPOA;

    /**
     * State of the Assembly.
     */
    private AssemblyState state;

    /**
     * List of HostCollocations involved in this Assembly.
     * (Vector is used because identifiers are not 
     *  required for hostcollocations)
     */
    private java.util.Vector hosts;

    /**
     * List of ComponentServers involved in this Assembly.
     * (Vector is used because identifiers are not 
     *  required for processcollocations)
     */
    private java.util.Vector servers;

    /**
     * List of Containers involved in this Assembly.
     * (Containers are indexed by their identifier).
     */
    private java.util.Map containers;

    /**
     * List of Homes involved in this Assembly.
     * (Homes are indexed by their identifier).
     */
    private java.util.Map homes;

    /**
     * List of SingleComponents involved in this Assembly.
     * (SingleComponents are indexed by their identifier).
     */
    private java.util.Map components;

    /**
     * List of ComponentGroups involved in this Assembly.
     * (ComponentGroups are indexed by their identifier).
     */
    private java.util.Map componentGroups;

    /**
     * List of Connections involved in this Assembly.
     * (Connections are indexed by their identifier).
     */
    private cdmw.ccm.deployment.OrderedMap connections;
    
    /**
     * ConfiguratorManager object.
     * (specific to each Assembly instance)
     */
    private ConfiguratorManager configManager;


    /**
     * Constructor.
     *
     * @param assembliesPOA the POA for Assembly objects.
     */
    public AssemblyImpl(org.omg.PortableServer.POA assembliesPOA) {
        this.assembliesPOA = assembliesPOA;
        state = AssemblyState.INACTIVE;
        
        hosts            = new java.util.Vector();
        servers          = new java.util.Vector();
        containers       = new cdmw.ccm.deployment.OrderedMap();
        homes            = new cdmw.ccm.deployment.OrderedMap();
        components       = new cdmw.ccm.deployment.OrderedMap();
        componentGroups  = new cdmw.ccm.deployment.OrderedMap();
        connections      = new cdmw.ccm.deployment.OrderedMap();
        configManager    = 
            new ConfiguratorManager(AssemblyFactoryImpl.getInstance().getXMLParser(),
                                    AssemblyFactoryImpl.getInstance().getOrb(),
                                    AssemblyFactoryImpl.getInstance().getConfiguratorFactory());
    }   

        
    /**
     * Returns the parent POA of this Assembly object.
     *
     * @return the parent POA of this Assembly object.
     */
    public org.omg.PortableServer.POA _default_POA() {
        return assembliesPOA;
    }
    
    /**
     * Returns the ConfiguratorManager of this Assembly.
     *
     * @return the ConfiguratorManager of this Assembly.
     */
    public ConfiguratorManager getConfiguratorManager() {
        return configManager;
    }
    
    /**
     * Adds a HostCollocation involved in this Assembly.
     *
     * @param hostCollocation the HostCollocation to be added.
     */
    void addHostCollocation(HostCollocation hostCollocation) {
        hosts.add(hostCollocation);
    }    
        
    /**
     * Returns the HostCollocation involved in this Assembly 
     * with the specified identifier.
     *
     * @param hostId the HostCollocation's identifier
     * @return the HostCollocation with specified identifier
     *         or null if not found.
     */
    public HostCollocation getHostCollocation(String hostId) {
        if (hostId == null)
            return null;

        java.util.Iterator it = hosts.iterator();
        while (it.hasNext()) {
            HostCollocation host = (HostCollocation) it.next();
            if (hostId.equals(host.getId()))
                return host;
        }
        
        return null;
    }
        
    /**
     * Adds a ComponentServer involved in this Assembly.
     *
     * @param server the ComponentServer to be added.
     */
    void addComponentServer(ComponentServer server) {
        servers.add(server);
    }
        
    /**
     * Returns the ComponentServer involved in this Assembly 
     * with the specified identifier.
     *
     * @param compSrvId the ComponentServer's identifier
     * @return the ComponentServer with specified identifier
     *         or null if not found.
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
     * Adds a Container involved in this Assembly.
     *
     * @param container the Container to be added.
     * @return false if a Container with the same identifier was already present.
     */
    boolean addContainer(Container container) {
        if (containers.containsKey(container.getId())) {
            return false;
        }
        
        containers.put(container.getId(), container);
        return true;
    }

    /**
     * Returns the Container involved in this Assembly 
     * with the specified identifier.
     *
     * @param containerId the Container's identifier
     * @return the Container with specified identifier
     *         or null if not found.
     */
    public Container getContainer(String containerId) {
        return (Container) containers.get(containerId);
    }

    
    /**
     * Adds a Home involved in this Assembly.
     *
     * @param home the Home to be added.
     * @return false if a Home with the same identifier was already present.
     */
    boolean addHome(Home home) {
        if (homes.containsKey(home.getId())) {
            return false;
        }
        
        homes.put(home.getId(), home);
        return true;
    }
    
    /**
     * Returns the Home involved in this Assembly 
     * with the specified identifier.
     *
     * @param homeId the Home's identifier
     * @return the Home with specified identifier
     *         or null if not found.
     */
    public Home getHome(String homeId) {
        return (Home) homes.get(homeId);
    }
    
    
    /**
     * Adds a SingleComponent involved in this Assembly.
     *
     * @param component the SingleComponent to be added.
     * @return false if a SingleComponent with the same identifier was already present.
     */
    boolean addComponent(SingleComponent component) {
        if (components.containsKey(component.getId())) {
            return false;
        }
        
        components.put(component.getId(), component);
        return true;
    }
    
    /**
     * Returns the SingleComponent involved in this Assembly 
     * with the specified identifier.
     *
     * @param componentId the SingleComponent's identifier
     * @return the SingleComponent with specified identifier
     *         or null if not found.
     */
    public SingleComponent getComponent(String componentId) {
        return (SingleComponent) components.get(componentId);
    }
    
    /**
     * Adds a ComponentGroup involved in this Assembly.
     *
     * @param group the ComponentGroup to be added.
     * @return false if a ComponentGroup with the same identifier was already present.
     */
    boolean addComponentGroup(ComponentGroup group) {
        if (componentGroups.containsKey(group.getId())) {
            return false;
        }
        
        componentGroups.put(group.getId(), group);
        return true;
    }
    
    /**
     * Returns the ComponentGroup involved in this Assembly 
     * with the specified identifier.
     *
     * @param groupId the ComponentGroup's identifier
     * @return the ComponentGroup with specified identifier
     *         or null if not found.
     */
    public ComponentGroup getComponentGroup(String groupId) {
        return (ComponentGroup) componentGroups.get(groupId);
    }
    
    /**
     * Adds a Connection involved in this Assembly.
     *
     * @param connection the Connection to be added.
     * @return false if a Connection with the same identifier was already present.
     */
    public boolean addConnection(Connection connection) {
        if (connections.containsKey(connection.getId())) {
            return false;
        }
        
        connections.put(connection.getId(), connection);
        return true;
    }
    
    /**
     * Returns the Connection involved in this Assembly 
     * with the specified identifier.
     *
     * @param connectionId the Connection's identifier
     * @return the Connection with specified identifier
     *         or null if not found.
     */
    public Connection getConnection(String connectionId) {
        return (Connection) connections.get(connectionId);
    }
    

    /**
     * Implementation of the IDL operation:
     *  IDL:omg.org/Components/Deployment/Assembly/build:1.0.
     * Builds this Assembly.
     * Activates HostCollocations, ComponentServers, Containers, Homes,
     * Components, ComponentGroups and Connections.
     *
     * @exception CreateFailure if build fails.
     */
     public void build()
        throws CreateFailure
    {
        if (state == AssemblyState.INSERVICE) {
            throw new CreateFailure(
                "Assembly already in service",
                com.thalesgroup.CdmwDeployment.ASSEMBLY_ALREADY_IN_SERVICE.value);
        }
        
        try {
            System.out.println("ASSEMBLY INFO: Build assembly");
            
            // pre-activate component groups
            // (retrieve group references and get some specific properties)
            java.util.Iterator it = componentGroups.values().iterator();
            while (it.hasNext()) {
                ComponentGroup group =  (ComponentGroup) it.next();
                group.preactivate();
            }

            // activate HostCollocations
            // (all sub-elements will be activated)
            it = hosts.iterator();
            while (it.hasNext()) {
                HostCollocation host = (HostCollocation) it.next();
                host.activate();
            }
            
            // activate component groups
            it = componentGroups.values().iterator();
            if (it.hasNext())
                System.out.println("ASSEMBLY INFO: activate component groups");
            while (it.hasNext()) {
                ComponentGroup group =  (ComponentGroup) it.next();
                group.activate();
            }
            
            // activate connections
            System.out.println("ASSEMBLY INFO: establish connections");
            it = connections.values().iterator();
            while (it.hasNext()) {
                Connection connection = (Connection) it.next();
                connection.activate();
            }

            // complete configuration of component groups
            it = componentGroups.values().iterator();
            if (it.hasNext())
                System.out.println("ASSEMBLY INFO: complete component groups configurations");
            while (it.hasNext()) {
                ComponentGroup group =  (ComponentGroup) it.next();
                group.completeConfiguration();
            }

            // complete configuration of homes an components
            System.out.println("ASSEMBLY INFO: configure components");
            it = homes.values().iterator();
            while (it.hasNext()) {
                Home home = (Home) it.next();
                home.completeConfiguration();
            }
            
            System.out.println("ASSEMBLY INFO: Assembly in service");
            state = AssemblyState.INSERVICE;
    
        } catch (CreateFailure e) {
            System.err.println(e.getMessage());
            try {
                // deactivate all
                tear_down();
            } catch (RemoveFailure ex) {
                // ignore it,
            }
        
            throw e;
        
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            try {
                // deactivate all
                tear_down();
            } catch (RemoveFailure ex) {
                // ignore it,
            }

            throw e;
        }
        
    }
    

    /**
     * Implementation of the IDL operation:
     *  IDL:omg.org/Components/Deployment/Assembly/tear_down:1.0.
     * Tears this Assembly down.
     * Dectivates Connections, ComponentGroups, Components, Homes,
     * Containers, ComponentServers and HostCollocations.
     *
     * @exception RemoveFailure if tear down fails.
     */
    public void tear_down()
        throws RemoveFailure
    {
        if (state == AssemblyState.INACTIVE) {
            throw new RemoveFailure(
                "Assembly already inactive",
                com.thalesgroup.CdmwDeployment.ASSEMBLY_ALREADY_INACTIVE.value);
        }

        try {
            // deactivate connections (backward order)
            java.util.ListIterator it = connections.valuesListIterator(connections.size());
            while (it.hasPrevious()) {
                Connection connection = (Connection) it.previous();
                connection.deactivate();
            }
            
            // deactivate HostCollocations (backward order)
            it = hosts.listIterator(hosts.size());
            while (it.hasPrevious()) {
                HostCollocation host = (HostCollocation) it.previous();
                host.deactivate();
            }
            
            state = AssemblyState.INACTIVE;
        
        } catch (RemoveFailure e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;
        }
    }
    

    /**
     * Implementation of the IDL operation:
     *  IDL:omg.org/Components/Deployment/Assembly/get_state:1.0.
     * Returns the Assembly's state.
     *
     * @return the Assembly's state.
     */
    public AssemblyState get_state() {
        return state;
    }
    
    
    /**
     * Destroys this Assembly.
     * This operation has to be called by AssemblyFactory 
     * when it destroys this Assambly.
     */
    void destroy() 
    {
        configManager.removeConfigurators();
    }


}


