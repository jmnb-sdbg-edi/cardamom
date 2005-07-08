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
import cdmw.ccm.deployment.ConfigValueImpl;
import cdmw.ccm.deployment.ConfigValues;

import org.omg.Components.Configurator;
import com.thalesgroup.CdmwDeployment.HomeConfigurator;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ComponentKindValueHelper;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValue;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValueHelper;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValue;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValueHelper;
import com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription;
import com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescriptionSeqHelper;

import com.thalesgroup.CdmwDeployment.INVALID_CONFIGURATION_FOR_HOME;
import com.thalesgroup.CdmwDeployment.IMPL_ENTRY_POINT_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.UNKOWN_IMPL_ID;
import com.thalesgroup.CdmwDeployment.INSTALLATION_FAILURE;
import com.thalesgroup.CdmwDeployment.PROPERTIES_CONFIGURATION_ERROR;
import com.thalesgroup.CdmwDeployment.INTERFACE_REPOSITORY_ERROR;
import com.thalesgroup.CdmwDeployment.NOT_A_HOME;
import com.thalesgroup.CdmwDeployment.NOT_A_COMPONENT;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CSD;
import com.thalesgroup.CdmwDeployment.NAMING_SERVICE_ERROR;
import com.thalesgroup.CdmwDeployment.ILLEGAL_STATE;

import com.thalesgroup.CdmwDeployment.COMPONENT_KIND;    
import com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME;
import com.thalesgroup.CdmwDeployment.THREADING_POLICY;
import com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID;
import com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME;
import com.thalesgroup.CdmwDeployment.VALUETYPE_FACTORY_DEPENDENCIES;


/**
 * Class representing a Home.
 */
class Home {

    /**
     * Identifier of the Home.
     */
    private String id;
    
    /**
     * Parent Container.
     */
    private Container parentContainer;
    
    /**
     * UUID of the Home, as specified
     * into the .csd file.
     */
    private String uuid;
    
    /**
     * Entrypoint of the Home, as pecified
     * into the .csd file.
     */
    private String entrypoint;
    
    /**
     * Boolean specifying if configuration_complete() operation
     * has to be called on each component of this Home.
     */
    private boolean completeConfig;
    
    /**
     * Map of SingleComponents which are into this Home.
     * They are indexed by their identifier.
     */
    private cdmw.ccm.deployment.OrderedMap components;
    
    /**
     * The CCMHome CORBA object this class represent.
     */
    private org.omg.Components.CCMHome home;
    
    /**
     * Configurator for this Home.
     */
    private HomeConfigurator homeConfigurator;
    
     /**
     * Configurator for the components created by this Home.
     */
    private Configurator componentConfigurator;
    
    /**
     * Boolean set to true if home has been activated.
     */
    private boolean isActivated;

    
    /**
     * ConfigValues for Home creation.
     * Other ConfigValues will be added at Home creation, according to other 
     * Home's attributes (componentKind, servantLifetime, threadPolicy...)
     */
    private ConfigValues config;

    /**
     * ComponentKind to be used for this Home configuration.
     */
    private ComponentKindValue componentKind;
    
    /**
     * ServantLifetime to be used for this Home configuration.
     */
    private ServantLifetimeValue servantLifetime;
    
    /**
     * ThreadingPolicy to be used for this Home configuration.
     */
    private ThreadingPolicyValue threadPolicy;
    
    /**
     * Home's RepositoryId to be used for this Home configuration.
     */
    private String homeRepid;
    
    /**
     * ServantClassname to be used for this Home configuration.
     */
    private String homeServantClassname;
    
    /**
     * map of valuetype factories to be used for this Home configuration.
     */
    private java.util.Map valuetypeFactories;

    /**
     * RepositoryId of components this home creates.
     */
    private String compRepid;
    
     /**
     * List of names the home has to be registered with.
     */
    private java.util.Collection namingRegistrations;


    /**
     * Constructor.
     * It automatically add the new SingleComponent to its parent home
     * and to its parent assembly.
     *
     * @param id the home identifier. 
     *        (as specified in .cad file in homeplacement).
     * @param parentComponentServer parent Container of the Home.
     * @param uuid UUID of the home and component implementation.
     * @param entrypoint name of the static operation for Home creation.
     */
    public Home(String id,
                Container parentContainer,
                String uuid,
                String entrypoint)
    {
        System.out.println("            Home " + id + " (entrypoint=" + entrypoint + ")");
        if (id == null) {
            throw new NullPointerException("id is null");
        }
        if (parentContainer == null) {
            throw new NullPointerException("parentContainer is null");
        }
        if (uuid == null) {
            throw new NullPointerException("uuid is null");
        }
        if (entrypoint == null) {
            throw new NullPointerException("entrypoint is null");
        }

        this.id = id;
        this.parentContainer = parentContainer;
        this.uuid = uuid;
        this.entrypoint = entrypoint;
        this.completeConfig = false;
        components = new cdmw.ccm.deployment.OrderedMap();
        home = null;
        homeConfigurator = null;
        componentConfigurator = null;
        isActivated = false;

        config = new ConfigValues();
        componentKind = null;
        servantLifetime = null;
        threadPolicy = null;
        homeRepid = null;
        compRepid = null;
        homeServantClassname = null;
        valuetypeFactories = new cdmw.ccm.deployment.OrderedMap();
        namingRegistrations = new java.util.HashSet();

        parentContainer.addHome(this);
        parentContainer.getParentAssembly().addHome(this);
    }

    /**
     * Returns the identifier of this Home.
     *
     * @return the identifier of this Home.
     */
    public String getId() {
        return id;
    }

    /**
     * Returns the parent Container of this Home.
     *
     * @return the parent Container of this Home.
     */
    public Container getParentContainer() {
        return parentContainer;
    }
    
    /**
     * Returns the parent Assembly of this Home.
     *
     * @return the parent Assembly of this Home.
     */
    public AssemblyImpl getParentAssembly() {
        return parentContainer.getParentAssembly();
    }
    
    /**
     * Returns the UUID of this Home's implementation.
     *
     * @return the UUID of this Home's implementation.
     */
    public String getUUID() {
        return uuid;
    }

    /**
     * Returns the entrypoint of this Home's implementation.
     *
     * @return the entrypoint of this Home's implementation.
     */
    public String getEntryPoint() {
        return entrypoint;
    }

    /**
     * Set the configuration complete property.
     * If true, the configuration_complete() operation will
     * be called on each component created by this Home.
     *
     * @param completeConfig the configuration complete property.
     */
    public void setConfigurationComplete(boolean completeConfig) {
        this.completeConfig = completeConfig;
    }
    
    /**
     * Returns the value of the configuration complete property.
     *
     * @return the value of the configuration complete property.
     */
    public boolean getConfigurationComplete() {
        return completeConfig;
    }
    
    /**
     * Returns true is the Home has been activated.
     *
     * @return true is the Home has been activated.
     */
    public boolean isActivated() {
        return isActivated;
    }

    /**
     * Returns the CCMHome CORBA object corresponding 
     * to this Home.
     *
     * @return the CCMHome CORBA object.
     *         (null is the Home is not active)
     */
    public org.omg.Components.CCMHome getHome() {
        return home;
    }

    /**
     * Set a Configurator to configure this home.
     * Replace any previously set Configurator.
     *
     * @param homeConfigurator Configurator to configure this home. 
     */
    public void setHomeConfigurator(HomeConfigurator homeConfigurator) {
        this.homeConfigurator = homeConfigurator;
    }

    /**
     * Returns the Configurator set to this home.
     *
     * @return the Configurator set to this home or
     *         null if no Configurator was set.
     */
    public HomeConfigurator getHomeConfigurator() {
        return homeConfigurator;
    }
    
    /**
     * Set a Configurator to configure the components created by this home.
     * Replace any previously set Configurator.
     *
     * @param configurator Configurator to configure the components 
     *        created by this home.
     */
    public void setComponentConfigurator(Configurator configurator) {
        this.componentConfigurator = configurator;
    }

    /**
     * Returns the Configurator set to configure the components
     * created by this home.
     *
     * @return the Configurator set to configure the components created
     *         by this home or null if no Configurator was set.
     */
    public Configurator getComponentConfigurator() {
        return componentConfigurator;
    }

    /**
     * Adds a SingleComponent to this Home.
     *
     * @param component the SingleComponent to be added.
     */
    void addComponent(SingleComponent component) {
        components.put(component.getId(), component);
    }

    /**
     * Returns the SingleComponent of this Home with the
     * specified identifier.
     *
     * @param componentId the SingleComponent's identifier
     * @return the SingleComponent with the specified identifier.
     *         (null is the Home doesn't have such SingleComponent)
     */
    public SingleComponent getComponent(String componentId) {
        return (SingleComponent) components.get(componentId);
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
     * Set the ComponentKind to be used as configuration value
     * for this Home.
     *
     * @param compKind the ComponentKind.
     *
     * @exception CreateFailure if a ComponentKind was previously set.
     */
    public void setComponentKind(ComponentKindValue compKind)
        throws CreateFailure
    {
        if (componentKind != null) {
            throw new CreateFailure(
                "A ComponentKind is already specified for Home " +
                getId(),
                INVALID_CONFIGURATION_FOR_HOME.value);
        }
        componentKind = compKind;
    }

    /**
     * Returns the ComponentKind used as configuration value
     * by this home.
     *
     * @return the ComponentKind.
     */
    public ComponentKindValue getComponentKind() {
        return componentKind;
    }
    
    /**
     * Set the ServantLifetime to be used as configuration value
     * for this Home.
     *
     * @param lifetime the ServantLifetime.
     *
     * @exception CreateFailure if a ServantLifetime was previously set.
     */
    public void setServantLifetime(ServantLifetimeValue lifetime)
        throws CreateFailure
    {
        if (servantLifetime != null) {
            throw new CreateFailure(
                "A ServantLifetime is already specified for Home " +
                getId(),
                INVALID_CONFIGURATION_FOR_HOME.value);
        }
        servantLifetime = lifetime;
    }

    /**
     * Returns the ServantLifetime used as configuration value
     * by this home.
     *
     * @return the ServantLifetime.
     */
    public ServantLifetimeValue getServantLifetime() {
        return servantLifetime;
    }

    /**
     * Set the ThreadingPolicy to be used as configuration value
     * for this Home.
     *
     * @param lifetime the ThreadingPolicy.
     *
     * @exception CreateFailure if a ThreadingPolicy was previously set.
     */
    public void setThreadingPolicy(ThreadingPolicyValue policy)
        throws CreateFailure
    {
        if (threadPolicy != null) {
            throw new CreateFailure(
                "A ThreadingPolicy is already specified for Home " +
                getId(),
                INVALID_CONFIGURATION_FOR_HOME.value);
        }
        threadPolicy = policy;
    }

    /**
     * Returns the ThreadingPolicy used as configuration value
     * by this home.
     *
     * @return the ThreadingPolicy.
     */
    public ThreadingPolicyValue getThreadingPolicy() {
        return threadPolicy;
    }

    /**
     * Set the RepositoryId of this Home.
     * The validity of RepositoryId is checked with the help
     * of the InterfaceRepository.
     *
     * @param repid the Home's RepositoryId.
     *
     * @exception CreateFailure if a RepositoryId was previously set,
     *            or if RepositoryId doesn't correspond to a home.
     */
    public void setHomeRepositoryId(String repid)
        throws CreateFailure
    {
        if (homeRepid != null) {
            throw new CreateFailure(
                "A HomeRepositoryId is already specified for Home " +
                getId(),
                INVALID_CONFIGURATION_FOR_HOME.value);
        }
        
        // check if repid corresponds to an Home
        org.omg.CORBA.Repository repository;
        try {
            repository = AssemblyFactoryImpl.getInstance().getInterfaceRepository();
        } catch (Exception e) {
		throw new CreateFailure(
                "Cannot contact InterfaceRepository :\n" + e.getMessage(),
                INTERFACE_REPOSITORY_ERROR.value);
        }
        
        org.omg.CORBA.Contained contained = 
            repository.lookup_id(repid);
        
        if (contained == null) {
            throw new CreateFailure(
                "Home RepositoryId " + repid + " is not registred in InterfaceRepository",
                NOT_A_HOME.value);
        }
        
/*        try {
            org.omg.CORBA.InterfaceDef interfaceDef =
                org.omg.CORBA.InterfaceDefHelper.narrow(contained);
            
            if (!interfaceDef.is_a(org.omg.Components.CCMHomeHelper.id())) {
                System.out.println("NOT AN HOME !!");
                throw new CreateFailure(
                    "Home RepositoryId " + repid + " doesn't correspond to an Home",
                    NOT_A_HOME.value);
            }
        } catch (org.omg.CORBA.SystemException e) {
            throw new CreateFailure(
                "Home RepositoryId " + repid + " doesn't correspond to an Interface",
                NOT_A_HOME.value);
        }
*/
        if (!contained._is_a("IDL:omg.org/CORBA/ComponentIR/HomeDef:1.0")) {
            System.out.println("NOT AN HOME !!");
            throw new CreateFailure(
                "Home RepositoryId " + repid + " doesn't correspond to an Home",
                NOT_A_HOME.value);
        }
        
        homeRepid = repid;
    }
    
    /**
     * Returns the Home's RepositoryId.
     *
     * @return the Home's RepositoryId.
     */
    public String getHomeRepositoryId() {
        return homeRepid;
    }
    
    /**
     * Set the RepositoryId of the components created by this Home.
     * The validity of RepositoryId is checked with the help
     * of the InterfaceRepository.
     *
     * @param repid the component's RepositoryId.
     *
     * @exception CreateFailure if a RepositoryId was previously set,
     *            or if RepositoryId doesn't correspond to a component.
     */
    public void setComponentRepositoryId(String repid)
        throws CreateFailure
    {
        if (compRepid != null) {
            throw new CreateFailure(
                "A ComponentRepositoryId is already specified for Home " +
                getId(),
                INVALID_CONFIGURATION_FOR_HOME.value);
        }

        // check if repid corresponds to an Component
        org.omg.CORBA.Repository repository;
        try {
            repository = AssemblyFactoryImpl.getInstance().getInterfaceRepository();
        } catch (Exception e) {
              throw new CreateFailure(
                "Cannot contact InterfaceRepository :\n" + e.getMessage(),
                INTERFACE_REPOSITORY_ERROR.value);
        }

        org.omg.CORBA.Contained contained = 
            repository.lookup_id(repid);

        if (contained == null) {
            throw new CreateFailure(
                "Component RepositoryId " + repid + " is not registred in InterfaceRepository",
                NOT_A_COMPONENT.value);
        }
        
/*        try {
            org.omg.CORBA.InterfaceDef interfaceDef =
                org.omg.CORBA.InterfaceDefHelper.narrow(contained);
            
            if (!interfaceDef.is_a(org.omg.Components.CCMObjectHelper.id())) {
                throw new CreateFailure(
                    "Component RepositoryId " + repid + 
                    " doesn't correspond to a Component",
                    NOT_A_COMPONENT.value);
            }
        } catch (org.omg.CORBA.SystemException e) {
            throw new CreateFailure(
                "Component RepositoryId " + repid + 
                " doesn't correspond to an Interface",
                NOT_A_COMPONENT.value);
        }
*/
        if (!contained._is_a("IDL:omg.org/CORBA/ComponentIR/ComponentDef:1.0")) {
            System.out.println("NOT AN COMPONENT !!");
            throw new CreateFailure(
                "Component RepositoryId " + repid + 
                " doesn't correspond to a Component",
                NOT_A_COMPONENT.value);
        }

        compRepid = repid;
    }
    
    /**
     * Returns the components' RepositoryId.
     *
     * @return the components' RepositoryId.
     */
    public String getComponentRepositoryId() {
        return compRepid;
    }
    

    /**
     * Set the ServantClassname to be used as configuration value
     * for this Home.
     *
     * @param lifetime the ServantClassname.
     *
     * @exception CreateFailure if a ServantClassname was previously set.
     */
    public void setHomeServantClassname(String servantClassname)
        throws CreateFailure
    {
        if (homeServantClassname != null) {
            throw new CreateFailure(
                "A HomeServantClassname is already specified for Home " +
                getId(),
                INVALID_CONFIGURATION_FOR_HOME.value);
        }
        homeServantClassname = servantClassname;
    }

    /**
     * Returns the ServantClassname used as configuration value
     * by this home.
     *
     * @return the ServantClassname.
     */
    public String getHomeServantClassname() {
        return homeServantClassname;
    }

    /**
     * Adds a valuetype factory entry point o be used in configuration
     * for this Home.
     *
     * @param repid the RepositoryId of the valuetypes created by the factory.
     * @param factoryEntrypoint the entrypoint for factory creation.
     *
     * @exception CreateFailure if a valuetype factory for the same
     *            RepositoryId was previously added.
     */
    public void addValuetypeFactory(String repid, 
                                    String factoryEntrypoint)
        throws CreateFailure
    {
        if (valuetypeFactories.containsKey(repid)) {
            throw new CreateFailure(
                "The csd corresponding to Home " + getId() + 
                " contains two valuetypefactory dependencies with same repid",
                ERROR_IN_CSD.value);
        }
        
        valuetypeFactories.put(repid, 
                               new ValuetypeFactoryDescription(repid,
                                                               factoryEntrypoint));
    }
    
     /**
     * Adds a name for the home to be registered
     * into the naming service.
     *
     * @param name the registration name.
     */
    public void addNamingRegistration(String name) {
        namingRegistrations.add(name);
    }
    
    /**
     * Returns the configuration values to be used for this
     * Home creation.
     *
     * @return the configuration values.
     */
    protected ConfigValue[] getConfig() {
        ConfigValues configCopy = (ConfigValues) config.clone();

        // add COMPONENT_KIND config
        if (componentKind != null) {
            ConfigValue c = new ConfigValueImpl();
            c.name = COMPONENT_KIND.value;
            c.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
            ComponentKindValueHelper.insert(c.value, componentKind);
            configCopy.setConfigValue(c);
        }

        // add SERVANT_LIFETIME config
        if (servantLifetime != null) {
            ConfigValue c = new ConfigValueImpl();
            c.name = SERVANT_LIFETIME.value;
            c.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
            ServantLifetimeValueHelper.insert(c.value, servantLifetime);
            configCopy.setConfigValue(c);
        }

        // add THREADING_POLICY config
        if (threadPolicy != null) {
            ConfigValue c = new ConfigValueImpl();
            c.name = THREADING_POLICY.value;
            c.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
            ThreadingPolicyValueHelper.insert(c.value, threadPolicy);
            configCopy.setConfigValue(c);
        }

        // add HOME_REPOSITORY_ID config
        if (homeRepid != null) {
            ConfigValue c = new ConfigValueImpl();
            c.name = HOME_REPOSITORY_ID.value;
            c.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
            c.value.insert_string(homeRepid);
            configCopy.setConfigValue(c);
        }

        // add HOME_SERVANT_CLASSNAME config
        if (homeServantClassname != null) {
            ConfigValue c = new ConfigValueImpl();
            c.name = HOME_SERVANT_CLASSNAME.value;
            c.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
            c.value.insert_string(homeServantClassname);
            configCopy.setConfigValue(c);
        }

        // add VALUETYPE_FACTORY_DEPENDENCIES config
        if (valuetypeFactories.size() != 0) {
            ValuetypeFactoryDescription[] factoryList = 
                new ValuetypeFactoryDescription[valuetypeFactories.size()];
                
            java.util.Iterator it = valuetypeFactories.values().iterator();
            int i = 0;
            while (it.hasNext()) {
                factoryList[i] = (ValuetypeFactoryDescription) it.next();
                i++;
            }
            
            ConfigValue c = new ConfigValueImpl();
            c.name = VALUETYPE_FACTORY_DEPENDENCIES.value;
            c.value = AssemblyFactoryImpl.getInstance().getOrb().create_any();
            ValuetypeFactoryDescriptionSeqHelper.insert(c.value, factoryList);
            configCopy.setConfigValue(c);
        }

        return configCopy.getConfigValueArray();
    }

    /**
     * Activates the Home.
     *  - requests its parent Container to create the Home.
     *  - set home configurator and component configurator
     *    to the home.
     *  - register the home into the Naming Service (if necessary).
     *  - for each component of this home, activate it. 
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

        System.out.println("ASSEMBLY INFO: create home " + id);

        if (! parentContainer.isActivated()) {
            throw new CreateFailure(
                "The parent Container ("+ parentContainer.getId() + 
                ") of Home " + id + " is not activated",
                ILLEGAL_STATE.value);
        }

        // create Home
        try {
            org.omg.Components.Deployment.Container container = 
                parentContainer.getContainer();
            home =
                container.install_home(uuid, entrypoint, getConfig());
        } catch (org.omg.Components.Deployment.InvalidConfiguration e) {
            throw new CreateFailure(
                "The Home " + id + " has an invalid configuration:\n" +
                invalidConfigurationToString(e),
                INVALID_CONFIGURATION_FOR_HOME.value);
        } catch (org.omg.Components.Deployment.UnknownImplId e) {
            throw new CreateFailure(
                "Cannot find UUID " + uuid + " for home " + id,
                UNKOWN_IMPL_ID.value);
        } catch (org.omg.Components.Deployment.ImplEntryPointNotFound e) {
            throw new CreateFailure(
                "Cannot find entrypoint " + entrypoint + " for home " + id,
                IMPL_ENTRY_POINT_NOT_FOUND.value);
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            throw new CreateFailure(
                "Installation of home " + id + " failed",
                INSTALLATION_FAILURE.value);
        }

        // set as activated
        isActivated = true;

        if (homeConfigurator != null) {
            System.out.println("ASSEMBLY INFO: configure home " + id);
            // apply homeConfigurator
            try {
                homeConfigurator.configure_home(home);
            } catch (org.omg.Components.WrongComponentType e) {
                throw new CreateFailure(
                    "Error while configuring homeproperties of home " + id +
                    ":\nConfigurator cannot configure this type of home.",
                    PROPERTIES_CONFIGURATION_ERROR.value);
            }
        }

        if (componentConfigurator != null) {
            System.out.println("ASSEMBLY INFO: set component configurator to home " + id);
            // apply componentConfigurator
            // home must be a HomeConfiguration
            org.omg.Components.HomeConfiguration homeConf;
            try {
                homeConf =
                    org.omg.Components.HomeConfigurationHelper.narrow(home);

            } catch (org.omg.CORBA.SystemException e) {
                // narrow failed
                throw new CreateFailure(
                    "Error while configuring componentproperties of home " + id +
                    ":\nThis home is not a HomeConfiguration",
                    PROPERTIES_CONFIGURATION_ERROR.value);
            }

            // set configurator to HomeConfiguration
            homeConf.set_configurator(componentConfigurator);
        }

        // register home in naming service (if necessary)
        registerWithNaming();

        // for each component
        java.util.Iterator it = components.values().iterator();
        while(it.hasNext()) {
            Component component = (Component) it.next();

            // activate it
            component.activate();
        }
    }

    /**
     * Registers the home into the Naming Service 
     * with any previously added registration name.
     *
     * @exception CreateFailure if registration fails.
     */
    private void registerWithNaming() 
        throws CreateFailure
    {
        if (namingRegistrations.size() > 0) {
            // get NameService
            org.omg.CosNaming.NamingContext nameServ;
            try {
                nameServ = AssemblyFactoryImpl.getInstance().getNamingService();
            } catch (Exception e) {
                throw new CreateFailure(
                    "Cannot contact NamingService: " + e.getMessage(),
                    NAMING_SERVICE_ERROR.value);
            }
            
            // create NamingInterface
            cdmw.namingandrepository.NamingInterface naming = 
                new cdmw.namingandrepository.NamingInterface(nameServ);
        
            java.util.Iterator it = namingRegistrations.iterator();
            while (it.hasNext()) {
                String name = (String) it.next();
                
                // register home
                try {
                    naming.bind(name, home, true);
                } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                    throw new CreateFailure(
                        "NotFound exception during registration of home " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                    throw new CreateFailure(
                        "CannotProceed exception during registration of home " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                    throw new CreateFailure(
                        "InvalidName exception during registration of home " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                } catch (org.omg.CosNaming.NamingContextPackage.AlreadyBound e) {
                    throw new CreateFailure(
                        "AlreadyBound exception during registration of home " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                }                
            }
        }
    }
    
    /**
     * Unregisters the home from the Naming Service.
     *
     * @exception CreateFailure if registration fails.
     */
    private void unregisterFromNaming() 
        throws RemoveFailure
    {
        if (namingRegistrations.size() > 0) {
            // get NameService
            org.omg.CosNaming.NamingContext nameServ;
            try {
                nameServ = AssemblyFactoryImpl.getInstance().getNamingService();
            } catch (Exception e) {
                throw new RemoveFailure(
                    "Cannot contact NamingService: " + e.getMessage(),
                    NAMING_SERVICE_ERROR.value);
            }
            
            // create NamingInterface
            cdmw.namingandrepository.NamingInterface naming = 
                new cdmw.namingandrepository.NamingInterface(nameServ);
        
            java.util.Iterator it = namingRegistrations.iterator();
            while (it.hasNext()) {
                String name = (String) it.next();

                // unregister home
                try {
                    naming.unbind(name);
                } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                    // Home has already been unregistered by someone else ??
                    // Ignore.
                } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed e) {
                    throw new RemoveFailure(
                        "CannotProceed exception during unregistration of home " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value);
                } catch (org.omg.CosNaming.NamingContextPackage.InvalidName e) {
                    throw new RemoveFailure(
                        "InvalidName exception during unregistration of home " + id +
                        " with name " + name,
                        NAMING_SERVICE_ERROR.value); 
                }                
            }
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
     * Dectivates the Home.
     *  - deactivates all child Components
     *  - unregister the home from the Naming Service (if necessary)
     *  - requests its parent Container to remove the Home.
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

        // for each component (backward order)
        java.util.ListIterator it = components.valuesListIterator(components.size());
        while(it.hasPrevious()) {
            Component component = (Component) it.previous();

            // deactivate it
            component.deactivate();
        }

        // unregister home
        unregisterFromNaming();

        System.out.println("ASSEMBLY INFO: remove home " + id);

        // remove home
        org.omg.Components.Deployment.Container container =
            parentContainer.getContainer();
        container.remove_home(home);
        home = null;

        // set as deactivated
        isActivated = false;
    }

    
    /**
     * Complete configuration of all children components.
     *
     * @exception CreateFailure if a configuration_complete() fails
     */
    public void completeConfiguration() 
        throws CreateFailure
    {
        cdmw.common.Assert.check(isActivated);
        
        if (completeConfig) {
            // complete configuration of all components
            java.util.Iterator it = components.values().iterator();
            while(it.hasNext()) {
                SingleComponent component = (SingleComponent) it.next();
                component.completeConfiguration();
            }
        }
    }
    
}


