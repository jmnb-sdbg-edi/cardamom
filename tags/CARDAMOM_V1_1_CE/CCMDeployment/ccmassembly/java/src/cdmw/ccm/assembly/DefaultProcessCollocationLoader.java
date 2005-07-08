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

import com.thalesgroup.CdmwDeployment._AssemblyPackage;

import org.omg.Components.CreateFailure;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.NO_DESTINATION;
import com.thalesgroup.CdmwDeployment.DESTINATION_NOT_CONFORM;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.Node;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;


/**
 * This class loads processcollocation XML elements which have
 * no faulttolerant or loadbalanced child XML element,
 * and create a new corresponding ComponentServer object.
 */
class DefaultProcessCollocationLoader {

    private static final String XPATH_PROCESSCOLLOCATION_DESTINATION_VALUE =
        "destination/text()";
    private static final String XPATH_ALL_HOMEPLACEMENT_DESTINATION_VALUES =
        "homeplacement/destination/text()";

        
    /**
     * HomePlacementLoader to be used for homeplacement
     * XML elements loading.
     */
    private HomePlacementLoader homePlacementLoader;
    
    
    /**
     * Constructor.
     */
    public DefaultProcessCollocationLoader() {
        this.homePlacementLoader = new HomePlacementLoader();
    }
    
    
    /**
     * Loads a processcollocation XML element, create a corresponding ComponentServer
     * and add it to its parent HostCollocation and to the assembly.
     *
     * @param processElt the processcollocation XML element
     * @param assemblyPack the AssemblyPackage corresponding to the parent Assembly.
     * @param parentHome the parent HostCollocation of the ComponentServer.
     *
     * @exception CreateFailure if loading fails.
     */ 
    void load(Element processElt,
              _AssemblyPackage assemblyPack,
              HostCollocation parentHost)
        throws CreateFailure
    {
        // get id of processcollocation
        String id = getProcesscollocationId(processElt);
        // get destination of processcollocation
        String destination = getProcesscollocationDestination(processElt, id);

        // create ComponentServer (automatically added to its  
        // parentHost and to its parent assembly).
        ComponentServer server = new ComponentServer(id, parentHost, destination);

        // load children homeplacements
        loadHomeplacements(processElt, assemblyPack, server);
    }
    
       
    /**
     * Loads the identifier attribute of a processcollocation XML element,
     * and returns its value.
     *
     * @param processElt the processcollocation XML element.
     * @return the value of the identifier attribute.
     */
    protected String getProcesscollocationId(Element processElt) {
        // get id of hostcollocation
        String id = "";
        try {
            AttrDescription idDescr = processElt.get_attribute(
                XMLHelper.toShortArray("id"));
            id = XMLHelper.toString(idDescr.value());
        } catch (DOMException e) {
            // no id specified
        }
        return id;
    }
    
    /**
     * Loads the destination element of a processcollocation 
     * XML element.
     *
     * @param processElt the processcollocation XML element.
     * @return the value of the destination element or
     *         null if no destination is specified.
     */ 
    protected String getProcesscollocationDestination(Element processElt,
                                                      String id)
        throws CreateFailure
    {
        String destination = null;
        NodeDescription[] destDescr = null;
        try {
            destDescr = processElt.select_nodes(
                XMLHelper.toShortArray(XPATH_PROCESSCOLLOCATION_DESTINATION_VALUE));
            if (destDescr.length == 0) {
                // no destination for processcollocation
                // use destinations of children homeplacements
                destination = getHomeplacementsDestination(processElt);
            
            } else {
                TextDescription txtDesc = (TextDescription) destDescr[0];
                destination = XMLHelper.toString(txtDesc.data());               
                
                // check that destinations of children are not specified
                // or are the same that processcollocation's destination
                String homesDestination = getHomeplacementsDestination(processElt);
                if (homesDestination != null && homesDestination.equals(destination)) {
                    throw new CreateFailure(
                        "Homeplacements Destinations (" + homesDestination + 
                        ") of processcollocation " + id +
                        " are not the same that this processcollocation destination: " + destination,
                        DESTINATION_NOT_CONFORM.value);
                }
            }

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
        
        if (destination == null) {
            throw new CreateFailure(
                "No destination specified for processcollocation " + id + ".", 
                NO_DESTINATION.value);                
        }
                
        // convert destination to be usable in a corbaname
        org.omg.CosNaming.NameComponent[] nc = 
            new org.omg.CosNaming.NameComponent[1];
        nc[0] = new org.omg.CosNaming.NameComponent(destination, "");
        try {
            destination = cdmw.namingandrepository.NamingInterface.toString(nc);
        } catch (cdmw.namingandrepository.InvalidNameException e) {
            throw new CreateFailure(
                "Destination \"" + destination + 
                "\" specified in homeplacements of processcollocation " + id +
                " is not valid.",
                DESTINATION_NOT_CONFORM.value);
        }
        
        return destination;
    }
    
    
    /**
     * Loads the destination element of a all hostcollocation XML elements
     * which are children of a processcollocation XML element.
     * If thoses destinations are not the same, a CreateFailure exception
     * is thrown.
     *
     * @param processElt the processcollocation XML element.
     * @return the value of the destination elements or
     *         null if no destination is specified.
     */ 
    protected String getHomeplacementsDestination(Element processElt)
        throws CreateFailure
    {
        // get values of all <destination> elements
        // which are children of an <homeplacement> element
        NodeDescription[] destDescr = null;
        try {
            destDescr = processElt.select_nodes(
                XMLHelper.toShortArray(XPATH_ALL_HOMEPLACEMENT_DESTINATION_VALUES));

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        String destination = null;

        for (int i = 0; i < destDescr.length; i++) {
            // NodeDescription should be a TextDescription
            TextDescription txtDesc = (TextDescription) destDescr[i];
            String dest = XMLHelper.toString(txtDesc.data());

            if (destination == null) {
                destination = dest;
            } else {

                // check if dest is same that destination
                if (!destination.equalsIgnoreCase(dest)) {
                    String id = null;
                    try {
                        // get homeplacement element
                        Node homeNode = destDescr[i].reference().get_parent()   
                                                    .reference().get_parent()
                                                    .reference();  
                        Element homeElt = ElementHelper.narrow(homeNode);
                    
                        // get id of homeplacement
                        AttrDescription idDescr = homeElt.get_attribute(
                            XMLHelper.toShortArray("id"));
                        id = XMLHelper.toString(idDescr.value());
                    } catch (DOMException e) {
                        // shouldn't happen
                        e.printStackTrace();
                        cdmw.common.Assert.check(false);
                    }
                    
                    throw new CreateFailure(
                        "Destination " + dest + " of homeplacement " + id +
                        " is not conform with destination " + destination +
                        " of a previous homeplacement in same collocation.",
                        DESTINATION_NOT_CONFORM.value);
                }
            }
        }

        return destination;
    }
    
    
    /**
     * Loads the homeplacement XML elements which are children of a
     * processcollocation XML element, and create corresponding 
     * Containers and Homes.
     *
     * @param processElt the processcollocation XML element.
     * @param assemblyPack the AssemblyPackage corresponding to the parent Assembly.
     * @param parentServer the parent ComponentServer of the Containers.
     */
    protected void loadHomeplacements(Element processElt,
                                      _AssemblyPackage assemblyPack,
                                      ComponentServer parentServer)
        throws CreateFailure
    
    {
        // get all <homeplacement> children of <processcollocation> element
        NodeDescription[] parts = null;
        try {
            parts = processElt.select_nodes(
                XMLHelper.toShortArray("homeplacement"));

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        for (int i = 0; i < parts.length; i++) {
            ElementDescription eltDescr = (ElementDescription) parts[i];
            Element elt = ElementHelper.narrow(eltDescr.reference());

            // load homeplacement
            homePlacementLoader.load(elt, assemblyPack, parentServer);
        }
    }
    
    
    
    
}
