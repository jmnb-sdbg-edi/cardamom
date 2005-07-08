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

import org.omg.Components.Deployment.ComponentServer;
import org.omg.Components.Deployment.ComponentServerHelper;

import org.omg.Components.CreateFailure;
import org.omg.Components.RemoveFailure;
import org.omg.Components.Deployment.InvalidConfiguration;

import com.thalesgroup.CdmwDeployment.ComponentInstallation;
import com.thalesgroup.CdmwDeployment.PROCESS_DESTINATION;
import com.thalesgroup.CdmwDeployment.COMPONENT_SERVER_ALREADY_CREATED;
import com.thalesgroup.CdmwDeployment.COMPONENT_SERVER_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.COMPONENT_INSTALLATION_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.INVALID_PROCESS_DESTINATION;
import com.thalesgroup.CdmwDeployment.NAMING_SERVICE_ERROR;
import com.thalesgroup.CdmwDeployment.UNKNOWN_COMPONENT_SERVER;

import cdmw.namingandrepository.RepositoryInterface;
import cdmw.common.Locations;


class ServerActivatorImpl
    extends org.omg.Components.Deployment.ServerActivatorPOA
{
    
    private static final String COMPONENT_SERVER_NAME = "ComponentServer";
    private static final String COMPONENT_INSTALLATION_NAME = "ComponentInstallation";
    
    
    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA activatorPOA;
    private cdmw.namingandrepository.NamingInterface naming;
    private java.util.Collection servers;
    
    
    public ServerActivatorImpl(org.omg.CORBA.ORB orb,
                               org.omg.PortableServer.POA activatorPOA,
                               org.omg.CosNaming.NamingContext naming)
    {
        this.orb = orb;
        this.activatorPOA = activatorPOA;
        this.naming = new cdmw.namingandrepository.NamingInterface(naming);
        servers = new java.util.HashSet();
    }
    
    
    public org.omg.PortableServer.POA _default_POA() {
        return activatorPOA;
    }
    
    
    //
    // IDL:omg.org/Components/Deployment/ServerActivator/create_component_server:1.0
    //
    /**
     * Creates a ComponentServer object on the host on which the 
     * ServerActivator is located.
     *
     * @param  config  ConfigValues for creation of ComponentServer.
     *                 For CDMW CCM, this sequence should contain a
     *                 PROCESS_DESTINATION ConfigValue which specify the
     *                 destination of an already started ComponentServer process.
     *
     * @throws  CreateFailure  
     *          INVALID_PROCESS_DESTINATION: raised if the destination
     *          specified in PROCESS_DESTINATION ConfigValue is not valid.<br>
     *          COMPONENT_SERVER_NOT_CREATED: raised if the ComponentServer could
     *          not be contacted (it should be started by PlatformManagement before
     *          the call to ServerActivator).<br>
     *          COMPONENT_SERVER_ALREADY_CREATED: raised if this method has already
     *          been called with the same PROCESS_DESTINATION ConfigValue.
     * @throws  InvalidConfiguration
     *          raised if the config parameter doesn't contain a single
     *          PROCESS_DESTINATION ConfigValue.
     */
    public synchronized ComponentServer
    create_component_server(org.omg.Components.ConfigValue[] config)
        throws CreateFailure,
               InvalidConfiguration
    {
        try {      
            // expected ConfigValues
            org.omg.CORBA.Any processDestAny = null;
            
            if (config != null) {
                // read config
                for (int i = 0; i < config.length; i++) {
                    if (config[i].name.equals(PROCESS_DESTINATION.value)) {
                        processDestAny = config[i].value;
                    
                    } else {
                        throw new InvalidConfiguration(
                            "Unknown ConfigValue: " + config[i].name,
                            org.omg.Components.Deployment.UnknownConfigValueName.value,
                            config[i].name);
                    }
                }
            }
            
            String processDest;
            if (processDestAny == null) {
                throw new InvalidConfiguration(
                    "ConfigValue " + PROCESS_DESTINATION.value + " is required",
                    org.omg.Components.Deployment.ConfigValueRequired.value,
                    PROCESS_DESTINATION.value);
            } else {
                try {
                    processDest = processDestAny.extract_string();
                } catch (org.omg.CORBA.BAD_OPERATION e) {
                    throw new InvalidConfiguration(
                        "ConfigValue " + PROCESS_DESTINATION.value + 
                        " must have a string value.",
                        org.omg.Components.Deployment.InvalidConfigValueType.value,
                        PROCESS_DESTINATION.value);
                }
            }
                    
            // retrieve ComponentServer
            org.omg.CORBA.Object obj;
            ComponentServer server;
            try {
                obj = naming.resolve(processDest + "." + COMPONENT_SERVER_NAME);
                if (obj == null) {
                    // found a null ComponentServer
                    throw new CreateFailure(
                        "ComponentServer corresponding to process destination " +
                        processDest + " is null !!",
                        COMPONENT_SERVER_NOT_FOUND.value);
                }
                
            } catch (org.omg.CosNaming.NamingContextPackage.NotFound ex) {
                // ComponentServer not found in naming service
                throw new CreateFailure(
                    "No ComponentServer corresponding to process destination " +
                    processDest + " has been found in NamingService.",
                    COMPONENT_SERVER_NOT_FOUND.value);
                
            } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed ex) {
                throw new CreateFailure(
                    "CannotProceed exception while resolving " +
                    processDest + "/" + COMPONENT_SERVER_NAME +
                    " in NamingService.",
                    NAMING_SERVICE_ERROR.value);
            
            } catch (org.omg.CosNaming.NamingContextPackage.InvalidName ex) {
                // processDest is not valid
                throw new CreateFailure(
                    "The process destination \"" + processDest + "\" is not valid.",
                    INVALID_PROCESS_DESTINATION.value);
            
            } catch (org.omg.CORBA.SystemException ex) {
                // error while contacting NamingService
                throw new CreateFailure(
                    "Error while contacting NamingService: " + ex.getMessage(),
                    NAMING_SERVICE_ERROR.value);
            }
            
            
            try {
                server = ComponentServerHelper.narrow(obj);
            
            } catch (org.omg.CORBA.SystemException e) {
                // ComponentServer could not be contacted
                throw new CreateFailure(
                    "ComponentServer in process destination " + processDest + 
                    " could not be contacted.",
                    COMPONENT_SERVER_NOT_FOUND.value);
            }
                     
            // initialize ComponentServer
            initializeComponentServer(server);
                        
            if (!servers.add(server)) {
                // server was already created
                throw new CreateFailure(
                    "Component Server already created.",
                    COMPONENT_SERVER_ALREADY_CREATED.value);
            }
            
            return server;
            
        } catch (CreateFailure e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (InvalidConfiguration e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;
        }
        
    }
    

    //
    // IDL:omg.org/Components/Deployment/ServerActivator/remove_component_server:1.0
    //
    public synchronized void
    remove_component_server(ComponentServer server)
        throws org.omg.Components.RemoveFailure
    {
        try {

            if (server == null) {
                throw new org.omg.CORBA.BAD_PARAM(
                    cdmw.orbsupport.ExceptionMinorCodes.BAD_PARAMNilObjectReference,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
            
            // find equivalent server in servers list
            ComponentServer equivalentServer = null;
            java.util.Iterator it = servers.iterator();
            while (it.hasNext()) {
                ComponentServer cs = (ComponentServer) it.next();
                if (cs._is_equivalent(server)) {
                    equivalentServer = cs;
                    break;
                }
            }
            
            // if no equivalent found, throw exception
            if (equivalentServer == null) {
                throw new RemoveFailure(
                    "This ComponentServer has not been created by this ServerActivator.",
                    UNKNOWN_COMPONENT_SERVER.value);
            }
            
            // remove ComponentServer
            equivalentServer.remove();
            
            // remove ComponentServer from list
            servers.remove(equivalentServer);

        } catch (org.omg.Components.RemoveFailure e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;
        }
    }
    

    //
    // IDL:omg.org/Components/Deployment/ServerActivator/get_component_servers:1.0
    //
    public ComponentServer[]
    get_component_servers()
    {
        ComponentServer[] result = new ComponentServer[servers.size()];
        
        java.util.Iterator it = servers.iterator();
        int i = 0;
        while (it.hasNext()) {
            result[i] = (ComponentServer) it.next();
            i++;
        }
        
        return result;
    }
    
    
    
    private void initializeComponentServer(ComponentServer server) 
        throws CreateFailure
    {
        try {
            // the ComponentServer should be a Cdmw ComponentServer
            com.thalesgroup.CdmwCcmComponentServer.
            ComponentServer cdmwCompServ =
                com.thalesgroup.CdmwCcmComponentServer.
                ComponentServerHelper.narrow(server);
                
            // give it the ComponentInstallation
            cdmwCompServ.set_component_installation(getComponentInstallation());

            // give it the ServerActivator
            cdmwCompServ.set_server_activator(_this());
            
        } catch (org.omg.CORBA.BAD_PARAM ex) {
            // ComponentServer is not a Cdmw ComponentServer,
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }                        
    }
    
    
    
    private ComponentInstallation getComponentInstallation() 
        throws CreateFailure
    {
        String host = cdmw.ossupport.OS.getHostname();
        
        // Get AdminRootContext NamingInterface object from Repository
        cdmw.namingandrepository.NamingInterface adminNaming = null;
        try {
            // Get admin root naming context
            org.omg.CosNaming.NamingContext adminRoot =
                RepositoryInterface.getRepository().resolve_root_context(
                    cdmw.common.Locations.ADMIN_ROOT_CONTEXT_ID);

            // create naming interface on admin root context
            adminNaming = new cdmw.namingandrepository.NamingInterface(adminRoot);

        } catch (com.thalesgroup.CdmwNamingAndRepository
                                .RepositoryPackage.NoRootContext ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        }
    
        // Get ComponentInstallation for host "destination" from Repository
        ComponentInstallation compInstallation;
        try {
            org.omg.CORBA.Object obj = 
                adminNaming.resolve(host + "/" + COMPONENT_INSTALLATION_NAME);
            compInstallation = 
                com.thalesgroup.CdmwDeployment.ComponentInstallationHelper.narrow(obj);
            
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound ex) {
            throw new CreateFailure(
                "There is no object registred with " + 
                Locations.ADMIN_ROOT_CONTEXT_ID + "/" + host + "/" + 
                COMPONENT_INSTALLATION_NAME + " in Repository.",
                COMPONENT_INSTALLATION_NOT_FOUND.value);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed ex) {
            throw new CreateFailure(
                "Error while retrieving ComponentInstallation on host " + 
                host + " :\n" + ex.getMessage(),
                COMPONENT_INSTALLATION_NOT_FOUND.value);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName ex) {
            throw new CreateFailure(
                "Error while retrieving ComponentInstallation on host " +
                host + ". " + host + "/" + COMPONENT_INSTALLATION_NAME +
                " is an invalid name.",
                COMPONENT_INSTALLATION_NOT_FOUND.value);
        } catch (org.omg.CORBA.BAD_PARAM ex) {
            // narrow failed
            throw new CreateFailure(
                "Error while retrieving ComponentInstallation on host " +
                host + ". The registred object is not a Cdmw ComponentInstallation.",
                COMPONENT_INSTALLATION_NOT_FOUND.value);            
        }
    
        return compInstallation;
    }

    
    
}





