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

import org.omg.Components.Deployment.InstallationFailure;
import org.omg.Components.Deployment.InvalidLocation;
import org.omg.Components.RemoveFailure;
import com.thalesgroup.CdmwDeployment.ASSEMBLY_ALREADY_INSTALLED;
import com.thalesgroup.CdmwDeployment.COMPONENT_ALREADY_INSTALLED;
import com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR;
import com.thalesgroup.CdmwDeployment.BAD_LOCATION;
import com.thalesgroup.CdmwDeployment.COMPONENT_IN_USE;
import com.thalesgroup.CdmwDeployment.DELETE_ERROR;

import com.thalesgroup.CdmwDeployment._ComponentPackage;
import com.thalesgroup.CdmwDeployment._ComponentPackagePOATie;
import com.thalesgroup.CdmwDeployment._ComponentPackageHelper;
import com.thalesgroup.CdmwDeployment._AssemblyPackage;
import com.thalesgroup.CdmwDeployment._AssemblyPackagePOATie;
import com.thalesgroup.CdmwDeployment._AssemblyPackageHelper;
import com.thalesgroup.CdmwDeployment.PackageNotFound;

import cdmw.ccm.deployment.FileUtilities;
import com.thalesgroup.CdmwXML.XMLParser;

import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Document;
import com.thalesgroup.CdmwXML.DocumentHelper;
import com.thalesgroup.CdmwXML.DocumentDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;
import com.thalesgroup.CdmwXML.XPathFormatException;
import com.thalesgroup.CdmwXML.DOMException;
import cdmw.xmlutils.XMLHelper;


class PackageManagerImpl extends com.thalesgroup.CdmwDeployment.PackageManagerPOA
                         implements XMLPersistable
{

    private static final String SAVE_NAME_FOR_CODEBASE_MANAGER = "codebaseManager";
    private static final String SAVE_NAME_FOR_PACKAGE_MANAGER = "packageManager";
    private static final String SAVE_NAME_FOR_INSTALL_DIR = "installDir";
    private static final String SAVE_NAME_FOR_LOCATION_FACTORY = "locationFactory";
    private static final String SAVE_NAME_FOR_COMPONENT_PACKAGE = "componentPackage";
    private static final String SAVE_NAME_FOR_ASSEMBLY_PACKAGE = "assemblyPackage";

    private static final String ASSEMBLIES_DIR = "assemblies";
    private static final String COMPONENTS_DIR = "components";
    private static final String CODEBASE_DIR   = "codebases";

    private static final String TMP_ASSEMBLY_DIR_PREFIX = "assemblyPack";
    private static final String TMP_ASSEMBLY_PACK_NAME  = "assembly.pack";

    private static final String TMP_COMPONENT_DIR_PREFIX = "componentPack";
    private static final String TMP_COMPONENT_PACK_NAME  = "component.pack";

    private static final String CAD_EXTENSION = ".cad";
    private static final String CSD_EXTENSION = ".csd";

    private static final String UUID_DCE_PREFIX = "DCE:";

    private static final String EMPTY_SAVE_FILE =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
        "<!DOCTYPE CcmPackageManagerSave SYSTEM \"CcmPackageManagerSave.dtd\">\n" +
        "<CcmPackageManagerSave/>\n";

    private static final String ROOT_ELT_NAME_FOR_SAVE_FILE = "CcmPackageManagerSave";


    private static PackageManagerImpl instance = null;

    private static boolean isInitDone = false;



    private org.omg.PortableServer.POA packageManagerPOA;
    private org.omg.PortableServer.POA packagesPOA;

    private String installationDirectory;
    private java.io.File assembliesDir;
    private java.io.File componentsDir;
    private java.io.File codebasesDir;

    private java.io.File saveFile;

    private LocationFactory locationFactory;
    private XMLParser parser;

    private java.util.Map assemblyPackages;
    private java.util.Map componentPackages;
    


    protected PackageManagerImpl(org.omg.PortableServer.POA packageManagerPOA,
                                 org.omg.PortableServer.POA packagesPOA,
                                 XMLParser parser,
                                 java.io.File saveFile,
                                 String installationDirectory,
                                 LocationFactory locationFactory)
    {

        this.packageManagerPOA = packageManagerPOA;
        this.packagesPOA = packagesPOA;
        this.parser = parser;
        this.saveFile = saveFile;
        
        // Note: Use a TreeMap instead of a Hashtable because a Hashtable doesn't
        //       guarantee that elements will be saved and loaded in the same
        //       order. This may cause unit tests of CcmPackageManager fail.    
        this.assemblyPackages = new java.util.TreeMap();
        this.componentPackages = new java.util.TreeMap();
        
        cdmw.common.Assert.check(installationDirectory != null);
        this.installationDirectory = installationDirectory;
        cdmw.common.Assert.check(locationFactory != null);
        this.locationFactory = locationFactory;

    }


    private void initInstallationDirectories() {
        try {
            // check installationDirectory
            FileUtilities.checkDirectory(installationDirectory);
            // create 'assemblies' directory
            assembliesDir = new java.io.File(installationDirectory, ASSEMBLIES_DIR);
            FileUtilities.checkDirectory(assembliesDir.getPath());
            // create 'components' directory
            componentsDir = new java.io.File(installationDirectory, COMPONENTS_DIR);
            FileUtilities.checkDirectory(componentsDir.getPath());
            // create 'codebases' directory
            codebasesDir = new java.io.File(installationDirectory, CODEBASE_DIR);
            FileUtilities.checkDirectory(codebasesDir.getPath());

        } catch (java.io.IOException e) {
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }
    }


    public static PackageManagerImpl init(org.omg.PortableServer.POA packageManagerPOA,
                                          org.omg.PortableServer.POA packagesPOA,
                                          XMLParser parser,
                                          java.io.File saveFile,
                                          String installationDirectory,
                                          LocationFactory locationFactory)
    {
        instance = new PackageManagerImpl(packageManagerPOA,
                                          packagesPOA,
                                          parser,
                                          saveFile,
                                          installationDirectory,
                                          locationFactory);

        isInitDone = true;

        if ((saveFile != null) && (saveFile.exists())) {
            if (installationDirectory != null) {
                System.out.println(
                    "Warning, specified installation directory will be ignored. " +
                    "Only the installation directory provided by the saveFile " +
                    "will be considered.");
            }
            if (locationFactory != null) {
                System.out.println(
                    "Warning, specified location factory will be ignored. " +
                    "Only the location factory provided by the saveFile " +
                    "will be considered.");
            }
            // load saveFile
            try {
                instance.load();
            } catch (XMLPersistable.LoadException e) {
                System.out.println(e.getMessage());
                cdmw.common.Assert.check(false);
            }
        }

        // initialize installation directories
        instance.initInstallationDirectories();

        return instance;
    }


    public static PackageManagerImpl getInstance() {
        cdmw.common.Assert.check(isInitDone);
        return instance;
    }

    public LocationFactory getLocationFactory() {
        return locationFactory;
    }

    public java.io.File getAssembliesDir() {
        return assembliesDir;
    }

    public java.io.File getComponentsDir() {
        return componentsDir;
    }

    public java.io.File getCodebasesDir() {
        return codebasesDir;
    }


    public org.omg.PortableServer.POA _default_POA() {
        return packageManagerPOA;
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/PackageManager/install_assembly_package:1.0
    //
    /***/
    public _AssemblyPackage install_assembly_package(String assembly_loc)
        throws InvalidLocation, 
               InstallationFailure
    {
        java.io.File tmpDir      = null;    // download directory
        java.io.File uuidDir     = null;    // assembly installation directory

        Archive mainArchive    = null;
        FileReference cadRef   = null;

        System.out.println("---- Installation of assembly from " + assembly_loc);

        try {
            // create tmp dir for downloding assembly package
            tmpDir = FileUtilities.createTempDir(TMP_ASSEMBLY_DIR_PREFIX);

            // download assembly package
            java.io.File packFile = new java.io.File(tmpDir, TMP_ASSEMBLY_PACK_NAME);
            System.out.println(" download " + assembly_loc +
                               " as " + packFile.toString());
            try {
                FileUtilities.downloadFile(assembly_loc, packFile.getPath());
            } catch (cdmw.common.BadParameterException e) {
                // invalid source location
                throw new InvalidLocation(e.getMessage());
            }
            

            // check if package is an archive
            if (FileUtilities.isZip(packFile.getPath())) {
                // create mainArchive
                try {
                    mainArchive = new Archive(assembly_loc);
                } catch (cdmw.common.BadParameterException e) {
                    // error in assembly_loc
                    throw new InstallationFailure(e.getMessage(),
                                                  BAD_LOCATION.value);
                }
                mainArchive.setActualLocation(packFile);

                // extract CAD from archive
                cadRef = PackageInstaller.extractDescriptorWithSuffix(CAD_EXTENSION,
                                                                      mainArchive,
                                                                      tmpDir);
            } else {
                // there is no mainArchive
                mainArchive = null;
                // package must be directly the CAD
                try {
                    cadRef = new Link(assembly_loc);
                } catch (cdmw.common.BadParameterException e) {
                    // error in assembly_loc
                    throw new InstallationFailure(e.getMessage(),
                                                  BAD_LOCATION.value);
                }
                cadRef.setActualLocation(packFile);
            }

            // analyze assembly package and component packages used by assembly
            AssemblyPackageInstaller assemblyInstaller =
                new AssemblyPackageInstaller(cadRef, tmpDir, parser);
            assemblyInstaller.analyze();
            AssemblyPackageImpl assemblyImpl =
                assemblyInstaller.getAssemblyPackageImpl();

            // create an 'uuid' directory into 'assemblies' directory
            String uuid = assemblyImpl.get_uuid();
            if (uuid.toUpperCase().startsWith(UUID_DCE_PREFIX)) {
                // remove "DCE:" from uuid
                uuid = uuid.substring(UUID_DCE_PREFIX.length());
            }
            uuidDir = new java.io.File(assembliesDir, uuid);
            FileUtilities.checkDirectory(uuidDir);
            if (uuidDir.list().length > 0) {
                // if this directory exists and is not empty,
                // set uuidDir to null to not remove it and throw InstallationFailure
                String uuidDirName = uuidDir.toString();
                uuidDir = null;
                throw new InstallationFailure(
                    "Directory " + uuidDirName + " exists and is not empty.",
                    READ_WRITE_ERROR.value);
            }

            // install assembly package and component packages
            assemblyInstaller.install(uuidDir, componentsDir, codebasesDir);

            // close installer
            assemblyInstaller.remove();

            // create AssemblyPackage object
            _AssemblyPackage assPack = createAssemblyPackage(assemblyImpl);

            // add it to installed assemblies list
            addAssemblyPackage(assPack);

            // set to null directories to be not deleted
            uuidDir = null;

            // save current state
            save();

            return assPack;

        } catch (java.io.IOException e) {
            InstallationFailure installFail =
                new InstallationFailure(e.getMessage(),
                                        READ_WRITE_ERROR.value);
            System.err.println(e.getMessage());
            throw installFail;

        } catch (InvalidLocation e) {
            System.err.println(e.getMessage());
            throw e;
            
        } catch (InstallationFailure e) {
            System.err.println(e.getMessage());
            throw e;

        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;
            
        } finally {
            // remove all files of assembly package
            if (tmpDir != null) {
                try {
                    FileUtilities.removeDirectoryRecursively(tmpDir);
                } catch (java.io.IOException ioe) {
                    // ignore
                }
            }
            if (uuidDir != null) {
                try {
                    FileUtilities.removeDirectoryRecursively(uuidDir);
                } catch (java.io.IOException ioe) {
                    // ignore
                }
            }
            System.out.println("---- End of installation of " + assembly_loc);
            System.out.println();
        }
    }



    //
    // IDL:thalesgroup.com/CdmwDeployment/PackageManager/install_component_package:1.0
    //
    /***/
    public _ComponentPackage install_component_package(String component_loc)
        throws InvalidLocation, 
               InstallationFailure
    {
        java.io.File tmpDir      = null;    // download directory
        java.io.File compDir     = null;    // component installation directory

        Archive mainArchive    = null;
        FileReference csdRef   = null;

        System.out.println("---- Installation of component from " + component_loc);

        try {
            // create tmp dir for downloding assembly package
            tmpDir = FileUtilities.createTempDir(TMP_COMPONENT_DIR_PREFIX);

            // download assembly package
            java.io.File packFile = new java.io.File(tmpDir, TMP_COMPONENT_PACK_NAME);
            System.out.println(" download " + component_loc +
                               " as " + packFile.toString());
            try {
                FileUtilities.downloadFile(component_loc, packFile.getPath());
            } catch (cdmw.common.BadParameterException e) {
                // invalid source location
                throw new InvalidLocation(e.getMessage());
            }


            // check if package is an archive
            if (FileUtilities.isZip(packFile.getPath())) {
                // create mainArchive
                try {
                    mainArchive = new Archive(component_loc);
                } catch (cdmw.common.BadParameterException e) {
                    // error in component_loc
                    throw new InstallationFailure(e.getMessage(),
                                                  BAD_LOCATION.value);
                }
                mainArchive.setActualLocation(packFile);

                // extract CSD from archive
                csdRef = PackageInstaller.extractDescriptorWithSuffix(CSD_EXTENSION,
                                                                      mainArchive,
                                                                      tmpDir);
            } else {
                // there is no mainArchive
                mainArchive = null;
                // package must be directly the CSD
                try {
                    csdRef = new Link(component_loc);
                } catch (cdmw.common.BadParameterException e) {
                    // error in component_loc
                    throw new InstallationFailure(e.getMessage(),
                                                  BAD_LOCATION.value);
                }
                csdRef.setActualLocation(packFile);
            }

            // analyze component package
            ComponentPackageInstaller componentInstaller =
                new ComponentPackageInstaller(csdRef, tmpDir, parser);
            componentInstaller .analyze();
            ComponentPackageImpl componentImpl =
                componentInstaller.getComponentPackageImpl();

            // create an 'name'+'version' directory into 'components' directory
            compDir = new java.io.File(
                componentsDir,
                componentImpl.get_name() + componentImpl.get_version());
            FileUtilities.checkDirectory(compDir);
            if (compDir.list().length > 0) {
                // if this directory exists and is not empty,
                // set compDir to null to not remove it and throw InstallationFailure
                String compDirName = compDir.toString();
                compDir = null;
                throw new InstallationFailure(
                    "Directory " + compDirName + " exists and is not empty.",
                    READ_WRITE_ERROR.value);
            }


            // install component package
            componentInstaller.install(compDir, codebasesDir);

            // close installer
            componentInstaller.remove();

            // create ComponentPackage object
            _ComponentPackage compPack = createComponentPackage(componentImpl);
            
            // add it to installed components list
            addComponentPackage(compPack);

            // set to null directories to be not deleted
            compDir = null;

            // save current state
            save();

            return compPack;

        } catch (java.io.IOException e) {
            InstallationFailure installFail = 
                new InstallationFailure(e.getMessage(),
                                        READ_WRITE_ERROR.value);
            System.err.println(installFail.getMessage());
            throw installFail;

        } catch (InvalidLocation e) {
            System.err.println(e.getMessage());
            throw e;
            
        } catch (InstallationFailure e) {
            System.err.println(e.getMessage());
            throw e;

        } catch (org.omg.CORBA.SystemException e) {
            System.err.println(e.toString());
            throw e;

        } finally {
            // remove all files of assembly package
            if (tmpDir != null) {
                try {
                    FileUtilities.removeDirectoryRecursively(tmpDir);
                } catch (java.io.IOException ioe) {
                    // ignore
                }
            }
            if (compDir != null) {
                try {
                    FileUtilities.removeDirectoryRecursively(compDir);
                } catch (java.io.IOException ioe) {
                    // ignore
                }
            }

            System.out.println("---- End of installation of " + component_loc);
        }
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/PackageManager/remove_assembly_package:1.0
    //
    /***/
    public void remove_assembly_package(String assemblyUUID)
        throws PackageNotFound,
               RemoveFailure
    {
        try {
            // get AssemblyPackage
            _AssemblyPackage assPack = get_assembly_package(assemblyUUID);
            
            // retrieve AssemblyPackageImpl
            AssemblyPackageImpl assPackImpl = getAssemblyPackageImpl(assPack);
            
            // deactivate AssemblyPackage
            try {
                byte[] oid = packagesPOA.reference_to_id(assPack);
                packagesPOA.deactivate_object(oid);
            } catch (org.omg.PortableServer.POAPackage.WrongAdapter e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.ObjectNotActive e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            
            // remove AssemblyPackage
            assemblyPackages.remove(assemblyUUID);
            
            // remove AssemblyPackageImpl
            assPackImpl.remove();
            
            // remove assembly directory
            String uuid = assemblyUUID;
            if (uuid.toUpperCase().startsWith(UUID_DCE_PREFIX)) {
                // remove "DCE:" from uuid
                uuid = uuid.substring(UUID_DCE_PREFIX.length());
            }
            java.io.File uuidDir = new java.io.File(assembliesDir, uuid);
            try {
                FileUtilities.removeDirectoryRecursively(uuidDir);
            } catch (java.io.IOException e) {
                throw new RemoveFailure(
                    "Cannot remove directory " + uuidDir.toString(),
                    DELETE_ERROR.value);
            }
            
            // save current state
            save();
        
        } catch (PackageNotFound e) {
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
    // IDL:thalesgroup.com/CdmwDeployment/PackageManager/remove_assembly_package:1.0
    //
    /***/
    public void remove_component_package(String name, String version)
        throws PackageNotFound,
               RemoveFailure
    {
        try {
            // get _ComponentPackage
            _ComponentPackage compPack = get_component_package(name, version);
            
            // check if component package is used by an assembly
            java.util.Iterator it = assemblyPackages.values().iterator();
            while (it.hasNext()) {
                _AssemblyPackage assPack = (_AssemblyPackage) it.next();
                AssemblyPackageImpl assPackImpl = getAssemblyPackageImpl(assPack);
                if (assPackImpl.needsComponent(name, version)) {
                    throw new RemoveFailure(
                        "Component " + name + 
                        (version.equals("") ? "" : " "+version) +
                        " is needed by assembly " + assPackImpl.get_uuid(),
                        COMPONENT_IN_USE.value);
                }
            }
            
            // retrieve ComponentPackageImpl
            ComponentPackageImpl compPackImpl = getComponentPackageImpl(compPack);
            
            // deactivate ComponentPackage
            try {
                byte[] oid = packagesPOA.reference_to_id(compPack);
                packagesPOA.deactivate_object(oid);
            } catch (org.omg.PortableServer.POAPackage.WrongAdapter e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            } catch (org.omg.PortableServer.POAPackage.ObjectNotActive e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            
            // remove ComponentPackage
            componentPackages.remove(name + version);
            
            // remove ComponentPackageImpl
            compPackImpl.remove();
            
            // remove component directory
            java.io.File uuidDir = 
                new java.io.File(componentsDir, name + version);
            try {
                FileUtilities.removeDirectoryRecursively(uuidDir);
            } catch (java.io.IOException e) {
                throw new RemoveFailure(
                    "Cannot remove directory " + uuidDir.toString(),
                    DELETE_ERROR.value);
            }
            
            // save current state
            save();

        } catch (PackageNotFound e) {
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
    // IDL:thalesgroup.com/CdmwDeployment/PackageManager/get_assembly_package:1.0
    //
    /***/
    public _AssemblyPackage get_assembly_package(String assemblyUUID)
        throws PackageNotFound
    {
        _AssemblyPackage pack =
            (_AssemblyPackage) assemblyPackages.get(assemblyUUID);
        
        if (pack == null) {
            throw new PackageNotFound("Unkown assembly: " + assemblyUUID);
        }
        
        return pack;
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/PackageManager/get_component_package:1.0
    //
    /***/
    public _ComponentPackage get_component_package(String name,
                                                   String version)
        throws PackageNotFound
    {
        _ComponentPackage pack =
            (_ComponentPackage) componentPackages.get(name + version);

        if (pack == null) {
            throw new PackageNotFound("Unkown component: " + name +
                (version.equals("") ? "" : " "+version));
        }

        return pack;
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/PackageManager/get_component_package_for_uuid:1.0
    //
    /***/
    public _ComponentPackage get_component_package_for_uuid(String implUUID)
        throws PackageNotFound
    {
        java.util.Iterator it = componentPackages.values().iterator();

        while (it.hasNext()) {
            _ComponentPackage pack = (_ComponentPackage) it.next();

            ComponentPackageImpl packImpl = getComponentPackageImpl(pack);

            if (packImpl.containsImplementation(implUUID)) {
                return pack;
            }
        }

        throw new PackageNotFound("Component with implementation " + implUUID +
                                  " not found.");
    }


    boolean isUuidAlreadyDefined(String implUUID) {
        java.util.Iterator it = componentPackages.values().iterator();

        while (it.hasNext()) {
            _ComponentPackage pack = (_ComponentPackage) it.next();
            ComponentPackageImpl packImpl = getComponentPackageImpl(pack);
            if (packImpl.containsImplementation(implUUID)) {
                return true;
            }
        }
        return false;
    }



    _ComponentPackage createComponentPackage(ComponentPackageImpl packImpl) {
        _ComponentPackage compPack = null;
        try {
            // create component package servant
            _ComponentPackagePOATie servant = new _ComponentPackagePOATie(packImpl,
                                                                          packagesPOA);

            // activate component package servant
            byte[] oid = packagesPOA.activate_object(servant);
            compPack = _ComponentPackageHelper.narrow(packagesPOA.id_to_reference(oid));
        } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (org.omg.PortableServer.POAPackage.ObjectNotActive e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        return compPack;
    }
    
    _AssemblyPackage createAssemblyPackage(AssemblyPackageImpl packImpl) {
        _AssemblyPackage assPack = null;
        try {
            // create assembly package servant
            _AssemblyPackagePOATie servant = new _AssemblyPackagePOATie(packImpl,
                                                                        packagesPOA);

            // activate component package servant
            byte[] oid = packagesPOA.activate_object(servant);
            assPack = _AssemblyPackageHelper.narrow(packagesPOA.id_to_reference(oid));
        } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (org.omg.PortableServer.POAPackage.ObjectNotActive e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        return assPack;
    }
    


    void addComponentPackage(_ComponentPackage compPack) 
        throws InstallationFailure
    {
        // check if ComponentPackage already exists
        String name = compPack.get_name();
        String version = compPack.get_version();
        if (containsComponentPackage(name, version)) {
            throw new InstallationFailure(
                "Component " + name + (version.equals("") ? "" : " " + version) +
                " is already installed.",
                COMPONENT_ALREADY_INSTALLED.value);
        }

        // add component to installed components list
        componentPackages.put(name + version, compPack);
    }

    void addAssemblyPackage(_AssemblyPackage assPack) 
        throws InstallationFailure
    {
        // check if ComponentPackage already exists
        String uuid = assPack.get_uuid();
        if (containsAssemblyPackage(uuid)) {
            throw new InstallationFailure(
                "Assembly " + uuid + " is already installed.",
                COMPONENT_ALREADY_INSTALLED.value);
        }

        // add component to installed components list
        assemblyPackages.put(uuid, assPack);

        // add each ComponentPackage used by assembly to 
        // installed components list (if not already present)
        AssemblyPackageImpl packImpl = getAssemblyPackageImpl(assPack);        
        java.util.Iterator it = packImpl.getComponentPackagesIterator();
        while (it.hasNext()) {
            _ComponentPackage compPack = (_ComponentPackage) it.next();
            try {
                addComponentPackage(compPack);
            } catch (InstallationFailure e) {
                // ignore
            }
        }
    }
    

    public boolean containsAssemblyPackage(String uuid) {
        return assemblyPackages.containsKey(uuid);
    }


    public boolean containsComponentPackage(String name, String version) {
        if (version == null) {
            return componentPackages.containsKey(name);
        }
        return componentPackages.containsKey(name + version);
    }



    private ComponentPackageImpl getComponentPackageImpl(_ComponentPackage pack) {
        ComponentPackageImpl packImpl = null;

        try {
            _ComponentPackagePOATie packTie =
                (_ComponentPackagePOATie) packagesPOA.reference_to_servant(pack);

            packImpl = (ComponentPackageImpl) packTie._delegate();

        } catch (org.omg.PortableServer.POAPackage.ObjectNotActive e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (org.omg.PortableServer.POAPackage.WrongAdapter e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        return packImpl;
    }


    private AssemblyPackageImpl getAssemblyPackageImpl(_AssemblyPackage pack) {
        AssemblyPackageImpl packImpl = null;
        try {
            _AssemblyPackagePOATie packTie =
                (_AssemblyPackagePOATie) packagesPOA.reference_to_servant(pack);
            packImpl = (AssemblyPackageImpl) packTie._delegate();

        } catch (org.omg.PortableServer.POAPackage.ObjectNotActive e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (org.omg.PortableServer.POAPackage.WrongAdapter e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        return packImpl;
    }

    public void save() {
        Document doc = null;
        try {
            if (saveFile == null) {
                return;
            }

            // if saveFile exists, delete it first and create an empty file
            if (saveFile.exists() &&
                !saveFile.delete() &&
                !saveFile.createNewFile())
            {
                System.out.println("Cannot overwrite file: " + saveFile.toString());
            }

            // write empty save file (xml format)
            java.io.FileWriter writer = new java.io.FileWriter(saveFile);
            writer.write(EMPTY_SAVE_FILE);
            writer.close();

            // open file with XMLParser
            DocumentDescription docDescr = parser.parse(saveFile.getAbsolutePath());
            doc = DocumentHelper.narrow(docDescr.reference());
            ElementDescription rootEltDescr = doc.get_document_element();
            Element rootElt = ElementHelper.narrow(rootEltDescr.reference());

            // save CodebaseManager
            Element codebaseMgrElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_CODEBASE_MANAGER));
            CodebaseManager.getInstance().save(doc, codebaseMgrElt);
            rootElt.append_child(codebaseMgrElt);

            // save PackageManagerImpl
            Element packageMgrElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_PACKAGE_MANAGER));
            save(doc, packageMgrElt);
            rootElt.append_child(packageMgrElt);

            try {
                doc.save();
            } catch (com.thalesgroup.CdmwXML.DocumentPackage.SaveException e) {
                System.err.println("Cannot save file: " + saveFile.toString() +
                                              ".\n" + e.reason);
            }

        } catch (java.io.IOException e) {
                System.err.println("Cannot save file: " + saveFile.toString() +
                                              ".\n" + e.getMessage());
        } catch (ParsingException e) {
            // shouldn't happen (parsing empty save file)
            cdmw.common.Assert.check(false);
        } catch (DOMException e) {
            System.err.println("Error while saving in file " + saveFile.toString() +
                               ".\n" + e.reason);
        } catch (XMLPersistable.SaveException e) {
            System.err.println("Error while saving in file " + saveFile.toString() +
                               ".\n" + e.getMessage());
        } finally {
            if (doc != null) {
                doc.close();
            }
        }
    }

    private void load()
        throws XMLPersistable.LoadException
    {
        System.out.println("Load file " + saveFile.toString());
        try {
            if (!saveFile.isFile()) {
                throw new XMLPersistable.LoadException(
                    "Cannot find file: " + saveFile.toString());
            }

            // open file with XMLParser
            DocumentDescription docDescr = parser.parse(saveFile.getAbsolutePath());
            Document doc = DocumentHelper.narrow(docDescr.reference());
            ElementDescription rootEltDescr = doc.get_document_element();
            Element rootElt = ElementHelper.narrow(rootEltDescr.reference());

            // check if root element is a CcmPackageManagerSave
            String rootEltName = XMLHelper.toString(rootEltDescr.name());
            if (! rootEltName.equals(ROOT_ELT_NAME_FOR_SAVE_FILE)) {
                throw new XMLPersistable.LoadException(
                    "Save file is not a " + ROOT_ELT_NAME_FOR_SAVE_FILE);
            }


            // load CodebaseManager
            NodeDescription[] descriptors = null;
            try {
                descriptors = rootElt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_CODEBASE_MANAGER));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            if (descriptors.length < 1) {
                throw new XMLPersistable.LoadException(
                    "Cannot find an element corresponding to CodebaseManager.");
            }
            Element cMgrElt = ElementHelper.narrow(descriptors[0].reference());
            CodebaseManager.getInstance().load(cMgrElt);

            // load PackageManagerImpl
            try {
                descriptors = rootElt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_PACKAGE_MANAGER));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            if (descriptors.length < 1) {
                throw new XMLPersistable.LoadException(
                    "Cannot find an element corresponding to PackageManager.");
            }
            Element pMgrElt = ElementHelper.narrow(descriptors[0].reference());
            load(pMgrElt);

        } catch (ParsingException e) {
            throw new XMLPersistable.LoadException(
                "Error while pasing save file " + saveFile.toString() + " :\n" +
                e.reason);
        }

        System.out.println("Load done.");
    }


    public void save(com.thalesgroup.CdmwXML.Document doc,
                     com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.SaveException
    {
        try {
            // save installDir
            elt.set_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_INSTALL_DIR),
                              XMLHelper.toShortArray(installationDirectory));

            // save locationFactory
            Element locFactoryElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_LOCATION_FACTORY));
            locationFactory.save(doc, locFactoryElt);
            elt.append_child(locFactoryElt);

            // save components
            java.util.Iterator itComp = componentPackages.values().iterator();
            while (itComp.hasNext()) {
                _ComponentPackage pack = (_ComponentPackage) itComp.next();
                Element compElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMPONENT_PACKAGE));
                getComponentPackageImpl(pack).save(doc, compElt);
                elt.append_child(compElt);
            }

            // save assemblies
            java.util.Iterator itAss = assemblyPackages.values().iterator();
            while (itAss.hasNext()) {
                _AssemblyPackage pack = (_AssemblyPackage) itAss.next();
                Element assElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_ASSEMBLY_PACKAGE));
                getAssemblyPackageImpl(pack).save(doc, assElt);
                elt.append_child(assElt);
            }

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.SaveException(
                "Error while saving PackageManagerImpl:\n" + e.toString());
        }
    }


    public void load(com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.LoadException
    {
        try {
            // load installDir
            AttrDescription installDirAttr = elt.get_attribute(
                XMLHelper.toShortArray(SAVE_NAME_FOR_INSTALL_DIR));
            installationDirectory = XMLHelper.toString(installDirAttr.value());

            // initialize installation directories
            initInstallationDirectories();

            // load locationFacory
            NodeDescription[] descriptors = null;
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_LOCATION_FACTORY + "/*"));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            if (descriptors.length < 1) {
                throw new XMLPersistable.LoadException(
                    "Cannot find an element corresponding to XPath: " +
                    SAVE_NAME_FOR_LOCATION_FACTORY + "/*");
            }
            Element locFactElt = ElementHelper.narrow(descriptors[0].reference());
            String factoryType = XMLHelper.toString(((ElementDescription) descriptors[0]).name());
            locationFactory = (LocationFactory) Class.forName(factoryType).newInstance();
            locationFactory.load(locFactElt);

            // load components
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMPONENT_PACKAGE));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            for (int i = 0; i < descriptors.length; i++) {
                Element compElt = ElementHelper.narrow(descriptors[i].reference());
                ComponentPackageImpl compImpl = new ComponentPackageImpl();
                compImpl.load(compElt);
                _ComponentPackage compPack = createComponentPackage(compImpl);
                addComponentPackage(compPack);
            }

           // load assemblies
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_ASSEMBLY_PACKAGE));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            for (int i = 0; i < descriptors.length; i++) {
                Element assElt = ElementHelper.narrow(descriptors[i].reference());
                AssemblyPackageImpl assImpl = new AssemblyPackageImpl();
                assImpl.load(assElt);
                _AssemblyPackage assPack = createAssemblyPackage(assImpl);
                addAssemblyPackage(assPack);
            }

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.LoadException(
                "Error while loading PackageManagerImpl :\n" + e.toString());
        } catch (InstallationFailure e) {
            throw new XMLPersistable.LoadException(
                "Error while loading PackageManagerImpl :\n" + e.toString());
        } catch (ClassNotFoundException e) {
            // shouldn't happen
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (IllegalAccessException e) {
            // shouldn't happen
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (InstantiationException e) {
            // shouldn't happen
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }

    }


}





