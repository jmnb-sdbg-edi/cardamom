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


package cdmw.ccm.assembly;

import com.thalesgroup.CdmwDeployment._AssemblyPackage;

import org.omg.Components.CreateFailure;
import com.thalesgroup.CdmwDeployment.ASSEMBLY_PACKAGE_NOT_INSTALLED;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.NOT_IMPLEMENTED;

import cdmw.ccm.deployment.CadReader;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;


/**
 * This class creates Assembly objects according to .cad files.
 */
class AssemblyLoader {

    private static final String XPATH_PARTITIONING_CHILDREN =
        "/componentassembly/partitioning/hostcollocation | " +
        "/componentassembly/partitioning/processcollocation | " +
        "/componentassembly/partitioning/executableplacement | " +
        "/componentassembly/partitioning/homeplacement";

    private static final String XPATH_CONNECTIONS_CHILDREN =
        "/componentassembly/connections/connectinterface | " +
        "/componentassembly/connections/connectevent |" +
        "/componentassembly/connections/connecthomes";

    private static final String XPATH_FAULTTOLERANCE_CHILDREN =
        "/componentassembly/faulttolerance/ftcomponentgroup";

    private static final String XPATH_LOADBALANCING_CHILDREN =
        "/componentassembly/loadbalancing/lbcomponentgroup";

    private static final String XPATH_USESPORT_ID =
        "usesport/usesidentifier/text()";

    private static final String XPATH_USESPORT_COMPONENT_REF =
        "usesport/componentinstantiationref | " +
        "usesport/findby";

    private static final String XPATH_OBJ_TO_CONNECT_REF =
        "providesport | componentsupportedinterface | " +
        "existinginterface | homeinterface ";

    private static final String XPATH_CONSUMESPORT_ID =
        "consumesport/consumesidentifier/text()";

    private static final String XPATH_CONSUMESPORT_COMPONENT_REF =
        "consumesport/componentinstantiationref | " +
        "consumesport/findby";

    private static final String XPATH_EVENT_PORT_TO_CONNECT_REF =
        "emitsport | publishesport";

    private static final String XPATH_EMITSPORT_ID =
        "emitsport/emitsidentifier/text()";

    private static final String XPATH_EMITSPORT_COMPONENT_REF =
        "emitsport/componentinstantiationref | " +
        "emitsport/findby";

    private static final String XPATH_PUBLISHESPORT_ID =
        "publishesport/publishesidentifier/text()";

    private static final String XPATH_PUBLISHESPORT_COMPONENT_REF =
        "publishesport/componentinstantiationref | " +
        "publishesport/findby";


    /**
     * POA for Assembly objects.
     */
    private org.omg.PortableServer.POA assembliesPOA;

    /**
     * Loader of hostcollocations' .cad elements.
     */
    private HostCollocationLoader hostCollocationLoader;

    /**
     * Loader of references specified into connections' .cad elements.
     */
    private ObjectRefLoader objectRefLoader;

    /**
     * Loader for faulttolerance/componentgroups' .cad elements.
     * (not null only if Cardamom is compiled with Fault Tolerance)
     */
    private ComponentGroupLoader ftComponentGroupLoader = null;

    /**
     * Loader for loadbalancing/componentgroups' .cad elements.
     * (not null only if Cardamom is compiled with Load Balancing)
     */
    private ComponentGroupLoader lbComponentGroupLoader = null;



    /**
     * Constructor.
     *
     * @param assembliesPOA POA for Assembly objects.
     */
    public AssemblyLoader(org.omg.PortableServer.POA assembliesPOA)
    {
        this.assembliesPOA = assembliesPOA;
        this.hostCollocationLoader = new HostCollocationLoader();
        this.objectRefLoader = new ObjectRefLoader();

        // try to load ftComponentGroupLoader
        try {
            this.ftComponentGroupLoader = (ComponentGroupLoader)
                Class.forName("cdmw.ccm.assembly.FTComponentGroupLoader").newInstance();
        } catch (ClassNotFoundException e) {
            // FT is not available
        } catch (InstantiationException e) {
            // Shouldn't happen since FTComponentGroupLoader is not abstract
            // and has a default constructor
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (IllegalAccessException e) {
            // Shouldn't happen since FTComponentGroupLoader's
            // default constructor is accessible
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }

        // try to load lbComponentGroupLoader
        try {
            this.lbComponentGroupLoader = (ComponentGroupLoader)
                Class.forName("cdmw.ccm.assembly.LBComponentGroupLoader").newInstance();
        } catch (ClassNotFoundException e) {
            // LB is not available
        } catch (InstantiationException e) {
            // Shouldn't happen since LBComponentGroupLoader is not abstract
            // and has a default constructor
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        } catch (IllegalAccessException e) {
            // Shouldn't happen since LBComponentGroupLoader's
            // default constructor is accessible
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }
    }


    /**
     * Loads a .cad file and create an Assembly object.
     *
     * @param cadReader A CadReader reading a .cad file.
     * @return a new AssemblyImpl servant corrsponding to a new Assembly object.
     *
     * @exception CreateFailure if loading fails.
     */
    public AssemblyImpl load(CadReader cadReader)
        throws CreateFailure
    {
        // read assembly uuid
        String uuid = cadReader.getUUID();

        // get AssemblyPackage object from PackageManager
        _AssemblyPackage assemblyPack;
        try {
            assemblyPack = AssemblyFactoryImpl.getInstance()
                                              .getPackageManager()
                                              .get_assembly_package(uuid);
        } catch (com.thalesgroup.CdmwDeployment.PackageNotFound e) {
	     throw new CreateFailure(
                "The assembly package " + uuid +
                " has not been installed by PackageManager",
                ASSEMBLY_PACKAGE_NOT_INSTALLED.value);
        }

        // create empty assemblyImpl
        AssemblyImpl assembly = new AssemblyImpl(assembliesPOA);

        // load <partitioning> element and fill assembly
        loadPartitioning(cadReader, assembly, assemblyPack);

        // load component groups (if specified)
        loadComponentGroups(cadReader, assembly, assemblyPack);

        // load <connections> element and fill assembly
        loadConnections(cadReader, assembly);

        return assembly;
    }


    /**
     * Loads the partitioning element from the .cad file and fill the Assembly.
     *
     * @param cadReader the CadReader reading the .cad file.
     * @param assembly the Assembly to loaded with partitioning informations.
     * @param assemblyPack the AssemblyPackage corrseponding to the .cad file.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadPartitioning(CadReader cadReader,
                                  AssemblyImpl assembly,
                                  _AssemblyPackage assemblyPack)
        throws CreateFailure
    {

        // Get all children of <partitioning> element which are <hostcollocation>,
        // <processcollocation>, <executableplacement> or <homeplacement>.
        // But accept only <hostcollocation> there.
        NodeDescription[] parts = null;
        try {
            parts = cadReader.selectNodes(XPATH_PARTITIONING_CHILDREN);

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        for (int i = 0; i < parts.length; i++) {
            ElementDescription eltDescr = (ElementDescription) parts[i];
            Element elt = ElementHelper.narrow(eltDescr.reference());

            // get name of element
            String eltName = XMLHelper.toString(eltDescr.name());

            if (eltName.equals("hostcollocation")) {
                // load hostcollocation
                hostCollocationLoader.load(elt, assembly, assemblyPack);

            } else if (eltName.equals("processcollocation")) {
                throw new CreateFailure(
                    "Only <hostcollocation> are accepted as child of <partitioning>",
                    ERROR_IN_CAD.value);

            } else if (eltName.equals("executableplacement")) {
                // not yet implemented
                System.err.println("<executableplacement> not implemented");
                throw new CreateFailure(
                    "<executableplacement> not implemented",
                    NOT_IMPLEMENTED.value);

            } else if (eltName.equals("homeplacement")) {
                throw new CreateFailure(
                    "Only <hostcollocation> are accepted as child of <partitioning>",
                    ERROR_IN_CAD.value);


            } else {
                // shouldn't happen, only <hostcollocation>, <processcollocation>,
                // <executableplacement> and <homeplacement> are selected
                cdmw.common.Assert.check(false);
            }

        } // end for
    }



    /**
     * Loads the componentgroup elements from the .cad file and fill the Assembly.
     *
     * @param cadReader the CadReader reading the .cad file.
     * @param assembly the Assembly to loaded with componentgroup informations.
     * @param assemblyPack the AssemblyPackage corrseponding to the .cad file.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadComponentGroups(CadReader cadReader,
                                     AssemblyImpl assembly,
                                     _AssemblyPackage assemblyPack)
        throws CreateFailure
    {
        // get <ftcomponentgroup> children element of <faulttolerance> element
        NodeDescription[] parts = null;
        try {
            parts = cadReader.selectNodes(XPATH_FAULTTOLERANCE_CHILDREN);
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        if (parts.length > 0) {
            if (ftComponentGroupLoader == null) {
                throw new CreateFailure(
                    "<faulttolerance> element is specified in .cad file," +
                    " but Fault Tolerance is not compiled with this version of Cardamom",
                    ERROR_IN_CAD.value);
            }

            for (int i = 0; i < parts.length; i++) {
                ElementDescription eltDescr = (ElementDescription) parts[i];
                Element elt = ElementHelper.narrow(eltDescr.reference());

                // load <ftcomponentgroup> as FTComponentGroup
                ftComponentGroupLoader.load(elt, assembly, assemblyPack);
            }
        }

        // Get all the loadbalancing/lbcomponentgroup nodes.
        parts = null;
        try {
            parts = cadReader.selectNodes(XPATH_LOADBALANCING_CHILDREN);
        } catch (XPathFormatException e) {
            // this should not happen unless there is a typo in the XPath expr.
            cdmw.common.Assert.check(false);
        }

        if (parts.length > 0) {
            if (lbComponentGroupLoader == null) {
                throw new CreateFailure(
                    "A <loadbalancing> element was found in the .CAD file " +
                    "although this configuration of CDMW has no support for " +
                    "Load Balancing.",
                    ERROR_IN_CAD.value);
            }

            for (int i = 0; i < parts.length; i++) {
                ElementDescription eltDescr = (ElementDescription) parts[i];
                Element elt = ElementHelper.narrow(eltDescr.reference());

                // Convert <lbcomponentgroup> into a LBComponentGroup object.
                lbComponentGroupLoader.load(elt, assembly, assemblyPack);
            }
        }
    }



    /**
     * Loads the connections element from the .cad file and fill the Assembly.
     *
     * @param cadReader the CadReader reading the .cad file.
     * @param assembly the Assembly to loaded with connections informations.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadConnections(CadReader cadReader,
                                 AssemblyImpl assembly)
        throws CreateFailure
    {
        // get all children of <connections> element which are
        // <connectinterface>, <connectevent> or <connecthomes>
        NodeDescription[] parts = null;
        try {
            parts = cadReader.selectNodes(XPATH_CONNECTIONS_CHILDREN);

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        for (int i = 0; i < parts.length; i++) {
            ElementDescription eltDescr = (ElementDescription) parts[i];
            Element elt = ElementHelper.narrow(eltDescr.reference());

            // get name of element
            String eltName = XMLHelper.toString(eltDescr.name());

            if (eltName.equals("connectinterface")) {
                // load connectinterface
                loadConnectInterface(elt, assembly, ""+i+1);

                // Use i+1 as defaultId (used if id of connection is not specified)
                // It could not be in conflict with another specified id,
                // because numbers are not valid XML IDs

            } else if (eltName.equals("connectevent")) {
                // load connectinterface
                loadConnectEvent(elt, assembly, ""+i+1);

            } else if (eltName.equals("connecthomes")) {
                throw new CreateFailure(
                    "<connecthomes> not supported.",
                    NOT_IMPLEMENTED.value);

            } else {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }
        }
    }


    /**
     * Loads a connectinterface element from the .cad file and fill the Assembly.
     *
     * @param connInterfaceElt the connectinterface element to be loaded.
     * @param assembly the Assembly to loaded with connectinterface informations.
     * @param defaultId identifier to be used for the Connection if 
     *        connectinterface element has no identifier.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadConnectInterface(Element connInterfaceElt,
                                      AssemblyImpl assembly,
                                      String defaultId)
        throws CreateFailure
    {
        // get id of connection
        String id = defaultId;
        try {
            AttrDescription idDescr = connInterfaceElt.get_attribute(
                XMLHelper.toShortArray("id"));
            id = XMLHelper.toString(idDescr.value());
        } catch (DOMException e) {
            // no id specified
        }

        // get id of <usesport>
        String usesId = null;
        try {
            NodeDescription[] nodes = connInterfaceElt.select_nodes(
                XMLHelper.toShortArray(XPATH_USESPORT_ID));
            TextDescription usesNameDescr = (TextDescription) nodes[0];
            usesId = XMLHelper.toString(usesNameDescr.data());

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // get Component reference for usesport
        ObjectRef usesComponent = null;
        try {
            NodeDescription[] nodes = connInterfaceElt.select_nodes(
                XMLHelper.toShortArray(XPATH_USESPORT_COMPONENT_REF));
            ElementDescription compRefDescr = (ElementDescription) nodes[0];
            Element compRefElt = ElementHelper.narrow(compRefDescr.reference());
            String refType = XMLHelper.toString(compRefDescr.name());

            if (refType.equals("componentinstantiationref")) {
                // load componentinstantiationref
                usesComponent = 
                    objectRefLoader.loadComponentinstantiationref(compRefElt,
                                                                  assembly);
            } else if (refType.equals("findby")) {
                // load findby
                usesComponent = objectRefLoader.loadFindby(compRefElt);
            } else {
                // shouldn't happen (only <componentinstantiationref>
                // or <findby> are valid as child of <usesport>
                cdmw.common.Assert.check(false);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // get reference to object to be connected to usesport
        ObjectRef objToBeConnected = null;
        try {
            NodeDescription[] nodes = connInterfaceElt.select_nodes(
                XMLHelper.toShortArray(XPATH_OBJ_TO_CONNECT_REF));
            ElementDescription refDescr = (ElementDescription) nodes[0];
            Element refElt = ElementHelper.narrow(refDescr.reference());
            String refType = XMLHelper.toString(refDescr.name());

            if (refType.equals("providesport")) {
                // load providesport
                objToBeConnected = 
                    objectRefLoader.loadProvidesport(refElt, assembly);

            } else if (refType.equals("componentsupportedinterface")) {
                // get child of <componentsupportedinterface>
                // (must have 1 child; validated by dtd)
                ElementDescription[] eltsDescr = refElt.get_child_elements();
                Element elt = ElementHelper.narrow(eltsDescr[0].reference());
                String eltName = XMLHelper.toString(eltsDescr[0].name());

                if (eltName.equals("componentinstantiationref")) {
                    // load componentinstantiationref
                    objToBeConnected = 
                        objectRefLoader.loadComponentinstantiationref(elt,
                                                                      assembly);
                } else if (eltName.equals("findby")) {
                    // load findby
                    objToBeConnected = objectRefLoader.loadFindby(elt);
                } else {
                    // shouldn't happen (only <componentinstantiationref>
                    // or <findby> are valid as child of <componentsupportedinterface>
                    cdmw.common.Assert.check(false);
                }

            } else if (refType.equals("existinginterface")) {
                // get child of <existinginterface>
                // (must have 1 child; validated by dtd)
                ElementDescription[] eltsDescr = refElt.get_child_elements();
                Element elt = ElementHelper.narrow(eltsDescr[0].reference());
                String eltName = XMLHelper.toString(eltsDescr[0].name());

                if (eltName.equals("findby")) {
                    // load findby
                    objToBeConnected = objectRefLoader.loadFindby(elt);
                } else {
                    // shouldn't happen (only <findby> is valid
                    // as child of <existinginterface>
                    cdmw.common.Assert.check(false);
                }

            } else if (refType.equals("homeinterface")) {
                // get child of <homeinterface>
                // (must have 1 child; validated by dtd)
                ElementDescription[] eltsDescr = refElt.get_child_elements();
                Element elt = ElementHelper.narrow(eltsDescr[0].reference());
                String eltName = XMLHelper.toString(eltsDescr[0].name());

                if (eltName.equals("homeplacementref")) {
                    // load homeplacementref
                    objToBeConnected = 
                        objectRefLoader.loadHomeplacementref(elt, assembly);
                } else if (eltName.equals("findby")) {
                    // load findby
                    objToBeConnected = objectRefLoader.loadFindby(elt);
                } else {
                    // shouldn't happen (only <componentinstantiationref>
                    // or <findby> are valid as child of <componentsupportedinterface>
                    cdmw.common.Assert.check(false);
                }

            } else {
                // shouldn't happen (only <providesport>,
                // <componentsupportedinterface>, <existinginterface> or
                // <homeinterface> are valid for second child of <connectinterface>
                cdmw.common.Assert.check(false);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // if component referenced for uses port is a ComponentGroup,
        // create a connection for each member of this group.
        if (usesComponent instanceof ComponentGroupRef) {

            ComponentGroup group = ((ComponentGroupRef) usesComponent).getComponentGroup();
            java.util.Iterator it = group.getComponents().iterator();
            int counter = 0;

            while (it.hasNext()) {
                Component comp = (Component) it.next();
                ComponentInstantiationRef usesComp = new ComponentInstantiationRef(comp);

                // create InterfaceConnection
                InterfaceConnection c = new InterfaceConnection(id + "_part_" + (++counter),
                                                                usesId,
                                                                usesComp,
                                                                objToBeConnected);
                if (!assembly.addConnection(c)) {
                    // A connection with same id already exists.
                    // This shouldn't happen, because id is an XML ID, 
                    // and .cad is validated by dtd; 
                    // and counter increase for each connection
                    cdmw.common.Assert.check(false);
                }
            }

        } else {

            // create InterfaceConnection
            InterfaceConnection c = new InterfaceConnection(id,
                                                            usesId,
                                                            usesComponent,
                                                            objToBeConnected);
            if (!assembly.addConnection(c)) {
                // A connection with same id already exists.
                // This shouldn't happen, because id is an XML ID, 
                // and .cad is validated by dtd.
                cdmw.common.Assert.check(false);
            }
        }
    }



    /**
     * Loads a connectevent element from the .cad file and fill the Assembly.
     *
     * @param connEventElt the connectevent element to be loaded.
     * @param assembly the Assembly to loaded with connectevent informations.
     * @param defaultId identifier to be used for the Connection if 
     *        connectevent element has no identifier.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadConnectEvent(Element connEventElt,
                                  AssemblyImpl assembly,
                                  String defaultId)
        throws CreateFailure
    {
        // get id of connection
        String connectionId = defaultId;
        try {
            AttrDescription idDescr = connEventElt.get_attribute(
                XMLHelper.toShortArray("id"));
            connectionId = XMLHelper.toString(idDescr.value());
        } catch (DOMException e) {
            // no id specified
        }

        // get id of <consumesport>
        String consumesId = null;
        try {
            NodeDescription[] nodes = connEventElt.select_nodes(
                XMLHelper.toShortArray(XPATH_CONSUMESPORT_ID));
            TextDescription consumesNameDescr = (TextDescription) nodes[0];
            consumesId = XMLHelper.toString(consumesNameDescr.data());

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // get Component reference for consumesport
        ObjectRef consumesComponent = null;
        try {
            NodeDescription[] nodes = connEventElt.select_nodes(
                XMLHelper.toShortArray(XPATH_CONSUMESPORT_COMPONENT_REF));
            ElementDescription compRefDescr = (ElementDescription) nodes[0];
            Element compRefElt = ElementHelper.narrow(compRefDescr.reference());
            String refType = XMLHelper.toString(compRefDescr.name());

            if (refType.equals("componentinstantiationref")) {
                // load componentinstantiationref
                consumesComponent = 
                    objectRefLoader.loadComponentinstantiationref(compRefElt,
                                                                  assembly);
            } else if (refType.equals("findby")) {
                // load findby
                consumesComponent = objectRefLoader.loadFindby(compRefElt);
            } else {
                // shouldn't happen (only <componentinstantiationref>
                // or <findby> are valid as child of <consumesport>
                cdmw.common.Assert.check(false);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // get event producer port to be connected to consumesport
        ObjectRef objToBeConnected = null;
        try {
            NodeDescription[] nodes = connEventElt.select_nodes(
                XMLHelper.toShortArray(XPATH_EVENT_PORT_TO_CONNECT_REF));
            ElementDescription eventPortDescr = (ElementDescription) nodes[0];
            String eventPortType = XMLHelper.toString(eventPortDescr.name());

            if (eventPortType.equals("emitsport")) {

                // get id of <emitsport>
                String emitsId = null;
                NodeDescription[] idNodes = connEventElt.select_nodes(
                    XMLHelper.toShortArray(XPATH_EMITSPORT_ID));
                TextDescription emitsNameDescr = (TextDescription) idNodes[0];
                emitsId = XMLHelper.toString(emitsNameDescr.data());

                // get Component reference for emitsport
                ObjectRef emitsComponent = null;
                NodeDescription[] compNodes = connEventElt.select_nodes(
                    XMLHelper.toShortArray(XPATH_EMITSPORT_COMPONENT_REF));
                ElementDescription compRefDescr = (ElementDescription) compNodes[0];
                Element compRefElt = ElementHelper.narrow(compRefDescr.reference());
                String refType = XMLHelper.toString(compRefDescr.name());

                if (refType.equals("componentinstantiationref")) {
                    // load componentinstantiationref
                    emitsComponent = 
                        objectRefLoader.loadComponentinstantiationref(compRefElt,
                                                                      assembly);
                } else if (refType.equals("findby")) {
                    // load findby
                    emitsComponent = objectRefLoader.loadFindby(compRefElt);
                } else {
                    // shouldn't happen (only <componentinstantiationref>
                    // or <findby> are valid as child of <emitsport>
                    cdmw.common.Assert.check(false);
                }

                // if component referenced for emits port is a ComponentGroup,
                // create a connection for each member of this group.
                if (emitsComponent instanceof ComponentGroupRef) {

                    ComponentGroup group = ((ComponentGroupRef) emitsComponent).getComponentGroup();
                    java.util.Iterator it = group.getComponents().iterator();
                    int counter = 0;

                    while (it.hasNext()) {
                        Component comp = (Component) it.next();
                        ComponentInstantiationRef emitsComp = new ComponentInstantiationRef(comp);

                        // create emits connection
                        EmitsConnection c = 
                            new EmitsConnection(connectionId + "_part_" + (++counter),
                                                consumesId,
                                                consumesComponent,
                                                emitsId,
                                                emitsComp);
                        if (!assembly.addConnection(c)) {
                            // A connection with same id already exists.
                            // This shouldn't happen, because id is an XML ID, 
                            // and .cad is validated by dtd; 
                            // and counter increase for each connection
                            cdmw.common.Assert.check(false);
                        }
                    }

                } else {

                    // create emits connection
                    EmitsConnection emitsConnection = 
                        new EmitsConnection(connectionId,
                                            consumesId,
                                            consumesComponent,
                                            emitsId,
                                            emitsComponent);

                    if (!assembly.addConnection(emitsConnection)) {
                        // A connection with same id already exists.
                        // This shouldn't happen, because id is an XML ID, 
                        // and .cad is validated by dtd.
                        cdmw.common.Assert.check(false);
                    }
                }

            } else if (eventPortType.equals("publishesport")) {

                // get id of <publishesport>
                String publishesId = null;
                NodeDescription[] idNodes = connEventElt.select_nodes(
                    XMLHelper.toShortArray(XPATH_PUBLISHESPORT_ID));
                TextDescription publishesNameDescr = (TextDescription) idNodes[0];
                publishesId = XMLHelper.toString(publishesNameDescr.data());

                // get Component reference for publishesport
                ObjectRef publishesComponent = null;
                NodeDescription[] compNodes = connEventElt.select_nodes(
                    XMLHelper.toShortArray(XPATH_PUBLISHESPORT_COMPONENT_REF));
                ElementDescription compRefDescr = (ElementDescription) compNodes[0];
                Element compRefElt = ElementHelper.narrow(compRefDescr.reference());
                String refType = XMLHelper.toString(compRefDescr.name());

                if (refType.equals("componentinstantiationref")) {
                    // load componentinstantiationref
                    publishesComponent = 
                        objectRefLoader.loadComponentinstantiationref(compRefElt,
                                                                      assembly);
                } else if (refType.equals("findby")) {
                    // load findby
                    publishesComponent = objectRefLoader.loadFindby(compRefElt);
                } else {
                    // shouldn't happen (only <componentinstantiationref>
                    // or <findby> are valid as child of <emitsport>
                    cdmw.common.Assert.check(false);
                }

                // if component referenced for publishes port is a ComponentGroup,
                // create a connection for each member of this group.
                if (publishesComponent instanceof ComponentGroupRef) {

                    ComponentGroup group = ((ComponentGroupRef) publishesComponent).getComponentGroup();
                    java.util.Iterator it = group.getComponents().iterator();
                    int counter = 0;

                    while (it.hasNext()) {
                        Component comp = (Component) it.next();
                        ComponentInstantiationRef publishesComp = new ComponentInstantiationRef(comp);

                        // create emits connection
                        PublishesConnection c = 
                            new PublishesConnection(connectionId + "_part_" + (++counter),
                                                    consumesId,
                                                    consumesComponent,
                                                    publishesId,
                                                    publishesComp);
                        if (!assembly.addConnection(c)) {
                            // A connection with same id already exists.
                            // This shouldn't happen, because id is an XML ID,
                            // and .cad is validated by dtd;
                            // and counter increase for each connection
                            cdmw.common.Assert.check(false);
                        }
                    }

                } else {

                    // create publishes connection
                    PublishesConnection publishesConnection = 
                        new PublishesConnection(connectionId,
                                                consumesId,
                                                consumesComponent,
                                                publishesId,
                                                publishesComponent);

                    if (!assembly.addConnection(publishesConnection)) {
                        // A connection with same id already exists.
                        // This shouldn't happen, because id is an XML ID,
                        // and .cad is validated by dtd.
                        cdmw.common.Assert.check(false);
                    }
                }

            } else {
                // shouldn't happen (only <emitsport> or <publishesport>
                // are valid for second child of <connectevent>
                cdmw.common.Assert.check(false);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

    }


}
