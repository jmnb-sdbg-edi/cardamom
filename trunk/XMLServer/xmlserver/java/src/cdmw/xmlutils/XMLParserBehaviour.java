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


package cdmw.xmlutils;

import cdmw.common.Locations;
import cdmw.orbsupport.ORBSupport;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.cdmwinit.InitUtils;
import cdmw.platformmngt.ProcessBehaviour;

import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwNamingAndRepository.NameDomain;
import com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT;

/**
 * CDMW XML Server behaviour.
 *
 */
public class XMLParserBehaviour extends ProcessBehaviour {

    /**
     * Reference to the current ORB
     */
    private org.omg.CORBA.ORB orb;

    /**
     * The Root POA
     */
    private org.omg.PortableServer.POA rootPOA;

    /**
     * Command line arguments
     */
    private String[] args;

    /**
     * XML Parser Name Domain
     */
    private NameDomain parserDomain;

    /**
     * The XML parser object;
     */
    private com.thalesgroup.CdmwXML.XMLParser parser;

    /**
     * Constructor
     *
     * @param orb The initialised ORB.
     * @param poa The Root POA reference
     */
    public XMLParserBehaviour(org.omg.CORBA.ORB orb, 
                              org.omg.PortableServer.POA poa, 
                              String[] args) {
        this.orb = orb;
        this.rootPOA = poa;
        this.args = args;
    }


    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
     * operation
     */
    public org.omg.CORBA.Object getService() {
        return parser;
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {

        // Create the parser
        this.parser = XMLParserImpl.init(orb, rootPOA, args);

        // Bind the XML Parser to a Corbaloc name
        ORBSupport.bindObjectToCorbaloc(
            orb, XMLParserImpl.XML_PARSER_CORBALOC_ID, parser);

        // Register parser in the Name Service
        registerParser();
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    public void run()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun {
        // nothing to do
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    public void stop() {
        ORBSupport.removeCorbalocBinding(
            orb, XMLParserImpl.XML_PARSER_CORBALOC_ID);
        unregisterParser();
        orb.shutdown(false);
    }


    /**
     * Registers the parser in the Name Service.
     *
     * @param parser The XML parser object to be registered.
     */
    private void registerParser() {
        try {
            // Retrieve Repository object reference
            com.thalesgroup.CdmwNamingAndRepository.Repository repository
                = InitUtils.getCdmwRepository();

            // Init of RepositoryInterface
            String defaultDomainName= Locations.CDMW_SERVICES_NAME_DOMAIN;
            RepositoryInterface.init(defaultDomainName, repository);

            // Get the name domain from the repository
            parserDomain = repository.resolve_name_domain(defaultDomainName);

            // Register xmlParser in domain SERVICES
            parserDomain.register_new_object(
                XMLParserImpl.XML_PARSER_NAME, parser);

        } catch (cdmw.orbsupport.CORBASystemExceptionWrapper csew) {
            csew.getSystemException().printStackTrace();
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            nnd.printStackTrace();
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            in.printStackTrace();
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
            // Persistent Server
        }

    }

    /**
     * Releases the binding name of the parser in the Name Service.
     */
    private void unregisterParser() {
        try {
            parserDomain.release_name(XMLParserImpl.XML_PARSER_NAME);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            in.printStackTrace();
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered nr) {
            nr.printStackTrace();
        }
    }
}


