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
import com.thalesgroup.CdmwDeployment.PACKAGE_MANAGER_ERROR;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;
import com.thalesgroup.CdmwDeployment.NOT_IMPLEMENTED;

import org.omg.Components.Configurator;

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
 * This class loads componentinstantiation XML elements
 * and create a new corresponding SingleComponent object.
 */
class ComponentInstantiationLoader {
    
    private static final String XPATH_REGISTER_COMPONENT =
        "registercomponent";

    private static final String XPATH_REGISTER_WITH_NAMING =
        "registerwithnaming/@name";

    private static final String XPATH_PORT_IDENTIFIER =
        "*[self::emitsidentifier or " +
        "  self::providesidentifier or " +
        "  self::publishesidentifier]";

        
    /**
     * Loads a componentinstantiation XML element, create a corresponding
     * SinglComponent and add it to its parent home and to the assembly.
     *
     * @param componentElt the componentinstantiation XML element
     * @param assembly the assembly to which the FTComponentGroup
     *        has to be added.
     * @param parentHome the parent Home of the SinglComponent to be created.
     *
     * @exception CreateFailure if loading fails.
     */ 
    void load(Element componentElt,
              _AssemblyPackage assemblyPack,
              Home parentHome)
        throws CreateFailure
    {
        // get id of componentinstantiation
        String id = null;
        try {
            AttrDescription idDescr = componentElt.get_attribute(
                XMLHelper.toShortArray("id"));
            id = XMLHelper.toString(idDescr.value());
        } catch (DOMException e) {
            // shouldn't happen, id required and cad validated by dtd
            cdmw.common.Assert.check(false);
        }

        // create Component (automatically added to its  
        // parentHome and to its parent assembly).
        SingleComponent component = new SingleComponent(id, parentHome);

        // load specific componentproperties
        loadComponentProperties(component, 
                                parentHome.getParentAssembly().getConfiguratorManager(),
                                assemblyPack);
        
        // load naming registrations
        loadNamingRegistrations(component, componentElt);
    }


    /** 
     * Loads a componentproperties XML element, and apply the corresponding
     * Configurator to the SingleComponent.
     *
     * @param component the SingleComponent to be configured by the componentproperties.
     * @param configManager the ConfiguratorManager to retrieve or create 
     *        a Configurator.
     * @param assemblyPackage the AssemblyPackage corresponding to the assembly.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadComponentProperties(SingleComponent component,
                                         ConfiguratorManager configManager,
                                         _AssemblyPackage assemblyPack)
        throws CreateFailure
    {
        // try to get component CPF for this componentinstantiation 
        // from AssemblyPackage
        String cpfLocation = null;
        try {
            cpfLocation = 
                assemblyPack.get_instantiation_CPF_location(component.getId());
        
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            // cpf has not been installed
            throw new CreateFailure(
                "Component properties file of componentinstantiation " + component.getId() +
                " has not been installed by PackageManager",
                PACKAGE_MANAGER_ERROR.value);
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            throw new CreateFailure(
                "PackageManager cannot create location for component .cpf" +
                " for componentinstantiation " + component.getId() + 
                " from assembly package " + assemblyPack.get_uuid(),
                PACKAGE_MANAGER_ERROR.value);
        
        } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
            // No default componentproperties specified for 
            // this componentinstantiation; do nothing.
            return;
        }

        System.out.println("                  specific component configuration: " + cpfLocation);

        Configurator configurator =
            configManager.getComponentConfigurator(component.getRepositoryId(),
                                                   cpfLocation);
        
        component.setComponentConfigurator(configurator);
    }


    /**
     * Loads registercomponent XML elements which are child of a 
     * componentinstantiation XML element, and add corresponding 
     * naming registration informations to the SingleComponent. 
     *
     * @param component the SingleComponent to add naming registrations.
     * @param compInstElt the componentinstantiation XML element corresponding 
     *        to the SingleComponent.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadNamingRegistrations(SingleComponent component,
                                         Element compInstElt)
        throws CreateFailure
    {
        try {
            // get all <registercomponent> child elements
            NodeDescription[] nodes = compInstElt.select_nodes(
                XMLHelper.toShortArray(XPATH_REGISTER_COMPONENT));
                
            for (int i = 0; i < nodes.length; i++) {
                Element registerElt = ElementHelper.narrow(nodes[i].reference());
                
                // get name attributes of all <registerwithnaming> child elements
                NodeDescription[] nameNodes = registerElt.select_nodes(
                    XMLHelper.toShortArray(XPATH_REGISTER_WITH_NAMING));
                
                // get <emitsidentifier>, <providesidentifier> 
                // or <publishesidentifier> element
                NodeDescription[] idNodes = registerElt.select_nodes(
                    XMLHelper.toShortArray(XPATH_PORT_IDENTIFIER));
                
                if (idNodes.length == 0) {
                    // register component itself
                    for (int j = 0; j < nameNodes.length; j++) {
                        AttrDescription nameDescr = (AttrDescription) nameNodes[j];
                        String name = XMLHelper.toString(nameDescr.value());
                        
                        // add naming registration
                        component.addComponentNamingRegistration(name);
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
                            "Error in registerwithnaming of componentinstantiation " +
                            component.getId() + ": <" + portType + 
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
                            component.addProvidesNamingRegistration(portId, name);
                        }
                        
                    } else if (portType.equals("emitsidentifier")) {
                        throw new CreateFailure(
                            "Registration of emitsidentifier not supported.",
                            NOT_IMPLEMENTED.value);
                            
                    } else if (portType.equals("publishesidentifier")) {
                        throw new CreateFailure(
                            "Registration of publishesidentifier not supported.",
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


