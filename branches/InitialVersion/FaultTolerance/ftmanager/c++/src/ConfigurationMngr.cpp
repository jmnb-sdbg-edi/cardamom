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


#include <cstdlib>

#include "ftmanager/ConfigurationMngr.hpp"
#include "Foundation/common/Assert.hpp"

#include <sstream>
#include <dom/DOM.hpp>
#include <parsers/DOMParser.hpp>
#include <util/PlatformUtils.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#include "Foundation/orbsupport/CORBA.hpp"
#include "idllib/CdmwPlatformMngtCommon.stub.hpp"

namespace Cdmw
{
namespace FT
{

/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/

class  ErrorReporter : public ErrorHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    ErrorReporter()
    {
    }

    ~ErrorReporter()
    {
    } 


    // Each error reported by the parsing is translated into an exception to be
    //  catch in the main procedure
    void warning(const SAXParseException& ex) { 
        throw ex; 
    }

    void error(const SAXParseException& ex)
    {
        throw ex;
    }

    void fatalError(const SAXParseException& ex)
    {
        throw ex;

    }

    void resetErrors() { 
    }
};





//
// Convert a Xerces string into a C++ string
std::string to_string(const DOMString &s)
{
    char *c_str = s.transcode();

    std::string str(c_str);

    delete c_str;
    
    return str;

}



//
// Returns the value associated to an Element node
//  
inline 
std::string
get_value(const DOM_Node& node)
{
    DOMString domString = node.getFirstChild().getNodeValue();
    return to_string( domString );

}



//
// Ctor
//
ConfigurationMngr::ConfigurationMngr()
    throw( OutOfMemoryException )
{
    try
    {
        
        m_duration_time = 0;
        m_domain_id = "Cdmw_domain";        
        m_simple_pull_corbaloc = "no_corbaloc";    
        m_simple_push_corbaloc = "no_corbaloc";
        m_mutual_pull_corbaloc = "no_corbaloc";    
        m_mutual_push_corbaloc = "no_corbaloc";
        m_localisation_service_corbaloc = "no_corbaloc";

        m_master_interrogation = "0";
        m_slave_acknowledgement = "0";
        m_master_confirmation = "0";
        m_master_election = "0";        
        m_daemon_acknowledgement = "0";

        m_state_tranfert_domain = "ftdomain";
        m_state_tranfert_IP_address = "225.1.1.8";
        m_cohort_timeout.seconds = 2;
        m_cohort_timeout.microseconds = 0;
        m_coordinator_timeout.seconds = 2;
        m_coordinator_timeout.microseconds = 0;

        m_transaction_in_progress = 100;
        m_transaction_done = 100;
        m_security_level = "SAFE_SECURITY_LEVEL";
        m_replication_level = "STRICT";
    }

    catch (std::bad_alloc& )
    {
        CDMW_THROW(OutOfMemoryException);
    }
}



std::list<std::string> ConfigurationMngr::tokenize(std::string str, std::string delims)
{
    std::string::size_type start_index, end_index;
    std::list<std::string> ret;
    // Skip leading delimiters, to get to the first token
    start_index = str.find_first_not_of(delims);
    
    // While found a beginning of a new token
    //
    while (start_index != std::string::npos)
    {
        // Find the end of this token
        end_index = str.find_first_of(delims, start_index);
        
        // If this is the end of the string
        if (end_index == std::string::npos)
            end_index = str.length();
        
        if (str.substr(start_index, end_index - start_index) != "\n")
            {
                std::string res = str.substr(start_index, end_index - start_index);
                std::string res2;
                if (res.find('\n') > 0 && res.find('\n') < res.size())
                    {
                        res2 = res.replace(res.find('\n'), 1, "");
                        res = res2;
                    }
                ret.push_back(res);
            }
        
        // Find beginning of the next token
        start_index = str.find_first_not_of(delims, end_index);
    }

    return ret;
}

void ConfigurationMngr::parse(const std::string& xml_conf, bool validate)
    throw(OutOfMemoryException, ConfigurationException)
{
    using namespace std;            
    
    try {
    
        // Xerces initialisation
        XMLPlatformUtils::Initialize();

        // We parse the configuration file
        DOMParser *parser = new DOMParser;
      if (validate)
        {
           parser->setValidationScheme(DOMParser::Val_Auto);
            parser->setDoNamespaces(false);
        }
        ErrorReporter errorReporter;
       parser->setErrorHandler( &errorReporter );

        parser->parse( xml_conf.c_str() );

        DOM_Document doc = parser->getDocument();
       
        // test if the dtd used for the xml file is the good
        if (!doc.getDoctype().getName().equals("CdmwFaultToleranceManager"))
        {
            CDMW_THROW2(ConfigurationException,
                        "The XML file used is not the good type of file for the FT Manager",
                        0 );
        }
        

        // We build a TreeWalker to skip all comment and automatically resolve
        // the entities
        DOM_TreeWalker walker = doc.createTreeWalker(doc,
            DOM_NodeFilter::SHOW_ELEMENT |
            DOM_NodeFilter::SHOW_ATTRIBUTE |
            DOM_NodeFilter::SHOW_TEXT |
            DOM_NodeFilter::SHOW_DOCUMENT_TYPE,
            NULL, true);

        // The first child is the DTD definition of the XML data, we skip it
        walker.firstChild();
      
        // We obtain the "CdmwPlatformMngtDaemon" node
        walker.nextSibling();

        // Each element configuration is under the root node,
        // so we get the first one
        DOM_Node node = walker.firstChild();

        // We can iter on each configuration element
        while (node != NULL) {

            if (node.getNodeName().equals("SystemPort"))
            {
                m_systemPort = get_value(node);
            }
            else if (node.getNodeName().equals("RequestDurationTime"))
            {
                std::string time = get_value(node);
                m_duration_time = ::atoi(time.c_str());
            }
            else if (node.getNodeName().equals("FaultToleranceDomainID"))
            {
               m_domain_id  = get_value(node);
            }
            else if (node.getNodeName().equals("SimplePullCorbaloc"))
            {
                m_simple_pull_corbaloc = get_value(node);
            }
            else if (node.getNodeName().equals("SimplePushCorbaloc"))
            {
                m_simple_push_corbaloc = get_value(node);
            }
            else if (node.getNodeName().equals("MutualPullCorbaloc"))
            {
                m_mutual_pull_corbaloc = get_value(node);
            }
            else if (node.getNodeName().equals("MutualPushCorbaloc"))
            {
                m_mutual_push_corbaloc = get_value(node);
            }
            else if (node.getNodeName().equals("LocalisationServiceAddress"))
            {
                m_localisation_service_corbaloc = get_value(node);
            }
            else if (node.getNodeName().equals("MasterInterrogationTime"))
            {
                m_master_interrogation = get_value(node);
            }
            else if (node.getNodeName().equals("SlaveAcknowledgementTime"))
            {
                m_slave_acknowledgement = get_value(node);
            }
            else if (node.getNodeName().equals("MasterConfirmationTime"))
            {
                m_master_confirmation = get_value(node);
            }
            else if (node.getNodeName().equals("MasterElectionTime"))
            {
                m_master_election = get_value(node);
            }
            else if (node.getNodeName().equals("DaemonAcknowledgementTime"))
            {
                m_daemon_acknowledgement = get_value(node);
            }
            else if (node.getNodeName().equals("ORBSvcConf"))
            {
                m_svcConf = get_value(node);
            }
            else if (node.getNodeName().equals("StateTransfertDomain"))
            {
                m_state_tranfert_domain = get_value(node);
            }
            else if (node.getNodeName().equals("StateTransfertIPAddress"))
            {
                m_state_tranfert_IP_address = get_value(node);
            }
            else if (node.getNodeName().equals("CohortTimeOut"))
            {
                std::string time = get_value(node);
                m_cohort_timeout.seconds = (::atoi(time.c_str())/1000);
                m_cohort_timeout.microseconds = ((::atoi(time.c_str())%1000)*1000);
            }
            else if (node.getNodeName().equals("CoordinatorTimeOut"))
            {
                std::string time = get_value(node);
                m_coordinator_timeout.seconds = (::atoi(time.c_str())/1000);
                m_coordinator_timeout.microseconds = ((::atoi(time.c_str())%1000)*1000);
            }
            else if (node.getNodeName().equals("MaxTransactionInProgress"))
            {
                std::string nb = get_value(node);
                m_transaction_in_progress = ::atoi(nb.c_str());
            }
            else if (node.getNodeName().equals("MaxTransactionDone"))
            {
                std::string nb = get_value(node);
                m_transaction_done = ::atoi(nb.c_str());
            }
            else if (node.getNodeName().equals("SecurityLevel"))
            {
                m_security_level = get_value(node);
            }
            else if (node.getNodeName().equals("ReplicationLevel"))
            {
                m_replication_level = get_value(node);
            }
            else if (node.getNodeName().equals("SupportedInterface"))
            {
                std::list<std::string> mylist = tokenize(to_string(node.getChildNodes().item(1).getFirstChild().getNodeValue()), " ");
                m_supportedInterfacesMap.insert(make_pair(to_string(node.getAttributes().getNamedItem("typename").getNodeValue()),mylist));
            }

            // Next Element to be processed
            node = walker.nextSibling();
        }

        delete parser;

	
    }
    catch(const SAXParseException& ex1)
    {
       CDMW_THROW2(ConfigurationException,
            to_string(DOMString(ex1.getMessage())), ex1.getLineNumber());
    }
    catch(const XMLException& ex2)
    {
        CDMW_THROW2(ConfigurationException,
           to_string(DOMString(ex2.getMessage())), ex2.getSrcLine());

    }
    catch ( std::bad_alloc& )
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


ConfigurationMngr::~ConfigurationMngr()
    throw()
{
        XMLPlatformUtils::Terminate();

};


const char* ConfigurationMngr::getSystemPortString() const throw()
{
    return m_systemPort.c_str();
};

const char* ConfigurationMngr::getDomainID() throw()
{
    return m_domain_id.c_str();    
}


size_t ConfigurationMngr::getRequestDurationTime() throw()
{
    return m_duration_time;
}


const char* ConfigurationMngr::getSimplePullCorbaloc() throw()
{
    return m_simple_pull_corbaloc.c_str();    
}


const char* ConfigurationMngr::getSimplePushCorbaloc() throw()
{
    return m_simple_push_corbaloc.c_str();    
}

const char* ConfigurationMngr::getMutualPullCorbaloc() throw()
{
    return m_mutual_pull_corbaloc.c_str();    
}


const char* ConfigurationMngr::getMutualPushCorbaloc() throw()
{
    return m_mutual_push_corbaloc.c_str();    
}


const char* ConfigurationMngr::getLocalisationServiceAddress() throw()
{
    return m_localisation_service_corbaloc.c_str();    
}

const char* ConfigurationMngr::getMasterInterrogationTime() throw()
{
    return m_master_interrogation.c_str();    
}


const char* ConfigurationMngr::getSlaveAcknowledgementTime() throw()
{
    return m_slave_acknowledgement.c_str();    
}


const char* ConfigurationMngr::getMasterConfirmationTime() throw()
{
    return m_master_confirmation.c_str();    
}


const char* ConfigurationMngr::getMasterElectionTime() throw()
{
    return m_master_election.c_str();    
}


const char* ConfigurationMngr::getDaemonAcknowledgementTime() throw()
{
    return m_daemon_acknowledgement.c_str();    
}


const char* ConfigurationMngr::getSvcConf() const throw()
{
    if (m_svcConf.empty())
        return NULL;
    else
        return m_svcConf.c_str();
}

const char* ConfigurationMngr::getStateTransfertDomain() throw()
{
    return m_state_tranfert_domain.c_str();   
}


const char* ConfigurationMngr::getStateTransfertIPAddress() throw()
{
    return m_state_tranfert_IP_address.c_str();
}


OsSupport::OS::Timeval ConfigurationMngr::getCohortTimeOut() throw()
{
    return m_cohort_timeout;
}


OsSupport::OS::Timeval ConfigurationMngr::getCoordinatorTimeOut() throw()
{
    return m_coordinator_timeout;
}

int ConfigurationMngr::getMaxTransactionInProgress() throw()
{
    return m_transaction_in_progress;
}

int ConfigurationMngr::getMaxTransactionDone() throw()
{
    return m_transaction_done;
}

const char* ConfigurationMngr::getSecurityLevel() throw()
{
    return m_security_level.c_str();
}

const char* ConfigurationMngr::getReplicationLevel() throw()
{
    return m_replication_level.c_str();
}

SupportedInterfacesMap ConfigurationMngr::getSupportedInterfaceMap() throw()
{
    return m_supportedInterfacesMap;
}


} // End namespace FT
} // End namespace Cdmw


