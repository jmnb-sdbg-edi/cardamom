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


package cdmw.ccm.container;

import cdmw.common.Assert;
import cdmw.common.Locations;
import cdmw.common.BadParameterException;
import cdmw.ossupport.JarClassLoader;

import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.namingandrepository.NamingInterface;
import cdmw.lifecycle.RetainObjectDeactivator;

import cdmw.ccm.common.CCMUtils;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValue;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValue;

/**
 * Implementation of a CDMW Generic Container.
 * <B>NOTA:</B> This is not thread safe. It is advised to incarnate only objects
 * with a SINGLE_THREAD threading policy.
 */
public class ContainerImpl
    extends com.thalesgroup.CdmwCcmContainer.ContainerPOA {

    //
    // Attributes
    //

    /**
     * A reference to the ORB pseudo-object
     */
    private org.omg.CORBA.ORB orb;

    /**
     * A parent POA used for the container activation
     */
    private org.omg.PortableServer.POA poa;

    /**
     * A POA used for single thread component homes
     */
    private org.omg.PortableServer.POA homesSerializePoa;

    /**
     * A POA used for thread-safe component homes
     */
    private org.omg.PortableServer.POA homesMultithreadPoa;

    /**
     * A reference to the ComponentInstallation object
     */
    private
        com.thalesgroup.CdmwDeployment.ComponentInstallation cpntInstallation;

    /**
     * A reference to the ComponentServer object
     */
    private org.omg.Components.Deployment.ComponentServer componentServer;

    /**
     * A sequence of ConfigValue objects passed in to the
     * <I>ComponentServer.create_container</I>
     * operation at the Container creation.
     */
    private org.omg.Components.ConfigValue[] configValues;

    /**
     * Object Deactivator for use by homes for their deactivation.
     */
    private RetainObjectDeactivator objectDeactivator;

    /**
     * A reference to a HomeRegistration for use to register component homes
     * so they may be located by the HomeFinder.
     */
    private org.omg.Components.HomeRegistration homeRegistration;

    /**
     * The CDMW Event Channel Factory.
     */
    private com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory;

    /**
     * The CDMW Process name.
     */
    private String processName;

    /**
     * The CDMW Application name.
     */
    private String applicationName;

    /**
     * A list of all the homes created by the container
     * and their associated data.
     */
    private java.util.Hashtable ccmHomesList = new java.util.Hashtable();

    /**
     * Indicates whether the remove operation has been called or not.
     */
    private boolean isRemoved = false;

    /**
     * Constructor.
     *
     * @param orb A reference to the ORB pseudo-object
     * @param poa A parent POA used for the container activation.
     * Should have a RETAIN servant retention policy
     * @param homesSerializePoa A POA used for single thread
     * component homes. Single thread homes will be activated on this POA.
     * (Use of HomesServantActivator needed with the POA)
     * @param homesMultithreadPoa A POA used for thread-safe component homes.
     * Multi-thread (thread-safe) homes will be activated on this POA.
     * (Use of HomesServantActivator needed with the POA)
     * @param cpntInstallation A ComponentInstallation object
     * @param componentServer A ComponentServer object
     * @param homeRegistration A HomeRegistration for use to register component
     * homes so they may be located by the HomeFinder.
     * @param processName The name of the CDMW Process.
     * @param applicationName The name of the CDMW Application.
     * @param config A sequence of ConfigValue objects passed in to the
     * <I>ComponentServer.create_container</I> operation
     * at the Container creation.
     */
    public ContainerImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA poa, // With RETAIN servant retention policy
        org.omg.PortableServer.POA homesSerializePoa,
        org.omg.PortableServer.POA homesMultithreadPoa,
        com.thalesgroup.CdmwDeployment.ComponentInstallation cpntInstallation,
        org.omg.Components.Deployment.ComponentServer componentServer,
        org.omg.Components.HomeRegistration homeRegistration,
        com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory,
        String processName,
        String applicationName,
        org.omg.Components.ConfigValue[] config) {

        this.orb = orb;
        this.poa = poa;
        this.homesSerializePoa = homesSerializePoa;
        this.homesMultithreadPoa = homesMultithreadPoa;
        this.cpntInstallation = cpntInstallation;
        this.componentServer = componentServer;
        this.configValues = config;
        this.objectDeactivator = new RetainObjectDeactivator(orb);
        this.homeRegistration = homeRegistration;
        this.eventChannelFactory = eventChannelFactory;
        this.processName = processName;
        this.applicationName = applicationName;

    }

    /**
     * Destructor.
     */
    public void destroy() {

        if (isRemoved) {
            try {
                removeAllHomes();
            } catch (Exception e) {}
        }
        ccmHomesList.clear();

    }

    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    /**
     * This operation returns whether the Container supports the
     * creation of component homes with the specified repository Id.
     *
     * @param repository_id The Repository Id of the desired home interface.
     *
     * @return <B>true</B> if the container supports/creates homes of
     * <I>repository_id</I> type or of derived types. Otherwise it returns
     * <B>false</B>.
     */
    public boolean supports(String repositoryId) {
        java.util.Enumeration e = this.ccmHomesList.keys();
        for( ; e.hasMoreElements(); ) {
            org.omg.Components.CCMHome home =
                (org.omg.Components.CCMHome) e.nextElement();
            if ( home._is_a(repositoryId) ) return true;
        }
        return false;
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    /**
     * This operation returns a sequence of CORBA Repository IDs
     * of Components' homes that can be created by the container.
     *
     * @return A sequence of CORBA Repository IDs of components' homes that
     * can be created by the container.
     */
    public String[] get_supported_objects() {

        java.util.Enumeration e = this.ccmHomesList.keys();
        String[] seq = new String[this.ccmHomesList.size()];
        int i = 0;
        for( ; e.hasMoreElements(); ) {
            org.omg.Components.CCMHome home = 
                (org.omg.Components.CCMHome) e.nextElement();
            seq[i++] = ((CCMHomeData) this.ccmHomesList.get(home)).repositoryId;
        }
        return seq;

    }

    //
    // IDL:omg.org/Components/Deployment/Container/configuration:1.0
    //
    /**
     * Gives access to the ConfigValue objects passed in
     * to the create_container operation at the Container creation.
     * @return A sequence of ConfigValue objects passed in
     * to the <I>create_container</I> operation at the Container creation.
     */
    public org.omg.Components.ConfigValue[] configuration() {
        return this.configValues;
    }

    //
    // IDL:omg.org/Components/Deployment/Container/get_component_server:1.0
    //
    /**
     * Gives access to the ComponentServer object that created this Container.
     *
     * @return Object reference to the ComponentServer object
     * that created this Container.
     */
    public org.omg.Components.Deployment.ComponentServer
        get_component_server() {
        return this.componentServer;
    }

    //
    // IDL:omg.org/Components/Deployment/Container/install_home:1.0
    //
    /**
     * The install_home operation installs and returns a new CCMHome object.
     * The id and entrypt parameters are used by the container to locate
     * an implementation file and instantiate a new home object.
     * The config parameter represents a sequence of
     * ConfigValue objects that provide name value pairs used to configure
     * the installation of the new home instance, e.g. provide persistency
     * source, transaction and security policies which must be applied
     * to the home and its components. The operation raises an UnknownImplId
     * exception if the id parameter does not correspond to any component
     * packages installed using the ComponentInstallation object. The operation
     * raises an ImplEntryPointNotFound exception if the entrypt parameter
     * cannot be found in the implementation returned
     * from ComponentInstallation. The operation raises an InstallationFailure
     * exception if the home could not be installed in the container
     * for internal reasons such as insufficient resources or inadequate
     * implementation for this container (e.g. installing a C++ home
     * implementation in a Java container). The operation raises
     * an InvalidConfiguration exception if the config parameter
     * does not contain valid configuration name value pairs.
     *
     * @param id Refers to a particular implementation
     * within a component package.
     * @param entrypt Entry point to create a component home
     * within the [shared] library.
     * @param config Represents a sequence of ConfigValue objects
     * that provides name value pairs used to configure the installation
     * of the new home instance, e.g. provide persistency source,
     * transaction and security policies which must be applied
     * to the home and its components.
     *
     * <p/> <B>NOTA:</B> These are the configuration values understood
     * by the container for component home installation.
     * <TABLE>
     *   <TR>
     *      <TD><U><B>Config Name</B></U></TD><TD><U><B>Type</B></U></TD>
     *   </TR>
     *   <TR>
     *      <TD>COMPONENT_KIND (*)</TD><TD>ComponentKindValue</TD>
     *   </TR>
     *   <TR>
     *      <TD>SERVANT_LIFETIME (**)</TD><TD>ServantLifetimeValue</TD>
     *   </TR>
     *   <TR>
     *      <TD>POA_POLICIES_FOR_UNCLASSIFIED (***)</TD><TD>sequence< PoaPolicy ></TD>
     *   </TR>
     *   <TR>
     *      <TD>THREADING_POLICY (*)</TD><TD>ThreadingPolicyValue</TD>
     *   </TR>
     *   <TR>
     *      <TD>EXTENDED_POA_POLICIES</TD><TD>sequence< PoaPolicy ></TD>
     *   </TR>
     *   <TR>
     *      <TD>HOME_REPOSITORY_ID (*)</TD><TD>CORBA::RepositoryId</TD>
     *   </TR>
     *   <TR>
     *      <TD>HOME_SERVANT_CLASSNAME (*)</TD><TD>string</TD>
     *   </TR>
     *   <TR>
     *      <TD>VALUETYPE_FACTORY_DEPENDENCIES</TD><TD>sequence< ValuetypeFactoryDescription ></TD>
     *   </TR>
     * </TABLE>
     * <p/>
     * <TABLE>
     *   <TR>
     *      <TD>(*)</TD><TD>mandatory</TD>
     *   </TR>
     *   <TR>
     *      <TD>(**)</TD><TD>mandatory for session, process, and entity and NOT for service component kind</TD>
     *   </TR>
     *   <TR>
     *      <TD>(***)</TD><TD>mandatory for unclassified component kind</TD>
     *   </TR>
     * </TABLE>
     *
     *
     * @return Object reference to the new CCMHome object.
     *
     * @exception org.omg.Components.Deployment.UnknownImplId
     * Raised if the <I>id</I> parameter does not correspond to
     * any component packages installed using the ComponentInstallation object.
     * @exception org.omg.Components.Deployment.ImplEntryPointNotFound
     * Raised if the <I>entrypt</I> parameter cannot be found
     * in the implementation returned from ComponentInstallation.
     * @exception org.omg.Components.Deployment.InstallationFailure
     * Raised if the home could not be installed in the container
     * for internal reasons such as insufficient resources
     * or inadequate implementation for this container.
     * @exception org.omg.Components.Deployment.InvalidConfiguration
     * Raised if the <I>config</I> parameter does not contain
     * valid configuration name value pairs.
     */
    public org.omg.Components.CCMHome install_home(
        String id,
        String entrypt,
        org.omg.Components.ConfigValue[] config)
        throws
            org.omg.Components.Deployment.UnknownImplId,
            org.omg.Components.Deployment.ImplEntryPointNotFound,
            org.omg.Components.Deployment.InstallationFailure,
            org.omg.Components.Deployment.InvalidConfiguration {

        // TODO: Add traces...

        // Get the Java .class file location.
        // This can raise UnknownImplId or InstallationFailure exceptions.
        if (this.cpntInstallation == null) {
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.COMPONENT_INSTALLATION_NOT_FOUND.value);
        }
        String location = this.cpntInstallation.get_implementation(id);

        // Get the home executor factory method

        // In Java, the entry point is the name of a class and static method
        // which may be invoked to create a servant which implements
        // the component home. The method must have the following signature:
        //
        // public static HomeExecutorBase
        // foo();

        java.lang.reflect.Method factoryMethod = null;
        String classname = "";
        try {
            // get the method name and the class name from the entry point
            int ind = entrypt.lastIndexOf('.');
            classname = entrypt.substring(0, ind);
            String methodname = entrypt.substring(ind + 1);
            // load the class
            Class factory = loadClass(classname, location);
            // invoke the method
            java.lang.reflect.Method[] methods = factory.getMethods();
            for(int i=0; i<methods.length; i++) {
                if (methods[i].getName().equals(methodname)) {
                    factoryMethod = methods[i];
                    break;
                }
            }
        } catch(IndexOutOfBoundsException ioobe) {
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                ioobe.toString());
        } catch(LinkageError le) {
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                le.toString());
        } catch(ClassNotFoundException cnfe) {
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                cnfe.toString());
        }

        // Get Home executor
        if (factoryMethod == null) {
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                "Entry point '" + entrypt + "' not found.");
        }
        org.omg.Components.HomeExecutorBase homeExec = null;
        try {
            homeExec = (org.omg.Components.HomeExecutorBase)
                factoryMethod.invoke(null, null);
        } catch(IllegalAccessException iae) {
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                iae.toString());
        } catch(IllegalArgumentException iae) {
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                iae.toString());
        } catch(java.lang.reflect.InvocationTargetException ite) {
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                ite.toString());
        }

        // NOTA: These are the configuration values understood by the compiler
        // for component home installation.
        //
        // +====================================+======================================+
        // | Config Name                        | Type                                 |
        // +====================================+======================================+
        // | COMPONENT_KIND (*)                 | ComponentKindValue                   |
        // +------------------------------------+--------------------------------------+
        // | SERVANT_LIFETIME (**)              | ServantLifetimeValue                 |
        // +------------------------------------+--------------------------------------+
        // | POA_POLICIES_FOR_UNCLASSIFIED (***)| sequence<PoaPolicy>                  |
        // +------------------------------------+--------------------------------------+
        // | THREADING_POLICY (*)               | ThreadingPolicyValue                 |
        // +------------------------------------+--------------------------------------+
        // | EXTENDED_POA_POLICIES              | sequence<PoaPolicy>                  |
        // +------------------------------------+--------------------------------------+
        // | HOME_REPOSITORY_ID (*)             | CORBA::RepositoryId                  |
        // +------------------------------------+--------------------------------------+
        // | HOME_SERVANT_CLASSNAME (*)         | string                               |
        // +------------------------------------+--------------------------------------+
        // | VALUETYPE_FACTORY_DEPENDENCIES     | sequence<ValuetypeFactoryDescription>|
        // +------------------------------------+--------------------------------------+
        //
        // (*) compulsory
        // (**) compulsory for session, process, and entity and NOT for service component kind
        // (***) compulsory for unclassified component kind
        //
        // We have a home executor.
        // 1) Get the home allocator for the specified home servant classname
        // 2) Create a CCM context
        // 3) Create/get an object deactivator
        // home type and allocate a home servant.

        // *NOTA*
        // HomeAllocators do not only depend on the home's repository Id
        // but are also dependent on the home and component executors. This is
        // expressed by the <composition> in CIDL. We have then chosen the home's
        // (generated) servant's class name as a key to the home allocator.
        // Also note that there is an analogy between a CDMW LifeCycle framework
        // and CIDL composition. The composition defines a tuple
        // (home executor,component category, component executor)
        // that can be mapped to CDMW's (factory, strategy, servant provider).

        // *NOTA*
        // 1) The container creates a POA for all component homes it hosts
        // associated with a servant activator.
        // 2) It creates a POA per home instance that activates both components created by
        // the home and all components' facets.
        // 3) Depending on the servant lifetime policy, an appropriate servant manager
        // will be created.

        // I - # Get appropriate home allocator
        //     #

        //
        // - Get Component kind (compulsory)
        //
        ComponentKindValue componentKind = null;
        org.omg.CORBA.Any featureAny = readConfigValue(
            com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value, config);
        if ( featureAny != null ) {
            componentKind = 
                com.thalesgroup.CdmwDeployment.ComponentKindValueHelper.extract(featureAny);
        } else {
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InvalidConfiguration();
        }

        //
        // - Get's home repository Id (compulsory)
        //
        String homeRepid = "";
        featureAny = readConfigValue(
            com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value, config);
        if ( featureAny != null ) {
            homeRepid = featureAny.extract_string();
        } else {
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InvalidConfiguration();
        }

        //
        // - Get Home's servant classname (compulsory)
        //
        String homeServantClassname = "";
        featureAny = readConfigValue(
            com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME.value,
            config);
        if ( featureAny != null ) {
            homeServantClassname = featureAny.extract_string();
        } else {
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InvalidConfiguration();
        }

        //
        // - Get servant lifetime policy (not compulsory for service)
        //
        ServantLifetimeValue servantLifetime = null;
        featureAny = readConfigValue(
            com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME.value, config);
        if ( featureAny != null ) {
            servantLifetime = 
                com.thalesgroup.CdmwDeployment.ServantLifetimeValueHelper.extract(featureAny);
        } else {
            if (componentKind.value() == ComponentKindValue.SERVICE.value()) {
                servantLifetime = ServantLifetimeValue.METHOD_LIFETIME;
            } else {
                // TODO: Add traces
                throw new org.omg.Components.Deployment.InvalidConfiguration();
            }
        }
        if (componentKind.value() == componentKind.SERVICE.value() &&
            servantLifetime.value() != ServantLifetimeValue.METHOD_LIFETIME.value()) {
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InvalidConfiguration();
        }

        //
        // - Get POA policies for unclassified if componentKind == UNCLASSIFIED
        //
        com.thalesgroup.CdmwDeployment.PoaPolicy[] unclassifiedPoaPolicies =
            null;
        if (componentKind.value() == ComponentKindValue.UNCLASSIFIED.value()) {
            featureAny = readConfigValue(
                com.thalesgroup.CdmwDeployment.POA_POLICIES_FOR_UNCLASSIFIED.value,
                config);
            if (featureAny != null) {
                unclassifiedPoaPolicies =
                    com.thalesgroup.CdmwDeployment.PoaPolicySeqHelper.extract(featureAny);
                // Ignore for the moment as this is not supported yet!
            } else {
                // TODO: Add traces
                throw new org.omg.Components.Deployment.InvalidConfiguration();
            }
        }

        //
        // - Get the threading policy (compulsery)
        //
        ThreadingPolicyValue threadingPolicy = null;
        featureAny = readConfigValue(
            com.thalesgroup.CdmwDeployment.THREADING_POLICY.value, config);
        if (featureAny != null) {
            threadingPolicy =
                com.thalesgroup.CdmwDeployment.ThreadingPolicyValueHelper.extract(featureAny);
        } else {
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InvalidConfiguration();
        }

        //
        // - Get extended poa policies
        //
        com.thalesgroup.CdmwDeployment.PoaPolicy[] extendedPoaPolicies = null;
        featureAny = readConfigValue(
            com.thalesgroup.CdmwDeployment.EXTENDED_POA_POLICIES.value,
            config);
        if (featureAny != null) {
            extendedPoaPolicies =
                com.thalesgroup.CdmwDeployment.PoaPolicySeqHelper.extract(featureAny);
            // Ignore for the moment as this is not supported yet!
        }

        //
        // - Get possible Valuetype factories dependencies
        //
        java.util.Vector dependencies = new java.util.Vector();
        com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription[]
            valuetypeDependencies = null;
        featureAny = readConfigValue(
            com.thalesgroup.CdmwDeployment.VALUETYPE_FACTORY_DEPENDENCIES.value,
            config);

        if (featureAny != null) {
            valuetypeDependencies =
                com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescriptionSeqHelper.extract(featureAny);
            com.thalesgroup.CdmwCcmContainer.ValueTypeInstaller vtInstaller =
                new ValueTypeInstallerImpl(orb);

            try {
                for(int i = 0; i<valuetypeDependencies.length; ++i) {
                    com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription desc =
                        valuetypeDependencies[i];
                    // Register needed valuetype factories

                    // Get shared library location.
                    // This can raise org.omg.Components.Deployment.UnknownImplId,
                    // com.thalesgroup.CdmwDeployment.UnknownValuetype,
                    // and org.omg.Components.Deployment.InstallationFailure
                    String repid = desc.repid;
                    String valLocation =
                        cpntInstallation.get_valuetypefactory_location(
                            id,
                            repid);
                    String factoryEntrypoint = desc.factory_entrypoint;
                    com.thalesgroup.CdmwCcmContainer.ValueTypeRegistration reg =
                        vtInstaller.install_valuetype_factory(repid,
                                                              valLocation,
                                                              factoryEntrypoint);
                    dependencies.add(reg);
                }
            } catch(com.thalesgroup.CdmwDeployment.UnknownValuetype uv) {
                throw new org.omg.Components.Deployment.InvalidConfiguration();
            }
        }

        HomeAllocatorBase homeallocator =
            HomeAllocatorRegistry.getAllocator(homeServantClassname);
        if (homeallocator == null) {
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.UNSUPPORTED_COMPONENT_TYPE.value);
        }

        // Create a component context!
        org.omg.PortableServer.POA homeParentPoa = homesMultithreadPoa;
        if (threadingPolicy.value() == ThreadingPolicyValue.SERIALIZE.value()) {
            homeParentPoa = homesSerializePoa;
        }

        // Allocate a home servant.
        org.omg.PortableServer.Servant servant = null;
        try {
            servant = homeallocator.allocate(homeExec, objectDeactivator);
        } catch (BadParameterException bpe) {
            // TODO: Add Traces
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                bpe.toString());
        }

        //ComponentServantProvider servantProvider = null;
        ComponentServantProviderInterface servantProvider = null;
        try {
            servantProvider = (ComponentServantProviderInterface) 
                homeallocator.getDelegate(servant);
        } catch(ClassCastException cce) {
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.NO_COMPONENT_SERVANT_PROVIDER.value);
        } catch (BadParameterException bpe) {
            // TODO: Add Traces
            throw new org.omg.Components.Deployment.ImplEntryPointNotFound(
                bpe.toString());
        }

        // Create Home object reference
        String homeIdentifier = createHomeIdentifier(id);
        byte[] hoid;
        try {
            hoid = homeIdentifier.getBytes();
            homeParentPoa.activate_object_with_id(hoid, servant);
        } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive saa) {
            // Thrown by activate_object_with_id.
            // We use UNIQUE_ID policy so the servant should not be reused for
            // another ObjectId with this POA.
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        } catch (org.omg.PortableServer.POAPackage.ObjectAlreadyActive oaa) {
            // Thrown by activate_object_with_id.
            // The object is already active! Has the servant provider activated it?
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            // Thrown by activate_object_with_id.
            // This should not happen as wa are using RETAIN policy.
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        } catch (org.omg.CORBA.SystemException se) {
            // This should not happen at this stage since the object is just
            // about to be created ...
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        }
        
        // Create object reference
        // This may raise
        //    org.omg.PortableServer.POAPackage.ObjectNotActive,
        //    org.omg.PortableServer.POAPackage.WrongPolicy, and
        //    org.omg.CORBA.SystemException
        org.omg.Components.CCMHome homeRef = null;
        try {
            org.omg.CORBA.Object obj = homeParentPoa.id_to_reference(hoid);
            homeRef = org.omg.Components.CCMHomeHelper.narrow(obj);
            // [d] Assert the object is really of the expected type
            Assert.check(homeRef != null);
        } catch (Exception e) {
            // TODO: Add traces
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        }

        // Register the home allocator using homeIdentifier (the ObjectId)
        // for use in the ServantActivator.etherealize for home 'disposal'.
        HomeAllocatorRegistry.register(homeIdentifier, homeallocator);

        // Create component context
        try {
            com.thalesgroup.CdmwCcmContainer.CCM2Context componentContext =
                CCMContextFactory.TheInstance.createCCMContext(
                    orb,
                    homeParentPoa,
                    homeRef,
                    homeIdentifier,
                    servantProvider,
                    componentKind,
                    servantLifetime,
                    threadingPolicy,
                    homeRegistration,
                    eventChannelFactory,
                    processName,
                    applicationName);
            servantProvider.set_context(componentContext);
        } catch (Exception e) {
            // TODO: Add traces
            e.printStackTrace();
            throw new org.omg.Components.Deployment.InstallationFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        }
        // ** store current home reference
        ccmHomesList.put(homeRef, new CCMHomeData(homeRepid, dependencies));

        // Store the Home reference within the CDMW Naming and Repository
        if (homeRegistration != null) {
            //  Register it as cdmw.common.Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN/
            //      home_identifier@process_name@application_name.ComponentHome
            try {
                org.omg.CosNaming.NameComponent[] name =
                    new org.omg.CosNaming.NameComponent[1];
                name[0].id =
                    homeIdentifier + "@" + processName + "@" + applicationName;
                name[0].kind = "ComponentHome";

                String homeName = Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN
                    + "/" + NamingInterface.toString(name);

                homeRegistration.register_home(homeRef, homeName);
            } catch (Exception e) {
                // TODO: Add traces
            }
        }

        return homeRef;

    }

    /**
     * Read a config value
     *
     * @param name Name of the configuration data
     * @param config A sequence of all configuration values
     *
     * @return The configration value if found, null otherwise.
     */
    public org.omg.CORBA.Any readConfigValue(
        String name,
        org.omg.Components.ConfigValue[] config) {
        for (int count = 0; count < config.length; ++count) {
            if (name.equals(config[count].name)) {
                return config[count].value;
            }
        }
        return null;
    }

    //
    // IDL:omg.org/Components/Deployment/Container/remove_home:1.0
    //
    /**
     * The remove_home operation removes a home from the container
     * and all components that were created by this home.
     *
     * @param href Object reference of the component home to remove.
     *
     * @exception org.omg.Components.RemoveFailure Raised if the home
     * or any of its components could not be removed from the container
     * for any internal reasons.
     * @exception org.omg.CORBA.BAD_PARAM
     * Raised if <i>href</I> parameter was not created by this container.
     */
    public void remove_home(org.omg.Components.CCMHome href)
        throws org.omg.Components.RemoveFailure {

        java.util.Enumeration e = this.ccmHomesList.keys();
        for( ; e.hasMoreElements(); ) {
            org.omg.Components.CCMHome home =
                (org.omg.Components.CCMHome) e.nextElement();
            if (home._is_equivalent(href)) {
                org.omg.CosLifeCycle.LifeCycleObject obj =
                    org.omg.CosLifeCycle.LifeCycleObjectHelper.narrow(home);
                if (obj == null) {
                    throw new org.omg.Components.RemoveFailure(
                        com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
                }
                try {
                    obj.remove();
                } catch (org.omg.CosLifeCycle.NotRemovable nr) {
                    throw new org.omg.Components.RemoveFailure(
                        com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
                }
                this.ccmHomesList.remove(home);
                return;
            }
        }
        throw new org.omg.CORBA.BAD_PARAM(
            ExceptionMinorCodes.BAD_PARAMUnknownHomeReference,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);

    }

    //
    // IDL:omg.org/Components/Deployment/Container/get_homes:1.0
    //
    /**
     * The get_homes operation returns a sequence
     * of all CCMHome objects created by this Container.
     */
    public org.omg.Components.CCMHome[] get_homes() {

        java.util.Enumeration e = this.ccmHomesList.keys();
        org.omg.Components.CCMHome[] result =
            new org.omg.Components.CCMHome[this.ccmHomesList.size()];
        int i = 0;
        for( ; e.hasMoreElements(); ) {
            result[i++] = (org.omg.Components.CCMHome) e.nextElement();
        }
        return result;

    }

    //
    // IDL:omg.org/Components/Deployment/Container/remove:1.0
    //
    /**
     * The remove operation removes the container and all homes and components
     * created in the container
     *
     * @exception org.omg.Components.RemoveFailure Raised if the container
     * or any of its homes could not be removed for any internal reasons.
     */
    public void remove() throws org.omg.Components.RemoveFailure {

        if (this.isRemoved) {
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                ExceptionMinorCodes.OBJECT_NOT_EXIST,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        try {
            byte[] oid = poa.servant_to_id(this);
            poa.deactivate_object(oid);
            this.isRemoved = true;
        } catch (Exception e) {
            throw new org.omg.Components.RemoveFailure(
                com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
        }

    }

    /**
     * Removes all homes created by the container
     * and all the components they've created.
     *
     * @exception org.omg.Components.RemoveFailure
     * Raised if any of the homes could not be removed for any internal reasons.
     */
    private void removeAllHomes() throws org.omg.Components.RemoveFailure {

        java.util.Enumeration e = this.ccmHomesList.keys();
        for( ; e.hasMoreElements(); ) {
            org.omg.Components.CCMHome home =
                (org.omg.Components.CCMHome) e.nextElement();
            org.omg.CosLifeCycle.LifeCycleObject obj =
                org.omg.CosLifeCycle.LifeCycleObjectHelper.narrow(home);
            if (obj == null) {
                throw new org.omg.Components.RemoveFailure(
                    com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
            }
            try {
                obj.remove();
            } catch (org.omg.CosLifeCycle.NotRemovable nr) {
                throw new org.omg.Components.RemoveFailure(
                    com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value);
            }
        }

    }

    /**
     * Creates a home Identifier for use for home registration
     * within the home CDMW naming and repository service.
     *
     * @param uuid The Home's UUID as received from Container.install_home()
     *
     * @return A process wide unique id.
     */
    private String createHomeIdentifier(String uuid) {
        return CCMUtils.createUniqueIdentifier(uuid, 3);
    }
    
    /**
     * Loads the class with the given name. If the classname is not found 
     * in the current classpath, then the location param is used to try 
     * to load it from the specified jar file.
     * 
     * @param classname The name of the class to load
     * @param location Optional pathname to an external jar file
     * @return The resulting Class object
     * @exception ClassNotFoundException if the classname is not found
     */
    private Class loadClass(String classname, String location) 
        throws ClassNotFoundException {
        
        try {
            // try to find the class in the current classpath
            return Class.forName(classname);
        } catch(ClassNotFoundException cnfe) {
            // class was not found, so we try to find it at the given location
            System.out.println("Loading " + classname + " from jar file " + location);
            JarClassLoader jarLoader = new JarClassLoader(location);
            return jarLoader.loadClass(classname);
        }
        
    }

    /**
     * Data type that holds any home instance-specific information
     * needed by the container.
     */
    private final class CCMHomeData {

        /**
         * The homes's repository Id.
         */
        public String repositoryId;

        /**
         * Value type factories dependencies.
         */
        public java.util.Vector valuetypeDependencies;

        /**
         * Constructor.
         * @param repid A homes's repository Id.
         * @param dependencies A list of value type factories dependencies
         */
        public CCMHomeData(
            String repid,
            java.util.Vector dependencies) {

            this.repositoryId = repid;
            this.valuetypeDependencies = dependencies;

        }

    }

}

