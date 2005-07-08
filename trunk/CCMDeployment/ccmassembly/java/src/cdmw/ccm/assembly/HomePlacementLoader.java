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
import com.thalesgroup.CdmwDeployment._ComponentPackage;

import org.omg.Components.Deployment.InstallationFailure;
import org.omg.Components.CreateFailure;
import com.thalesgroup.CdmwDeployment.PACKAGE_MANAGER_ERROR;
import com.thalesgroup.CdmwDeployment.COMPONENT_PACKAGE_NOT_INSTALLED;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CSD;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CCD;
import com.thalesgroup.CdmwDeployment.SEVERAL_IMPLEMENTATIONS_IN_PACKAGE;
import com.thalesgroup.CdmwDeployment.UNKOWN_IMPL_ID;
import com.thalesgroup.CdmwDeployment.IMPL_ENTRY_POINT_NOT_FOUND;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValue;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValue;

import org.omg.Components.Configurator;
import com.thalesgroup.CdmwDeployment.HomeConfigurator;

import cdmw.ccm.deployment.CcdReader;
import cdmw.ccm.deployment.CsdReader;

import cdmw.xmlutils.XMLHelper;
import com.thalesgroup.CdmwXML.NodeDescription;
import com.thalesgroup.CdmwXML.Element;
import com.thalesgroup.CdmwXML.ElementDescription;
import com.thalesgroup.CdmwXML.ElementHelper;
import com.thalesgroup.CdmwXML.AttrDescription;
import com.thalesgroup.CdmwXML.DOMException;
import com.thalesgroup.CdmwXML.XPathFormatException;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;


/**
 * This class loads homeplacement XML elements
 * and create new corresponding Home object and Container object.
 */
class HomePlacementLoader {

    private static final String XPATH_COMPONENTFILEREF_IDREF =
        "componentfileref/@idref";

    private static final String XPATH_COMPONENTIMPLREF_IDREF =
        "componentimplref/@idref";

    private static final String XPATH_ALL_IMPLEMENTATION_UUID =
        "/softpkg/implementation/@id";

    private static final String XPATH_REGISTER_WITH_NAMING =
        "registerwithnaming/@name";

    /**
     * ComponentInstantiationLoader to be used for
     * componentinstantiation XML elements loading.
     */
    private ComponentInstantiationLoader componentInstantiationLoader;

    
    /**
     * Constructor.
     */
    public HomePlacementLoader() {
        this.componentInstantiationLoader = new ComponentInstantiationLoader();
    }
    
    /**
     * Loads a homeplacement XML element, create corresponding Container
     * and Home, add it respectively to its parent ComponentServer, to
     * its parent Container, and both to the assembly.
     *
     * @param homePlacElt the homeplacement XML element
     * @param assemblyPack the AssemblyPackage corresponding to the parent Assembly.
     * @param parentServer the parent ComponentServer of the Container.
     *
     * @exception CreateFailure if loading fails.
     */ 
    void load(Element homePlacElt,
              _AssemblyPackage assemblyPack,
              ComponentServer parentServer)
        throws CreateFailure
    {
        // get id of homeplacement
        String id = null;
        try {
            AttrDescription idDescr = homePlacElt.get_attribute(
                XMLHelper.toShortArray("id"));
            id = XMLHelper.toString(idDescr.value());
        } catch (DOMException e) {
            // shouldn't happen, id required and cad validated by dtd
            cdmw.common.Assert.check(false);
        }

        // get value of idref attribute of <componentfileref> element
        String componentfile_id = null;
        try {
            NodeDescription[] nodeDescr = homePlacElt.select_nodes(
                XMLHelper.toShortArray(XPATH_COMPONENTFILEREF_IDREF));
            // nodeDescr cannot be empty (dtd validation of cad assert this)
            AttrDescription idrefDescr = (AttrDescription) nodeDescr[0];
            componentfile_id = XMLHelper.toString(idrefDescr.value());

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // get corresponding ComponentPackage
        _ComponentPackage compPack = null;
        try {
            compPack = assemblyPack.get_component_package(componentfile_id);
        } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
            // shouldn't happen: cad is not conform with the assembly
            // that as been installed by the PackageManager
            cdmw.common.Assert.check(false);
        }

        // get location of csd file and create CsdReader
        String csdLocation = null;
        CsdReader csdReader;
        try {
            csdLocation = compPack.get_CSD_location();
            csdReader = new CsdReader(AssemblyFactoryImpl.getInstance().getXMLParser(), 
                                      csdLocation);
        } catch (InstallationFailure e) {
            throw new CreateFailure(
                "ComponentPackage referenced as " + componentfile_id +
                " in .cad is not fully installed by PackageManager.",
                COMPONENT_PACKAGE_NOT_INSTALLED.value);
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            throw new CreateFailure(
                "PackageManager cannot create location for .csd of component package " +
                compPack.get_name() + " " + compPack.get_version(),
                PACKAGE_MANAGER_ERROR.value);
        } catch (ParsingException e) {
            throw new CreateFailure(
                "Error while parsing " + csdLocation + " : " + e.reason,
                ERROR_IN_CSD.value);
        }

        // get location of ccd file and create CcdReader
        String ccdLocation = null;
        CcdReader ccdReader;
        try {
            ccdLocation = compPack.get_CCD_location();
            ccdReader = new CcdReader(AssemblyFactoryImpl.getInstance().getXMLParser(), 
                                      ccdLocation);
        } catch (InstallationFailure e) {
            throw new CreateFailure(
                "ComponentPackage referenced as " + componentfile_id +
                " in .cad is not fully installed by PackageManager.",
                COMPONENT_PACKAGE_NOT_INSTALLED.value);
        } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
            throw new CreateFailure(
                "ComponentPackage referenced as " + componentfile_id +
                " in .cad is not fully installed by PackageManager.",
                COMPONENT_PACKAGE_NOT_INSTALLED.value);
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            throw new CreateFailure(
                "PackageManager cannot create location for .ccd of component package " +
                compPack.get_name() + " " + compPack.get_version(),
                PACKAGE_MANAGER_ERROR.value);
        } catch (ParsingException e) {
            throw new CreateFailure(
                "Error while parsing " + ccdLocation + " : " + e.reason,
                ERROR_IN_CCD.value);
        }

        // load container corresponding to homeplacement
        // (automatically added to its parentServer and
        // to its parent assembly)
        Container container = loadContainer(id, 
                                            parentServer,
                                            ccdReader); 
   
        Home home = loadHome(id, 
                             homePlacElt, 
                             csdReader, 
                             ccdReader,
                             assemblyPack,
                             compPack,
                             container);
        
        // close CsdReader and CcdReader
        csdReader.close();
        ccdReader.close();

        // get all <componentinstantiation> children of <homeplacement> element
        NodeDescription[] parts = null;
        try {
            parts = homePlacElt.select_nodes(
                XMLHelper.toShortArray("componentinstantiation"));

        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        for (int i = 0; i < parts.length; i++) {
            ElementDescription eltDescr = (ElementDescription) parts[i];
            Element elt = ElementHelper.narrow(eltDescr.reference());

            // load componentinstantiation
            componentInstantiationLoader.load(elt, assemblyPack, home);
        }
    }

    
    /**
     * Loads a Container accroding to properties set into
     * a .ccd file.
     *
     * @param id the identifier of the Container.
     * @param parentServer parent ComponentServer of the Container.
     * @param ccdReader A CcdReader reading a .ccd file.
     *
     * @exception CreateFailure if loading fails
     */
    private Container loadContainer(String id,
                                    ComponentServer parentServer,
                                    CcdReader ccdReader)
        throws CreateFailure
    {
        // load componentkind from .ccd
        ComponentKindValue kind = ccdReader.getComponentKind();

        // try to find an existing container with the same config values
        Container container = parentServer.findContainer(kind);
        if (container == null) {
            container = new Container(id + "_container", parentServer, kind);
        }
        
        return container;
    }
    
    /**
     * Loads a Home according to properties set in a homeplacement
     * XML element, in a .csd file and in a .ccd file.
     *
     * @param id the identifier of the homeplacement.
     * @param homePlacElt the homeplacement XML element.
     * @param csdReader a CsdReader to the .csd file  
     *        corresponding to the homeplacement.
     * @param csdReader a CcdReader to the .ccd file  
     *        corresponding to the homeplacement.
     * @param assemblyPack the AssemblyPackage corresponding to the parent Assembly.
     * @param compPack the ComponentPackage corresponding to the home.
     * @param parentContainer the parent Container of the home.
     *
     * @exception CreateFailure if loading fails.
     */
    private Home loadHome(String id,
                          Element homePlacElt,
                          CsdReader csdReader,
                          CcdReader ccdReader,
                          _AssemblyPackage assemblyPack,
                          _ComponentPackage compPack,
                          Container parentContainer)
        throws CreateFailure
    {
        // get UUID of component implementation
        String implUUID = loadImplUUID(homePlacElt, csdReader, compPack);
        
        // get entrypoint of component implementation
        String entrypoint = csdReader.getEntrypoint(implUUID);
        if (entrypoint == null) {
            throw new CreateFailure(
                "No entry point specified in implementation " + implUUID +
                " of component package " +
                compPack.get_name() + " " + compPack.get_version(),
                IMPL_ENTRY_POINT_NOT_FOUND.value);
        }
        
        // create new Home (automatically added to its  
        // parentContainer and to its parent assembly).
        Home home = new Home(id, parentContainer, implUUID, entrypoint);
        
        // read <configurationcomplete> element (if exists)
        boolean configurationComplete = ccdReader.getConfigurationcomplete();
        home.setConfigurationComplete(configurationComplete);

        // load ConfigValues for Home
        loadHomeConfigValues(home, ccdReader, csdReader);

        // load homeproperties
        loadHomeProperties(home,
                           home.getParentAssembly().getConfiguratorManager(),
                           assemblyPack, 
                           compPack);

        // load default componentproperties
        loadDefaultComponentProperties(home, 
                                       home.getParentAssembly().getConfiguratorManager(),
                                       assemblyPack, 
                                       compPack);

        // load naming registrations
        loadNamingRegistrations(home, homePlacElt);
        
        return home;
    }                            


    /**
     * Loads from a .csd file, the UUID corresponding to a homeplacement.
     *
     * @param homePlacElt the homeplacement XML element.
     * @param csdReader a CsdReader to the .csd file  
     *        corresponding to the homeplacement.
     * @param compPack the ComponentPackage corresponding to the home.
     *
     * @exception CreateFailure if loading fails.
     */
    private String loadImplUUID(Element homePlacElt, 
                                CsdReader csdReader,
                                _ComponentPackage compPack)
        throws CreateFailure
    {
        // get value of idref attribute of <componentimplref> element (if exists)
        String implUUID = null;
        try {
            NodeDescription[] nodeDescr = homePlacElt.select_nodes(
                XMLHelper.toShortArray(XPATH_COMPONENTIMPLREF_IDREF));
            if (nodeDescr.length != 0) {
                AttrDescription idrefDescr = (AttrDescription) nodeDescr[0];
                implUUID = XMLHelper.toString(idrefDescr.value());
            }
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }

        // if componentimplref is not specified, try to get the uuid of
        // the unique implementation in .csd file
        if (implUUID == null) {
            try {
                NodeDescription[] nodeDescr =
                    csdReader.selectNodes(XPATH_ALL_IMPLEMENTATION_UUID);

                if (nodeDescr.length == 1) {
                    AttrDescription uuidDescr = (AttrDescription) nodeDescr[0];
                    implUUID = XMLHelper.toString(uuidDescr.value());
                } else {
                    throw new CreateFailure(
                        "The component package " + compPack.get_name() + " " +
                        compPack.get_version() + " has more than one implementation. " +
                        "Please, specify which implementation with <componentimplref>.",
                        SEVERAL_IMPLEMENTATIONS_IN_PACKAGE.value);
                }
            } catch (XPathFormatException e) {
                // shouldn't happen, xpath is valid
                cdmw.common.Assert.check(false);
            }

        } else {
            // check if the implementation implUUID is specified in .csd
            try {
                NodeDescription[] nodeDescr = csdReader.selectNodes(
                    "/softpkg/implementation[@id=\"" + implUUID + "\"]");
                if (nodeDescr.length == 0) {
                    throw new CreateFailure(
                        "UUID " + implUUID + " doesn't correspond to an " +
                        "implementation in component package " +
                        compPack.get_name() + " " + compPack.get_version(),
                        UNKOWN_IMPL_ID.value);
                }
            } catch (XPathFormatException e) {
                // shouldn't happen, xpath is valid
                cdmw.common.Assert.check(false);
            }
        }   
        
        return implUUID;
    }
    
    
    /**
     * Loads configuration values of a home, 
     * according to its .csd file and its .ccd file.
     *
     * @param home The home to apply configuration values.
     * @param csdReader a CsdReader to the .csd file  
     *        corresponding to the homeplacement.
     * @param csdReader a CcdReader to the .ccd file  
     *        corresponding to the homeplacement.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadHomeConfigValues(Home home,
                                      CcdReader ccdReader,
                                      CsdReader csdReader)
        throws CreateFailure
    {
        // load componentkind from .ccd
        ComponentKindValue kind = ccdReader.getComponentKind();
        home.setComponentKind(kind);

        if ((kind == ComponentKindValue.SESSION) ||
            (kind == ComponentKindValue.PROCESS) ||
            (kind == ComponentKindValue.ENTITY))
        {
            // load servant lifetime from .ccd
            ServantLifetimeValue lifetime = ccdReader.getServantLifetime();
            home.setServantLifetime(lifetime);
        }

        // load threading policy from .ccd
        ThreadingPolicyValue policy = ccdReader.getThreadingPolicy();
        home.setThreadingPolicy(policy);

        // load Home RepositoryId from .ccd
        String homeRepid = ccdReader.getHomeRepositoryId();
        home.setHomeRepositoryId(homeRepid);

        // load Component RepositoryId from .ccd
        String compRepid = ccdReader.getComponentRepositoryId();
        home.setComponentRepositoryId(compRepid);

        // load Home servant classname from .ccd
        String homeServantClassname = ccdReader.getHomeServantClassname(homeRepid);
        home.setHomeServantClassname(homeServantClassname);
        
        // load valuetypefactory dependencies from .csd
        try {
            // get all <dependency><valuetypefactory> for this implementation
            NodeDescription[] nodes = csdReader.selectNodes(
                "/softpkg/implementation[@id=\"" + home.getUUID() + 
                "\"]/dependency/valuetypefactory");
           
            for (int i = 0; i < nodes.length; i++) {
                Element factoryElt = ElementHelper.narrow(nodes[i].reference());
                
                // get repid attribute of valuetypefactory
                String repid = null;
                try {
                    AttrDescription repidDescr = factoryElt.get_attribute(
                        XMLHelper.toShortArray("repid"));
                    repid = XMLHelper.toString(repidDescr.value());
                } catch (DOMException e) {
                    // shouldn't append, repid is required
                    cdmw.common.Assert.check(false);
                }

                // get factoryentrypoint attribute of valuetypefactory
                String entrypoint = null;
                try {
                    AttrDescription entryDescr = factoryElt.get_attribute(
                        XMLHelper.toShortArray("factoryentrypoint"));
                    entrypoint = XMLHelper.toString(entryDescr.value());
                } catch (DOMException e) {
                    // Cdmw need this attribute
                    throw new CreateFailure(
                        "Error in CSD of home " + home.getId() + ": " +
                        " the valuetypefactory dependency " + repid + 
                        " doesn't specify a factoryentrypoint attribute.",
                        ERROR_IN_CSD.value);
                }
                
                // add valuetypefactory dependency
                home.addValuetypeFactory(repid, entrypoint);                
            }
           
        } catch (XPathFormatException e) {
            // shouldn't append, xpath is valid
            cdmw.common.Assert.check(false);
        }
        
    }
        
    /** 
     * Loads a homeproperties XML element, and apply the corresponding
     * Configurator to the Home.
     *
     * @param home the Home to be configured by the homeproperties.
     * @param configManager the ConfiguratorManager to retrieve or create 
     *        a Configurator.
     * @param assemblyPackage the AssemblyPackage corresponding to the assembly.
     * @param compPack the ComponentPackage corresponding to the home.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadHomeProperties(Home home,
                                    ConfiguratorManager configManager,
                                    _AssemblyPackage assemblyPack,
                                    _ComponentPackage compPack)
        throws CreateFailure
    {
        // try to get home CPF for this homeplacement from AssemblyPackage
        String cpfLocation = null;
        try {
            cpfLocation = assemblyPack.get_home_CPF_location(home.getId());
        
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            // cpf has not been installed
            throw new CreateFailure(
                "Home properties file of homeplacement " + home.getId() +
                " has not been installed by PackageManager",
                PACKAGE_MANAGER_ERROR.value);
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            throw new CreateFailure(
                "PackageManager cannot create location for home .cpf" +
                " for homeplacement " + home.getId() + 
                " from assembly package " + assemblyPack.get_uuid(),
                PACKAGE_MANAGER_ERROR.value);
        
        } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
            // No homeproperties specified for this homeplacement.
            // try to get default home CPF from ComponentPackage
            
            try {
                cpfLocation = compPack.get_home_CPF_location();
                
            } catch (org.omg.Components.Deployment.InstallationFailure ex) {
                // cpf has not been installed
                throw new CreateFailure(
                    "Home properties file of component package " + 
                    compPack.get_name() + " " + compPack.get_version() +
                    " has not been installed by PackageManager",
                    PACKAGE_MANAGER_ERROR.value);
            } catch (com.thalesgroup.CdmwDeployment.LocationCreationException ex) {
                throw new CreateFailure(
                    "PackageManager cannot create location for home .cpf" +
                    " from component package " + 
                    compPack.get_name() + " " + compPack.get_version(),
                    PACKAGE_MANAGER_ERROR.value);
            
            } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced ex) {
                // No default homeproperties specified for ComponentPackage.
                // do nothing
                return;
            }
        }

        System.out.println("                  home configuration: " + cpfLocation);

        HomeConfigurator homeConfigurator =
            configManager.getHomeConfigurator(home.getHomeRepositoryId(),
                                              cpfLocation);
        
        home.setHomeConfigurator(homeConfigurator);
    }
    
    /** 
     * Loads a componentproperties XML element, and apply the corresponding
     * Configurator to the Home.
     *
     * @param home the Home to be configured by the componentproperties.
     * @param configManager the ConfiguratorManager to retrieve or create 
     *        a Configurator.
     * @param assemblyPackage the AssemblyPackage corresponding to the assembly.
     * @param compPack the ComponentPackage corresponding to the home.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadDefaultComponentProperties(Home home,
                                                ConfiguratorManager configManager,
                                                _AssemblyPackage assemblyPack,
                                                _ComponentPackage compPack)
        throws CreateFailure
    {
        // try to get component CPF for this homeplacement from AssemblyPackage
        String cpfLocation = null;
        try {
            cpfLocation = assemblyPack.get_component_CPF_location(home.getId());
        
        } catch (org.omg.Components.Deployment.InstallationFailure e) {
            // cpf has not been installed
            throw new CreateFailure(
                "Component properties file of homeplacement " + home.getId() +
                " has not been installed by PackageManager",
                PACKAGE_MANAGER_ERROR.value);
        } catch (com.thalesgroup.CdmwDeployment.LocationCreationException e) {
            throw new CreateFailure(
                "PackageManager cannot create location for component .cpf" +
                " for homeplacement " + home.getId() + 
                " from assembly package " + assemblyPack.get_uuid(),
                PACKAGE_MANAGER_ERROR.value);
        
        } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced e) {
            // No componentproperties specified for this homeplacement.
            // try to get default component CPF from ComponentPackage
            
            try {
                cpfLocation = compPack.get_component_CPF_location();
                
            } catch (org.omg.Components.Deployment.InstallationFailure ex) {
                // cpf has not been installed
                throw new CreateFailure(
                    "Component properties file of component package " + 
                    compPack.get_name() + " " + compPack.get_version() +
                    " has not been installed by PackageManager",
                    PACKAGE_MANAGER_ERROR.value);
            } catch (com.thalesgroup.CdmwDeployment.LocationCreationException ex) {
                throw new CreateFailure(
                    "PackageManager cannot create location for component .cpf" +
                    " from component package " + 
                    compPack.get_name() + " " + compPack.get_version(),
                    PACKAGE_MANAGER_ERROR.value);
            
            } catch (com.thalesgroup.CdmwDeployment.FileNotReferenced ex) {
                // No default componentproperties specified for ComponentPackage.
                // do nothing
                return;
            }
        }

        System.out.println("               default components configuration: " + cpfLocation);

        Configurator configurator =
            configManager.getComponentConfigurator(home.getComponentRepositoryId(),
                                                   cpfLocation);
        
        home.setComponentConfigurator(configurator);
    }

    /**
     * Loads registercomponent XML elements which are child of a 
     * homeplacement XML element, and add corresponding 
     * naming registration informations to the Home. 
     *
     * @param component the Home to add naming registrations.
     * @param homePlacElt the homeplacement XML element corresponding 
     *        to the Home.
     *
     * @exception CreateFailure if loading fails.
     */
    private void loadNamingRegistrations(Home home,
                                         Element homePlacElt)
        throws CreateFailure
    {
        try {
            // get name attributes of all <registerwithnaming> child elements
            NodeDescription[] nodes = homePlacElt.select_nodes(
                XMLHelper.toShortArray(XPATH_REGISTER_WITH_NAMING));
                
            for (int i = 0; i < nodes.length; i++) {
                AttrDescription nameDescr = (AttrDescription) nodes[i];
                String name = XMLHelper.toString(nameDescr.value());
                
                // add naming registration
                home.addNamingRegistration(name);
            }
                
        } catch (XPathFormatException e) {
            // shouldn't happen, xpath is valid
            cdmw.common.Assert.check(false);
        }
            
    }
    
                                 
}

