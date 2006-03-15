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
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.NO_DESTINATION;
import com.thalesgroup.CdmwDeployment.NOT_IMPLEMENTED;

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
 * This class loads hostcollocation XML elements,
 * and create a new corresponding HostCollocation object.
 */
class HostCollocationLoader {
    
    private static final String XPATH_DESTINATION_VALUE =
        "destination/text()";

    private static final String XPATH_HOSTCOLLOCATION_CHILDREN =
        "processcollocation | executableplacement | homeplacement";

    
    /**
     * ProcessCollocationLoader to be used for processcollocation
     * XML elements loading.
     */
    private ProcessCollocationLoader processCollocationLoader;
    
    
    /**
     * Constructor.
     */
    public HostCollocationLoader() {
        this.processCollocationLoader = new ProcessCollocationLoader();
    }

    
    /**
     * Loads a hostcollocation XML element, create a corresponding HostCollocation
     * and add it to its parent assembly.
     *
     * @param hostElt the hostcollocation XML element
     * @param assembly the parent Assembly of the HostCollocation.
     * @param assemblyPack the AssemblyPackage corresponding to the parent Assembly.
     *
     * @exception CreateFailure if loading fails.
     */ 
    void load(Element hostElt,
              AssemblyImpl assembly,
              _AssemblyPackage assemblyPack)
        throws CreateFailure
    {
        // get id of hostcollocation
        String id = "";
        try {
            AttrDescription idDescr = hostElt.get_attribute(
                XMLHelper.toShortArray("id"));
            id = XMLHelper.toString(idDescr.value());
        } catch (DOMException e) {
            // no id specified
        }

        // get destination of hostcollocation
        String destination = null;
        try {
            NodeDescription[] nodeDescr = hostElt.select_nodes(
                XMLHelper.toShortArray(XPATH_DESTINATION_VALUE));
            if (nodeDescr.length == 0) {
                // no destination for homeplacement
                throw new CreateFailure(
                    "hostcollocation " + id + " has no destination.",
                    NO_DESTINATION.value);
            }
            TextDescription destDescr = (TextDescription) nodeDescr[0];
            destination = XMLHelper.toString(destDescr.data());

        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }

        // create HostCollocation 
        // (automatically added to its parent assembly).
        HostCollocation host = new HostCollocation(id, assembly, destination);

        // get all children of <hostcollocation> element which are
        // <processcollocation>, <executableplacement> or <homeplacement>
        NodeDescription[] parts= null;
        try {
            parts = hostElt.select_nodes(
                XMLHelper.toShortArray(XPATH_HOSTCOLLOCATION_CHILDREN));

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        for (int i = 0; i < parts.length; i++) {
            ElementDescription eltDescr = (ElementDescription) parts[i];
            Element elt = ElementHelper.narrow(eltDescr.reference());

            // get name of element
            String eltName = XMLHelper.toString(eltDescr.name());

            if (eltName.equals("processcollocation")) {
                // load processcollocation
                processCollocationLoader.load(elt, assemblyPack, host);

            } else if (eltName.equals("executableplacement")) {
                // not yet implemented
                System.err.println("<executableplacement> not implemented");
                throw new CreateFailure(
                    "<executableplacement> not implemented",
                    NOT_IMPLEMENTED.value);

            } else if (eltName.equals("homeplacement")) {
                throw new CreateFailure(
                    "Only <processcollocation> are accepted as child of <hostcollocation>",
                    ERROR_IN_CAD.value);

            } else {
                // shouldn't happen, only <processcollocation>,
                // <executableplacement> and <homeplacement> are selected
                cdmw.common.Assert.check(false);
            }
        }
    }


    
}

