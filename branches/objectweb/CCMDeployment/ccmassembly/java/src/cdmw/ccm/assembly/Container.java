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
import org.omg.Components.ConfigValue;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ComponentKindValueHelper;
import com.thalesgroup.CdmwDeployment.INVALID_CONFIGURATION_FOR_CONTAINER;
import com.thalesgroup.CdmwDeployment.COMPONENT_KIND;
import com.thalesgroup.CdmwDeployment.ILLEGAL_STATE;

import cdmw.ccm.deployment.ConfigValueImpl;
import cdmw.ccm.deployment.ConfigValues;


/**
 * Class representing a Container.
 */
class Container {

    /**
     * Identifier of the Container.
     */
    private String id;
    
    /**
     * Parent ComponentServer.
     */
    private ComponentServer parentComponentServer;
    
    /**
     * Map of Homes which are into this Container.
     * They are indexed by their identifier.
     */
    private cdmw.ccm.deployment.OrderedMap homes;
   
    /**
     * The Container CORBA object this class represent.
     */
    private org.omg.Components.Deployment.Container container;
    
    /**
     * Boolean set to true if Container has been activated.
     */
    private boolean isActivated;

    /**
     * ComponentKind to be used for this Container configuration.
     */
    private ComponentKindValue componentKind;


    /**
     * Constructor.
     * It automatically add the new Container to its parent 
     * ComponentServer and to its parent assembly.
     *
     * @param id Identifier of the Container.
     * @param parentComponentServer parent ComponentServer of the Container.
     * @param componentKind kind of the components which will be into the container.
     */
    public Container(String id,
                     ComponentServer parentComponentServer,
                     ComponentKindValue componentKind)
    {
        System.out.println("         Container " + id);
        if (id == null) {
            throw new NullPointerException("id is null");
        }
        if (parentComponentServer == null) {
            throw new NullPointerException("parentComponentServer is null");
        }
        if (componentKind == null) {
            throw new NullPointerException("componentKind is null");
        }

        this.id = id;
        this.parentComponentServer = parentComponentServer;
        container = null;
        homes = new cdmw.ccm.deployment.OrderedMap();
        isActivated = false;
        this.componentKind = componentKind;
        
        parentComponentServer.addContainer(this);
        parentComponentServer.getParentAssembly().addContainer(this);
    }

    /**
     * Returns the identifier of this Container.
     *
     * @return the identifier of this Container.
     */
    public String getId() {
        return id;
    }

    /**
     * Returns the parent ComponentServer of this Container.
     *
     * @return the parent ComponentServer of this Container.
     */
    public ComponentServer getParentComponentServer() {
        return parentComponentServer;
    }
    
    /**
     * Returns the parent Assembly of this Container.
     *
     * @return the parent Assembly of this Container.
     */
    public AssemblyImpl getParentAssembly() {
        return parentComponentServer.getParentAssembly();
    }
    
    /**
     * Returns true is the Container has been activated.
     *
     * @return true is the Container has been activated.
     */
    public boolean isActivated() {
        return isActivated;
    }

    /**
     * Returns the Container CORBA object corresponding 
     * to this Container.
     *
     * @return the Container CORBA object.
     *         (null is the Container is not active)
     */
    public org.omg.Components.Deployment.Container getContainer() {
        return container;
    }

    /**
     * Adds a Home to this Container.
     *
     * @param home the Home to be added.
     */
    void addHome(Home home) {
        homes.put(home.getId(), home);
    }

    /**
     * Returns the Home of this Container with the
     * specified identifier.
     *
     * @param homeId the Home's identifier
     * @return the Home with the specified identifier.
     *         (null is the Container doesn't have such Home)
     */
    public Home getHome(String homeId) {
        return (Home) homes.get(homeId);
    }
    
    /**
     * Returns the ComponentKind used as configuration value
     * by this container.
     *
     * @return the ComponentKind.
     */
    public ComponentKindValue getComponentKind() {
        return componentKind;
    }

    /**
     * Returns the configuration values to be used for this
     * Container creation.
     *
     * @return the configuration values.
     */
    protected ConfigValues getConfig() {
        ConfigValues config = new ConfigValues();

        // add COMPONENT_KIND config
        if (componentKind != null) {
            ConfigValue c = new ConfigValueImpl();
            c.name = COMPONENT_KIND.value;
            c.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
            ComponentKindValueHelper.insert(c.value, componentKind);
            config.setConfigValue(c);
        }

        return config;
    }

    /**
     * Activates the Container.
     *  - requests its parent ComponentServer to create the Container
     *  - for each Home of this Container, activates it.
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

        System.out.println("ASSEMBLY INFO: create container " + id);

        if (! parentComponentServer.isActivated()) {
            throw new CreateFailure(
                "The parent ComponentServer ("+ parentComponentServer.getId() + 
                ") of Container " + id + " is not activated",
                ILLEGAL_STATE.value);
        }
        
        // create Container
        try {
            org.omg.Components.Deployment.ComponentServer componentServer =
                parentComponentServer.getComponentServer();
            container =
                componentServer.create_container(getConfig().getConfigValueArray());
        } catch (org.omg.Components.Deployment.InvalidConfiguration e) {
            throw new CreateFailure(
                "The Container " + id + " has an invalid configuration:\n" +
                invalidConfigurationToString(e),
                INVALID_CONFIGURATION_FOR_CONTAINER.value);
        }

        // set as activated
        isActivated = true;

        // for each home
        java.util.Iterator it = homes.values().iterator();
        while(it.hasNext()) {
            Home home = (Home) it.next();

            // activate it
            home.activate();
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
     * Dectivates the Container.
     *  - deactivates all child Home
     *  - requests its parent ComponentServer to remove the Container.
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

        // for each home (backward order)
        java.util.ListIterator it = homes.valuesListIterator(homes.size());
        while(it.hasPrevious()) {
            Home home = (Home) it.previous();

            // deactivate it
            home.deactivate();
        }

        System.out.println("ASSEMBLY INFO: remove container " + id);

        // remove container
        org.omg.Components.Deployment.ComponentServer componentServer =
            parentComponentServer.getComponentServer();
        componentServer.remove_container(container);
        container = null;

        // set as deactivated
        isActivated = false;
    }


}




