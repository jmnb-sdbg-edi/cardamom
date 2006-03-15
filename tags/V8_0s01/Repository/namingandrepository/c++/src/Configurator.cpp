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


#include "namingandrepository/Configurator.hpp"
#include "Foundation/common/Locations.hpp"
#include <iostream>
#include <memory>

#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <util/XMLUniDefs.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>

#include <dom/DOM_DOMException.hpp>

#include <parsers/DOMParser.hpp>
#include <dom/DOM.hpp>


// For the DOMTreeErrorReporter class
#include <util/XercesDefs.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>
#include <dom/DOMString.hpp>


namespace Cdmw
{
namespace NamingAndRepository
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*Code specific to Xerces XML Parser
*/


std::string to_string(const DOMString &s)
{
    char *c_str = s.transcode();

    std::string str(c_str);

    delete c_str;
    
    return str;

}


class DOMTreeErrorReporter : public ErrorHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    DOMTreeErrorReporter()
    {
    }

    ~DOMTreeErrorReporter()
    {
    } 


    // -----------------------------------------------------------------------
    //  Implementation of the error handler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& toCatch) { }

    void error(const SAXParseException& toCatch)
    {
        std::string reason(to_string(toCatch.getMessage()));

        CDMW_THROW1(XMLErrorException, reason);

    }

    void fatalError(const SAXParseException& toCatch)
    {

        error(toCatch);

    }

    void resetErrors() { }
};


/*
* The implementation of Configurator
*/
void Configurator::configure(const DOM_Node& node, const std::string& element)
    throw(ConfigurationException, InternalErrorException)
{

    try
    {

        DOM_NodeList children = node.getChildNodes();

        size_t nbChildren = children.getLength();

        for (size_t i = 0; i < nbChildren; ++i)
        {
            DOM_Node node = children.item(i);

            if (node.getNodeType() == DOM_Node::ELEMENT_NODE)
            {

                std::string nodeName(to_string(node.getNodeName()));
            
                if (nodeName.compare(element) == 0)
                {

                    // get the name of the root context
                    DOM_NamedNodeMap attributes = node.getAttributes();

                    DOM_Node nameAttrib = attributes.getNamedItem("name");

                    // configure the child
                    configureChild(node);

                }

            }

        }

    }
    catch (const XMLException& e)
    {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    }
    catch (const DOM_DOMException& e)
    {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

}


/*
* The implementation of RootContextConfigurator
*/
RootContextConfigurator::RootContextConfigurator(CosNaming::NamingContextExt_var rootContext)
    throw(InternalErrorException)
{
    m_rootContext = CosNaming::NamingContextExt::_duplicate(rootContext.in());
}


void RootContextConfigurator::configureChild(const DOM_Node& node)
{

    try
    {

        // get the name of the child
        DOM_NamedNodeMap attributes = node.getAttributes();

        DOM_Node nameAttrib = attributes.getNamedItem("name");

        std::string childName(to_string(nameAttrib.getNodeValue()));
        // create the child
        CdmwNamingAndRepository::NameDomain_var nameDomain;
        NameDomain_impl* nameDomain_i = NULL;

        nameDomain_i = NameDomain_impl::createNameDomain(m_rootContext.in(), childName, nameDomain.out());

        // configure the child 
        NameDomainConfigurator childConfigurator(nameDomain_i);
        childConfigurator.configure(node, "NameDomain");

    }
    catch (const XMLException& e)
    {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    }
    catch (const DOM_DOMException& e)
    {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }
    catch(const AlreadyExistsException&)
    {
        CDMW_THROW1(ConfigurationException, "Duplicate name domain");
    }
    catch(const BadParameterException&)
    {
        CDMW_THROW1(ConfigurationException, "Invalid domain name");
    }
    catch(const ConfigurationException &)
    {
        throw;
    }
    catch(const Cdmw::Exception &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


/*
* The implementation of NameDomainConfigurator
*/
NameDomainConfigurator::NameDomainConfigurator(Cdmw::NamingAndRepository::NameDomain_impl* nameDomain_i)
    throw(InternalErrorException)
{
    m_nameDomain_i = nameDomain_i;
}


void NameDomainConfigurator::configureChild(const DOM_Node& node)
{

    try
    {

        // get the name of the child
        DOM_NamedNodeMap attributes = node.getAttributes();

        DOM_Node nameAttrib = attributes.getNamedItem("name");

        std::string childName(to_string(nameAttrib.getNodeValue()));

        // create the child
        NameDomain_impl* subNameDomain_i = NULL;
        subNameDomain_i = m_nameDomain_i->createSubNameDomain(childName);


        // configure the child 
        NameDomainConfigurator childConfigurator(subNameDomain_i);
        childConfigurator.configure(node, "NameDomain");

    }
    catch (const XMLException& e)
    {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    }
    catch (const DOM_DOMException& e)
    {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }
    catch(const AlreadyExistsException&)
    {
        CDMW_THROW1(ConfigurationException, "Duplicate name domain");
    }
    catch(const BadParameterException&)
    {
        CDMW_THROW1(ConfigurationException, "Invalid domain name");
    }
    catch(const ConfigurationException &)
    {
        throw;
    }
    catch(const Cdmw::Exception &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


/*
* The implementation of RepositoryConfigurator
*/
RepositoryConfigurator::RepositoryConfigurator(CdmwNamingAndRepository::Repository_ptr repository,
    bool validate)
    throw(InternalErrorException)
{

    m_repository = CdmwNamingAndRepository::Repository::_duplicate(repository);

    // Initialize the XML4C2 system
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException& toCatch)
    {
        std::cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << to_string(toCatch.getMessage()) << std::endl;

        CDMW_THROW(InternalErrorException);
    }

    try
    {
        //
        //  Create our parser, then attach an error handler to the parser.
        //  The parser will call back to methods of the ErrorHandler if it
        //  discovers errors during the course of parsing the XML document.
        //
        std::auto_ptr<DOMParser> pParser(new DOMParser());
        if (validate)
        {
            pParser->setValidationScheme(DOMParser::Val_Auto);
            pParser->setDoNamespaces(false);
        }

        std::auto_ptr<DOMTreeErrorReporter> pErrorReporter(new DOMTreeErrorReporter());
        (pParser.get())->setErrorHandler(pErrorReporter.get());

        m_parser = pParser.release();
        m_errorReporter = pErrorReporter.release();

    }
    catch(const std::bad_alloc &)
    {

        XMLPlatformUtils::Terminate();
        CDMW_THROW(InternalErrorException);

    }

}


RepositoryConfigurator::~RepositoryConfigurator()
{

    delete m_errorReporter;
    delete m_parser;

    XMLPlatformUtils::Terminate();

}


void RepositoryConfigurator::configureChild(const DOM_Node& node)
{

    try
    {

        // get the name of the child
        DOM_NamedNodeMap attributes = node.getAttributes();

        DOM_Node nameAttrib = attributes.getNamedItem("name");

        std::string childName(to_string(nameAttrib.getNodeValue()));
        // create the child
        CosNaming::NamingContextExt_var rootContext;
        
        if (childName.compare(Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID) != 0)
        {
            
            if (childName.compare(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT) != 0)
            {
                // If the root context differs from the default root context,
                // create it
                rootContext = RootNamingContext_impl::createContext(childName);
            }
            else
            {
                CosNaming::NamingContext_var temp_rootContext =
                m_repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
                rootContext = CosNaming::NamingContextExt::_narrow(temp_rootContext.in());
            }
            
            // configure the child 
            RootContextConfigurator childConfigurator(rootContext);
            childConfigurator.configure(node, "NameDomain");
        }
    }
    catch (const XMLException& e)
    {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    }
    catch (const DOM_DOMException& e)
    {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }
    catch(const AlreadyExistsException&)
    {
        CDMW_THROW1(ConfigurationException, "Duplicate root context");
    }
    catch(const ConfigurationException &)
    {
        throw;
    }
    catch(const Cdmw::Exception &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


void RepositoryConfigurator::configure(const std::string& XMLConfigurationFile)
    throw(ConfigurationException, InternalErrorException)
{

    try
    {

        m_parser->parse(XMLConfigurationFile.c_str());

        DOM_Document document = m_parser->getDocument();

        DOM_Element rootElement = document.getDocumentElement();

        Configurator::configure(rootElement, "RootContext");

    }
    catch (const XMLException& e)
    {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    }
    catch (const DOM_DOMException& e)
    {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

}


} // End namespace namingandrepository
} // End namespace Cdmw

