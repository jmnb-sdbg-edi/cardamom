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


package cdmw.ccm.assembly;

import org.omg.Components.CreateFailure;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.NAMING_SERVICE_ERROR;
import com.thalesgroup.CdmwDeployment.HOME_FINDER_ERROR;
import com.thalesgroup.CdmwDeployment.NOT_IMPLEMENTED;


import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.XPathFormatException;
import com.thalesgroup.CdmwXML.DOMException;


/**
 * This class loads object references from XML elements 
 * specifying a connection.
 */
class ObjectRefLoader {


    private static final String XPATH_PROVIDESPORT_ID =
        "providesidentifier/text()";

    private static final String XPATH_PROVIDING_COMPONENT_REF =
        "componentinstantiationref | findby";

    private static final String XPATH_CONSUMESPORT_ID =
        "consumesport/consumesidentifier/text()";


    /**
     * Loads a facet reference according to a providesport
     * XML element.
     *
     * @param provElt the providesport XML element.
     * @param assembly the parent Assembly.
     * @return the facet reference.
     *
     * @exception CreateFailure if loading fails.
     */
    public static FacetRef loadProvidesport(Element provElt,
                                            AssemblyImpl assembly)
        throws CreateFailure
    {
        // read providesidentifier
        String providesId = null;
        try {
            NodeDescription[] nodes = provElt.select_nodes(
                XMLHelper.toShortArray(XPATH_PROVIDESPORT_ID));
            TextDescription provIdDescr = (TextDescription) nodes[0];
            providesId = XMLHelper.toString(provIdDescr.data());

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // get Component providing the facet
        ObjectRef compRef = null;
        try {
            NodeDescription[] nodes = provElt.select_nodes(
                XMLHelper.toShortArray(XPATH_PROVIDING_COMPONENT_REF));
            ElementDescription refDescr = (ElementDescription) nodes[0];
            Element refElt = ElementHelper.narrow(refDescr.reference());
            String refType = XMLHelper.toString(refDescr.name());

            if (refType.equals("componentinstantiationref")) {
                // load componentinstantiationref
                compRef = loadComponentinstantiationref(refElt,
                                                        assembly);
            } else if (refType.equals("findby")) {
                // load findby
                compRef = loadFindby(refElt);
            } else {
                // shouldn't happen
                cdmw.common.Assert.check(false);
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // create FacetRef
        try {
            return new FacetRef(providesId, compRef);
        
        } catch (cdmw.common.BadParameterException e) {
            throw new CreateFailure(
                "Error in <providesidentifier> " + providesId +
                ": " + e.getMessage(),
                ERROR_IN_CAD.value);
        }
    }


    /**
     * Loads a component instantiation reference according 
     * to a componentinstantiationref XML element.
     *
     * @param refElt the componentinstantiationref XML element.
     * @param assembly the parent Assembly.
     * @return the component instantiation reference.
     *
     * @exception CreateFailure if loading fails.
     */
    public static ComponentInstantiationRef
    loadComponentinstantiationref(Element refElt,
                                  AssemblyImpl assembly)
        throws CreateFailure
    {
        // read idref attribute of <componentinstantiationref>
        String idref = null;
        try {
            AttrDescription idDescr = refElt.get_attribute(
                XMLHelper.toShortArray("idref"));
            idref = XMLHelper.toString(idDescr.value());

        } catch (DOMException e) {
            // shouldn't happen, idref is required and cad is validated by dtd
            cdmw.common.Assert.check(false);
        }

        // get corresponding Component from AssemblyImpl
        Component component = assembly.getComponent(idref);
        if (component != null) {
            // create ComponentInstantationRef
            return new ComponentInstantiationRef(component);
        }            

        // if not found, try to find a corresponding component group
        ComponentGroup componentGroup = assembly.getComponentGroup(idref);    
        if (componentGroup != null) {
            // create ComponentGroupRef
            return new ComponentGroupRef(componentGroup);
        }
        
        throw new CreateFailure(
            "<componentinstantationref idref=\"" + idref +
            "\"> doesn't correspond to a <componentinstantiation> or to a component group in cad",
            ERROR_IN_CAD.value);
    }


    /**
     * Loads a homeplacement reference according 
     * to a homeplacementref XML element.
     *
     * @param refElt the homeplacementref XML element.
     * @param assembly the parent Assembly.
     * @return the homeplacement instantiation reference.
     *
     * @exception CreateFailure if loading fails.
     */
    public static HomePlacementRef loadHomeplacementref(Element refElt,
                                                        AssemblyImpl assembly)
        throws CreateFailure
    {
        // read idref attribute of <homeplacementref>
        String idref = null;
        try {
            AttrDescription idDescr = refElt.get_attribute(
                XMLHelper.toShortArray("idref"));
            idref = XMLHelper.toString(idDescr.value());

        } catch (DOMException e) {
            // shouldn't happen, idref is required and cad is validated by dtd
            cdmw.common.Assert.check(false);
        }

        // get corresponding Home from AssemblyImpl
        Home home = assembly.getHome(idref);
        if (home == null) {
            throw new CreateFailure(
                "<homeplacementref idref=\"" + idref +
                "\"> doesn't correspond to a <homeplacement> in cad",
                ERROR_IN_CAD.value);
        }

        // create ComponentInstantationRef
        return new HomePlacementRef(home);
    }



    /**
     * Loads a findby reference according 
     * to a findby XML element.
     *
     * @param findbyElt the findby XML element.
     *
     * @exception CreateFailure if loading fails.
     */
    public static Findby loadFindby(Element findbyElt)
        throws CreateFailure
    {
        // get child of findbyElt (must have 1 child)
        ElementDescription[] eltDescr = findbyElt.get_child_elements();
        Element elt = ElementHelper.narrow(eltDescr[0].reference());
        String eltName = XMLHelper.toString(eltDescr[0].name());

        if (eltName.equals("namingservice")) {
            // get name
            String name = null;
            try {
                AttrDescription nameDescr = elt.get_attribute(
                    XMLHelper.toShortArray("name"));
                name = XMLHelper.toString(nameDescr.value());
            } catch (DOMException e) {
                // shouldn't happen, name is required and cad is validated by dtd
                cdmw.common.Assert.check(false);
            }

            // get NamingService
            org.omg.CosNaming.NamingContext nameServ;
            try {
                nameServ = AssemblyFactoryImpl.getInstance().getNamingService();
            } catch (Exception e) {
                throw new CreateFailure(
                    "Cannot contact NamingService: " + e.getMessage(),
                    NAMING_SERVICE_ERROR.value);
            }

            // create FindbyNamingService
            return new FindbyNamingService(nameServ, name);

        } else if (eltName.equals("stringifiedobjectref")) {
            // get objectref (text child)
            TextDescription[] texts = elt.get_texts();
            String ref = XMLHelper.toString(texts[0].data());

            // create stringifiedObject
            return new FindbyStringifiedObject(ref, 
                AssemblyFactoryImpl.getInstance().getOrb());

        } else if (eltName.equals("traderquery")) {
            throw new CreateFailure(
                "<traderquery> not supported",
                NOT_IMPLEMENTED.value);

        } else if (eltName.equals("homefinder")) {
            // get name
            String name = null;
            try {
                AttrDescription nameDescr = elt.get_attribute(
                    XMLHelper.toShortArray("name"));
                name = XMLHelper.toString(nameDescr.value());
            } catch (DOMException e) {
                // shouldn't happen, name is required and cad is validated by dtd
                cdmw.common.Assert.check(false);
            }

            // get HomeFinder
            org.omg.Components.HomeFinder homeFinder;
            try {
                homeFinder = AssemblyFactoryImpl.getInstance().getHomeFinder();
            } catch (Exception e) {
                throw new CreateFailure(
                    "Cannot contact HomeFinder: " + e.getMessage(),
                    HOME_FINDER_ERROR.value);
            }

            // create FindbyHomeFinder
            return new FindbyHomeFinder(homeFinder, name);

        } else if (eltName.equals("extension")) {
            throw new CreateFailure(
                "<extension> not supported for <findby>",
                NOT_IMPLEMENTED.value);

        } else {
            // shouldn't happen
            cdmw.common.Assert.check(false);
            return null;
        }
    }

    





}








