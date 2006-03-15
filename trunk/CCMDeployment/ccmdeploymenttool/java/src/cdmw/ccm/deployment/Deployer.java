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

import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwDeployment.PackageManager;
import com.thalesgroup.CdmwDeployment.AssemblyFactory;
import org.omg.Components.Deployment.Assembly;
import org.omg.Components.Deployment.ComponentInstallation;

import com.thalesgroup.CdmwDeployment._AssemblyPackage;
import com.thalesgroup.CdmwDeployment._ComponentPackage;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;




class Deployer {

    private static final String XPATH_HOST_DESTINATIONS =
        "/componentassembly/partitioning/hostcollocation/destination/text()";
    private static final String XPATH_HOST_COLLOCATION =
        "/componentassembly/partitioning/hostcollocation";
    private static final String XPATH_ALL_HOMEPLACEMENTS =
        "//homeplacement";
    private static final String XPATH_COMPONENT_FILE_REF =
        "componentfileref/@idref";
    private static final String XPATH_COMPONENT_IMPL_REF =
        "componentimplref/@idref";
    private static final String XPATH_IMPLEMENTATION_UUID =
        "/softpkg/implementation/@id";

    private static final String COMPONENT_INSTALLATION_NAME = "ComponentInstallation";

    private Repository repository;
    private XMLParser parser;
    private PackageManager packManager;
    private AssemblyFactory assemblyFactory;
    


    Deployer(Repository repository,
             XMLParser parser,
             PackageManager packManager,
             AssemblyFactory assemblyFactory)
    {
        this.repository = repository;
        this.parser = parser;
        this.packManager = packManager;
        this.assemblyFactory = assemblyFactory;
    }
    
    
    public void installComponent(String location) 
        throws Exception
    {
        try {
            packManager.install_component_package(location);

        } catch (org.omg.Components.Deployment.InvalidLocation e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.CORBA.SystemException e) {
            throw new Exception(exceptionToString(e));
        }
    }
    
    
    public void removeComponent(String name, String version)
        throws Exception
    {
        try {
            if (version == null) {
                packManager.remove_component_package(name, "");
            } else {
                packManager.remove_component_package(name, version);        
            }
            
        } catch (com.thalesgroup.CdmwDeployment.PackageNotFound e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.RemoveFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.CORBA.SystemException e) {
            throw new Exception(exceptionToString(e));
        }
    }
    
    
    public void installAssembly(String location)
        throws Exception
    {
        try {
            packManager.install_assembly_package(location);
            
        } catch (org.omg.Components.Deployment.InvalidLocation e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.CORBA.SystemException e) {
            throw new Exception(exceptionToString(e));
        }
    }
    
    
    public void removeAssembly(String assemblyUUID)
        throws Exception
    {
        try {
            packManager.remove_assembly_package(assemblyUUID);
            
        } catch (com.thalesgroup.CdmwDeployment.PackageNotFound e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.RemoveFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.CORBA.SystemException e) {
            throw new Exception(exceptionToString(e));
        }
    }
    
    
    public void createAssembly(String assemblyUUID, String assemblyName)
        throws Exception
    {
        try {
            // get AssemblyPackage from PackageManager
            _AssemblyPackage assemblyPack =
                packManager.get_assembly_package(assemblyUUID);
            
            // get CAD location
            String cadLocation = assemblyPack.get_CAD_location();
            
            // create Assembly
            assemblyFactory.create_with_name(cadLocation, assemblyName);
            
            // install implementations used by assembly on appropriate hosts
            installImplementations(assemblyPack, cadLocation);
            
        } catch (com.thalesgroup.CdmwDeployment.PackageNotFound e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.Deployment.InvalidLocation e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.CreateFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            throw new Exception(exceptionToString(e));
        } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.CORBA.SystemException e) {
            throw new Exception(exceptionToString(e));
        }
    }
    
    
    public void destroyAssembly(String assemblyName)
        throws Exception
    {
        try {
            assemblyFactory.destroy_with_name(assemblyName);
            
        } catch (org.omg.Components.Deployment.InvalidAssembly e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.RemoveFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.CORBA.SystemException e) {
            throw new Exception(exceptionToString(e));
        }
    }
    
    
    public void buildAssembly(String assemblyName)
        throws Exception
    {
        try {
            Assembly assembly = assemblyFactory.lookup_with_name(assemblyName);
            
            assembly.build();
            
        } catch (org.omg.Components.Deployment.InvalidAssembly e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.CreateFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.CORBA.SystemException e) {
            throw new Exception(exceptionToString(e));
        }
    }
    
    public void tearAssemblyDown(String assemblyName)
        throws Exception
    {
        try {
            Assembly assembly = assemblyFactory.lookup_with_name(assemblyName);
            
            assembly.tear_down();
            
        } catch (org.omg.Components.Deployment.InvalidAssembly e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.Components.RemoveFailure e) {
            throw new Exception(exceptionToString(e));
        } catch (org.omg.CORBA.SystemException e) {
            throw new Exception(exceptionToString(e));
        }
    }
    
    
    private void installImplementations(_AssemblyPackage assemblyPack,
                                        String cadLocation) 
        throws Exception
    {
        // parse CAD file
        cdmw.ccm.deployment.CadReader cadReader =
            new cdmw.ccm.deployment.CadReader(parser, cadLocation);

        try {
            // get all destinations of hostcollocations
            NodeDescription[] destNodes =
                cadReader.selectNodes(XPATH_HOST_DESTINATIONS);

            for (int i=0; i < destNodes.length; i++) {
                // get destination of hostcollocation
                String destination = XMLHelper.toString(
                    ((TextDescription) destNodes[i]).data());

                // get all homeplacements of
                // the hostcollocation with this destination
                NodeDescription[] homeNodes =
                    cadReader.selectNodes(XPATH_HOST_COLLOCATION +
                                          "[destination=\"" + destination + "\"]" +
                                          XPATH_ALL_HOMEPLACEMENTS);

                for (int j=0; j < homeNodes.length; j++) {
                    // get <homeplacement> element
                    Element homePlacElt =
                        ElementHelper.narrow(homeNodes[j].reference());

                    // get the idref attribute of its
                    // <componentfileref> child element
                    NodeDescription[] fileRefNodes = homePlacElt.select_nodes(
                        XMLHelper.toShortArray(XPATH_COMPONENT_FILE_REF));

                    // <componentfileref> is required
                    cdmw.common.Assert.check(fileRefNodes.length == 1);
                    String fileRef = XMLHelper.toString(
                        ((AttrDescription) fileRefNodes[0]).value());

                    // get the idref attribute of its
                    // <componentimplref> child element
                    NodeDescription[] implRefNodes = homePlacElt.select_nodes(
                        XMLHelper.toShortArray(XPATH_COMPONENT_IMPL_REF));

                    // <componentimplref> is implied
                    String implRef = "";
                    if (implRefNodes.length == 1) {
                        implRef = XMLHelper.toString(
                            ((AttrDescription) implRefNodes[0]).value());
                    }

                    // install the corresponding implementation on destination
                    installImplementation(assemblyPack,
                                          fileRef,
                                          implRef,
                                          destination);
                }
            }

        } catch (com.thalesgroup.CdmwXML.XPathFormatException e) {
            cadReader.close();
            // shouldn't happen, xpath are valid
            cdmw.common.Assert.check(false);
        }

        cadReader.close();
    }
    

    private void installImplementation(_AssemblyPackage assemblyPack,
                                       String componentFileId,
                                       String componentImplId,
                                       String host)
        throws Exception
    {
        String csdLocation;
        try {
            // retrieve ComponentPackage corresponding to idref
            _ComponentPackage compPack =
                assemblyPack.get_component_package(componentFileId);

            // get location of its CSD
            csdLocation = compPack.get_CSD_location();

        } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
            throw new Exception(
                exceptionToString(e) + "\n" +
                assemblyPack.get_CAD_location() + " is not conform with the " +
                "corresponding assembly installed by the PackageManager:\n" +
                "This assembly should have a " + componentFileId +
                " componentfile.\n");
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            throw new Exception(
                exceptionToString(e) + "\n" +
                "Error while getting location of CSD of component " +
                "referenced by " + componentFileId + " in " + 
                assemblyPack.get_CAD_location());
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            throw new Exception    (
                exceptionToString(e) + "\n" +
                "Error while getting location of CSD of component " +
                "referenced by " + componentFileId + " in " + 
                assemblyPack.get_CAD_location());
        }

        if (componentImplId.equals("")) {
            // retrieve the unique implementation UUID declared in CSD
            cdmw.ccm.deployment.CsdReader csdReader =
                new cdmw.ccm.deployment.CsdReader(parser, csdLocation);

            try {
                // get id attribute of <implementation> elements
                NodeDescription[] uuidNodes =
                    csdReader.selectNodes(XPATH_IMPLEMENTATION_UUID);

                // check that there is only 1 implementation
                if (uuidNodes.length != 1) {
                    throw new Exception(
                        "Error in " + assemblyPack.get_CAD_location() + 
                        ": it doesn't specifies <componentimplref> for " +
                        "homeplacement with <componentfileref idref=\"" + 
                        componentFileId + "\">. It is required because " +
                        "the corresponding component package " +
                        "contains several implementations.");
                }

                componentImplId = XMLHelper.toString(
                    ((AttrDescription) uuidNodes[0]).value());

            } catch (com.thalesgroup.CdmwXML.XPathFormatException e) {
                // shouldn't happen, xpath is valid
                cdmw.common.Assert.check(false);
            }
        }

        try {
            // retrieve ComponentInstallation on specified host
            ComponentInstallation compInstall =
                getComponentInstallation(host, repository);

            // install implementation on host
            compInstall.install(componentImplId, csdLocation);
            
        } catch (org.omg.Components.Deployment.InvalidLocation e) {
            throw new Exception(
                exceptionToString(e) + "\n" +
                "Error while installing implementation " + componentImplId +
                " from " + csdLocation + " on host " + host + ".");
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            if (e.reason ==
                com.thalesgroup.CdmwDeployment.FILE_ALREADY_INSTALLED.value)
            {
                // implementation already installed; ignore.
            } else {
                throw new Exception(exceptionToString(e) + "\n" +
                                    e.getMessage());
            }
        }
    }


    private ComponentInstallation getComponentInstallation(String host,
                                                           Repository repository)
        throws org.omg.Components.Deployment.InstallationFailure
    {
        // Get AdminRootContext NamingInterface object from Repository
        cdmw.commonsvcs.naming.NamingInterface adminNaming = null;
        try {
            // Get admin root naming context
            org.omg.CosNaming.NamingContext adminRoot =
                repository.resolve_root_context(
                    cdmw.common.Locations.ADMIN_ROOT_CONTEXT_ID);

            // create naming interface on admin root context
            adminNaming = new cdmw.commonsvcs.naming.NamingInterface(adminRoot);

        } catch (com.thalesgroup.CdmwNamingAndRepository
                                .RepositoryPackage.NoRootContext ex) {
            // shouldn't happen
            ex.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        // Get ComponentInstallation for host "host" from RepositoryInterface
        ComponentInstallation compInstallation;
        try {
            org.omg.CORBA.Object obj =
                adminNaming.resolve(host + "/" + COMPONENT_INSTALLATION_NAME);
            compInstallation =
                org.omg.Components.Deployment.ComponentInstallationHelper.narrow(obj);

        } catch (org.omg.CosNaming.NamingContextPackage.NotFound ex) {
            throw new org.omg.Components.Deployment.InstallationFailure(
                "There is no object registred with " +
                cdmw.common.Locations.ADMIN_ROOT_CONTEXT_ID +
                "/" + host + "/" + COMPONENT_INSTALLATION_NAME +
                " in Repository.",
                com.thalesgroup.CdmwDeployment.COMPONENT_INSTALLATION_NOT_FOUND.value);
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed ex) {
            throw new org.omg.Components.Deployment.InstallationFailure(
                "Error while retrieving ComponentInstallation on host " +
                host + " :\n" + ex.getMessage(),
                com.thalesgroup.CdmwDeployment.COMPONENT_INSTALLATION_NOT_FOUND.value);
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName ex) {
            throw new org.omg.Components.Deployment.InstallationFailure(
                "Error while retrieving ComponentInstallation on host " +
                host + ". " + host + "/" + COMPONENT_INSTALLATION_NAME +
                " is an invalid name.",
                com.thalesgroup.CdmwDeployment.COMPONENT_INSTALLATION_NOT_FOUND.value);
        } catch (org.omg.CORBA.BAD_PARAM ex) {
            // narrow failed
            throw new org.omg.Components.Deployment.InstallationFailure(
                "Error while retrieving ComponentInstallation on host " +
                host + ". The registred object is not a ComponentInstallation.",
                com.thalesgroup.CdmwDeployment.COMPONENT_INSTALLATION_NOT_FOUND.value);
        }

        return compInstallation;
    }



    private String exceptionToString(
        com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException ex)
    {
        return "ParsingException \"" + ex.reason + "\".";
    }

    private String exceptionToString(
        com.thalesgroup.CdmwDeployment.PackageNotFound ex)
    {
        return "PackageNotFound Exception.";
    }

    private String exceptionToString(
        com.thalesgroup.CdmwDeployment.FileNotReferenced ex)
    {
        return "FileNotReferenced Exception.";
    }

    private String exceptionToString(
        com.thalesgroup.CdmwDeployment.LocationCreationException ex)
    {
        return "LocationCreationException Exception.";
    }

    private String exceptionToString(
        org.omg.Components.Deployment.InvalidLocation ex)
    {
        return "InvalidLocation Exception.";
    }

    private String exceptionToString(
        org.omg.Components.Deployment.InvalidAssembly ex)
    {
        return "InvalidAssembly Exception.";
    }

    private String exceptionToString(
        org.omg.Components.Deployment.InstallationFailure ex)
    {
        return "InstallationFailure: " + failureReasonToString(ex.reason) + ".";
    }

    private String exceptionToString(
        org.omg.Components.CreateFailure ex)
    {
        return "CreateFailure: " + failureReasonToString(ex.reason) + ".";
    }

    private String exceptionToString(
        org.omg.Components.RemoveFailure ex)
    {
        return "RemoveFailure: " + failureReasonToString(ex.reason) + ".";
    }

    private String exceptionToString(
        org.omg.CORBA.SystemException ex)
    {
        return "SystemException: " + ex.toString() + ".";
    }

    private static String failureReasonToString(int failureReason) {
        switch (failureReason) {
            /**
             * FailureReasons for all Failure exceptions
             */
            case com.thalesgroup.CdmwDeployment.NOT_IMPLEMENTED.value:   
                return "NOT_IMPLEMENTED";
            case com.thalesgroup.CdmwDeployment.ERROR_IN_CSD.value:       
                return "ERROR_IN_CSD";
            case com.thalesgroup.CdmwDeployment.ERROR_IN_CCD.value:       
                return "ERROR_IN_CCD";
            case com.thalesgroup.CdmwDeployment.ERROR_IN_CAD.value:       
                return "ERROR_IN_CAD";
            case com.thalesgroup.CdmwDeployment.ERROR_IN_CPF.value:       
                return "ERROR_IN_CPF";
            case com.thalesgroup.CdmwDeployment.WRONG_CONTEXT_KIND.value: 
                return "WRONG_CONTEXT_KIND";
            case com.thalesgroup.CdmwDeployment.ILLEGAL_STATE.value:      
                return "ILLEGAL_STATE";
                            
            /**
             * FailureReasons for InstallationFailure exceptions
             */
            case com.thalesgroup.CdmwDeployment.BAD_LOCATION.value:                  
                return "BAD_LOCATION";
            case com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR.value:              
                return "READ_WRITE_ERROR";
            case com.thalesgroup.CdmwDeployment.ERROR_IN_ARCHIVE.value:              
                return "ERROR_IN_ARCHIVE";
            case com.thalesgroup.CdmwDeployment.FILE_NOT_INSTALLED.value:            
                return "FILE_NOT_INSTALLED";
            case com.thalesgroup.CdmwDeployment.FILE_ALREADY_INSTALLED.value:                        
                return "FILE_ALREADY_INSTALLED";
            case com.thalesgroup.CdmwDeployment.ASSEMBLY_ALREADY_INSTALLED.value:     
                return "ASSEMBLY_ALREADY_INSTALLED";
            case com.thalesgroup.CdmwDeployment.COMPONENT_ALREADY_INSTALLED.value:   
                return "COMPONENT_ALREADY_INSTALLED";
            case com.thalesgroup.CdmwDeployment.UNSUPPORTED_COMPONENT_TYPE.value:       
                return "UNSUPPORTED_COMPONENT_TYPE";
            case com.thalesgroup.CdmwDeployment.UUID_NOT_FOUND.value:                
                return "UUID_NOT_FOUND";
            case com.thalesgroup.CdmwDeployment.PACKAGE_MANAGER_ERROR.value:         
                return "PACKAGE_MANAGER_ERROR";
            case com.thalesgroup.CdmwDeployment.CANNOT_REPLACE_IMPLEMENTATION.value: 
                return "CANNOT_REPLACE_IMPLEMENTATION";
            
            /**
             * FailureReasons for RemoveFailure exceptions
             */
            case com.thalesgroup.CdmwDeployment.COMPONENT_IN_USE.value:        
                return "COMPONENT_IN_USE";
            case com.thalesgroup.CdmwDeployment.DELETE_ERROR.value:            
                return "DELETE_ERROR";
            case com.thalesgroup.CdmwDeployment.BAD_COMPONENT_REFERENCE.value: 
                return "BAD_COMPONENT_REFERENCE";
            case com.thalesgroup.CdmwDeployment.ASSEMBLY_ALREADY_INACTIVE.value:
                return "ASSEMBLY_ALREADY_INACTIVE";
            
            /**
             * FailureReasons for CreateFailure exceptions
             */
            case com.thalesgroup.CdmwDeployment.CREATE_FAILURE.value:                           
                return "CREATE_FAILURE";
            case com.thalesgroup.CdmwDeployment.NO_COMPONENT_SERVANT_PROVIDER.value:              
                return "NO_COMPONENT_SERVANT_PROVIDER";
            case com.thalesgroup.CdmwDeployment.UNKNOWN_HOME.value:                               
                return "UNKNOWN_HOME";
            case com.thalesgroup.CdmwDeployment.CONTAINER_ERROR.value:                            
                return "CONTAINER_ERROR";
            case com.thalesgroup.CdmwDeployment.CANNOT_OPEN_SHARED_LIB.value:                     
                return "CANNOT_OPEN_SHARED_LIB";
            case com.thalesgroup.CdmwDeployment.ASSEMBLY_PACKAGE_NOT_INSTALLED.value:             
                return "ASSEMBLY_PACKAGE_NOT_INSTALLED";
            case com.thalesgroup.CdmwDeployment.COMPONENT_PACKAGE_NOT_INSTALLED.value:            
                return "COMPONENT_PACKAGE_NOT_INSTALLED";
            case com.thalesgroup.CdmwDeployment.INVALID_CONFIGURATION_FOR_COMPONENT_SERVER.value: 
                return "INVALID_CONFIGURATION_FOR_COMPONENT_SERVER";
            case com.thalesgroup.CdmwDeployment.INVALID_CONFIGURATION_FOR_CONTAINER.value:        
                return "INVALID_CONFIGURATION_FOR_CONTAINER";
            case com.thalesgroup.CdmwDeployment.INVALID_CONFIGURATION_FOR_HOME.value:             
                return "INVALID_CONFIGURATION_FOR_HOME";
            case com.thalesgroup.CdmwDeployment.UNKOWN_IMPL_ID.value:                             
                return "UNKOWN_IMPL_ID";
            case com.thalesgroup.CdmwDeployment.IMPL_ENTRY_POINT_NOT_FOUND.value:                 
                return "IMPL_ENTRY_POINT_NOT_FOUND";
            case com.thalesgroup.CdmwDeployment.INSTALLATION_FAILURE.value:                       
                return "INSTALLATION_FAILURE";
            case com.thalesgroup.CdmwDeployment.WRONG_HOME_KIND.value:                            
                return "WRONG_HOME_KIND";
            case com.thalesgroup.CdmwDeployment.PORT_NOT_FOUND.value:                             
                return "PORT_NOT_FOUND";
            case com.thalesgroup.CdmwDeployment.CONNECTION_FAILED.value:                          
                return "CONNECTION_FAILED";
            case com.thalesgroup.CdmwDeployment.DISCONNECTION_FAILED.value:                       
                return "DISCONNECTION_FAILED";
            case com.thalesgroup.CdmwDeployment.PROPERTIES_CONFIGURATION_ERROR.value:             
                return "PROPERTIES_CONFIGURATION_ERROR";
            case com.thalesgroup.CdmwDeployment.NO_DESTINATION.value:                             
                return "NO_DESTINATION";
            case com.thalesgroup.CdmwDeployment.DESTINATION_NOT_CONFORM.value:                    
                return "DESTINATION_NOT_CONFORM";
            case com.thalesgroup.CdmwDeployment.SEVERAL_IMPLEMENTATIONS_IN_PACKAGE.value:         
                return "SEVERAL_IMPLEMENTATIONS_IN_PACKAGE";
            case com.thalesgroup.CdmwDeployment.INTERFACE_REPOSITORY_ERROR.value:                 
                return "INTERFACE_REPOSITORY_ERROR";
            case com.thalesgroup.CdmwDeployment.NAMING_SERVICE_ERROR.value:                       
                return "NAMING_SERVICE_ERROR";
            case com.thalesgroup.CdmwDeployment.HOME_FINDER_ERROR.value:                          
                return "HOME_FINDER_ERROR";
            case com.thalesgroup.CdmwDeployment.NOT_A_HOME.value:                                 
                return "NOT_A_HOME";
            case com.thalesgroup.CdmwDeployment.NOT_A_COMPONENT.value:                            
                return "NOT_A_COMPONENT";
            case com.thalesgroup.CdmwDeployment.COMPONENT_SERVER_NOT_FOUND.value:                 
                return "COMPONENT_SERVER_NOT_FOUND";
            case com.thalesgroup.CdmwDeployment.COMPONENT_SERVER_ALREADY_CREATED.value:           
                return "COMPONENT_SERVER_ALREADY_CREATED";
            case com.thalesgroup.CdmwDeployment.INVALID_PROCESS_DESTINATION.value:                
                return "INVALID_PROCESS_DESTINATION";
            case com.thalesgroup.CdmwDeployment.UNKNOWN_COMPONENT_SERVER.value:                   
                return "UNKNOWN_COMPONENT_SERVER";
            case com.thalesgroup.CdmwDeployment.SERVER_ACTIVATOR_NOT_FOUND.value:                 
                return "SERVER_ACTIVATOR_NOT_FOUND";
            case com.thalesgroup.CdmwDeployment.COMPONENT_INSTALLATION_NOT_FOUND.value:           
                return "COMPONENT_INSTALLATION_NOT_FOUND";
            case com.thalesgroup.CdmwDeployment.WRONG_COMPONENT_KIND.value:                       
                return "WRONG_COMPONENT_KIND";
            case com.thalesgroup.CdmwDeployment.INVALID_COMPONENT_EXECUTOR.value:                 
                return "INVALID_COMPONENT_EXECUTOR";
            case com.thalesgroup.CdmwDeployment.NO_EVENT_CHANNEL_FACTORY.value:                   
                return "NO_EVENT_CHANNEL_FACTORY";
            case com.thalesgroup.CdmwDeployment.EVENT_CHANNEL_ALREADY_EXISTS.value:               
                return "EVENT_CHANNEL_ALREADY_EXISTS";
            case com.thalesgroup.CdmwDeployment.ASSEMBLY_NAME_ALREADY_USED.value:                 
                return "ASSEMBLY_NAME_ALREADY_USED";
            case com.thalesgroup.CdmwDeployment.ASSEMBLY_ALREADY_IN_SERVICE.value:
                return "ASSEMBLY_ALREADY_IN_SERVICE";
             default:
                return "Unkown reason : " + Integer.toHexString(failureReason);
        }
    }
    
    
}


