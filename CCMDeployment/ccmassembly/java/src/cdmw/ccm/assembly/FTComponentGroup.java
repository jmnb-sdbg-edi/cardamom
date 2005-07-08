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
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.NOT_A_COMPONENT;
import com.thalesgroup.CdmwDeployment.REPLICATION_MANAGER_ERROR;
import com.thalesgroup.CdmwDeployment.INVALID_FT_GROUP_DESTINATION;

import org.omg.Components.ConfigValue;
import cdmw.ccm.deployment.ConfigValueImpl;
import com.thalesgroup.CdmwDeployment.FAULT_TOLERANCE_REPLICATION_STYLE;
import com.thalesgroup.CdmwDeployment.FAULT_TOLERANCE_GROUP_REF;
import com.thalesgroup.CdmwDeployment.FTReplicationStyleValue;
import com.thalesgroup.CdmwDeployment.FTReplicationStyleValueHelper;


/**
 * Class representing a fault tolerant component group.
 */
class FTComponentGroup extends ComponentGroup {

    /**
     * Primary component at group creation time.
     */
    private SingleComponent primary;
    
    /**
     * Destination of the FT component group.
     * This will be used to retrieve the ObjectGroup reference.
     */
    private String destination;
    
     
    /**
     * static reference to the ReplicationManager. 
     */
    private static org.omg.FT.ReplicationManager TheReplicationManager = null;
    
    /**
     * static syncronization Object for access to TheReplicationManager.
     * (we cannot synchronize on TheReplicationManager since it could be null)
     */
    private static Object TheReplicationManagerMutex = new Object();
    
    
    /**
     * Constructor.
     *
     * @param id identifier of the FTComponentGroup
     * @param parentAssembly the parent assembly of this FTComponentGroup.
     * @param destination the destination of this FTComponentGroup
     *        as specified into the .cad file.
     */
    public FTComponentGroup (String id,
                             AssemblyImpl parentAssembly,
                             String destination)
    {
        super(id, parentAssembly);
        System.out.println("   FTComponentGroup " + getId());

        if (destination == null) {
            throw new NullPointerException("destination is null");
        }
        this.destination = destination;
        this.primary = null;
    }

        
    /**
     * Returns the FTComponentGroup destination.
     *
     * @return the FTComponentGroup destination.
     */
    public String getDestination()
    {
        return destination;
    }
    
    /**
     * Returns the primary component of this group.
     *
     * @return the primary component or null if
     *         no primary has been set.
     */
    public SingleComponent getPrimary()
    {
        return primary;
    }

    
    /**
     * Returns the CCMObject correcsponding to the component.
     *
     * @return the CCMObject correcsponding to the component.
     *         (null is the component is not active)
     */
    public org.omg.Components.CCMObject getComponent() {
        return component;
    }

    
    /**
     * Adds a SingleComponent as member of this group.
     * If the group is not empty, this component should have 
     * the same RepositoryId than the other members.
     * If the parent FTComponentServer of this component
     * is primary, this component is set as primary of this group.
     * If primary was already present in this group, a CreateFailure
     * exception is thrown.
     *
     * @param component the member to be added to his group.
     * @return false if a component with the same identifier
     *         is already member of this group.
     * 
     * @exception CreateFailure if the component's RepositoryId
     *            is not specified or if it is not the same than
     *            other group members.
     */
    public boolean addComponent(SingleComponent component) 
        throws CreateFailure
    {     
        // check if parent ComponentServer is a FTComponentServer
        FTComponentServer parentServer;
        try {
            parentServer = (FTComponentServer) component.getParentHome()
                                                        .getParentContainer()
                                                        .getParentComponentServer();
        } catch (ClassCastException e) {
            throw new CreateFailure(
                "The parent ComponentServer (" + 
                component.getParentHome().getParentContainer().getParentComponentServer().getId() +
                ") of component " + component.getId() +
                " is not declared as fault tolerant",
                ERROR_IN_CAD.value);
        }
        
        // check if parent ComponentServer is declared as primary 
        // and check conflict is a primary was already set
        if (parentServer.isPrimary() && primary != null) {
            throw new CreateFailure(
                "Two primaries are declared in fault tolerant group " + getId() +
                ": Component " + primary.getId() + " and Component " + component.getId() +
                " have both their parent ComponentServer declared as primary (" +
                primary.getParentHome().getParentContainer().getParentComponentServer().getId() + " & " +
                parentServer.getId() + ").",
                ERROR_IN_CAD.value);
        }
        
        // add component to group
        if (super.addComponent(component)) {
        
            // set as primary is required
            if (parentServer.isPrimary()) {
                primary = component;
                System.out.println("         (" + component.getId() +
                                   " is primary)");
            }
            return true;
        } else {
            return false;
        }
    }    
    
    
    /**
     * Operation called by the assembly before any activation.
     *  - retrieve the ObjectGroup according to the destination.
     *  - retrieve the GroupId and add it as ConfigValue to each member
     *
     * @exception CreateFailure if activation fails.
     */
    public void preactivate()
        throws CreateFailure
    {
        // create config value FAULT_TOLERANCE_REPLICATION_STYLE
        // (only WARM_PASSIVE is implemented for now)
        ConfigValue repStyleConf = new ConfigValueImpl();
        repStyleConf.name = FAULT_TOLERANCE_REPLICATION_STYLE.value;
        repStyleConf.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
        FTReplicationStyleValueHelper.insert(repStyleConf.value, FTReplicationStyleValue.WARM_PASSIVE);

        // create config value FAULT_TOLERANCE_GROUP_REF
        retrieveObjectGroup();
        ConfigValue groupRefConf = new ConfigValueImpl();
        groupRefConf.name = FAULT_TOLERANCE_GROUP_REF.value;
        groupRefConf.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
        org.omg.FT.ObjectGroupHelper.insert(groupRefConf.value, component);
        
        // add config value to each member and its home
        java.util.Iterator it = components.values().iterator();
        while(it.hasNext()) {
            SingleComponent comp = (SingleComponent) it.next();
            comp.addConfigValue(groupRefConf);
            comp.getParentHome().addConfigValue(repStyleConf);
        }
    }
    
    

    /**
     * Activates the FTComponentGroup.
     * Only register component group into Naming Service if necessary
     * (additions of members to the group are done at 
     *  configuration completion)
     *
     * @exception CreateFailure if activation fails.
     */
    public void activate()
        throws CreateFailure
    {
        System.out.println("ASSEMBLY INFO: activate FT component group " + getId());

        // register component group and facets in naming service (if necessary)
        registerWithNaming();
    }


    /**
     * Complete configuration of the FTComponentGroup.
     *  - first, complete configuration of the primary component 
     *    of the group and and it to the group.
     *  - the complete configuration of  each member and 
     *    add each member to the group.
     *
     * @exception CreateFailure if a configuration_complete() fails
     */
    public void completeConfiguration() 
        throws CreateFailure
    {
        cdmw.common.Assert.check(isActivated);
        
        System.out.println("ASSEMBLY INFO: complete configuration of FT component group " + getId());

        // complete configuration and add the primary component 
        // first (if specified)
        if (primary != null) {
            primary.completeConfiguration();
            addMember(primary);
        }
        
        // for all components of the group, except primary, 
        // complete configuration and add them
        java.util.Iterator it = components.values().iterator();
        while(it.hasNext()) {
            SingleComponent comp = (SingleComponent) it.next();
            if (!comp.equals(primary)) {
                comp.completeConfiguration();
                addMember(comp);
            }
        }
    }


    
    /**
     * Dectivates the FTComponentGroup.
     *  - unregister the component group from Naming Service if necessary
     *  - remove each member from the group (no exception if member
     *    was already removed).
     *
     * @exception RemoveFailure if deactivation fails.
     */
    public void deactivate()
        throws RemoveFailure
    {
        // unregister component and facets from naming service
        unregisterFromNaming();

        // for all components of the group, remove it from the group
        java.util.Iterator it = components.values().iterator();
        while(it.hasNext()) {
            SingleComponent comp = (SingleComponent) it.next();
            removeMember(comp);
        }
        
        isActivated = false;
        this.component = null;
    }
    

    /**
     * Returns the static reference to the ReplicationManager if not null.
     * Else, retrieve the ReplicationManager with a resolve_initial_reference()
     * call, set the static reference to the ReplicationManager and returns it.
     * 
     * @return the reference to the ReplicationManager.
     *
     * @exception Exception if the ReplicationMAnager cannot be found.
     */
    private org.omg.FT.ReplicationManager getReplicationManager()
        throws Exception
    {
        synchronized (TheReplicationManagerMutex) {
            if (TheReplicationManager == null) {
                org.omg.CORBA.ORB orb = AssemblyFactoryImpl.getInstance().getOrb();

                // retrieve the FTManager
                org.omg.CORBA.Object obj = null;
                try {
                    obj = orb.resolve_initial_references("FTManager");
                } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
                    throw new Exception("FTManager is not specified as InitRef");
                } 
                if (obj == null) {
                    throw new Exception(
                        "The specified FTManager reference is invalid.");
                }
                com.thalesgroup.CdmwFT.FTManager ftmanager = null;
                try {
                    ftmanager = com.thalesgroup.CdmwFT.FTManagerHelper.narrow(obj);
                } catch (org.omg.CORBA.BAD_PARAM e) {
                    throw new Exception(
                        "The specified reference for FTManager " +
                        "does not implement a FTManager.");
                }

                // retrieve the ReplicationManager
                try {
                    TheReplicationManager = ftmanager.get_replication_manager();
                } catch (org.omg.FT.ObjectGroupNotFound e) {
                    throw new Exception(
                        "ftmanager.get_replication_manager() throws exception FT.ObjectGroupNotFound !!");
                } catch (com.thalesgroup.CdmwFT.FTManagerPackage.NoAlreadyActivated e) {
                    throw new Exception(
                        "The ReplicationManager is not already active.");
                }

            }
        }
        
        return TheReplicationManager;
    }
    
    
    /**
     * Use the destination attribute to retrieve the ObjectGroup reference.
     * First, consider the destination as a group name and try to get
     * the object group reference from the ReplicationManager.
     * If this fails, consider the destination as an URL and try to
     * resolve it with an string_to_object() call.
     *
     * @return the ObjectGroup found at destination.
     *
     * @exception if no ObjectGroup could be found with destination.
     */
    private org.omg.CORBA.Object resolveDestination()
        throws CreateFailure
    {
        System.out.println("ASSEMBLY INFO: resolve destination " + destination +
                           " for FTComponentGroup " + getId());

        // get ReplicationManager
        com.thalesgroup.CdmwFT.ReplicationManager replicationManager = null;
        try {
            org.omg.FT.ReplicationManager repMgr = getReplicationManager();
            
            replicationManager = 
                com.thalesgroup.CdmwFT.ReplicationManagerHelper.narrow(repMgr);
            
        } catch (Exception e) {
            throw new CreateFailure(e.getMessage(),
                                    REPLICATION_MANAGER_ERROR.value);
        }
        
        // Consider destination as a group name,
        // and try to get group reference from Replication Manager
        try {
            org.omg.CosNaming.NameComponent[] name = 
                cdmw.namingandrepository.NameParser.toName(destination);
            return replicationManager.get_object_group_ref_from_name(name);
            
        } catch (cdmw.namingandrepository.InvalidNameException e) {
            // destination is not a group name.
            // ignore exception
        } catch (org.omg.FT.ObjectGroupNotFound e) {
            // destination is not a group name.
            // ignore exception
        } catch (Exception e) {
            throw new CreateFailure(e.getMessage(),
                                    REPLICATION_MANAGER_ERROR.value);
        }
        
        // Consider destination as an URL, and try string_to_object()
        try {
            org.omg.CORBA.ORB orb = AssemblyFactoryImpl.getInstance().getOrb();
            org.omg.CORBA.Object obj = orb.string_to_object(destination);
        
            // check if object is an object group
            replicationManager.get_object_group_id(obj);
        
            return obj;
        
        } catch (org.omg.CORBA.BAD_PARAM e) {
            // destination is not an URL
            throw new CreateFailure("The destination '" + destination + 
                                    "' of fault tolerant group " + getId() + 
                                    " is not a group name or an URL.",
                                    INVALID_FT_GROUP_DESTINATION.value);
        } catch (org.omg.FT.ObjectGroupNotFound e) {
            // object is not an object group.
            throw new CreateFailure("The object found at destination '" + destination + 
                                    "' of fault tolerant group " + getId() + 
                                    " is an object group.",
                                    INVALID_FT_GROUP_DESTINATION.value);
        } catch (Exception e) {
            throw new CreateFailure(e.getMessage(),
                                    REPLICATION_MANAGER_ERROR.value);
        }
    }
    
    
    /**
     * Retrieve the ObjectGroup according to this FTComponentGroup's destination.
     *
     * @exception CreateFailure if ObjectGroup cannot be retreived.
     */
    private void retrieveObjectGroup() 
        throws CreateFailure
    {
        // get object reference according to destination
        org.omg.CORBA.Object obj = resolveDestination();
        
        // Check if it is a CCMObject group
        try {
            component = org.omg.Components.CCMObjectHelper.narrow(obj);
            if (component == null) {
                throw new CreateFailure(
                    "The object reference for fault tolerant group " + getId() +
                    " (destination: " + destination + ") is null.",
                    NOT_A_COMPONENT.value);
            }
            
        } catch (org.omg.CORBA.BAD_PARAM e) {
            component = null;
            throw new CreateFailure(
                "The object reference for fault tolerant group " + getId() +
                " (destination: " + destination + ") doesn't correspond to a Component group.",
                NOT_A_COMPONENT.value);
            
        } catch (org.omg.CORBA.SystemException e) {
            component = null;
            throw new CreateFailure(
                "SystemException while trying to narrow object reference for fault tolerant group " +
                getId() +  " into a Component group: " + e.toString(),
                NOT_A_COMPONENT.value);
        }

        // A FTComponentGroup is considered as acivated as soon
        // as its reference is retrieved. (This allows connections with empty groups)
        isActivated = true;
    }
    
    
    
    /**
     * Gets FT::Location of a component.
     *
     * @param comp the component.
     * @return the FT::Location of the component.
     */
    private org.omg.CosNaming.NameComponent[] getLocation(SingleComponent comp)
    {
        // get location infos from parent ComponentServer
        ComponentServer parentServer = comp.getParentHome()
                                           .getParentContainer()
                                           .getParentComponentServer();
        String hostname = parentServer.getParentHost().getDestination();
        String appliname = parentServer.getApplicationName();
        String processname = parentServer.getProcessName();

        // Create FT Location
        org.omg.CosNaming.NameComponent[] location = new org.omg.CosNaming.NameComponent[3];
        location[0] = new org.omg.CosNaming.NameComponent();
        location[0].id = hostname;
        location[0].kind = "hostname";
        location[1] = new org.omg.CosNaming.NameComponent();
        location[1].id = appliname;
        location[1].kind = "applicationname";
        location[2] = new org.omg.CosNaming.NameComponent();
        location[2].id = processname;
        location[2].kind = "processname";     
        
        return location;
    }        

    
    /**
     * Adds a component as member of the ObjectGroup represented by this FTComponentGroup.
     *
     * @param comp the component to be added.
     *
     * @exception CreateFailure if adding fails.
     */
    private void addMember(SingleComponent comp) 
        throws CreateFailure
    {
        System.out.println("ASSEMBLY INFO:    adding component " + comp.getId() + " to FT group " + getId());

        // get the ReplicationManager
        org.omg.FT.ReplicationManager replicationManager;
        try {
            replicationManager = getReplicationManager();
        } catch (Exception e) {
            throw new CreateFailure(e.getMessage(),
                                    REPLICATION_MANAGER_ERROR.value);
        }
        
        // get location of the component
        org.omg.CosNaming.NameComponent[] location = getLocation(comp);
        
        // add CCMObject to the group
        try {
            replicationManager.add_member(component, location, comp.getComponent());
        } catch (org.omg.FT.ObjectGroupNotFound e) {
            throw new CreateFailure(
                "ReplicationManager didn't found the ObjectGroup " + getId() +
                "with destination: " + destination,
                REPLICATION_MANAGER_ERROR.value);
        } catch (org.omg.FT.MemberAlreadyPresent e) {
            throw new CreateFailure(
                "The component " + comp.getId() +
                " is already present in FT group " + getId(),
                REPLICATION_MANAGER_ERROR.value);
        } catch (org.omg.FT.ObjectNotAdded e) {
            throw new CreateFailure(
                "The ReplicationManager failed to add component " + comp.getId() +
                " into the FT group " + getId(),
                REPLICATION_MANAGER_ERROR.value);
        } catch (org.omg.CORBA.SystemException e) {
            throw new CreateFailure(
                "SystemException while contacting ReplicationManager for member addition: " +
                e.toString(),
                REPLICATION_MANAGER_ERROR.value);
        }
    }
    
    /**
     * Removes a component from the ObjectGroup represented by this FTComponentGroup.
     *
     * @param comp the component to be removed
     *
     * @exception RemoveFailure if removal fails
     */
    private void removeMember(SingleComponent comp) 
        throws RemoveFailure
    {
        // get the ReplicationManager
        org.omg.FT.ReplicationManager replicationManager;
        try {
            replicationManager = getReplicationManager();
        } catch (Exception e) {
            throw new RemoveFailure(e.getMessage(),
                                    REPLICATION_MANAGER_ERROR.value);
        }
        
        // get location of the component
        org.omg.CosNaming.NameComponent[] location = getLocation(comp);

        // remove member from the group
        try {
            replicationManager.remove_member(component, location);
        } catch (org.omg.FT.ObjectGroupNotFound e) {
            throw new RemoveFailure(
                "ReplicationManager didn't found the ObjectGroup " + getId() +
                "with destination: " + destination,
                REPLICATION_MANAGER_ERROR.value);
        } catch (org.omg.FT.MemberNotFound e) {
            // ignore, member has already been removed
        } catch (org.omg.CORBA.SystemException e) {
            throw new RemoveFailure(
                "SystemException while contacting ReplicationManager for member removal: " +
                e.toString(),
                REPLICATION_MANAGER_ERROR.value);
        }
    }
    
    
}

