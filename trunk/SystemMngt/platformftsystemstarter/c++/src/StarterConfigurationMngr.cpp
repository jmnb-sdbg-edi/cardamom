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

#include "platformftsystemstarter/StarterConfigurationMngr.hpp"
#include "common/Assert.hpp"
#include "ossupport/OS.hpp"
#include <sstream>
#include <dom/DOM.hpp>
#include <parsers/DOMParser.hpp>
#include <util/PlatformUtils.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#include "orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"

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
StarterConfigurationMngr::StarterConfigurationMngr()
    throw (OutOfMemoryException)
{
    try
    {
        m_daemonHost = Cdmw::OsSupport::OS::get_hostname();

        std::ostringstream buffer;
        buffer << CdmwPlatformMngt::DEFAULT_SYSTEM_PORT;
 
        m_daemonPort = buffer.str();
        
        m_processName = "";
        m_processExecPath = "";
        m_processPort = "";
        m_processArgs = "";
        m_processRunDir = "";
    }
    catch (std::bad_alloc& )
    {
        CDMW_THROW(OutOfMemoryException);
    }
}


void StarterConfigurationMngr::parse(const std::string& xml_conf, bool validate)
    throw(OutOfMemoryException, StarterConfigurationException)
{
    using namespace std;            
    
    try 
    {
    
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
        while (node != NULL) 
        {

            if (node.getNodeName().equals("DaemonHost"))
            {
                m_daemonHost = get_value(node);
            }
            if (node.getNodeName().equals("DaemonPort"))
            {
                m_daemonPort = get_value(node);
            }
            else if (node.getNodeName().equals("ProcessName"))
            {
                m_processName = get_value(node);
            }
            else if (node.getNodeName().equals("ProcessExec-path"))
            {
                m_processExecPath = get_value(node);
            }
            else if (node.getNodeName().equals("ProcessPort"))
            {
                m_processPort = get_value(node);
            }
            else if (node.getNodeName().equals("ProcessArgs"))
            {
                m_processArgs = get_value(node);
            }
            else if (node.getNodeName().equals("ProcessWork-dir"))
            {
                m_processWorkDir = get_value(node);
            }
            else if (node.getNodeName().equals("ProcessRun-dir"))
            {
                m_processRunDir = get_value(node);
            }


            // Next Element to be processed
            node = walker.nextSibling();
        }

        delete parser;

        XMLPlatformUtils::Terminate();
	
    }
    catch(const SAXParseException& ex)
    {
        CDMW_THROW2(StarterConfigurationException,
            to_string(DOMString(ex.getMessage())), ex.getLineNumber());
    }
    catch(const XMLException& ex)
    {
        CDMW_THROW2(StarterConfigurationException,
           to_string(DOMString(ex.getMessage())), ex.getSrcLine());

    }
    catch ( std::bad_alloc& )
    {
        CDMW_THROW(OutOfMemoryException);
    }

}


StarterConfigurationMngr::~StarterConfigurationMngr()
    throw()
{

};


const char* StarterConfigurationMngr::getDaemonPortString() const throw()
{
    return m_daemonPort.c_str();
};

const char* StarterConfigurationMngr::getDaemonHostString() const throw()
{
    return m_daemonHost.c_str();
};


const char* StarterConfigurationMngr::getProcessName() const throw()
{
    return m_processName.c_str();
}

const char* StarterConfigurationMngr::getProcessExecPath() const throw()
{
    return m_processExecPath.c_str();
}

const char* StarterConfigurationMngr::getProcessPortString() const throw()
{
    return m_processPort.c_str();
}

const char* StarterConfigurationMngr::getProcessArgs() const throw()
{
    return m_processArgs.c_str();
}

const char* StarterConfigurationMngr::getProcessWorkDir() const throw()
{
    return m_processWorkDir.c_str();
}

const char* StarterConfigurationMngr::getProcessRunDir() const throw()
{
    return m_processRunDir.c_str();
}

      

} // End namespace PlatformMngt
} // End namespace Cdmw

