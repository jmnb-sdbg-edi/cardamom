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


import org.omg.Components.Deployment.InvalidLocation;
import org.omg.Components.Deployment.InstallationFailure;
import org.omg.Components.Deployment.UnknownImplId;
import org.omg.Components.RemoveFailure;

import com.thalesgroup.CdmwDeployment.PackageManager;
import com.thalesgroup.CdmwDeployment._ComponentPackage;
import com.thalesgroup.CdmwDeployment.LocationCreationException;
import com.thalesgroup.CdmwDeployment.UnknownValuetype;
import com.thalesgroup.CdmwDeployment.FILE_ALREADY_INSTALLED;
import com.thalesgroup.CdmwDeployment.COMPONENT_ALREADY_INSTALLED;
import com.thalesgroup.CdmwDeployment.UUID_NOT_FOUND;
import com.thalesgroup.CdmwDeployment.PACKAGE_MANAGER_ERROR;
import com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR;
import com.thalesgroup.CdmwDeployment.CANNOT_REPLACE_IMPLEMENTATION;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CSD;


import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;




import cdmw.ccm.deployment.FileUtilities;


class ComponentInstallationImpl 
    extends com.thalesgroup.CdmwDeployment.ComponentInstallationPOA
{

    private static final String UUID_DCE_PREFIX = "DCE:";
    
    private static final String XPATH_IMPLEMENTATION =
        "/softpkg/implementation";
    private static final String XPATH_VALUETYPE_FACTORY =
        "/dependency/valuetypefactory";
    private static final String XPATH_DEPENDENCIES =
        "/dependency/*[self::codebase or self::fileinarchive]";
    private static final String XPATH_FILE_REFERENCE =
        "*[self::codebase or self::fileinarchive or self::link]";

    private org.omg.PortableServer.POA poa;
    private PackageManager packageManager;
    private XMLParser parser;
    private java.io.File installDir;
    private java.io.File saveFile;
    private java.util.Map implementations;
    
    
    public ComponentInstallationImpl(org.omg.PortableServer.POA poa,
                                     PackageManager packageManager,
                                     XMLParser parser,
                                     java.io.File installDir,
                                     java.io.File saveFile) 
    {
        this.poa = poa;
        this.packageManager = packageManager;
        this.parser = parser;
        this.installDir = installDir;
        this.saveFile = saveFile;
        this.implementations = new java.util.Hashtable();
        
        try {
            FileUtilities.checkDirectory(installDir.getAbsolutePath());
        } catch (java.io.IOException e) {
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }
        
        if ((saveFile != null) && (saveFile.isFile())) {
            load();
        }        
    }
                                             

    public org.omg.PortableServer.POA _default_poa() {
        return poa;
    }
    

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/install:1.0
    //
    /***/
    public void install(String implUUID, String component_loc)
        throws InvalidLocation,
               InstallationFailure
    {
        ComponentImplementation implementation = null;

        System.out.println("---- Installation of implementation " + implUUID + 
                           " from " + component_loc);

        try {
            if (implementations.containsKey(implUUID)) {
                throw new InstallationFailure(
                    "Implementation " + implUUID + " is already installed.",
                    FILE_ALREADY_INSTALLED.value);
            }
            
            _ComponentPackage compPack;
            try {
                // try to get ComponentPackage from PackageManager
                // (sucessfull if package was installed by PackageManager)
                compPack = packageManager.get_component_package_for_uuid(implUUID);
            
            } catch (com.thalesgroup.CdmwDeployment.PackageNotFound e) {
                // Package not known by PackageManager.
                // ask PackagerManager to install it
                try {
                    compPack = packageManager.install_component_package(component_loc);
                
                } catch (InstallationFailure instFail) {
                    // the package was already installed, 
                    // but no such implementation was found by the PackageManager
                    if (instFail.reason == COMPONENT_ALREADY_INSTALLED.value) {
                        throw new InstallationFailure(
                            "There is no implementation with id=" + implUUID + 
                            " in component package " + component_loc,
                            UUID_NOT_FOUND.value);
                    } else {
                        throw instFail;
                    }
                }
            }
            
            // create an 'uuid' directory into 'installDir' directory
            String uuid = implUUID;
            if (implUUID.toUpperCase().startsWith(UUID_DCE_PREFIX)) {
                // remove "DCE:" from uuid
                uuid = implUUID.substring(UUID_DCE_PREFIX.length());
            }
            java.io.File uuidDir = new java.io.File(installDir, uuid);
            try {
                FileUtilities.checkDirectory(uuidDir);
            } catch (java.io.IOException e) {
                throw new InstallationFailure(
                    e.getMessage(),
                    READ_WRITE_ERROR.value);
            }
            
            // create new ComponentImplementation
            implementation = new ComponentImplementation(implUUID, uuidDir);
        
            // get code file location from ComponentPackage
            String codeLocation;    
            try {
                // ask PackageManager the location of code file
                codeLocation = compPack.get_code_location(implementation.getUUID());
            
            } catch (UnknownImplId e) {
                throw new InstallationFailure(
                    e.getMessage(),
                    UUID_NOT_FOUND.value);
            } catch (LocationCreationException e) {
                throw new InstallationFailure(
                    e.getMessage(),
                    PACKAGE_MANAGER_ERROR.value);
            }

            // install code
            implementation.installCode(codeLocation);        
            
            // get CSD location
            String csdLocation;
            try {
                csdLocation = compPack.get_CSD_location();
            } catch (LocationCreationException e) {
                throw new InstallationFailure(
                    e.getMessage(),
                    PACKAGE_MANAGER_ERROR.value);
            }
                                    
            // read CSD
            CsdReader csdReader;
            try {
                csdReader = new CsdReader(parser, csdLocation);
            } catch (ParsingException e) {
                throw new InstallationFailure(
                    e.reason,
                    ERROR_IN_CSD.value);
            }
                
            // install valutype factories
            installValuetypeFactories(implementation, csdReader, compPack);
            
            // install dependencies
            installDependencies(implementation, csdReader, compPack);
        
            // add implementation
            implementations.put(implementation.getUUID(), implementation);
            
            save();
            
        } catch (InstallationFailure e) {
            System.err.println(e.getMessage());
            
            // remove implementation files before throwing exception
            try {
                if (implementation != null) {
                    implementation.remove();
                }
            } catch (RemoveFailure ex) {
                // ignore
            }
            throw e;

        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            
            // remove implementation files before throwing exception
            try {
                if (implementation != null) {
                    implementation.remove();
                }
            } catch (RemoveFailure ex) {
                // ignore
            }
            throw e;

        } finally {
            System.out.println("---- End of installation of " + implUUID);
            System.out.println();
        }
    }
    

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/replace:1.0
    //
    /***/
    public void replace(String implUUID, String component_loc)
        throws InvalidLocation,
               InstallationFailure
    {
        try {
            remove(implUUID);
            
        } catch (UnknownImplId e) {
            InstallationFailure installFail = 
                new InstallationFailure(e.getMessage(),
                                        UUID_NOT_FOUND.value);
            System.err.println(installFail.getMessage());
            throw installFail;
            
        } catch (RemoveFailure e) {
            InstallationFailure installFail = 
                new InstallationFailure(e.getMessage(),
                                        CANNOT_REPLACE_IMPLEMENTATION.value);
            System.err.println(installFail.getMessage());
            throw installFail;
        }
        
        install(implUUID, component_loc);
    }
    

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/remove:1.0
    //
    /***/
    public void remove(String implUUID)
        throws UnknownImplId,
               RemoveFailure
    {
        try {
            ComponentImplementation implem = 
                (ComponentImplementation) implementations.get(implUUID);
            
            if (implem == null) {
                throw new UnknownImplId(
                    "UUID " + implUUID + " is unknown.");
            }
            
            // remove implementation files
            implem.remove();
            
            // remove implementation from implementations list
            implementations.remove(implUUID);
            
            save();
        
        } catch (UnknownImplId e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (RemoveFailure e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;
        }
    }
    

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/get_implementation:1.0
    //
    /***/
    public String get_implementation(String implUUID)
        throws UnknownImplId,
               InstallationFailure
    {
        try {
            ComponentImplementation implem = 
                (ComponentImplementation) implementations.get(implUUID);
            
            if (implem == null) {
                throw new UnknownImplId(
                    "UUID " + implUUID + " is unknown.");
            }
            
            return implem.getCodeLocation();

        } catch (UnknownImplId e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;
        }
    }
    

    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentInstallation/get_valuetypefactory_location:1.0
    //
    /***/
    public String get_valuetypefactory_location(String implUUID,
                                                String repid)
        throws UnknownImplId,
               com.thalesgroup.CdmwDeployment.UnknownValuetype,
               InstallationFailure
    {
        try {
            ComponentImplementation implem = 
                (ComponentImplementation) implementations.get(implUUID);
            
            if (implem == null) {
                throw new UnknownImplId(
                    "UUID " + implUUID + " is unknown.");
            }
            
            return implem.getValuetypeFactoryLocation(repid);
        
        } catch (UnknownImplId e) {
            System.err.println(e.getMessage());
            throw e;
        } catch (com.thalesgroup.CdmwDeployment.UnknownValuetype e) {
            System.err.println(e.getMessage());
            throw e;
        }
    }
                   


    private void installValuetypeFactories(ComponentImplementation implementation,
                                           CsdReader csdReader,
                                           _ComponentPackage compPack)
        throws InstallationFailure
    {
        try {
            // get all <valuetypefactory> element for this implementation
            NodeDescription[] nodes = csdReader.selectNodes(
                XPATH_IMPLEMENTATION +
                "[@id=\"" + implementation.getUUID() + "\"]/" +
                XPATH_VALUETYPE_FACTORY);
                        
            for (int i = 0; i < nodes.length; i++) {
                Element factoryElt = ElementHelper.narrow(nodes[i].reference());
                installValuetypeFactory(implementation, factoryElt, compPack);
            }
            
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        
        
    }
    
    private void installValuetypeFactory(ComponentImplementation implementation,
                                         Element factoryElt,
                                         _ComponentPackage compPack)
        throws InstallationFailure
    {
        String repid = null;
        try {
            // get repid attribute of <valuetypefactory>
            AttrDescription repidDescr = 
                factoryElt.get_attribute(XMLHelper.toShortArray("repid"));
            repid = XMLHelper.toString(repidDescr.value());
            
        } catch (DOMException e) {
            // shouldn't happen, repid attribute is required
            cdmw.common.Assert.check(false);
        }
        
        String location = null;
        try {
            NodeDescription[] nodes = factoryElt.select_nodes(
                XMLHelper.toShortArray(XPATH_FILE_REFERENCE));
            
            location = getFileReferenceLocation((ElementDescription) nodes[0], 
                                                compPack);
            
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        
        // install valuetype factory
        implementation.installValuetypeFactory(location, repid);
    }


                                          
    private void installDependencies(ComponentImplementation implementation,
                                     CsdReader csdReader,
                                     _ComponentPackage compPack)
        throws InstallationFailure
    {
        try {
            // get all <codebase> or <fileinarchive> child elements of
            // <dependency> element for this implementation
            NodeDescription[] nodes = csdReader.selectNodes(
                XPATH_IMPLEMENTATION +
                "[@id=\"" + implementation.getUUID() + "\"]/" +
                XPATH_DEPENDENCIES);

            for (int i = 0; i < nodes.length; i++) {
                // get location of dependency
                String location = 
                    getFileReferenceLocation((ElementDescription) nodes[i], 
                                             compPack);
                
                // install dependency
                implementation.installDependency(location);
            }
            
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        
    }
    

    private String getFileReferenceLocation(ElementDescription fileDescr,
                                            _ComponentPackage compPack)
        throws InstallationFailure
    {
        Element fileElt = ElementHelper.narrow(fileDescr.reference());
        String fileType = XMLHelper.toString(fileDescr.name());
        
        if (fileType.equals("codebase")) {
            String filename = null;
            try {
                // get filename attribute
                AttrDescription filenameDescr = 
                    fileElt.get_attribute(XMLHelper.toShortArray("filename"));
                filename = XMLHelper.toString(filenameDescr.value());
                
                // get location of codebase from ComponentPackage
                return compPack.get_codebase_location(filename);
                
            } catch (DOMException e) {
                // shouldn't happen, filename is required
                cdmw.common.Assert.check(false);
            } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
                throw new InstallationFailure(
                    "ComponentPackage " + compPack.get_name() + " " +
                    compPack.get_version() + " has no codebase " + filename +
                    ". This codebase is declared in .csd; it should have" +
                    " been installed by the PackageManager.",
                    PACKAGE_MANAGER_ERROR.value);
            } catch (LocationCreationException e) {
                throw new InstallationFailure(
                    e.getMessage(),
                    PACKAGE_MANAGER_ERROR.value);
            }
            
        } else if (fileType.equals("link")) {
            String href = null;
            try {
                // get href attribute
                AttrDescription hrefDescr = 
                    fileElt.get_attribute(XMLHelper.toShortArray("href"));
                href = XMLHelper.toString(hrefDescr.value());
                
                // get location of link from ComponentPackage
                return compPack.get_link_location(href);
                
            } catch (DOMException e) {
                // shouldn't happen, filename is required
                cdmw.common.Assert.check(false);
            } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
                throw new InstallationFailure(
                    "ComponentPackage " + compPack.get_name() + " " +
                    compPack.get_version() + " has no link " + href +
                    ". This link is declared in .csd; it should have" +
                    " been installed by the PackageManager.",
                    PACKAGE_MANAGER_ERROR.value);
            } catch (LocationCreationException e) {
                throw new InstallationFailure(
                    e.getMessage(),
                    PACKAGE_MANAGER_ERROR.value);
            }
        
        } else if (fileType.equals("fileinarchive")) {
            String name = null;
            try {
                // get name attribute
                AttrDescription nameDescr = 
                    fileElt.get_attribute(XMLHelper.toShortArray("name"));
                name = XMLHelper.toString(nameDescr.value());
                
                // get href attribute of <link> child element, if exists
                NodeDescription[] nodes = fileElt.select_nodes(
                    XMLHelper.toShortArray("link/@href"));
                String archiveLink = "";
                if (nodes.length != 0) {
                    AttrDescription hrefDescr = (AttrDescription) nodes[0];
                    archiveLink = XMLHelper.toString(hrefDescr.value());
                }
                
                // get location of fileinarchive from ComponentPackage
                return compPack.get_fileinarchive_location(name, archiveLink);
                
            } catch (DOMException e) {
                // shouldn't happen, filename is required
                cdmw.common.Assert.check(false);
            } catch (XPathFormatException e) {
                // shouldn't happen, xpath is valid
                cdmw.common.Assert.check(false);
            } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
                throw new InstallationFailure(
                    "ComponentPackage " + compPack.get_name() + " " +
                    compPack.get_version() + " has no fileinarchive " + name +
                    ". This fileinarchive is declared in .csd; it should have" +
                    " been installed by the PackageManager.",
                    PACKAGE_MANAGER_ERROR.value);
            } catch (LocationCreationException e) {
                throw new InstallationFailure(
                    e.getMessage(),
                    PACKAGE_MANAGER_ERROR.value);
            }
        
        } else {
            // shouldn't happen, only codebase, link and fileinarchive 
            // have been selected
            cdmw.common.Assert.check(false);
        }

        return null;    // for compiler
    }
    

    private void save() {
        if (saveFile != null) {
            try {
                java.io.ObjectOutputStream out = new java.io.ObjectOutputStream(
                    new java.io.FileOutputStream(saveFile));
                    
                out.writeObject(installDir);
                out.writeObject(implementations);
                out.close();
            
            } catch (java.io.FileNotFoundException e) {
                System.err.println("Cannot find save file " + saveFile.toString());
            
            } catch (java.io.IOException e) {
                System.err.println("Error while saving in file " + saveFile.toString() +
                                   " :\n " + e.getMessage());
            }
        }
    }
    
    
    private void load() {
        try {
            java.io.ObjectInputStream in = new java.io.ObjectInputStream(
                new java.io.FileInputStream(saveFile));
            
            installDir = (java.io.File) in.readObject();
            implementations = (java.util.Map) in.readObject();
            in.close();

        } catch (java.io.FileNotFoundException e) {
            System.err.println("Cannot find save file " + saveFile.toString());
            cdmw.common.Assert.check(false);          
        
        } catch (java.io.IOException e) {
            System.err.println("Error while loading file " + saveFile.toString() + 
                               ":\n" + e.getMessage());
            cdmw.common.Assert.check(false);          
        
        } catch (ClassNotFoundException e) {
            System.err.println("Error while loading file " + saveFile.toString() +
                               ": Unexpected data in file.");
            cdmw.common.Assert.check(false);          
        }
    
    }
    
}






