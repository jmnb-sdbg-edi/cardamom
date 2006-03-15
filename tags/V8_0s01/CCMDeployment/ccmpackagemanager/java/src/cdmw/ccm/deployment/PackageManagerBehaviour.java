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


package cdmw.ccm.deployment;

import cdmw.common.Locations;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.cdmwinit.InitUtils;
import cdmw.platformmngt.ProcessBehaviour;
import cdmw.platformmngt.PlatformInterface;


class PackageManagerBehaviour extends ProcessBehaviour {

    private static final String XML_SERVICE = "XMLService";
    private static final String PACKAGE_MANAGER_NAME = "PackageManager";

    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA packageManagerPOA;
    private org.omg.PortableServer.POA packagesPOA;
    private java.io.File saveFile;
    private String installDir;
    private LocationFactory locationFactory;

    private com.thalesgroup.CdmwNamingAndRepository.Repository repository;
    private com.thalesgroup.CdmwDeployment.PackageManager manager;
    private com.thalesgroup.CdmwNamingAndRepository.NameDomain deploymentDomain;


    public PackageManagerBehaviour(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA packageManagerPOA,
        org.omg.PortableServer.POA packagesPOA,
        java.io.File saveFile,
        String installDir,
        LocationFactory locationFactory)
    {
        this.orb = orb;
        this.packageManagerPOA = packageManagerPOA;
        this.packagesPOA = packagesPOA;
        this.saveFile = saveFile;
        this.installDir = installDir;
        this.locationFactory = locationFactory;

        this.repository = null;
        this.manager = null;
    }


    public org.omg.CORBA.Object getService() {
        return manager;
    }

    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) 
    {
        try {
                
            // Connect to Repository
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "PackageManager",
                "Connect to Repository");
            initRepositoryInterface();
    
            // Retrieve XMLParser
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "PackageManager",
                "Retrieve XMLParser");
            com.thalesgroup.CdmwXML.XMLParser xmlParser = getXMLParser();
    
            // initialize PackageManager
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "PackageManager",
                "Create servant");
            PackageManagerImpl managerImpl = PackageManagerImpl.init(
                packageManagerPOA,
                packagesPOA,
                xmlParser,
                saveFile,
                installDir,
                locationFactory);
    
            // Activate PackageManager
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "PackageManager",
                "Activate servant");
            try {
                byte[] oid = packageManagerPOA.activate_object(managerImpl);
                manager = com.thalesgroup.CdmwDeployment.PackageManagerHelper.narrow(
                    packageManagerPOA.id_to_reference(oid));
    
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
    
            // Register PackageManager
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "PackageManager",
                "Register PackageManager");
            registerPackageManager();
    
            // activate POA Managers
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "PackageManager",
                "Activate POAManagers");
            try {
                packageManagerPOA.the_POAManager().activate();
                packagesPOA.the_POAManager().activate();
            } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive ex) {
                // shouldn't happen
                ex.printStackTrace();
                cdmw.common.Assert.check(false);
            }
    
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "PackageManager",
                "Initialise done");
    
        } catch (cdmw.common.BadOrderException e) {
            e.printStackTrace();
        } catch (cdmw.common.BadParameterException e) {
            e.printStackTrace();
        }        
    }


    public void run()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun 
    {
        // nothing to do
    }

    public void stop() {
        unregisterPackageManager();
        orb.shutdown(false);
    }


    private void initRepositoryInterface() {
        try {
            // Retrieve Repository object reference
            repository = cdmw.cdmwinit.InitUtils.getCdmwRepository();

            // Init of RepositoryInterface
            RepositoryInterface.init(
                cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN,
                repository);

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

    private void registerPackageManager() {

        try {
            // Get the name domain from the repository
            String deploymentNameDomain = Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN;
            deploymentDomain = repository.resolve_name_domain(deploymentNameDomain);

            // Register the PackageManager in the package manager domain
            deploymentDomain.register_new_object(PACKAGE_MANAGER_NAME, manager);

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

    private void unregisterPackageManager() {

        try {
            deploymentDomain.release_name(PACKAGE_MANAGER_NAME);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            in.printStackTrace();
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered nr) {
            nr.printStackTrace();
        }

    }

}

