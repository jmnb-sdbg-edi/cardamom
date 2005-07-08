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

import cdmw.common.Locations;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.cdmwinit.InitUtils;
import cdmw.platformmngt.PlatformInterface;
import com.thalesgroup.CdmwDeployment.StandardConfiguratorFactory;


/**
 * ProcessBehaviour for the AssemblyFactory process.
 */
class AssemblyFactoryBehaviour
    extends cdmw.platformmngt.ProcessBehaviour 
{
    
    private static final String XML_SERVICE = "XMLService";
    private static final String PACKAGE_MANAGER_NAME = "PackageManager";
    private static final String ASSEMBLY_FACTORY_NAME = "AssemblyFactory";



    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA assemblyFactoryPOA;
    private org.omg.PortableServer.POA assembliesPOA;  
    private StandardConfiguratorFactory configFactory;

    private com.thalesgroup.CdmwNamingAndRepository.Repository repository;
    private org.omg.Components.Deployment.AssemblyFactory assemblyFactory;
    private com.thalesgroup.CdmwNamingAndRepository.NameDomain deploymentDomain;
    
    
    /**
     * Constructor
     *
     * @param orb the ORB object
     * @param assemblyFactoryPOA the POA for AssemblyFactory object.
     * @param assembliesPOA the POA for Assembly objects.
     * @param configFactory the ConfiguratorFactory object
     */
    public AssemblyFactoryBehaviour(org.omg.CORBA.ORB orb, 
                                    org.omg.PortableServer.POA assemblyFactoryPOA,
                                    org.omg.PortableServer.POA assembliesPOA,
                                    StandardConfiguratorFactory configFactory)
    {
        this.orb = orb;
        this.assemblyFactoryPOA = assemblyFactoryPOA;
        this.assembliesPOA = assembliesPOA;
        this.configFactory = configFactory;
    }
    
    /**
     * Returns the AssemblyFactory object as the service 
     * provided by this managed process.
     *
     * @return the AssemblyFactory object.
     */
    public org.omg.CORBA.Object getService() {
        return assemblyFactory;
    }

    
    /**
     * Initialise step of the AssemblyFactory process.
     * (connect to all required services, create and register
     * the AssemblyFactory object, activate the POAManager).
     */
    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind)
    {
        try {
        
            // Connect to Repository
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "AssemblyFactory",
                "Connect to Repository");
            initRepositoryInterface();
    
            // Retrieve PackageManager
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "AssemblyFactory",
                "Retrieve PackageManager");
            com.thalesgroup.CdmwDeployment.PackageManager manager =
                getPackageManager();
    
            // Retrieve XMLParser
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "AssemblyFactory",
                "Retrieve XMLParser");
            com.thalesgroup.CdmwXML.XMLParser parser = getXMLParser();
    
            // initialize AssemblyFactory
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "AssemblyFactory",
                "initialize AssemblyFactory");
            AssemblyFactoryImpl assemblyFactoryImpl =
                AssemblyFactoryImpl.init(orb,
                                         assemblyFactoryPOA,
                                         assembliesPOA,
                                         configFactory,
                                         parser,
                                         manager);
    
            // Activate AssemblyFactory
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "AssemblyFactory",
                "Create servant");
            try {
                byte[] oid = assemblyFactoryPOA.activate_object(assemblyFactoryImpl);
                assemblyFactory = org.omg.Components.Deployment.AssemblyFactoryHelper.narrow(
                    assemblyFactoryPOA.id_to_reference(oid));
    
            } catch (org.omg.PortableServer.POAPackage.WrongPolicy ex) {
                // shouldn't happen
                ex.printStackTrace();
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive ex) {
                // shouldn't happen
                ex.printStackTrace();
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.ObjectNotActive ex) {
                // shouldn't happen
                ex.printStackTrace();
                cdmw.common.Assert.check(false);
            }
    
            // Register AssemblyFactoryImpl
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "AssemblyFactory",
                "Register AssemblyFactory");
            registerAssemblyFactory();
    
            // activate POA Manager
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "AssemblyFactory",
                "activate POA Manager");
            try {
                assemblyFactoryPOA.the_POAManager().activate();
            } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive ex) {
                // shouldn't happen
                ex.printStackTrace();
                cdmw.common.Assert.check(false);
            }

            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "AssemblyFactory",
                "Initialise done");

        } catch (cdmw.common.BadOrderException e) {
            e.printStackTrace();
        } catch (cdmw.common.BadParameterException e) {
            e.printStackTrace();
        }
    
    }

    /**
     * Run step of the AssemblyFactory process.
     */
    public void run()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun 
    {
        // nothing to do
    }


    /**
     * Stop step of the AssemblyFactory process.
     */
    public void stop() {
        unregisterAssemblyFactory();
        orb.shutdown(false);
    }

    
    /**
     * Initializes the RepositoryInterface.
     */
    private void initRepositoryInterface() {
        try {
            // Retrieve Repository object reference
            repository = cdmw.cdmwinit.InitUtils.getCdmwRepository();

            // Init of RepositoryInterface
            RepositoryInterface.init(
                Locations.CDMW_SERVICES_NAME_DOMAIN, repository);

            repository.resolve_root_context(
                com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

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
        } catch (com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        }
    }


    /**
     * Retrieves the PackageManager object from Repository.
     */
    private com.thalesgroup.CdmwDeployment.PackageManager getPackageManager() {
        try {
            String deploymentNameDomain = Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN;
            // Get NamingInterface object from RepositoryInterface
            cdmw.namingandrepository.NamingInterface ni =
                RepositoryInterface.getDomainNamingInterface(deploymentNameDomain);

            // Get a reference on the PackageManager
            org.omg.CORBA.Object obj = ni.resolve(PACKAGE_MANAGER_NAME);
            return com.thalesgroup.CdmwDeployment.PackageManagerHelper.narrow(obj);

        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        // unused (Assert stop process)
        return null;
    }


    /**
     * Retrieves the PackageManager object from PlatformInterface.
     */
    private com.thalesgroup.CdmwXML.XMLParser getXMLParser() {

        try {
            org.omg.CORBA.Object obj
                = cdmw.platformmngt.PlatformInterface.getService(XML_SERVICE);

            return com.thalesgroup.CdmwXML.XMLParserHelper.narrow(obj);
        } catch(com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound snf) {
            // shouldn't happen
            snf.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch(com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable su) {
            // shouldn't happen
            su.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch(cdmw.common.BadOrderException boe) {
            // shouldn't happen
            boe.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch(cdmw.common.BadParameterException bpe) {
            // shouldn't happen
            bpe.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        // unused (Assert stop process)
        return null;

    }


    /**
     * Registers the AssemblyFactory object into Repository.
     */
    private void registerAssemblyFactory() {

        try {
            // Get the name domain from the repository
            String deploymentNameDomain = Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN;
            deploymentDomain = repository.resolve_name_domain(deploymentNameDomain);

            // Register the PackageManager in the package manager domain
            deploymentDomain.register_new_object(ASSEMBLY_FACTORY_NAME, assemblyFactory);

        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        }

    }


    /**
     * Unregisters the AssemblyFactory object from Repository.
     */
    private void unregisterAssemblyFactory() {

        try {
            deploymentDomain.release_name(ASSEMBLY_FACTORY_NAME);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            in.printStackTrace();
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered nr) {
            nr.printStackTrace();
        }

    }





}


