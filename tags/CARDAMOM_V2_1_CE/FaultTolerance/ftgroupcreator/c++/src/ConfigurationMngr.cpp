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

#include "ftgroupcreator/ConfigurationMngr.hpp"
#include "Foundation/common/Assert.hpp"

#include <sstream>
#include <dom/DOM.hpp>
#include <parsers/DOMParser.hpp>
#include <util/PlatformUtils.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#include "Foundation/orbsupport/CORBA.hpp"

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
inline 
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
ConfigurationMngr::ConfigurationMngr(const char * name)
    throw( OutOfMemoryException )
{
    try
    {
        m_systemDaemonPort = "0";
        m_duration_time = 0;
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
        if (!doc.getDoctype().getName().equals("CdmwFTGroupCreator"))
        {
            CDMW_THROW2(ConfigurationException,
                        "The XML file used is not the good type of file for the FT Group Creator",
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
            if (node.getNodeName().equals("SystemDaemonPort"))
            {
                m_systemDaemonPort = get_value(node);
            }
            else if (node.getNodeName().equals("RequestDurationTime"))
            {
                std::string time = get_value(node);
                m_duration_time = ::atoi(time.c_str());
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


const char* ConfigurationMngr::getSystemDaemonPort() throw()
{
    return m_systemDaemonPort.c_str();
};

size_t ConfigurationMngr::getRequestDurationTime() throw()
{
    return m_duration_time;
}




} // End namespace FT
} // End namespace Cdmw


