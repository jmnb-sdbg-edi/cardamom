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
import org.omg.Components.Configurator;

import org.omg.Components.CreateFailure;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.NO_DESTINATION;
import com.thalesgroup.CdmwDeployment.PACKAGE_MANAGER_ERROR;
import com.thalesgroup.CdmwDeployment.NOT_IMPLEMENTED;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.TextDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;


/**
 * This abstract class defines some usefull operations to
 * load some properties of a ComponentGroup (fault tolerant
 * group or load balanced group).
 * It also defines the abstract load() operation which should
 * be implemented by inheriting classes.
 */
abstract class ComponentGroupLoader {

    private static final String XPATH_DESTINATION_VALUE =
        "destination/text()";

    private static final String XPATH_COMPONENTREF_CHILDREN =
        "componentinstantiationlist/componentinstantiationref";
    
    private static final String XPATH_REGISTER_GROUP =
        "registergroup";

    private static final String XPATH_REGISTER_WITH_NAMING =
        "registerwithnaming/@name";

    private static final String XPATH_PORT_IDENTIFIER =
        "*[self::providesidentifier or " +
        "  self::consumeridentifier]";
        
    
    /**
     * Loads a componentgroup XML element and add the
     * ComponentGroup to the assembly.
     *
     * @param componentGroupElt the componentgroup XML element
     * @param assembly the assembly to which the ComponentGroup
     *        has to be added.
     * @param assemblyPack the AssemblyPackage corresponding
     *        to the assembly
     *
     * @exception CreateFailure if loading fails.
     */ 
    abstract void load(Element componentGroupElt,
                       AssemblyImpl assembly,
                       _AssemblyPackage assemblyPack)
        throws CreateFailure;
    
    
    
    /**
     * Loads the identifier attribute of a componentgroup 
     * XML element.
     *
     * @param componentGroupElt the componentgroup XML element.
     * @return the value of the identifier attribute.
     */ 
    protected String loadId(Element componentGroupElt)
    {
        // get id of componentgroup
        String id = "";
        try {
            AttrDescription idDescr = componentGroupElt.get_attribute(
                XMLHelper.toShortArray("id"));
            id = XMLHelper.toString(idDescr.value());
        } catch (DOMException e) {
            // shouldn't happen, id required and cad validated by dtd
            cdmw.common.Assert.check(false);
        }

        return id;        
    }
    

    /**
     * Loads the destination element of a componentgroup 
     * XML element.
     *
     * @param componentGroupElt the componentgroup XML element.
     * @return the value of the destination element or
     *         null if no destination is specified.
     */ 
    protected String loadDestination(Element componentGroupElt)
        throws CreateFailure
    {
        String destination = null;
        try {
            NodeDescription[] nodeDescr = componentGroupElt.select_nodes(
                XMLHelper.toShortArray(XPATH_DESTINATION_VALUE));
            if (nodeDescr.length == 0) {
                // no destination for homeplacement
                throw new CreateFailure(
                    "componentgroup " + loadId(componentGroupElt) + " has no destination.",
                    NO_DESTINATION.value);
            }
            TextDescription destDescr = (TextDescription) nodeDescr[0];
            destination = XMLHelper.toString(destDescr.data());

        } catch (XPathFormatException e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        
        return destination;
    }
    
    
    
    /**
     * Loads a the members list of a componentgroup XML element 
     * and add each SingleComponent member to the ComponentGroup.
     *
     * @param componentGroupElt the componentgroup XML element
     * @param group the ComponentGroup to which the members
     *        have to be added.
     * @param assemblyPack the AssemblyPackage corresponding
     *        to the assembly
     *
     * @exception CreateFailure if loading fails.
     */ 
    protected void loadGroupMembers(Element componentGroupElt,
                                    ComponentGroup group)
        throws CreateFailure
    {
        // get components members list
        NodeDescription[] parts= null;
        try {
            parts = componentGroupElt.select_nodes(
                XMLHelper.toShortArray(XPATH_COMPONENTREF_CHILDREN));

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        for (int i = 0; i < parts.length; i++) {
            ElementDescription eltDescr = (ElementDescription) parts[i];
            Element elt = ElementHelper.narrow(eltDescr.reference());
            
            // load <componentinstantiationref> element
            SingleComponent component = 
                loadComponentInstantiationRef(elt, group.getParentAssembly());
                
            // add component member to group
            group.addComponent(component);
        }
    }

    
    /**
     * Loads a componentinstantiationref XML element.
     * The idref attribute should correspond to a componentinstantiation
     * element already loaded, and thus to a SingleComponent already
     * added to the assembly.
     *
     * @param compRefElt the componentinstantiationref XML element.
     * @param assembly the assembly into which the component should
     *        be retrieved.
     * @return the SingleComponent from the assembly corresponding
     *         to the componentinstantiationref XML element.
     *
     * @exception CreateFailure if loading fails.
     */
    private SingleComponent loadComponentInstantiationRef(Element compRefElt,
                                                          AssemblyImpl assembly)
        throws CreateFailure
    {
        // read idref attribute of <componentinstantiationref>
        String idref = null;
        try {
            AttrDescription idDescr = compRefElt.get_attribute(
                XMLHelper.toShortArray("idref"));
            idref = XMLHelper.toString(idDescr.value());

        } catch (DOMException e) {
            // shouldn't happen, idref is required and cad is validated by dtd
            cdmw.common.Assert.check(false);
        }

        // get corresponding Component from AssemblyImpl
        SingleComponent component = assembly.getComponent(idref);
        
        if (component == null) {
            throw new CreateFailure(
                "<componentinstantationref idref=\"" + idref +
                "\"> doesn't correspond to a <componentinstantiation> or to a component group in cad",
                ERROR_IN_CAD.value);
        }

        return component;
    }
    
    
    /** 
     * Loads a groupproperties XML element, and apply the corresponding
     * Configurator to the ComponentGroup.
     *
     * @param group the ComponentGroup to be configured by the groupproperties.
     * @param configManager the ConfiguratorManager to retrieve or create 
     *        a Configurator.
     * @param assemblyPackage the AssemblyPackage corresponding to the assembly.
     *
     * @exception CreateFailure if loading fails.
     */
    protected void loadGroupProperties(ComponentGroup group,
                                       ConfiguratorManager configManager,
                                       _AssemblyPackage assemblyPack)
        throws CreateFailure
    {
        // try to get component CPF for this componentgroup 
        // from AssemblyPackage
        String cpfLocation = null;
        try {
            cpfLocation = 
                assemblyPack.get_componentgroup_CPF_location(group.getId());
        
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            // cpf has not been installed
            throw new CreateFailure(
                "Group properties file of componentgroup " + group.getId() +
                " has not been installed by PackageManager",
                PACKAGE_MANAGER_ERROR.value);
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            throw new CreateFailure(
                "PackageManager cannot create location for component .cpf" +
                " for componentgroup " + group.getId() + 
                " from assembly package " + assemblyPack.get_uuid(),
                PACKAGE_MANAGER_ERROR.value);
        
        } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
            // No groupproperties specified for 
            // this componentgroup; do nothing.
            return;
        }

        System.out.println("      component group configuration: " + cpfLocation);

        Configurator configurator =
            configManager.getComponentConfigurator(group.getRepositoryId(),
                                                   cpfLocation);
        
        group.setComponentConfigurator(configurator);        
    }
    
    /**
     * Loads registergroup XML elements which are child of a 
     * componentgroup XML element, and add corresponding 
     * naming registration informations to the ComponentGroup. 
     *
     * @param group the ComponentGroup to add naming registrations.
     * @param compGroupElt the componentgroup XML element corresponding 
     *        to the ComponentGroup.
     *
     * @exception CreateFailure if loading fails.
     */
    protected void loadNamingRegistrations(ComponentGroup group,
                                           Element compGroupElt) 
        throws CreateFailure
    {
        try {
            // get all <registergroup> child elements
            NodeDescription[] nodes = compGroupElt.select_nodes(
                XMLHelper.toShortArray(XPATH_REGISTER_GROUP));
                
            for (int i = 0; i < nodes.length; i++) {
                Element registerElt = ElementHelper.narrow(nodes[i].reference());
                
                // get name attributes of all <registerwithnaming> child elements
                NodeDescription[] nameNodes = registerElt.select_nodes(
                    XMLHelper.toShortArray(XPATH_REGISTER_WITH_NAMING));
                
                // get <providesidentifier> 
                // or <consumeridentifier> element
                NodeDescription[] idNodes = registerElt.select_nodes(
                    XMLHelper.toShortArray(XPATH_PORT_IDENTIFIER));
                
                if (idNodes.length == 0) {
                    // register group itself
                    for (int j = 0; j < nameNodes.length; j++) {
                        AttrDescription nameDescr = (AttrDescription) nameNodes[j];
                        String name = XMLHelper.toString(nameDescr.value());
                        
                        // add naming registration
                        group.addComponentNamingRegistration(name);
                    }
                
                } else {
                    // get name of idNodes[0] element
                    String portType = XMLHelper.toString(
                        ((ElementDescription) idNodes[0]).name());
                    
                    // get text child of idNodes[0] element
                    Element idElt = ElementHelper.narrow(idNodes[0].reference());
                    TextDescription[] textsDescr = idElt.get_texts();
                    if (textsDescr.length == 0) {
                        throw new CreateFailure(
                            "Error in registerwithnaming of component group " +
                            group.getId() + ": <" + portType + 
                            "> has no text for identifier.",
                            ERROR_IN_CAD.value);
                    }
                    
                    // get portId
                    String portId = XMLHelper.toString(textsDescr[0].data());
                    
                    if (portType.equals("providesidentifier")) {
                        // register provides
                        for (int j = 0; j < nameNodes.length; j++) {
                            AttrDescription nameDescr = (AttrDescription) nameNodes[j];
                            String name = XMLHelper.toString(nameDescr.value());
                            
                            // add naming registration
                            group.addProvidesNamingRegistration(portId, name);
                        }
                        
                    } else if (portType.equals("consumeridentifier")) {
                        throw new CreateFailure(
                            "Registration of consumeridentifier not supported.",
                            NOT_IMPLEMENTED.value);
                            
                    } else {
                        // shouldn't append, only providesidentifier,
                        // emitsidentifier or publishesidentifier selected
                        cdmw.common.Assert.check(false);
                    }
                }
            
            } // end for (int i)
                
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }          
    }
}
 
