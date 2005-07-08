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


package cdmw.ccm.serveractivator;

import cdmw.common.Locations;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.platformmngt.PlatformInterface;

public class ServerActivatorBehaviour
    extends cdmw.platformmngt.ProcessBehaviour {

    private org.omg.CORBA.ORB orb;

    private org.omg.PortableServer.POA serverActivatorPOA;

    private static final String SERVER_ACTIVATOR_NAME = "ServerActivator";

    private org.omg.Components.Deployment.ServerActivator activator;

    private com.thalesgroup.CdmwNamingAndRepository.Repository repository;


    public ServerActivatorBehaviour(
        org.omg.CORBA.ORB orb, org.omg.PortableServer.POA poa) {
        this.orb = orb;
        this.serverActivatorPOA = poa;
    }

    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) 
    {
        try {
            try {
                // init Repository
                PlatformInterface.notifyMessage(
                    com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                    "ServerActivator",
                    "Connect to Repository");
                initRepositoryInterface();
            
                // create ServerActivatorImpl
                PlatformInterface.notifyMessage(
                    com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                    "ServerActivator",
                    "Create servant");
                ServerActivatorImpl activatorImpl = new ServerActivatorImpl(
                    orb, serverActivatorPOA, getNamingContext());
            
                // Activate ServerActivator
                PlatformInterface.notifyMessage(
                    com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                    "ServerActivator",
                    "Activate ServerActivator");
                byte[] oid = serverActivatorPOA.activate_object(activatorImpl);
                activator =
                    org.omg.Components.Deployment.ServerActivatorHelper.narrow(
                        serverActivatorPOA.id_to_reference(oid));
            
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
            
            // Register ServerActivator
                PlatformInterface.notifyMessage(
                    com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                    "ServerActivator",
                    "Register ServerActivator");
            registerServerActivator();
            
            // activate POA Manager
                PlatformInterface.notifyMessage(
                    com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                    "ServerActivator",
                    "Activate POA Manager");
            try {
                // activate POA Manager
                serverActivatorPOA.the_POAManager().activate();
            } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive ex) {
                // shouldn't happen
                ex.printStackTrace();
                cdmw.common.Assert.check(false);
            }

            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                "ServerActivator",
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
        unregisterServerActivator();
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

    
    
    public org.omg.CosNaming.NamingContext getNamingContext() {

        try {
            // return the NamingContext corresponding to the 
            // CDMW/SERVICES/ASSEMBLYANDDEPLOYMENT NameDomain
            String assemblyNameDomain = Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN;
            
            // Get NamingInterface object from RepositoryInterface
            return RepositoryInterface.getDomainContext(assemblyNameDomain);
        } catch(com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            // shouldn't happen
            nnd.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch(com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            // shouldn't happen
            in.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        // unused (Assert stop process)
        return null;

    }


    private void registerServerActivator() {

        try {
            // set Component Installation location under admin root context
            // Pattern is :
            //   "<host_name>/ServerActivator"

            // Get Component Installation root naming context
            org.omg.CosNaming.NamingContext adminRoot =
                repository.resolve_root_context(Locations.ADMIN_ROOT_CONTEXT_ID);

            // create naming interface on admin root context
            cdmw.namingandrepository.NamingInterface ni
                = new cdmw.namingandrepository.NamingInterface(adminRoot);

            // build the name of the Component Installation object
            String activatorName = cdmw.ossupport.OS.getHostname()
                + "/" + SERVER_ACTIVATOR_NAME;

            // Register Component Installation object
            ni.bind(activatorName, activator, true);

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


    private void unregisterServerActivator() {

        try {
            org.omg.CosNaming.NamingContext adminRoot
                = repository.resolve_root_context(Locations.ADMIN_ROOT_CONTEXT_ID);

            // create naming interface on admin root context
            cdmw.namingandrepository.NamingInterface ni
                = new cdmw.namingandrepository.NamingInterface(adminRoot);

            // build the name of the Component Installation object
            String compInstallName = cdmw.ossupport.OS.getHostname()
                + "/" + SERVER_ACTIVATOR_NAME;

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

