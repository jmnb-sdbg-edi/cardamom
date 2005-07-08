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

import cdmw.ccm.configurator.GenericConfiguratorImpl;
import cdmw.ccm.deployment.ConfigValues;
import cdmw.ccm.deployment.CpfReader;
import com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException;

import org.omg.Components.CreateFailure;
import org.omg.Components.Configurator;
import org.omg.Components.StandardConfigurator;
import com.thalesgroup.CdmwDeployment.HomeConfigurator;
import com.thalesgroup.CdmwDeployment.StandardHomeConfigurator;
import com.thalesgroup.CdmwDeployment.StandardConfiguratorFactory;
import com.thalesgroup.CdmwDeployment.GenericConfigurator;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CPF;
import com.thalesgroup.CdmwDeployment.ERROR_IN_CAD;


/**
 * This class creates and manages component and home configurators.
 * Each Configurator is identified by a .cpf file location and by the
 * RepositoryId of the object it configures.
 * For the same .cpf location and RepositoryId, the same Configurator
 * is always returned.
 */
class ConfiguratorManager {

    /**
     * Reference to the XMLParser.
     */
    private com.thalesgroup.CdmwXML.XMLParser parser;

    /**
     * ORB instance.
     */
    private org.omg.CORBA.ORB orb;
    
    /**
     * Reference to the StandardConfiguratorFactory.
     * It is able to create ComponentConfigurators and HomeConfigurators.
     */
    private StandardConfiguratorFactory configuratorFactory;
    
    /**
     * Map of HomeConfigurators, indexed by a .cpf location and a RepositoryId
     */
    private java.util.Map homeConfigurators;

    /**
     * Map of ComponentConfigurators, indexed by a .cpf location and a RepositoryId
     */
    private java.util.Map compConfigurators;
    
    
    /**
     * Constructor.
     *
     * @param parser reference to the XMLParser.
     * @param orb ORB instance.
     * @param configuratorFactory reference to the StandardConfiguratorFactory.
     */
    public ConfiguratorManager(com.thalesgroup.CdmwXML.XMLParser parser,
                               org.omg.CORBA.ORB orb,
                               StandardConfiguratorFactory configuratorFactory)
    {
        this.parser = parser;
        this.orb = orb;
        this.configuratorFactory = configuratorFactory;
        homeConfigurators = new java.util.Hashtable();
        compConfigurators = new java.util.Hashtable();
    }


    /**
     * Returns a Configurator to configure a component with the specified RepositoryId,
     *  and according to the properties from the specified .cpf file.
     *
     * @param componentRepid RepositoryId of the component to be configured
     *        by the Configurator.
     * @param cpfLocation location of the .cpf file to be used by the Configurator.
     *
     * @exception CreateFailure if their is an error in .cpf file,
     *            or if it cannot configure a component with the specified
     *            RepositoryId.
     */
    public Configurator getComponentConfigurator(String componentRepid,
                                                 String cpfLocation) 
        throws CreateFailure
    {
        // System.out.println(">>>>> getComponentConfigurator: " + componentRepid + cpfLocation);
        
        // check if a configurator has already been created for this cpfLocation
        StandardConfigurator configurator = 
            (StandardConfigurator) compConfigurators.get(componentRepid + " " + cpfLocation);
        
        if (configurator == null) {
            // get properties from cpf
            ConfigValues properties = readProperties(componentRepid, cpfLocation);
            
            try {
                // create a new StandardConfigurator
                configurator = configuratorFactory.create_configurator(componentRepid);
            } catch (org.omg.Components.WrongComponentType e) {
                throw new CreateFailure(e.getMessage(),
                                        ERROR_IN_CAD.value);
            }
            
            // set properties to configurator
            configurator.set_configuration(properties.getConfigValueArray());

            // store configurator
            compConfigurators.put(componentRepid + " " + cpfLocation, configurator);
            
        }
        
        return configurator;
    }
    
    
    /**
     * Returns a Configurator to configure a home with the specified RepositoryId,
     *  and according to the properties from the specified .cpf file.
     *
     * @param componentRepid RepositoryId of the home to be configured
     *        by the Configurator.
     * @param cpfLocation location of the .cpf file to be used by the Configurator.
     *
     * @exception CreateFailure if their is an error in .cpf file,
     *            or if it cannot configure a home with the specified
     *            RepositoryId.
     */
    public HomeConfigurator getHomeConfigurator(String homeRepid,
                                                String cpfLocation) 
        throws CreateFailure
    {
        //System.out.println(">>>>> getHomeConfigurator: " + homeRepid + cpfLocation);
        
        // check if a configurator has already been created for this cpfLocation
        StandardHomeConfigurator configurator = 
            (StandardHomeConfigurator) homeConfigurators.get(homeRepid + " " + cpfLocation);
            
        if (configurator == null) {
            // get properties from cpf
            ConfigValues properties = readProperties(homeRepid, cpfLocation);
            
            try {
                // create a new GenericConfigurator
                configurator = configuratorFactory.create_home_configurator(homeRepid);
            } catch (org.omg.Components.WrongComponentType e) {
                throw new CreateFailure(e.getMessage(),
                                        ERROR_IN_CAD.value);
            }
            
            // set home properties to configurator
            configurator.set_home_configuration(properties.getConfigValueArray());
            
            // store configurator
            homeConfigurators.put(homeRepid + " " + cpfLocation, configurator);
        }
        
        return configurator;
    }
    
    /**
     * Reads the .cpf file and return the corresponding 
     * configuration values.
     *
     * @param repid the RepositoryId of the object to be configured.
     * @param the location of the .cpf file.
     *
     * @exception if their is an error in .cpf file,
     *            or if it cannot configure a home with the specified
     *            RepositoryId.
     */
    private ConfigValues readProperties(String repid,
                                        String cpfLocation)
        throws CreateFailure
    {
        // read cpf file
        ConfigValues properties;
        try {
            // create CpfReader
            CpfReader cpfReader = new CpfReader(parser,
                                                cpfLocation,
                                                orb,
                                                repid);
                              
            // get ConfigValues
            properties = cpfReader.getConfigValues();
            
            // close CpfReader
            cpfReader.close();
            
        } catch (ParsingException e) {
            throw new CreateFailure(
                "Error while reading " + cpfLocation + " :\n" + e.reason,
                ERROR_IN_CPF.value);
        }
        
        return properties;
    }
    
    /**
     * Remove all Configurators managed by this ConfiguratorManager.
     */
    public void removeConfigurators() {
        // remove HomeConfigurators
        java.util.Iterator it = homeConfigurators.entrySet().iterator();
        
        while (it.hasNext()) {
            java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
            StandardHomeConfigurator configurator = (StandardHomeConfigurator) entry.getValue();
            
            // try to remove configurator if it's a GenericConfigurator
            try {
                GenericConfigurator genConfigurator = 
                    com.thalesgroup.CdmwDeployment.GenericConfiguratorHelper.narrow(configurator);
                if (genConfigurator != null) 
                    genConfigurator.remove();
            } catch (org.omg.CORBA.SystemException ex) {
                // ignore
            }
            
            // remove entry from Hashtable
            it.remove();
        }

        // remove ComponentConfigurators
        it = compConfigurators.entrySet().iterator();
        
        while (it.hasNext()) {
            java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
            StandardConfigurator configurator = (StandardConfigurator) entry.getValue();
            
            // try to remove configurator if it's a GenericConfigurator
            try {
                GenericConfigurator genConfigurator = 
                    com.thalesgroup.CdmwDeployment.GenericConfiguratorHelper.narrow(configurator);
                if (genConfigurator != null) 
                    genConfigurator.remove();
            } catch (org.omg.CORBA.SystemException ex) {
                // ignore
            }
            
            // remove entry from Hashtable
            it.remove();
        }
    }
    
}








