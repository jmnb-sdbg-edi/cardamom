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


#include <cstdlib>

#include "platformdaemon/ConfigurationMngr.hpp"
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
namespace PlatformMngt
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
ConfigurationMngr::ConfigurationMngr(long port)
    throw( OutOfMemoryException )
{
    try
    {
        std::ostringstream buffer;

        if (port > 0)
        {
            buffer << port;
        }
        else
        {
            buffer << CdmwPlatformMngt::DEFAULT_SYSTEM_PORT;
        }

        m_systemPort = buffer.str();

        m_UID = -1;
        m_GID = -1;

        m_iorFile  = "CdmwPlatformDaemon.ior";

        m_notificationCallTimeout = 5000; // 5 seconds
        
        m_pidDirectory = ""; 
        m_duration_time = 20000000;
        m_pull_corbaloc = "corbaloc:miop:1.0@1.0-cdmwftdomain-1/225.1.1.8:4556";    
        m_push_corbaloc = "corbaloc:miop:1.0@1.0-cdmwftdomain-2/225.1.1.8:4557";


    }
    catch (std::bad_alloc& )
    {
        CDMW_THROW(OutOfMemoryException);
    }
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
            else if (node.getNodeName().equals("UID"))
            {
                std::string id = get_value(node);
                m_UID = ::atoi(id.c_str());
            }
            else if (node.getNodeName().equals("GID"))
            {
                std::string id = get_value(node);
                m_GID = ::atoi(id.c_str());
            }
            else if (node.getNodeName().equals("LogFile"))
            {
                m_logFile = get_value(node);
            }
            else if (node.getNodeName().equals("IorFile"))
            {
                m_iorFile = get_value(node);
            }
            else if (node.getNodeName().equals("NotificationCallTimeout"))
            {
                std::string timeoutStr = get_value(node);
                m_notificationCallTimeout = ::atoi(timeoutStr.c_str());
            }
            else if (node.getNodeName().equals("ORBSvcConf"))
            {
                m_svcConf = get_value(node);
            }
            else if (node.getNodeName().equals("ProcessesIdDirectory"))
            {
                m_pidDirectory = get_value(node);
            }
            else if (node.getNodeName().equals("RequestDurationTime"))
            {
                std::string time = get_value(node);
                m_duration_time = ::atoi(time.c_str());
            }
            else if (node.getNodeName().equals("SimplePullCorbaloc"))
            {
                m_pull_corbaloc = get_value(node);
            }
            else if (node.getNodeName().equals("SimplePushCorbaloc"))
            {
                m_push_corbaloc = get_value(node);
            }

            // Next Element to be processed
            node = walker.nextSibling();
        }

        delete parser;

        XMLPlatformUtils::Terminate();
	
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

};


const char* ConfigurationMngr::getSystemPortString() const throw()
{
    return m_systemPort.c_str();
};


int ConfigurationMngr::getUID() const throw()
{
    return m_UID;
};


int ConfigurationMngr::getGID() const throw()
{
    return m_GID;
};


const char* ConfigurationMngr::getLogFile() const throw()
{
    if (m_logFile.empty())
        return NULL;
    else
        return m_logFile.c_str();
}


const char* ConfigurationMngr::getIorFile() const throw()
{
    return m_iorFile.c_str();
}


unsigned long ConfigurationMngr::getNotificationCallTimeout() const throw()
{
    return m_notificationCallTimeout;
}
         
const char* ConfigurationMngr::getSvcConf() const throw()
{
    if (m_svcConf.empty())
        return NULL;
    else
        return m_svcConf.c_str();
}

const char* ConfigurationMngr::getProcessesIdDirectory() throw()
{
    if (m_pidDirectory == "") {
        m_pidDirectory = "/tmp/CdmwDaemon_" + m_systemPort;
    }
    
    return m_pidDirectory.c_str();
}

size_t ConfigurationMngr::getRequestDurationTime() throw()
{
    return m_duration_time;
}


const char* ConfigurationMngr::getPullCorbaloc() throw()
{
    return m_pull_corbaloc.c_str();    
}


const char* ConfigurationMngr::getPushCorbaloc() throw()
{
    return m_push_corbaloc.c_str();    
}


} // End namespace PlatformMngt
} // End namespace Cdmw


