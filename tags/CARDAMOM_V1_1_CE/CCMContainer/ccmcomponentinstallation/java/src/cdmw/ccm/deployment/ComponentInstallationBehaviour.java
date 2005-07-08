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


package cdmw.ccm.deployment;

import cdmw.common.Locations;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.cdmwinit.InitUtils;
import cdmw.platformmngt.ProcessBehaviour;
import cdmw.platformmngt.PlatformInterface;

class ComponentInstallationBehaviour extends ProcessBehaviour {

    private static final String XML_SERVICE = "XMLService";
    private static final String PACKAGE_MANAGER_NAME = "PackageManager";
    private static final String COMPONENT_INSTALLATION_NAME
        = "ComponentInstallation";

    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA compInstallPOA;
    private java.io.File installDir;
    private java.io.File saveFile;

    private com.thalesgroup.CdmwNamingAndRepository.Repository repository;
    private org.omg.Components.Deployment.ComponentInstallation compInstall;


    public ComponentInstallationBehaviour(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA compInstallPOA,
        java.io.File installDir,
        java.io.File saveFile)
    {
        this.orb = orb;
        this.compInstallPOA = compInstallPOA;
        this.installDir = installDir;
        this.saveFile = saveFile;

        this.repository = null;
        this.compInstall = null;
    }

    public org.omg.CORBA.Object getService() {
        return compInstall;
    }

    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind)
    {
        try {
        
            // Connect to Repository
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ComponentInstallation",
                "Connect to Repository");
            initRepositoryInterface();
    
            // Retrieve PackageManager
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ComponentInstallation",
                "Retrieve PackageManager");
            com.thalesgroup.CdmwDeployment.PackageManager manager =
                getPackageManager();
    
            // Retrieve XMLParser
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ComponentInstallation",
                "Retrieve XMLParser");
            com.thalesgroup.CdmwXML.XMLParser parser = getXMLParser();
    
            // initialize ComponentInstallation
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ComponentInstallation",
                "initialize ComponentInstallation");
            ComponentInstallationImpl compInstallImpl =
                new ComponentInstallationImpl(compInstallPOA,
                                              manager,
                                              parser,
                                              installDir,
                                              saveFile);
    
            // Activate ComponentInstallation
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ComponentInstallation",
                "Create servant");
            try {
                byte[] oid = compInstallPOA.activate_object(compInstallImpl);
                compInstall = org.omg.Components.Deployment.ComponentInstallationHelper.narrow(
                    compInstallPOA.id_to_reference(oid));
    
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
    
            // Register ComponentInstallation
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ComponentInstallation",
                "Register ComponentInstallation");
            registerComponentInstallation();
    
            // activate POA Manager
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ComponentInstallation",
                "activate POA Manager");
            try {
                compInstallPOA.the_POAManager().activate();
            } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive ex) {
                // shouldn't happen
                ex.printStackTrace();
                cdmw.common.Assert.check(false);
            }

            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ComponentInstallation",
                "Initialise done");

        } catch (cdmw.common.BadOrderException e) {
            e.printStackTrace();
        } catch (cdmw.common.BadParameterException e) {
            e.printStackTrace();
        }
    
    }


    public void run()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun 
    {
        // nothing to do
    }


    public void stop() {
        unregisterComponentInstallation();
        orb.shutdown(false);
    }



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


    private void registerComponentInstallation() {

        try {
            // set Component Installation location under admin root context
            // Pattern is :
            //   "<host_name>/ComponentInstallation"

            // Get Component Installation root naming context
            org.omg.CosNaming.NamingContext adminRoot =
                repository.resolve_root_context(Locations.ADMIN_ROOT_CONTEXT_ID);

            // create naming interface on admin root context
            cdmw.namingandrepository.NamingInterface ni
                = new cdmw.namingandrepository.NamingInterface(adminRoot);

            // build the name of the Component Installation object
            String compInstallName = cdmw.ossupport.OS.getHostname()
                + "/" + COMPONENT_INSTALLATION_NAME;

            // Register Component Installation object
            ni.bind(compInstallName, compInstall, true);

        } catch(com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext nrc) {
            // shouldn't happen
            nrc.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch(org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            // shouldn't happen
            nf.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch(org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            // shouldn't happen
            cp.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            // shouldn't happen
            in.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch(org.omg.CosNaming.NamingContextPackage.AlreadyBound ab) {
            // shouldn't happen
            ab.printStackTrace();
            cdmw.common.Assert.check(false);
        }

    }


    private void unregisterComponentInstallation() {

        try {
            org.omg.CosNaming.NamingContext adminRoot
                = repository.resolve_root_context(Locations.ADMIN_ROOT_CONTEXT_ID);

            // create naming interface on admin root context
            cdmw.namingandrepository.NamingInterface ni
                = new cdmw.namingandrepository.NamingInterface(adminRoot);

            // build the name of the Component Installation object
            String compInstallName = cdmw.ossupport.OS.getHostname()
                + "/" + COMPONENT_INSTALLATION_NAME;

            // remove the trace producer binding
            ni.unbind(compInstallName);
        } catch(com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext nrc) {
            nrc.printStackTrace();
        } catch(org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            nf.printStackTrace();
        } catch(org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            cp.printStackTrace();
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            in.printStackTrace();
        }

    }


}

