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

import org.omg.Components.Configurator;

import com.thalesgroup.CdmwDeployment.WRONG_HOME_KIND;
import com.thalesgroup.CdmwDeployment.PROPERTIES_CONFIGURATION_ERROR;
import com.thalesgroup.CdmwDeployment.NOT_A_COMPONENT;
import com.thalesgroup.CdmwDeployment.NAMING_SERVICE_ERROR;
import com.thalesgroup.CdmwDeployment.PORT_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.CREATE_FAILURE;
import com.thalesgroup.CdmwDeployment.ILLEGAL_STATE;

import org.omg.Components.ConfigValue;
import cdmw.ccm.deployment.ConfigValues;



/**
 * Class representing a single component instance
 * (in contrast with a component group).
 * A SingleComponent as a parent Home.
 */
class SingleComponent extends Component 
{

    /**
     * ConfigValues for component creation
     */
    private ConfigValues config;
    
    /**
     * Parent home of the component.
     */
    private Home parentHome;
    
    /**
     * Boolean specifying if is configuration_complete() operation
     * has been called on the component.
     */
    private boolean wasConfigurationCompleteCalled;
    
    /**
     * Constructor.
     * It automatically add the new SingleComponent to its parent home
     * and to its parent assembly.
     *
     * @param id the component identifier. 
     *        (as specified in .cad file in componentinstanciation).
     * @param parentHome the component's parent home.
     */
    public SingleComponent (String id,
                            Home parentHome)
    {
        super(id);
        System.out.println("               Component " + getId());
        
        if (parentHome == null) {
            throw new NullPointerException("parentHome is null");
        }
        
        this.config = new ConfigValues();
        this.parentHome = parentHome;
        this.wasConfigurationCompleteCalled = false;
        parentHome.addComponent(this);
        parentHome.getParentAssembly().addComponent(this);
    }

    
    /**
     * Add a ConfigValue for component creation.
     *
     * @param c the ConfigValue to be used for component creation.
     */
    public void addConfigValue(ConfigValue c) {
        config.setConfigValue(c);
    }
     
    
    /**
     * Return the component's parent home.
     *
     * @return the component's parent home.
     */
    public Home getParentHome() {
        return parentHome;
    }
    
    
    /**
     * Return the component's parent assembly.
     *
     * @return the component's parent assembly.
     */
    public AssemblyImpl getParentAssembly() {
        return parentHome.getParentAssembly();
    }
    

    /**
     * Return the component's RepositoryId.
     *
     * @return the component's RepositoryId.
     */
    public String getRepositoryId() {
        return parentHome.getComponentRepositoryId();
    }
    
    /**
     * Add a registration name for component to be 
     * registred into the NamingService.
     *
     * @param the registration name of the component.
     */
    public void addComponentNamingRegistration(String name) {
        componentNamingRegistrations.add(name);
    }


    /**
     * Activate the component.
     *  - request its parent home to create the CCMObject
     *  - request configurator to configure to configure it
     *  - register the CCMObject and/or its facets into the naming service if required
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

        System.out.println("ASSEMBLY INFO: create component " + getId());

        if (! parentHome.isActivated()) {
            throw new CreateFailure(
                "The parent Home ("+ parentHome.getId() + 
                ") of Component " + getId() + " is not activated",
                ILLEGAL_STATE.value);
        }

        // TODO: check component type (if entity...)

        // get KeylessCCMHome
        com.thalesgroup.CdmwCcmCif.KeylessCCMHome keylessHome;
        try {
            org.omg.Components.CCMHome home = 
                parentHome.getHome();
            keylessHome =
                com.thalesgroup.CdmwCcmCif.KeylessCCMHomeHelper.narrow(home);
        } catch (org.omg.CORBA.SystemException e) {
            // narrow error, home is not a KeylessCCMHome
            throw new CreateFailure(
                "Cannot create component " + getId() + ":\n" +
                "Its home is not a CdmwCcmCif.KeylessCCMHome.",
                WRONG_HOME_KIND.value);
        }

        // create Component (could raise CreateFailure exception)
        try {
            component = keylessHome.create_component_with_config_values(config.getConfigValueArray());
        } catch (org.omg.CORBA.SystemException e) {
            throw new CreateFailure(
                "SystemException while asking home to create component " + getId() + ":\n" +
                e.toString(),
                CREATE_FAILURE.value);
        }
            
        // set as activated
        isActivated = true;
            
        if (componentConfigurator != null) {
            System.out.println("ASSEMBLY INFO: configure component " + getId());
            // apply componentConfigurator
            try {
                componentConfigurator.configure(component);
            } catch (org.omg.Components.WrongComponentType e) {
                throw new CreateFailure(
                    "Error while configuring componentproperties of component " + getId() +
                    ":\nConfigurator cannot configure this type of component.",
                    PROPERTIES_CONFIGURATION_ERROR.value);
            }
        }

        // register component and facets in naming service (if necessary)
        registerWithNaming();
    }


    /**
     * Deactivate the component.
     *  - request its parent home to remove the CCMObject
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


        // unregister component from naming
        unregisterFromNaming();

        System.out.println("ASSEMBLY INFO: remove component " + getId());

        // remove component
        org.omg.Components.CCMHome home = 
            parentHome.getHome();
        home.remove_component(component);
        component = null;

        // set as deactivated
        isActivated = false;
    }

    
    /**
     * Call configuration_complete() component's operation.
     *
     * @exception CreateFailure if the component throws
     *            an InvalidConfiguration exception.
     */
    public void completeConfiguration() 
        throws CreateFailure
    {
        cdmw.common.Assert.check(isActivated);
        
        if (!wasConfigurationCompleteCalled) {
            try {
                System.out.println("ASSEMBLY INFO: complete configuration of component " + getId());
                component.configuration_complete();
                wasConfigurationCompleteCalled = true;
            } catch (org.omg.Components.InvalidConfiguration e) {
                throw new CreateFailure(
                    "The configuration state of component " + getId() +  
                    " is not acceptable.",
                    PROPERTIES_CONFIGURATION_ERROR.value);
            }
        }
    }
    
}


