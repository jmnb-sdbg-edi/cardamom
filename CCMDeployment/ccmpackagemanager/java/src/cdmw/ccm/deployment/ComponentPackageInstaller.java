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

import org.omg.Components.Deployment.InstallationFailure;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CSD;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CCD;
import com.thalesgroup.CdmwDeployment.COMPONENT_ALREADY_INSTALLED;

import cdmw.ccm.deployment.CsdReader;
import cdmw.ccm.deployment.CcdReader;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;


class ComponentPackageInstaller extends PackageInstaller {


    private static final String XPATH_ALL_DESCRIPTORS =
        "//descriptor";
    private static final String XPATH_HOMEPROPERTIES_CHILDREN =
        "/corbacomponent/homeproperties/*";
    private static final String XPATH_COMPONENTPROPERTIES_CHILDREN =
        "/corbacomponent/componentproperties/*";
    private static final String XPATH_ALL_IMPLEMENTATIONS =
        "/softpkg/implementation";
    private static final String XPATH_CODE_FILEREFERENCE =
        "code/*[self::codebase or self::fileinarchive or self::link]";
    private static final String XPATH_IDL_FILEREFERENCES =
        "/softpkg/idl/*[self::link or self::fileinarchive]";
    private static final String XPATH_DEPENDENCY_FILEREFERENCES =
        "//dependency//*[self::codebase or self::fileinarchive or self::link]";
    private static final String XPATH_PROPERTY_FILEREFERENCES =
        "/softpkg/propertyfile/*[self::link or self::fileinarchive]";


    private static final String CORBA_COMPONENT_TYPE = "CORBA Component";


    private FileReference csdRef;


    public ComponentPackageInstaller(FileReference csdRef,
                                     java.io.File tmpDir,
                                     XMLParser parser)
        throws InstallationFailure
    {
        super(tmpDir, parser);

        this.csdRef = csdRef;
    }


    public ComponentPackageImpl getComponentPackageImpl() {
        // check if analyze() has been called
        cdmw.common.Assert.check(packImpl != null);

        return (ComponentPackageImpl) packImpl;
    }


    // analyze a csd file and construct a ComponentPackageImpl object
    public void analyze()
        throws InstallationFailure
    {
        System.out.println(" analyze component package");

        // if csd is not installed, install it
        if (!csdRef.isInstalled()) {
            csdRef.installInto(tmpDir);
        }

        System.out.println("   read " + csdRef.getActualBasename());

        // create CsdReader
        CsdReader csdReader;
        try {
            csdReader = new CsdReader(parser, csdRef.getActualLocation());
        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            throw new InstallationFailure(e.reason,
                                          ERROR_IN_CSD.value);
        }

        // read name and version from .csd
        String name = csdReader.getName();
        String version = csdReader.getVersion();

        // check if component is not already installed
        if (PackageManagerImpl.getInstance()
                              .containsComponentPackage(name, version))
        {
            throw new InstallationFailure(
                "Component " + name +
                (version.equals("") ? "" : " "+version) +
                " is already installed.",
                COMPONENT_ALREADY_INSTALLED.value);
        }

        // retrieve mainArchive (if exists)
        Archive mainArchive = null;
        if (csdRef instanceof FileInArchive) {
            mainArchive = ((FileInArchive) csdRef).getArchive();
        }

        // create a new component package
        packImpl = new ComponentPackageImpl(csdReader.getName(),
                                            csdReader.getVersion(),
                                            csdRef,
                                            mainArchive);

        // complete component with descriptors, including .ccd which will
        // be installed into installDir
        completeComponentWithDescriptors(csdReader);

        // install .ccd if not done
        FileReference ccd = ((ComponentPackageImpl) packImpl).getCCD();
        if (!ccd.isInstalled()) {
            ccd.installInto(tmpDir);
        }

        System.out.println("   read " + ccd.getActualBasename());
        // create CcdReader
        CcdReader ccdReader;
        try {
            ccdReader = new CcdReader(parser, ccd.getActualLocation());
        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            throw new InstallationFailure(e.reason,
                                          ERROR_IN_CCD.value);
        }

        // complete component with default homeproperties and componentproperties
        completeComponentWithDefaultProperties(ccdReader);
        ccdReader.close();

        // complete component with codes
        completeComponentWithCodes(csdReader);

        // complete component with idl files
        completeComponentWithIdlFiles(csdReader);

        // complete component with dependency files
        completeComponentWithDependencyFiles(csdReader);

        // complete component with property files
        completeComponentWithPropertyFiles(csdReader);

        csdReader.close();

        isAnalyzeDone = true;
    }


    private void completeComponentWithDescriptors(CsdReader csdReader)
        throws InstallationFailure
    {
        try {
            // get all <descriptor> elements
            NodeDescription[] descriptors = csdReader.selectNodes(XPATH_ALL_DESCRIPTORS);

            for (int i = 0; i < descriptors.length; i++) {

                // NodeDescription should have an Element as reference
                Element elt = ElementHelper.narrow(descriptors[i].reference());

                // get type attribute of <descriptor> element
                AttrDescription typeAttr = elt.get_attribute(XMLHelper.toShortArray("type"));
                String type = XMLHelper.toString(typeAttr.value());

                // get child of descriptor (must be <link> or <fileinarchive>)
                ElementDescription[] children = elt.get_child_elements();

                // get descriptor file reference
                FileReference desc = packImpl.loadFileReference(children[0]);

                // test descriptor has 'CORBA Component' type
                if (type.equals(CORBA_COMPONENT_TYPE)) {
                    // this descriptor is the ccd
                    ((ComponentPackageImpl) packImpl).setCCD(desc);
                }
            }

        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (DOMException e) {
            throw new InstallationFailure(e.reason,
                                          ERROR_IN_CSD.value);
        } catch (FileAlreadyReferenced e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        } catch (XMLPersistable.LoadException e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        }
    }


    private void completeComponentWithDefaultProperties(CcdReader ccdReader)
        throws InstallationFailure
    {
        try {
            // retrieve the child element of <homeproperties> element (if exists)
            NodeDescription[] homePropChildren =
                ccdReader.selectNodes(XPATH_HOMEPROPERTIES_CHILDREN);

            if (homePropChildren.length > 0) {
                // get homeCPF FileReference
                FileReference homeCPF = packImpl.loadFileReference(
                    (ElementDescription) homePropChildren[0]);

                // set homeCPF
                ((ComponentPackageImpl) packImpl).setHomeCPF(homeCPF);
            }

            // retrieve the child element of <componentproperties> element (if exists)
            NodeDescription[] compPropChildren =
                ccdReader.selectNodes(XPATH_COMPONENTPROPERTIES_CHILDREN);

            if (compPropChildren.length > 0) {
                // get componentCPF FileReference
                FileReference compCPF = packImpl.loadFileReference(
                    (ElementDescription) compPropChildren[0]);

                // set componentCPF
                ((ComponentPackageImpl) packImpl).setComponentCPF(compCPF);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (FileAlreadyReferenced e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CCD.value);
        } catch (XMLPersistable.LoadException e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CCD.value);
        }
    }



    private void completeComponentWithCodes(CsdReader csdReader)
        throws InstallationFailure
    {
        try {
            // get all <implementation> elements
            NodeDescription[] implNodes =
                csdReader.selectNodes(XPATH_ALL_IMPLEMENTATIONS);

            for (int i = 0; i < implNodes.length; i++) {
                Element implElt = ElementHelper.narrow(implNodes[i].reference());

                // get uuid of implementation
                AttrDescription idAttr = implElt.get_attribute(XMLHelper.toShortArray("id"));
                String uuid = XMLHelper.toString(idAttr.value());

                // check if uuid is already in use
                if (PackageManagerImpl.getInstance().isUuidAlreadyDefined(uuid)) {
                    throw new InstallationFailure(
                        "UUID " + uuid +
                        " is already used for an implementation of a component.",
                        ERROR_IN_CSD.value);
                }

                // get file reference child of <code> element
                // (should be a <codebase>, a <link> or a <fileinarchive>)
                NodeDescription[] codeChildren =
                    implElt.select_nodes(XMLHelper.toShortArray(XPATH_CODE_FILEREFERENCE));
                if (codeChildren.length == 0) {
                    throw new InstallationFailure(
                        "No <codebase>, <fileinarchive> or <link> child " +
                        " to <code> element in .csd file.",
                        ERROR_IN_CSD.value);
                }

                FileReference code = packImpl.loadFileReference(
                    (ElementDescription) codeChildren[0]);

                // add code to component
                ((ComponentPackageImpl) packImpl).addCode(uuid, code);
            }

        } catch(XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (DOMException e) {
            throw new InstallationFailure(e.reason,
                                          ERROR_IN_CSD.value);
        } catch (FileAlreadyReferenced e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        } catch (XMLPersistable.LoadException e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        }
    }



    private void completeComponentWithIdlFiles(CsdReader csdReader)
        throws InstallationFailure
    {
        try {
            // get file reference children of all <idl> elements
            NodeDescription[] idlFileNodes =
                csdReader.selectNodes(XPATH_IDL_FILEREFERENCES);

            for (int i = 0; i < idlFileNodes.length; i++ ) {
                FileReference idl = packImpl.loadFileReference(
                    (ElementDescription) idlFileNodes[i]);
                packImpl.addFileReference(idl);
            }

        } catch(XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (FileAlreadyReferenced e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        } catch (XMLPersistable.LoadException e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        }
    }

    private void completeComponentWithDependencyFiles(CsdReader csdReader)
        throws InstallationFailure
    {
        try {
            // get file reference children of all <dependency> elements
            NodeDescription[] dependencyFileNodes =
                csdReader.selectNodes(XPATH_DEPENDENCY_FILEREFERENCES);

            for (int i = 0; i < dependencyFileNodes.length; i++ ) {
                FileReference dependency = packImpl.loadFileReference(
                    (ElementDescription) dependencyFileNodes[i]);
                packImpl.addFileReference(dependency);
            }

        } catch(XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (FileAlreadyReferenced e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        } catch (XMLPersistable.LoadException e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        }
    }


    private void completeComponentWithPropertyFiles(CsdReader csdReader)
        throws InstallationFailure
    {
        try {
            // get file reference children of all <propertyfile> elements
            NodeDescription[] propFileNodes =
                csdReader.selectNodes(XPATH_PROPERTY_FILEREFERENCES);

            for (int i = 0; i < propFileNodes.length; i++ ) {
                FileReference property = packImpl.loadFileReference(
                    (ElementDescription) propFileNodes[i]);
                packImpl.addFileReference(property);
            }

        } catch(XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        } catch (FileAlreadyReferenced e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        } catch (XMLPersistable.LoadException e) {
            throw new InstallationFailure(e.getMessage(),
                                          ERROR_IN_CSD.value);
        }
    }



    public void install(java.io.File installDir,
                        java.io.File codebaseDir)
        throws InstallationFailure
    {
        String name = getComponentPackageImpl().get_name();
        String version = getComponentPackageImpl().get_version();

        System.out.println(" install files of component package " +
                           name + (version != null ? " "+version : ""));

        super.install(installDir, codebaseDir);
    }


}


