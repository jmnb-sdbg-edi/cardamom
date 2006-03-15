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

import com.thalesgroup.CdmwDeployment.FileNotReferenced;
import com.thalesgroup.CdmwDeployment.LocationCreationException;
import com.thalesgroup.CdmwDeployment._ComponentPackage;

import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.XPathFormatException;



class AssemblyPackageImpl extends PackageImpl
    implements com.thalesgroup.CdmwDeployment._AssemblyPackageOperations,
               XMLPersistable
{


    private static final String SAVE_NAME_FOR_UUID              = "uuid";
    private static final String SAVE_NAME_FOR_CAD               = "cad";
    private static final String SAVE_NAME_FOR_COMPONENT_PACKAGE = "component";
    private static final String SAVE_NAME_FOR_COMP_FILE_ID      = "componentfile_id";
    private static final String SAVE_NAME_FOR_COMP_NAME         = "componentname";
    private static final String SAVE_NAME_FOR_COMP_VERSION      = "componentversion";
    private static final String SAVE_NAME_FOR_HOME_CPF          = "homeCPF";
    private static final String SAVE_NAME_FOR_HOME_PLAC_ID      = "homeplacement_id";
    private static final String SAVE_NAME_FOR_COMP_CPF          = "componentCPF";
    private static final String SAVE_NAME_FOR_COMP_INST_CPF     = "componentInstanceCPF";
    private static final String SAVE_NAME_FOR_COMP_INST_ID      = "componentinstantiation_id";
    private static final String SAVE_NAME_FOR_COMP_GROUP_CPF     = "componentGroupCPF";
    private static final String SAVE_NAME_FOR_COMP_GROUP_ID      = "componentgroup_id";

    
    private String uuid;

    private FileReference cad;

    private java.util.Hashtable componentPackages;

    private java.util.Hashtable homeCPFs;
    private java.util.Hashtable componentCPFs;
    private java.util.Hashtable componentInstanceCPFs;
    private java.util.Hashtable componentGroupCPFs;




    public AssemblyPackageImpl(String uuid, FileReference cad, Archive mainArchive) {
        super(mainArchive);
        this.uuid = uuid;
        this.componentPackages = new java.util.Hashtable();
        this.homeCPFs = new java.util.Hashtable();
        this.componentCPFs = new java.util.Hashtable();
        this.componentInstanceCPFs = new java.util.Hashtable();
        this.componentGroupCPFs = new java.util.Hashtable();

        try {
            addFileReference(cad);
            this.cad = cad;
        } catch (FileAlreadyReferenced e) {
             // Should not happen, package newly created
             cdmw.common.Assert.check(false);
        }
    }


    AssemblyPackageImpl() {
        super(null);
        uuid = null;
        componentPackages = new java.util.Hashtable();
        homeCPFs = new java.util.Hashtable();
        componentCPFs = new java.util.Hashtable();
        componentInstanceCPFs = new java.util.Hashtable();
        componentGroupCPFs = new java.util.Hashtable();
        cad = null;
    }

    java.util.Iterator getComponentPackagesIterator() {
        return componentPackages.values().iterator();
    }
    

    //
    // IDL:thalesgroup.com/CdmwDeployment/AssemblyPackage/get_uuid:1.0
    //
    /***/
    public String get_uuid() {
        return uuid;
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/AssemblyPackage/get_CAD_location:1.0
    //
    /***/
    public String get_CAD_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               LocationCreationException
    {
        String location = cad.getActualLocation();
        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/AssemblyPackage/get_component_package:1.0
    //
    /***/
    public _ComponentPackage get_component_package(String componentfile_id)
        throws FileNotReferenced
    {
        _ComponentPackage compPack =
            (_ComponentPackage) componentPackages.get(componentfile_id);

        if (compPack == null) {
            throw new FileNotReferenced(componentfile_id +
                " is not a referenced component package in assembly " + uuid + ".");
        }

        return compPack;
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/AssemblyPackage/get_home_CPF_location:1.0
    //
    /***/
    public String get_home_CPF_location(String homeplacement_id)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        FileReference cpf = (FileReference) homeCPFs.get(homeplacement_id);
        if (cpf == null) {
            throw new FileNotReferenced("homeplacement " + homeplacement_id +
                " has no home properties referenced.");
        }
        String location = cpf.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/AssemblyPackage/get_component_CPF_location:1.0
    //
    /***/
    public String get_component_CPF_location(String homeplacement_id)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        FileReference cpf = (FileReference) componentCPFs.get(homeplacement_id);
        if (cpf == null) {
            throw new FileNotReferenced("homeplacement " + homeplacement_id +
                " has no component properties referenced.");
        }
        String location = cpf.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }


    //
    // IDL:thalesgroup.com/CdmwDeployment/AssemblyPackage/get_instantiation_CPF_location:1.0
    //
    /***/
    public String get_instantiation_CPF_location(String componentinstantiation_id)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        FileReference cpf = (FileReference) componentInstanceCPFs.get(componentinstantiation_id);
        if (cpf == null) {
            throw new FileNotReferenced("componentinstantiation " + componentinstantiation_id +
                " has no component properties referenced.");
        }
        String location = cpf.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/AssemblyPackage/get_componentgroup_CPF_location:1.0
    //
    /***/
    public String get_componentgroup_CPF_location(String componentgroup_id)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        FileReference cpf = (FileReference) componentGroupCPFs.get(componentgroup_id);
        if (cpf == null) {
            throw new FileNotReferenced("componentgroup " + componentgroup_id +
                " has no component properties referenced.");
        }
        String location = cpf.getActualLocation();

        return PackageManagerImpl.getInstance()
                                 .getLocationFactory()
                                 .filenameToLocation(location);
    }




    void addComponentPackage(String componentfile_id, _ComponentPackage compPack)
        throws FileAlreadyReferenced
    {
        if (componentPackages.containsKey(componentfile_id)) {
            throw new FileAlreadyReferenced(componentfile_id +
                " already references a component package in this assembly package.");
        }

        componentPackages.put(componentfile_id, compPack);
    }


    void addHomeCPF(String homeplacement_id, FileReference homeCPF)
        throws FileAlreadyReferenced
    {
        if (homeCPFs.containsKey(homeplacement_id)) {
            throw new FileAlreadyReferenced("The " + homeplacement_id +
                " homeplacement has already homeproperties.");
        }

        addFileReference(homeCPF);
        homeCPFs.put(homeplacement_id, homeCPF);
    }


    void addComponentCPF(String homeplacement_id, FileReference compCPF)
        throws FileAlreadyReferenced
    {
        if (componentCPFs.containsKey(homeplacement_id)) {
            throw new FileAlreadyReferenced("The " + homeplacement_id +
                " homeplacement has already componentproperties.");
        }

        addFileReference(compCPF);
        componentCPFs.put(homeplacement_id, compCPF);
    }


    void addInstantiationCPF(String componentinstantiation_id, FileReference compCPF)
        throws FileAlreadyReferenced
    {
        if (componentInstanceCPFs.containsKey(componentinstantiation_id)) {
            throw new FileAlreadyReferenced("The " + componentinstantiation_id +
                " componentinstantiation has already componentproperties.");
        }

        addFileReference(compCPF);
        componentInstanceCPFs.put(componentinstantiation_id, compCPF);
    }

    void addComponentGroupCPF(String componentgroup_id, FileReference compCPF)
        throws FileAlreadyReferenced
    {
        if (componentGroupCPFs.containsKey(componentgroup_id)) {
            throw new FileAlreadyReferenced("The " + componentgroup_id +
                " componentgroup has already componentproperties.");
        }

        addFileReference(compCPF);
        componentGroupCPFs.put(componentgroup_id, compCPF);
    }


    boolean needsComponent(String name, String version) {
        java.util.Iterator it = componentPackages.values().iterator();
        while (it.hasNext()) {
            _ComponentPackage compPack = (_ComponentPackage) it.next();
            if (compPack.get_name().equals(name) &&
                compPack.get_version().equals(version))
            {
                return true;
            }
        }
        
        return false;        
    }
    


    public void save(com.thalesgroup.CdmwXML.Document doc,
                     com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.SaveException
    {
        try {
            // save uuid attribute
            elt.set_attribute(XMLHelper.toShortArray(SAVE_NAME_FOR_UUID),
                              XMLHelper.toShortArray(uuid));

            // save PackageImpl attributes
            super.save(doc, elt);

            // save cad
            Element cadElt = doc.create_element(
                XMLHelper.toShortArray(SAVE_NAME_FOR_CAD));
            saveFileReference(cad, doc, cadElt);
            elt.append_child(cadElt);

            // save componentPackages
            java.util.Iterator it = componentPackages.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                String compfile_id = (String) entry.getKey();
                _ComponentPackage c = (_ComponentPackage) entry.getValue();
                // create element for component
                Element compElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMPONENT_PACKAGE));
                // save componentfile_id
                compElt.set_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_FILE_ID),
                    XMLHelper.toShortArray(compfile_id));
                // save component's name
                compElt.set_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_NAME),
                    XMLHelper.toShortArray(c.get_name()));
                // save component's version
                String version = c.get_version();
                if (!version.equals("")) {
                    compElt.set_attribute(
                        XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_VERSION),
                        XMLHelper.toShortArray(version));
                }
                elt.append_child(compElt);
            }

            // save homeCPFs;
            it = homeCPFs.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                String homeplac_id = (String) entry.getKey();
                FileReference f = (FileReference) entry.getValue();
                // create element for homeCPF
                Element homeCpfElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_HOME_CPF));
                // save homeplacement_id
                homeCpfElt.set_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_HOME_PLAC_ID),
                    XMLHelper.toShortArray(homeplac_id));
                // save homeCPF
                saveFileReference(f, doc, homeCpfElt);
                elt.append_child(homeCpfElt);
            }

            // save componentCPFs;
            it = componentCPFs.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                String homeplac_id = (String) entry.getKey();
                FileReference f = (FileReference) entry.getValue();
                // create element for componentCPF
                Element compCpfElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_CPF));
                // save homeplacement_id
                compCpfElt.set_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_HOME_PLAC_ID),
                    XMLHelper.toShortArray(homeplac_id));
                // save componentCPF
                saveFileReference(f, doc, compCpfElt);
                elt.append_child(compCpfElt);
            }

            // save componentInstanceCPFs;
            it = componentInstanceCPFs.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                String compinst_id = (String) entry.getKey();
                FileReference f = (FileReference) entry.getValue();
                // create element for componentInstanceCPF
                Element compInstCpfElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_INST_CPF));
                // save compinst_id
                compInstCpfElt.set_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_INST_ID),
                    XMLHelper.toShortArray(compinst_id));
                // save componentCPF
                saveFileReference(f, doc, compInstCpfElt);
                elt.append_child(compInstCpfElt);
            }

            // save componentGroupCPFs;
            it = componentGroupCPFs.entrySet().iterator();
            while (it.hasNext()) {
                java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
                String compgroup_id = (String) entry.getKey();
                FileReference f = (FileReference) entry.getValue();
                // create element for componentGroupCPF
                Element compGroupCpfElt = doc.create_element(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_GROUP_CPF));
                // save compgroup_id
                compGroupCpfElt.set_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_GROUP_ID),
                    XMLHelper.toShortArray(compgroup_id));
                // save componentCPF
                saveFileReference(f, doc, compGroupCpfElt);
                elt.append_child(compGroupCpfElt);
            }

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.SaveException(
                "Error while saving AssemblyPackage " + uuid + " :\n" +
                e.toString());
        }
    }





    public void load(com.thalesgroup.CdmwXML.Element elt)
        throws XMLPersistable.LoadException
    {
        System.out.print("  loading AssemblyPackage ");
        System.out.flush();

        try {
            // load uuid
            AttrDescription uuidAttrDesc = elt.get_attribute(
                XMLHelper.toShortArray(SAVE_NAME_FOR_UUID));
            uuid = XMLHelper.toString(uuidAttrDesc.value());

            System.out.println(uuid + "...");

            // load PackageImpl attributes
            super.load(elt);

            // load cad
            NodeDescription[] descriptors = null;
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_CAD + "/*"));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            if (descriptors.length < 1) {
                throw new XMLPersistable.LoadException(
                    "Cannot find an element <" + SAVE_NAME_FOR_CAD +
                    "> while loading AssemblyPackage " + uuid);
            }
            cad = loadFileReference((ElementDescription) descriptors[0]);
            try {
                addFileReference(cad);
            } catch (FileAlreadyReferenced e) {
                throw new XMLPersistable.LoadException(
                    "Error while loading AssemblyPackageImpl " + uuid + " :\n" +
                    e.getMessage());
            }

            // load component packages
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMPONENT_PACKAGE));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            for (int i = 0; i < descriptors.length; i++) {
                Element compElt = ElementHelper.narrow(descriptors[i].reference());
                AttrDescription compIdDesc = compElt.get_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_FILE_ID));
                String compId = XMLHelper.toString(compIdDesc.value());
                AttrDescription compNameDesc = compElt.get_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_NAME));
                String compName = XMLHelper.toString(compNameDesc.value());
                String compVer = "";
                try {
                    AttrDescription compVerDesc = compElt.get_attribute(
                        XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_VERSION));
                    compVer = XMLHelper.toString(compVerDesc.value());
                } catch (com.thalesgroup.CdmwXML.DOMException e) {
                    // No version defined. Ignore exception
                }
                try {
                    _ComponentPackage pack =
                        PackageManagerImpl.getInstance()
                                          .get_component_package(compName, compVer);
                    addComponentPackage(compId, pack);
                } catch (com.thalesgroup.CdmwDeployment.PackageNotFound e) {
                    throw new XMLPersistable.LoadException(
                        "Error while loading AssemblyPackageImpl " + uuid + " :\n" +
                        "ComponentPackage " + compName +
                        (compVer.equals("") ? "" : " "+compVer) +
                        " was not previously declared.");
                } catch (FileAlreadyReferenced e) {
                    throw new XMLPersistable.LoadException(
                        "Error while loading AssemblyPackageImpl " + uuid + " :\n" +
                        "ComponentPackage " + compName +
                        (compVer.equals("") ? "" : " "+compVer) +
                        " is alredy declared for this assembly package.");
                }
            }

            // load homeCPFs
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_HOME_CPF));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            for (int i = 0; i < descriptors.length; i++) {
                Element cpfElt = ElementHelper.narrow(descriptors[i].reference());

                // load homeplacement_id
                AttrDescription homePlaceIdDesc = cpfElt.get_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_HOME_PLAC_ID));
                String homePlacId = XMLHelper.toString(homePlaceIdDesc.value());

                // load homeCPF FileReference
                ElementDescription[] fileDescr = cpfElt.get_child_elements();
                if (fileDescr.length == 0) {
                    throw new XMLPersistable.LoadException(
                        "No child element to <" + SAVE_NAME_FOR_HOME_CPF + " " +
                        SAVE_NAME_FOR_HOME_PLAC_ID + "=\"" + homePlacId + "\">.");
                }
                try {
                    addHomeCPF(homePlacId, loadFileReference(fileDescr[0]));
                } catch (FileAlreadyReferenced e) {
                    throw new XMLPersistable.LoadException(
                        "Error while loading AssemblyPackageImpl " + uuid + " :\n" +
                        e.getMessage());
                }
            }

            // load componentCPFs;
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_CPF));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            for (int i = 0; i < descriptors.length; i++) {
                Element cpfElt = ElementHelper.narrow(descriptors[i].reference());

                // load homeplacement_id
                AttrDescription homePlaceIdDesc = cpfElt.get_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_HOME_PLAC_ID));
                String homePlacId = XMLHelper.toString(homePlaceIdDesc.value());

                // load componentCPF FileReference
                ElementDescription[] fileDescr = cpfElt.get_child_elements();
                if (fileDescr.length == 0) {
                    throw new XMLPersistable.LoadException(
                        "No child element to <" + SAVE_NAME_FOR_COMP_CPF + " " +
                        SAVE_NAME_FOR_HOME_PLAC_ID + "=\"" + homePlacId + "\">.");
                }
                try {
                    addComponentCPF(homePlacId, loadFileReference(fileDescr[0]));
                } catch (FileAlreadyReferenced e) {
                    throw new XMLPersistable.LoadException(
                        "Error while loading AssemblyPackageImpl " + uuid + " :\n" +
                        e.getMessage());
                }
            }

            // load componentInstanceCPFs
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_INST_CPF));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            for (int i = 0; i < descriptors.length; i++) {
                Element cpfElt = ElementHelper.narrow(descriptors[i].reference());

                // load componentinstantiation_id
                AttrDescription compInstIdDesc = cpfElt.get_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_INST_ID));
                String compInstId = XMLHelper.toString(compInstIdDesc.value());

                // load componentCPF FileReference
                ElementDescription[] fileDescr = cpfElt.get_child_elements();
                if (fileDescr.length == 0) {
                    throw new XMLPersistable.LoadException(
                        "No child element to <" + SAVE_NAME_FOR_COMP_INST_CPF + " " +
                        SAVE_NAME_FOR_COMP_INST_ID + "=\"" + compInstId + "\">.");
                }
                try {
                    addInstantiationCPF(compInstId, loadFileReference(fileDescr[0]));
                } catch (FileAlreadyReferenced e) {
                    throw new XMLPersistable.LoadException(
                        "Error while loading AssemblyPackageImpl " + uuid + " :\n" +
                        e.getMessage());
                }
            }

            // load componentGroupCPFs
            try {
                descriptors = elt.select_nodes(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_GROUP_CPF));
            } catch (XPathFormatException e) {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
            for (int i = 0; i < descriptors.length; i++) {
                Element cpfElt = ElementHelper.narrow(descriptors[i].reference());

                // load componentgroup_id
                AttrDescription compGroupIdDesc = cpfElt.get_attribute(
                    XMLHelper.toShortArray(SAVE_NAME_FOR_COMP_GROUP_ID));
                String compGroupId = XMLHelper.toString(compGroupIdDesc.value());

                // load componentCPF FileReference
                ElementDescription[] fileDescr = cpfElt.get_child_elements();
                if (fileDescr.length == 0) {
                    throw new XMLPersistable.LoadException(
                        "No child element to <" + SAVE_NAME_FOR_COMP_GROUP_CPF + " " +
                        SAVE_NAME_FOR_COMP_GROUP_ID + "=\"" + compGroupId + "\">.");
                }
                try {
                    addComponentGroupCPF(compGroupId, loadFileReference(fileDescr[0]));
                } catch (FileAlreadyReferenced e) {
                    throw new XMLPersistable.LoadException(
                        "Error while loading AssemblyPackageImpl " + uuid + " :\n" +
                        e.getMessage());
                }
            }

        } catch (com.thalesgroup.CdmwXML.DOMException e) {
            throw new XMLPersistable.LoadException(
                "Error while loading AssemblyPackageImpl " + uuid + " :\n" +
                e.toString());
        }
    }
}

