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
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.NOT_A_COMPONENT;
import com.thalesgroup.CdmwDeployment.LB_GROUP_MANAGER_ERROR;
import com.thalesgroup.CdmwDeployment.INVALID_LB_GROUP_DESTINATION;

import org.omg.Components.ConfigValue;
import cdmw.ccm.deployment.ConfigValueImpl;
import com.thalesgroup.CdmwDeployment.LOAD_BALANCING_STRATEGY;
import com.thalesgroup.CdmwDeployment.LOAD_BALANCING_GROUP_REF;
import com.thalesgroup.CdmwDeployment.LBStrategyValue;
import com.thalesgroup.CdmwDeployment.LBStrategyValueHelper;


/**
 * This class represents a load balanced component group.
 */
class LBComponentGroup extends ComponentGroup {

    private static final String REPOSITORY_GROUP_MANAGER_GROUP_NAME= "lb_group_manager/object_groups";
    private static final String GROUP_MANAGER_OBJECT_GROUP_NAME = "LBGroupManagerGroup";
    private com.thalesgroup.CdmwNamingAndRepository.Repository repository;


    /**
     * Destination of the LB component group.
     * It is used to obtain the ObjectGroup reference.
     */
    private String destination;


    /**
     * Static reference to the LBGroupManager.
     */
    private static CdmwLB.LBGroupManager TheLBGroupManager = null;


    /**
     * Static synchronization Object for access to TheLBGroupManager.
     * (synchronize cannot work if TheLBGroupManager if null)
     */
    private static Object TheLBGroupManagerMutex = new Object();


    /**
     * Constructor.
     *
     * @param id identifier of the LBComponentGroup.
     * @param parentAssembly parent assembly of this LBComponentGroup.
     * @param destination destination of this LBComponentGroup as
     *                    specified in the .CAD file.
     */
    public LBComponentGroup(String id,
                            AssemblyImpl parentAssembly,
                            String destination)
    {
        super(id, parentAssembly);
        System.out.println("   LBComponentGroup " + getId());

        if (destination == null) {
            throw new NullPointerException("null is not a valid destination");
        }

        this.destination = destination;
    }


    /**
     * @return the destination of this LB component group.
     */
    public String getDestination() {
        return destination;
    }


    /**
     * @return the CCMObject correcsponding to the component.
     *         (null is the component is not active)
     */
    public org.omg.Components.CCMObject getComponent() {
        return component;
    }


    /**
     * Adds a SingleComponent as member of this group.
     * If the group is not empty, this component shall have the same
     * RepositoryId than the other members.
     *
     * @param component the member to be added to his group.
     *
     * @return <code>false</code> if a component with the same identifier
     *         is already member of this group.
     *
     * @exception CreateFailure if the component's RepositoryId is not
     *                          specified or if it differs from the
     *                          RepositoryId of the other members.
     */
    public boolean addComponent(SingleComponent component)
        throws CreateFailure
    {
        // Check if the parent ComponentServer is a LBComponentServer.
        LBComponentServer parentServer;
        try {
            parentServer =
                (LBComponentServer) component.getParentHome()
                                             .getParentContainer()
                                             .getParentComponentServer();
        } catch (ClassCastException e) {
            throw new CreateFailure(
                "The parent ComponentServer (" +
                component.getParentHome().getParentContainer().
                    getParentComponentServer().getId() +
                ") of component " + component.getId() +
                " is not load balanced",
                ERROR_IN_CAD.value);
        }

        // Add the component to group.
        return (super.addComponent(component)) ? true : false;
    }


    /**
     * This operation is called by the assembly before any activation.
     * It sets config values to each member of the group.
     *
     * @exception CreateFailure if activation fails.
     */
    public void preactivate()
        throws CreateFailure
    {
        // TODO
        // Create config value LOAD_BALANCING_STRATEGY

        // Create config value LOAD_BALANCING_GROUP_REF
        retrieveObjectGroup();
        ConfigValue groupRefConf = new ConfigValueImpl();
        groupRefConf.name = LOAD_BALANCING_GROUP_REF.value;
        groupRefConf.value =
            AssemblyFactoryImpl.getInstance().getOrb().create_any();
        org.omg.PortableGroup.ObjectGroupHelper.insert(groupRefConf.value,
                                                       component);

        // Add the config values to all the members and to their homes.
        java.util.Iterator it = components.values().iterator();
        while(it.hasNext()) {
            SingleComponent comp = (SingleComponent) it.next();
            comp.addConfigValue(groupRefConf);
            // TODO
            // comp.getParentHome().addConfigValue(strategyConf);
        }
    }


    /**
     * Activates the LB component group.
     * Registers the component group to the Naming Service if necessary
     * (the addition of members are performed once the configuration
     * completes).
     *
     * @throws CreateFailure if the activation fails.
     */
    public void activate()
        throws CreateFailure
    {
        System.out.println(
            "ASSEMBLY INFO: create LB component group " + getId());

        // Register the component group and the facets to the Naming
        // Service (if necessary).
        registerWithNaming();
    }


    /**
     * Completes the configuration of the LBComponentGroup.
     *
     * @exception CreateFailure if a configuration_complete() fails
     */
    public void completeConfiguration()
        throws CreateFailure
    {
        cdmw.common.Assert.check(isActivated);

        System.out.println(
            "ASSEMBLY INFO: complete configuration of LB component group " +
            getId());

        // For all the components of the group, complete their configuration
        // and add them to the group.
        java.util.Iterator it = components.values().iterator();
        while (it.hasNext()) {
            SingleComponent comp = (SingleComponent) it.next();
            comp.completeConfiguration();
            addMember(comp);
        }
    }


    /**
     * Deactivates the LB component group:
     *  + unregister the component group from the Naming Service (if necessary)
     *  + remove each member of the group (no exception is raised if the member
     *    was already removed).
     *
     * @throws RemoveFailure if the deactivation fails.
     */
    public void deactivate()
        throws RemoveFailure
    {
        // unregister the component group and the facets from the Naming Service.
        unregisterFromNaming();

        // remove the components from the group.
        java.util.Iterator it = components.values().iterator();
        while (it.hasNext()) {
            SingleComponent comp = (SingleComponent) it.next();
            removeMember(comp);
        }

        isActivated = false;
        this.component = null;
    }


    /**
     * Returns the static referencee to the LBGroupManager (if not null).
     * Otherwise, obtain the reference to the LBGroupManager by executing
     * resolve_initial_references(), save the reference internally and
     * returns the freshly obtained reference.
     *
     * @return the reference to the LBGroupManager.
     *
     * @throws Exception if the LBGroupManager could not be located.
     */
    private CdmwLB.LBGroupManager getLBGroupManager()
        throws Exception
    {
        synchronized (TheLBGroupManagerMutex) {
            if (TheLBGroupManager == null) {
                // Look for the LBGroupManager in the initial references.
                org.omg.CORBA.ORB orb =
                    AssemblyFactoryImpl.getInstance().getOrb();

                org.omg.CORBA.Object obj = null;
                try {
                    obj = orb.resolve_initial_references("LBGroupManager");
		 } catch (org.omg.CORBA.ORBPackage.InvalidName e){
                }

             if (obj == null) {
             try {
                        repository = cdmw.cdmwinit.InitUtils.getCdmwRepository();
                        // Get NamingInterface to object_groups (for reading)
                        cdmw.commonsvcs.naming.NamingInterface objGroupsInterface =
                            cdmw.namingandrepository.RepositoryInterface.getDomainNamingInterface(REPOSITORY_GROUP_MANAGER_GROUP_NAME);

                        // Get LB GroupManager Object Group
                        obj = objGroupsInterface.resolve(GROUP_MANAGER_OBJECT_GROUP_NAME);
                
                    } catch (cdmw.orbsupport.CORBASystemExceptionWrapper ex) {
                        // shouldn't happen
                        ex.printStackTrace();
                        cdmw.common.Assert.check(false);
                    } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName ex) {
                        // shouldn't happen
                        ex.printStackTrace();
                        cdmw.common.Assert.check(false);
                    } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain ex) {
                        // shouldn't happen
                        ex.printStackTrace();
                        cdmw.common.Assert.check(false);
                    }

                }

	  try {
                    TheLBGroupManager =
                        CdmwLB.LBGroupManagerHelper.narrow(obj);
                } catch (org.omg.CORBA.BAD_PARAM e) {
                    throw new Exception(
                        "the LBGroupManager reference is not" +
                        "implementing a LBGroupManager");
                }
            }
        }

        return TheLBGroupManager;
    }


    /**
     * Returns the ObjectGroup reference from the destination attribute.
     * Consider the destination as an URL and try to resolve it using
     * string_to_object().
     *
     * @return the ObjectGroup found at destination.
     *
     * @exception if none was found at destination.
     */
    private org.omg.CORBA.Object resolveDestination()
        throws CreateFailure
    {
        System.out.println(
            "ASSEMBLY INFO: resolve destination " + destination +
            " for LBComponentGroup " + getId());

        // Get the LBGroupManager.
        CdmwLB.LBGroupManager lbGroupManager = null;
        try {
            lbGroupManager = getLBGroupManager();
        } catch (Exception e) {
            throw new CreateFailure(e.getMessage(),
                                    LB_GROUP_MANAGER_ERROR.value);
        }

        try {
            org.omg.CORBA.ORB orb = AssemblyFactoryImpl.getInstance().getOrb();
            org.omg.CORBA.Object obj = orb.string_to_object(destination);

            // Check if object is an object group
            // (may raise ObjectGroupNotFound)
            lbGroupManager.get_object_group_id(obj);

            return obj;
        } catch (org.omg.CORBA.BAD_PARAM e) {
            // Destination is not an URL.
            throw new CreateFailure(
                "The destination '" + destination + "' of the load " +
                "balanced group " + getId() + " is not an URL.",
                INVALID_LB_GROUP_DESTINATION.value);
        } catch (org.omg.PortableGroup.ObjectGroupNotFound e) {
            // Object is not an object group.
            throw new CreateFailure(
                "The object found at destination '" + destination +
                "' of the load balanced group " + getId() +
                " is an object group.",
                INVALID_LB_GROUP_DESTINATION.value);
        } catch (Exception e) {
            throw new CreateFailure(e.getMessage(),
                                    LB_GROUP_MANAGER_ERROR.value);
        }
    }


    /**
     * Retrieves the ObjectGroup according to this LB destination.
     *
     * @exception CreateFailure if ObjectGroup cannot be retreived.
     */
    private void retrieveObjectGroup()
        throws CreateFailure
    {
        // Get object reference according to destination.
        org.omg.CORBA.Object obj = resolveDestination();

        // Check if it is a CCMObject group.
        try {
            component = org.omg.Components.CCMObjectHelper.unchecked_narrow(obj);
            if (component == null) {
                throw new CreateFailure(
                    "The object reference for the load balanced group " +
                    getId() + " (destination: " + destination + ") is null.",
                    NOT_A_COMPONENT.value);
            }
        } catch (org.omg.CORBA.BAD_PARAM e) {
            component = null;
            throw new CreateFailure(
                "The object reference for the load balanced group " +
                getId() + " (destination: " + destination + ") is not a " +
                "reference to a Component group.",
                NOT_A_COMPONENT.value);
        } catch (org.omg.CORBA.SystemException e) {
            component = null;
            throw new CreateFailure(
                "SystemException raised while trying to narrow the object " +
                "reference of the load balanced group " +
                getId() +  " into a Component group: " + e.toString(),
                NOT_A_COMPONENT.value);
        }

        // A LBComponentGroup is considered as activated as soon as its
        // reference is retrieved. (This allows connections with empty groups).
        isActivated = true;
    }


    /**
     * Returns the PortableGroup::Location of the component.
     *
     * @param comp the component.
     *
     * @return the PortableGroup::Location of the component.
     */
    private org.omg.CosNaming.NameComponent[]
    getLocation(SingleComponent comp)
    {
        // get the location infos from the parent ComponentServer.
        ComponentServer parentServer = comp.getParentHome()
                                           .getParentContainer()
                                           .getParentComponentServer();
        String hostname = parentServer.getParentHost().getDestination();
        String appliname = parentServer.getApplicationName();
        String processname = parentServer.getProcessName();

        // create the PortableGroup::Location
        org.omg.CosNaming.NameComponent[] location =
            new org.omg.CosNaming.NameComponent[3];

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
     * Adds a component as member of the LB component group.
     *
     * @param comp the component to add.
     *
     * @throws CreateFailure if any error occurs.
     */
    private void addMember(SingleComponent comp)
        throws CreateFailure
    {
        System.out.println(
            "ASSEMBLY INFO:    adding component " +
            comp.getId() + " to the LB component group " + getId());

        // Get a reference to the LBGroupManager.
        CdmwLB.LBGroupManager lbGroupManager = null;
        try {
            lbGroupManager = getLBGroupManager();
        } catch (Exception e) {
            throw new CreateFailure(e.getMessage(),
                                    LB_GROUP_MANAGER_ERROR.value);
        }

        // Get the location of the component.
        org.omg.CosNaming.NameComponent[] location = getLocation(comp);

        // Add the CCMObject to the component group.
        try {
            lbGroupManager.add_member(
                component, location, comp.getComponent());
        } catch (org.omg.PortableGroup.ObjectGroupNotFound e) {
            throw new CreateFailure(
                "the LBGroupManager could not find the ObjectGroup " +
                getId() + " at the destination: " + destination,
                LB_GROUP_MANAGER_ERROR.value);
        } catch (org.omg.PortableGroup.MemberAlreadyPresent e) {
            throw new CreateFailure(
                "the component " + comp.getId() + " is already a member " +
                "of the group " + getId(),
                LB_GROUP_MANAGER_ERROR.value);
        } catch (org.omg.PortableGroup.ObjectNotAdded e) {
            throw new CreateFailure(
                "the LBGroupManager failed to add the component " +
                comp.getId() + " to the LB group " + getId(),
                LB_GROUP_MANAGER_ERROR.value);
        } catch (org.omg.CORBA.SystemException e) {
            throw new CreateFailure(
                "SystemException raised while contacting the LBGroupManager " +
                "for the addition of a new component group member: " +
                e.toString(),
                LB_GROUP_MANAGER_ERROR.value);
        }
    }


    /**
     * Removes a component from the LB component group.
     *
     * @param comp the component to be removed.
     *
     * @throws RemoveFailure if the removal failed.
     */
    private void removeMember(SingleComponent comp)
        throws RemoveFailure
    {
        // Get a reference to the LBGroupManager.
        CdmwLB.LBGroupManager lbGroupManager = null;
        try {
            lbGroupManager = getLBGroupManager();
        } catch (Exception e) {
            throw new RemoveFailure(e.getMessage(),
                                    LB_GROUP_MANAGER_ERROR.value);
        }

        // Get the location of the component.
        org.omg.CosNaming.NameComponent[] location = getLocation(comp);

        // Remove the component from the LB component group.
        try {
            lbGroupManager.remove_member(component, location);
        } catch (org.omg.PortableGroup.ObjectGroupNotFound e) {
            throw new RemoveFailure(
                "the LBGroupManager could not find the ObjectGroup " +
                getId() + " at the destination: " + destination,
                LB_GROUP_MANAGER_ERROR.value);
        } catch (org.omg.PortableGroup.MemberNotFound e) {
            // Ignore, the component has already been removed.
        } catch (org.omg.CORBA.SystemException e) {
            throw new RemoveFailure(
                "SystemException raised while contacting the LBGroupManager " +
                "for the removal of a component group member: " + e.toString(),
                LB_GROUP_MANAGER_ERROR.value);
        }
    }


}
