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
import com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CSD;
import com.thalesgroup.CdmwDeployment.UNSUPPORTED_COMPONENT_TYPE;
import com.thalesgroup.CdmwDeployment.ASSEMBLY_ALREADY_INSTALLED;

import com.thalesgroup.CdmwDeployment._ComponentPackage;
import com.thalesgroup.CdmwDeployment.PackageNotFound;


import cdmw.ccm.deployment.FileUtilities;
import cdmw.ccm.deployment.CadReader;
import cdmw.ccm.deployment.CsdReader;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;


class AssemblyPackageInstaller extends PackageInstaller {


    private static final String XPATH_COMPONENTFILES =
        "/componentassembly/componentfiles/*";
    private static final String XPATH_HOMEPLACEMENTS_WITH_PROPERTIES =
        "//homeplacement[homeproperties or componentproperties]";
    private static final String XPATH_COMPONENT_INSTANTIATIONS_WITH_PROPERTIES =
        "//componentinstantiation[componentproperties]";
    private static final String XPATH_FTCOMPONENTGROUPS_WITH_PROPERTIES =
        "//ftcomponentgroup[groupproperties]";
    private static final String XPATH_LBCOMPONENTGROUPS_WITH_PROPERTIES =
        "//lbcomponentgroup[groupproperties]";
    private static final String XPATH_HOMEPROPERTIES_CHILDREN =
        "homeproperties/*";
    private static final String XPATH_COMPONENTPROPERTIES_CHILDREN =
        "componentproperties/*";
    private static final String XPATH_GROUPPROPERTIES_CHILDREN =
        "groupproperties/*";

    private static final String CSD_EXTENSION = ".csd";


    private FileReference cadRef;

    // List of ComponentPackageInstaller for which install() must be called
    // for installation of assembly's component packages
    private java.util.Vector componentInstallers;



    public AssemblyPackageInstaller(FileReference cadRef,
                                    java.io.File tmpDir,
                                    XMLParser parser)
        throws InstallationFailure
    {
        super(tmpDir, parser);

        this.cadRef = cadRef;
        componentInstallers = new java.util.Vector();
    }

    public AssemblyPackageImpl getAssemblyPackageImpl() {
        // check if analyze() has been called
        cdmw.common.Assert.check(packImpl != null);

        return (AssemblyPackageImpl) packImpl;
    }



    // analyze a cad file and associated csd files
    // and construct an AssemblyPackageImpl object
    public void analyze()
        throws InstallationFailure
    {
        System.out.println(" analyze assembly package");


        // if cad is not installed, install it
        if (!cadRef.isInstalled()) {
            cadRef.installInto(tmpDir);
        }

        System.out.println("   read " + cadRef.getActualBasename());
        // create CadReader
        CadReader cadReader;
        try {
            cadReader = new CadReader(parser, cadRef.getActualLocation());
        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            throw new InstallationFailure(e.reason,
                                          ERROR_IN_CAD.value);
        }

        // read uuid in .cad
        String uuid = cadReader.getUUID();

        // check if assembly is not already installed
        if (PackageManagerImpl.getInstance()
                              .containsAssemblyPackage(uuid))
        {
            throw new InstallationFailure(
                "Assembly " + uuid + " is already installed.",
                ASSEMBLY_ALREADY_INSTALLED.value);
        }

        // retrieve mainArchive (if exists)
        Archive mainArchive = null;
        if (cadRef instanceof FileInArchive) {
            mainArchive = ((FileInArchive) cadRef).getArchive();
        }

        // create a new assembly package
        packImpl = new AssemblyPackageImpl(uuid,
                                           cadRef,
                                           mainArchive);

        // complete assembly with references to used components
        completeAssemblyWithComponents(cadReader);

        // complete assembly with references to used CPFs
        completeAssemblyWithCPFs(cadReader);

        cadReader.close();

        isAnalyzeDone = true;
    }



    private void completeAssemblyWithComponents(CadReader cadReader)
        throws InstallationFailure
    {
        try {
            // get all <componentfile> elements
            NodeDescription[] componentfiles = cadReader.selectNodes(XPATH_COMPONENTFILES);

            for (int i = 0; i < componentfiles.length; i++) {

                // NodeDescription should have an Element as reference
                Element elt = ElementHelper.narrow(componentfiles[i].reference());

                // get 'id' attribute of <componentfile> element
                AttrDescription idAttr = elt.get_attribute(XMLHelper.toShortArray("id"));
                String id = XMLHelper.toString(idAttr.value());

                // check 'type' of <componentfile> element
                String type;
                try {
                    AttrDescription typeAttr = elt.get_attribute(XMLHelper.toShortArray("type"));
                    type = XMLHelper.toString(typeAttr.value());
                    // 'type' is specified: this is NOT a CORBA Component
                    throw new InstallationFailure(
                        "The component " + id + " declared in the assembly " +
                        ((AssemblyPackageImpl) packImpl).get_uuid() +
                        " has an unsupported type: " + type,
                        UNSUPPORTED_COMPONENT_TYPE.value);
                } catch(DOMException e) {
                    // 'type' is unspecified: this is a CORBA Component
                }

                // convert first child of <componentfile> into FileReference
                // (this must be a <fileinarchive>, a <link> or a <codebase>)
                ElementDescription[] childs = elt.get_child_elements();
                if (childs.length == 0) {
                    throw new InstallationFailure(
                        "There is no child element for <componentfile id=\"" +
                        id + "\">.",
                        ERROR_IN_CAD.value);
                }
                FileReference compRef = packImpl.loadFileReference(childs[0]);

                // install component file into a tempory subdir of tmpDir
                // (avoid conflicts if several .csd with same name)
                java.io.File compTmpDir;
                try {
                    compTmpDir = FileUtilities.createTempDir("component", tmpDir);

                    // if csd is a fileinarchive, and its archive is not installed,
                    // install the archive into compTmpDir
                    if (compRef instanceof FileInArchive) {
                        Archive a = ((FileInArchive) compRef).getArchive();
                        if (!a.isInstalled()) {
                            a.installInto(compTmpDir);
                        }
                    }
                    // install component file into compTmpDir
                    compRef.installInto(compTmpDir.getAbsolutePath());

                } catch (java.io.IOException e) {
                    throw new InstallationFailure(e.getMessage(),
                                                  READ_WRITE_ERROR.value);
                }


                FileReference csdRef = null;
                // check if component file is a zip
                if (FileUtilities.isZip(compRef.getActualLocation())) {
                    // create Archive with compRef
                    String sourceLink = null;
                    if (compRef instanceof Link) {
                        sourceLink = ((Link) compRef).getSourceLink();
                    } else if (compRef instanceof FileInArchive) {
                        // link of an archive in a zip. Syntax similar to JarURL:
                        // 'zip:http://xyz.com/archive.zip!/entry.file'.
                        // But this is not an official syntax !
                        sourceLink = "zip:" + ((FileInArchive) compRef).getArchive().getSourceLink() +
                                     "!/" + ((FileInArchive) compRef).getArchiveEntry();
                    }
                    Archive archiveRef = null;
                    try {
                        archiveRef = new Archive(sourceLink);
                    } catch (cdmw.common.BadParameterException e) {
                        // should not happen
                        cdmw.common.Assert.check(false);
                    }

                    archiveRef.setActualLocation(
                        new java.io.File(compRef.getActualLocation()));

                    // extract .csd from archive into compTmpDir
                    csdRef = PackageInstaller.extractDescriptorWithSuffix(CSD_EXTENSION,
                                                                          archiveRef,
                                                                          compTmpDir);

                } else {
                    // component file must be directly the .csd
                    csdRef = compRef;
                }

                // get name and version of component from csd
                CsdReader csdReader;
                try {
                    csdReader = new CsdReader(parser, csdRef.getActualLocation());
                } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
                    throw new InstallationFailure(e.reason,
                                                  ERROR_IN_CSD.value);
                }
                String name = csdReader.getName();
                String version = csdReader.getVersion();
                csdReader.close();

                System.out.println(" component package " + name +
                                   (version != null ? " "+version : "") +
                                   " required");

                // component package to be added to assembly package
                _ComponentPackage pack = null;

                // check if component is already installed
                if (PackageManagerImpl.getInstance()
                                      .containsComponentPackage(name, version))
                {
                    System.out.println(" component package " + name +
                                       (version != null ? " "+version : "") +
                                       " already installed");
                    try {
                        // set already installed component package
                        // as to be added to assembly package
                        pack = (_ComponentPackage) PackageManagerImpl.getInstance()
                                                      .get_component_package(name, version);
                    } catch (PackageNotFound e) {
                        // shouldn't happen
                        cdmw.common.Assert.check(false);
                    }

                    // delete csd file (no more needed)
                    csdRef.delete();

                } else {

                    try {
                        // create Installer for component package
                        ComponentPackageInstaller compInstaller =
                            new ComponentPackageInstaller(csdRef, compTmpDir, parser);

                        // analyze component package
                        compInstaller.analyze();
                        ComponentPackageImpl compImpl =
                            compInstaller.getComponentPackageImpl();

                        // add ComponentPackageInstaller to list of
                        // component to be installed
                        componentInstallers.add(compInstaller);

                        // create ComponentPackage object
                        pack = PackageManagerImpl.getInstance().createComponentPackage(compImpl);

                   } catch (InstallationFailure e) {
                        throw e;
                    }
                }

                // add component package to assembly package
                ((AssemblyPackageImpl) packImpl).addComponentPackage(id, pack);

            }  // end of iteration over csd FileReferences

        } catch(XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (DOMException e) {
            throw new InstallationFailure(e.reason,
                                          ERROR_IN_CAD.value);
        } catch (FileAlreadyReferenced e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CAD.value);
        } catch (XMLPersistable.LoadException e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CAD.value);
        }
    }



    private void completeAssemblyWithCPFs(CadReader cadReader)
        throws InstallationFailure
    {
        try {

            // find all homeplacements with homeproperties or componentproperties
            NodeDescription[] homeplacements =
                cadReader.selectNodes(XPATH_HOMEPLACEMENTS_WITH_PROPERTIES);

            for (int i = 0; i < homeplacements.length; i++) {

                // NodeDescription should have an Element as reference
                Element elt = ElementHelper.narrow(homeplacements[i].reference());

                // get 'id' attribute of <homeplacement> element
                AttrDescription idAttr = elt.get_attribute(XMLHelper.toShortArray("id"));
                String id = XMLHelper.toString(idAttr.value());

                // try to find child of <homeproperties> child
                NodeDescription[] homePropChild =
                    elt.select_nodes(XMLHelper.toShortArray(XPATH_HOMEPROPERTIES_CHILDREN));

                // if a <homeproperties> has been found
                if (homePropChild.length > 0) {
                    // convert first child of <homeproperties> into FileReference
                    // (this must be a <fileinarchive> or a <codebase>)
                    FileReference homeCPF = packImpl.loadFileReference(
                        (ElementDescription) homePropChild[0]);

                    // add homeCPF to assembly
                    ((AssemblyPackageImpl) packImpl).addHomeCPF(id, homeCPF);
                }

                // try to find child of <componentproperties> child
                NodeDescription[] compPropChild =
                    elt.select_nodes(XMLHelper.toShortArray(XPATH_COMPONENTPROPERTIES_CHILDREN));

                // if a <componentproperties> has been found
                if (compPropChild.length > 0) {
                    // convert first child of <componentproperties> into FileReference
                    // (this must be a <fileinarchive> or a <codebase>)
                    FileReference compCPF = packImpl.loadFileReference(
                        (ElementDescription) compPropChild[0]);

                    // add compCPF to assembly
                    ((AssemblyPackageImpl) packImpl).addComponentCPF(id, compCPF);
                }
            }


            // find all <componentinstantiation> which have a <componentproperties> child
            NodeDescription[] compInstances =
                cadReader.selectNodes(XPATH_COMPONENT_INSTANTIATIONS_WITH_PROPERTIES);

            for (int i = 0; i < compInstances.length; i++) {

                // NodeDescription should have an Element as reference
                Element elt = ElementHelper.narrow(compInstances[i].reference());

                // get 'id' attribute of <componentinstantiation> element
                AttrDescription idAttr = elt.get_attribute(XMLHelper.toShortArray("id"));
                String id = XMLHelper.toString(idAttr.value());

                // find child of <componentproperties> child
                NodeDescription[] compPropChild =
                    elt.select_nodes(XMLHelper.toShortArray(XPATH_COMPONENTPROPERTIES_CHILDREN));

                // if a <componentproperties> has been found (should always be true)
                if (compPropChild.length > 0) {
                    // convert first child of <componentproperties> into FileReference
                    // (this must be a <fileinarchive> or a <codebase>)
                    FileReference compCPF = packImpl.loadFileReference(
                        (ElementDescription) compPropChild[0]);

                    // add compCPF to assembly
                    ((AssemblyPackageImpl) packImpl).addInstantiationCPF(id, compCPF);
                }
            }

            // find all ftcomponentgroup with groupproperties
            NodeDescription[] ftcomponentgroups =
                cadReader.selectNodes(XPATH_FTCOMPONENTGROUPS_WITH_PROPERTIES);

            for (int i = 0; i < ftcomponentgroups.length; i++) {

                // NodeDescription should have an Element as reference
                Element elt = ElementHelper.narrow(ftcomponentgroups[i].reference());

                // get 'id' attribute of <ftcomponentgroup> element
                AttrDescription idAttr = elt.get_attribute(XMLHelper.toShortArray("id"));
                String id = XMLHelper.toString(idAttr.value());

                // try to find child of <groupproperties> child
                NodeDescription[] groupPropChild =
                    elt.select_nodes(XMLHelper.toShortArray(XPATH_GROUPPROPERTIES_CHILDREN));

                // if a <groupproperties> has been found
                if (groupPropChild.length > 0) {
                    // convert first child of <groupproperties> into FileReference
                    // (this must be a <fileinarchive> or a <codebase>)
                    FileReference groupCPF = packImpl.loadFileReference(
                        (ElementDescription) groupPropChild[0]);

                    // add groupCPF to assembly
                    ((AssemblyPackageImpl) packImpl).addComponentGroupCPF(id, groupCPF);
                }

                // try to find child of <componentproperties> child
                NodeDescription[] compPropChild =
                    elt.select_nodes(XMLHelper.toShortArray(XPATH_COMPONENTPROPERTIES_CHILDREN));

            }

            // find all lbcomponentgroup with groupproperties
            NodeDescription[] lbcomponentgroups =
                cadReader.selectNodes(XPATH_LBCOMPONENTGROUPS_WITH_PROPERTIES);

            for (int i = 0; i < lbcomponentgroups.length; i++) {

                // NodeDescription should have an Element as reference
                Element elt = ElementHelper.narrow(lbcomponentgroups[i].reference());

                // get 'id' attribute of <lbcomponentgroup> element
                AttrDescription idAttr = elt.get_attribute(XMLHelper.toShortArray("id"));
                String id = XMLHelper.toString(idAttr.value());

                // try to find child of <groupproperties> child
                NodeDescription[] groupPropChild =
                    elt.select_nodes(XMLHelper.toShortArray(XPATH_GROUPPROPERTIES_CHILDREN));

                // if a <groupproperties> has been found
                if (groupPropChild.length > 0) {
                    // convert first child of <groupproperties> into FileReference
                    // (this must be a <fileinarchive> or a <codebase>)
                    FileReference groupCPF = packImpl.loadFileReference(
                        (ElementDescription) groupPropChild[0]);

                    // add groupCPF to assembly
                    ((AssemblyPackageImpl) packImpl).addComponentGroupCPF(id, groupCPF);
                }
            }

        } catch(XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (DOMException e) {
            throw new InstallationFailure(e.reason,
                                          ERROR_IN_CAD.value);
        } catch (FileAlreadyReferenced e) {
             throw new InstallationFailure(e.getMessage(),
                                           ERROR_IN_CAD.value);
        } catch (XMLPersistable.LoadException e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CAD.value);
        }
    }


    public  void install(java.io.File installDir,
                         java.io.File componentDir,
                         java.io.File codebaseDir)
        throws InstallationFailure
    {
        String uuid = ((AssemblyPackageImpl) packImpl).get_uuid();
        System.out.println(" install files of assembly package " + uuid);

        // install assembly files
        super.install(installDir, codebaseDir);

        java.util.Vector installDirs = new java.util.Vector();
        try {
            FileUtilities.checkDirectory(componentDir);

            // install each needed component package
            for(int i = 0; i < componentInstallers.size(); i++) {
                ComponentPackageInstaller compInstaller =
                    (ComponentPackageInstaller) componentInstallers.get(i);

                String name = compInstaller.getComponentPackageImpl().get_name();
                String version = compInstaller.getComponentPackageImpl().get_version();

                java.io.File compInstallDir = new java.io.File(componentDir, name + version);
                FileUtilities.checkDirectory(compInstallDir);
                installDirs.add(compInstallDir);

                // install component files
                compInstaller.install(compInstallDir, codebaseDir);
            }

        } catch (InstallationFailure e) {
            // remove all component directories
            java.util.Iterator it = installDirs.iterator();
            while (it.hasNext()) {
                java.io.File compDir = (java.io.File) it.next();
                try {
                    FileUtilities.removeDirectoryRecursively(compDir);
                } catch (java.io.IOException ioe) {
                    // ignore
                }
            }
            throw e;

        } catch (java.io.IOException e) {
            // remove all component directories
            java.util.Iterator it = installDirs.iterator();
            while (it.hasNext()) {
                java.io.File compDir = (java.io.File) it.next();
                try {
                    FileUtilities.removeDirectoryRecursively(compDir);
                } catch (java.io.IOException ioe) {
                    // ignore
                }
            }
            throw new InstallationFailure(e.getMessage(),
                                          READ_WRITE_ERROR.value);
        }
    }



    public void remove() {
        // remove all ComponentPackageInstallers
        for(int i = 0; i < componentInstallers.size(); i++) {
            ComponentPackageInstaller compInstaller =
                (ComponentPackageInstaller) componentInstallers.get(i);
            compInstaller.remove();
        }
        // remove this AssemblyPackageInstaller
        super.remove();
    }

}


