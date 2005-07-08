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


import com.thalesgroup.CdmwDeployment.FileNotReferenced;
import com.thalesgroup.CdmwDeployment.LocationCreationException;

import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.XPathFormatException;


class ComponentPackageImpl extends PackageImpl
    implements com.thalesgroup.CdmwDeployment._ComponentPackageOperations
{

    private static final String SAVE_NAME_FOR_COMP_NAME     = "name";
    private static final String SAVE_NAME_FOR_COMP_VERSION  = "version";
    private static final String SAVE_NAME_FOR_CSD           = "csd";
    private static final String SAVE_NAME_FOR_CCD           = "ccd";
    private static final String SAVE_NAME_FOR_CODE          = "code";
    private static final String SAVE_NAME_FOR_CODE_UUID     = "uuid";
    private static final String SAVE_NAME_FOR_COMPONENT_CPF = "componentCPF";
    private static final String SAVE_NAME_FOR_HOME_CPF      = "homeCPF";


    private String name;
    private String version;

    private FileReference csd;
    private FileReference ccd;
    private java.util.Hashtable codes;
    private FileReference componentCPF;
    private FileReference homeCPF;


    public ComponentPackageImpl(String name,
                                String version,
                                FileReference csd,
                                Archive mainArchive)
    {
        super(mainArchive);
        this.name = name;
        if (version == null) {
            this.version = "";
        } else {
            this.version = version;
        }

        try {
            addFileReference(csd);
            this.csd = csd;
        } catch (FileAlreadyReferenced e) {
             // Should not happen, package newly created
             cdmw.common.Assert.check(false);
        }

        ccd = null;
        codes = new java.util.Hashtable();
        homeCPF = null;
        componentCPF = null;
    }


    ComponentPackageImpl() {
        super(null);
        name = null;
        version = null;
        csd = null;
        ccd = null;
        codes = new java.util.Hashtable();
        homeCPF = null;
        componentCPF = null;
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentPackage/get_name:1.0
    //

    public String get_name() {
        return name;
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentPackage/get_version:1.0
    //
    public String get_version() {
        return version;
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentPackage/get_CSD_location:1.0
    //
    public String get_CSD_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               LocationCreationException
    {
        String location = csd.getActualLocation();
        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentPackage/get_CCD_location:1.0
    //
    public String get_CCD_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        if (ccd == null) {
            throw new FileNotReferenced(
                "There is no .ccd file referenced in this package.");
        }
        String location = ccd.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentPackage/get_code_location:1.0
    //
    public String get_code_location(String implUUID)
        throws org.omg.Components.Deployment.UnknownImplId,
               org.omg.Components.Deployment.InstallationFailure,
               LocationCreationException
    {
        FileReference code = (FileReference) codes.get(implUUID);
        if (code == null) {
            throw new org.omg.Components.Deployment.UnknownImplId(
                implUUID + " doesn't correspond to an implementation in this package");
        }
        String location = code.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentPackage/get_component_CPF_location:1.0
    //
    public String get_component_CPF_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        if (componentCPF == null) {
            throw new FileNotReferenced(
                "There is no default component properties referenced in the package.");
        }
        String location = componentCPF.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentPackage/get_home_CPF_location:1.0
    //
    public String get_home_CPF_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        if (homeCPF == null) {
            throw new FileNotReferenced(
                "There is no default home properties referenced in the package.");
        }
        String location = homeCPF.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }



    void setCCD(FileReference ccd)
        throws FileAlreadyReferenced
    {
        if (this.ccd != null) {
            throw new FileAlreadyReferenced("The component package " +
                                            name + " " + version +
                                            " has already a .ccd file referenced.");
        }

        addFileReference(ccd);
        this.ccd = ccd;
    }

    FileReference getCCD() {
        return ccd;
    }


    void addCode(String implUUID, FileReference code)
        throws FileAlreadyReferenced
    {
        if (containsImplementation(implUUID)) {
            throw new FileAlreadyReferenced("The implementation " + implUUID +
                " is already referenced in this package.");
        }

        addFileReference(code);
        codes.put(implUUID, code);
    }

    void setComponentCPF(FileReference compCPF)
        throws FileAlreadyReferenced
    {
        if (this.componentCPF != null) {
            throw new FileAlreadyReferenced("The component package " +
                                            name + " " + version +
                                            " has already a component .cpf referenced.");
        }

        addFileReference(compCPF);
        this.componentCPF = compCPF;
    }


    void setHomeCPF(FileReference homeCPF)
        throws FileAlreadyReferenced
    {
        if (this.homeCPF != null) {
            throw new FileAlreadyReferenced("The component package " +
                                            name + " " + version +
                                            " has already a home .cpf referenced.");
        }

        addFileReference(homeCPF);
        this.homeCPF = homeCPF;
    }


    boolean containsImplementation(String implUUID) {
        return codes.containsKey(implUUID);
    }



    public void save(com.thalesgroup.CdmwXML.Document doc,
                     com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.SaveException
    {
        try {
            // save name
            elt.set_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_NAME),
                              XMLHelper.toShortArray(name));

            if (!version.equals("")) {
                // save version
                elt.set_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_VERSION),
                                  XMLHelper.toShortArray(version));
            }

            // save package's files
            super.save(doc, elt);

            // save reference to csd
            Element csdElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_CSD));
            saveFileReference(csd, doc, csdElt);
            elt.append_child(csdElt);

            // save reference to ccd
            if (ccd == null) {
                throw new XMLPersistable.SaveException(
                    "ComponentPackage " + name +
                    (version.equals("") ? "" : " " + version) +
                    " doesn't have ccd specified.");
            }
            Element ccdElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_CCD));
            saveFileReference(ccd, doc, ccdElt);
            elt.append_child(ccdElt);

            // save references to codes
            java.util.Iterator it = codes.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                String uuid = (String) entry.getKey();
                FileReference code = (FileReference) entry.getValue();
                FileReference f = (FileReference) entry.getValue();
                Element codeElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_CODE));
                codeElt.set_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_CODE_UUID),
                    XMLHelper.toShortArray(uuid));
                saveFileReference(code, doc, codeElt);
                elt.append_child(codeElt);
            }

            // save reference to componentCPF
            if (componentCPF != null) {
                Element compCpfElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMPONENT_CPF));
                saveFileReference(componentCPF, doc, compCpfElt);
                elt.append_child(compCpfElt);
            }

            // save reference to homeCPF
            if (homeCPF != null) {
                Element homeCpfElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_HOME_CPF));
                saveFileReference(homeCPF, doc, homeCpfElt);
                elt.append_child(homeCpfElt);
            }

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.SaveException(
                "Error while saving ComponentPackageImpl " +
                name + (version.equals("") ? "" : " " + version) +
                " :\n" + e.toString());
        }
    }


    public void load(com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.LoadException
    {
        System.out.print("  loading Component Package ");

        try {

            // load name
            AttrDescription nameAttrDesc = elt.get_attribute(
                XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_NAME));
            name = XMLHelper.toString(nameAttrDesc.value());

            // load version
            try {
                AttrDescription verAttrDesc = elt.get_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_VERSION));
                version = XMLHelper.toString(verAttrDesc.value());
            } catch (com.thalesgroup.CdmwXML.DOMException e) {
                // No version defined. Ignore exception
                version = "";
            }

            System.out.println(name +
                               (version.equals("") ? "" : " " + version) +
                               "...");

            // load PackageImpl attributes
            super.load(elt);

            // load csd
            NodeDescription[] descriptors = null;
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_CSD + "/*"));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            if (descriptors.length == 0) {
                throw new XMLPersistable.LoadException(
                    "Cannot find an element <" + SAVE_NAME_FOR_CSD +
                    "> while loading ComponentPackage " + name +
                    (version.equals("") ? "" : " " + version));
            }
            csd = loadFileReference((ElementDescription) descriptors[0]);
            try {
                addFileReference(csd);
            } catch (FileAlreadyReferenced e) {
                throw new XMLPersistable.LoadException(
                    "Error while loading ComponentPackage " + name +
                    (version.equals("") ? "" : " " + version) + " :\n" +
                    e.getMessage());
            }

            // load ccd
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_CCD + "/*"));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            if (descriptors.length == 0) {
                throw new XMLPersistable.LoadException(
                    "Cannot find an element " + SAVE_NAME_FOR_CCD +
                    "while loading ComponentPackage " + name +
                    (version.equals("") ? "" : " " + version));
            }
            FileReference ccdRef =
                loadFileReference((ElementDescription) descriptors[0]);
            try {
                setCCD(ccdRef);
            } catch (FileAlreadyReferenced e) {
                throw new XMLPersistable.LoadException(
                    "Error while loading ComponentPackage " + name +
                    (version.equals("") ? "" : " " + version) + " :\n" +
                    e.getMessage());
            }

            // load codes
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_CODE));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            for (int i = 0; i < descriptors.length; i++) {
                Element codeElt = ElementHelper.narrow(descriptors[i].reference());

                // load code uuid
                AttrDescription uuidDesc = codeElt.get_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_CODE_UUID));
                String uuid = XMLHelper.toString(uuidDesc.value());

                // check if uuid is already in use
                if (PackageManagerImpl.getInstance().isUuidAlreadyDefined(uuid)) {
                    throw new XMLPersistable.LoadException(
                        "UUID " + uuid +
                        " is already used for an implementation of a component.");
                }

                // load code FileReference
                ElementDescription[] fileDescr = codeElt.get_child_elements();
                if (fileDescr.length == 0) {
                    throw new XMLPersistable.LoadException(
                        "No child element to <" + SAVE_NAME_FOR_CODE + " " +
                        SAVE_NAME_FOR_CODE_UUID + "=\"" + uuid + "\">.");
                }
                try {
                    addCode(uuid, loadFileReference(fileDescr[0]));
                } catch (FileAlreadyReferenced e) {
                    throw new XMLPersistable.LoadException(e.getMessage());
                }
            }

            // load componentCPF
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMPONENT_CPF + "/*"));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            if (descriptors.length > 0) {
                FileReference cpf =
                    loadFileReference((ElementDescription) descriptors[0]);
                try {
                    setComponentCPF(cpf);
                } catch (FileAlreadyReferenced e) {
                    throw new XMLPersistable.LoadException(e.getMessage());
                }
            }

            // load homeCPF
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_HOME_CPF));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            if (descriptors.length > 0) {
                FileReference cpf =
                    loadFileReference((ElementDescription) descriptors[0]);
                try {
                    setHomeCPF(cpf);
                } catch (FileAlreadyReferenced e) {
                    throw new XMLPersistable.LoadException(e.getMessage());
                }
            }

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.LoadException(
                "Error while loading ComponentPackageImpl " +
                name + (version.equals("") ? "" : " " + version) +
                " :\n" + e.toString());
        }
    }


}


