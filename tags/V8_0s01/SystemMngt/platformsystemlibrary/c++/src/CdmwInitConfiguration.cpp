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

#include <iostream>
#include <memory>

#include "platformsystemlibrary/CdmwInitConfiguration.hpp"
#include "Foundation/common/Assert.hpp"

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
namespace PlatformMngt
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*Code specific to Xerces XML Parser
*/


std::string to_string(const DOMString &s)
    throw (std::bad_alloc)
{
    char *c_str = s.transcode();

    std::string str(c_str);

    delete[] c_str;
    
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
* The implementation of CdmwInitConfiguration
*/

CdmwInitConfiguration::CdmwInitConfiguration(bool do_validate)
    throw(ConfigurationException, InternalErrorException)
{
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

        if (do_validate){ // TODO: Chack exceptions to be thrown...
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


CdmwInitConfiguration::~CdmwInitConfiguration()
    throw()
{
    delete m_errorReporter;
    delete m_parser;

    XMLPlatformUtils::Terminate();
}


DOM_Node 
CdmwInitConfiguration::findNode(const DOM_Node& startingNode, 
                                const std::string& element,
                                int occurrence)
    throw(XMLErrorException, std::bad_alloc)
{
    DOM_Node nodeToFind;

    CDMW_ASSERT(!startingNode.isNull());

    try
    {
        DOM_NodeList children = startingNode.getChildNodes();

        size_t nbChildren = children.getLength();
        int index = 1;
        for (size_t i = 0; i < nbChildren; ++i)
        {
            DOM_Node node = children.item(i);

            if (node.getNodeType() == DOM_Node::ELEMENT_NODE)
            {
                std::string nodeName(to_string(node.getNodeName()));
                if (nodeName.compare(element) == 0)
                {
                    if (index == occurrence)
                    {                        
                        nodeToFind = node;
                        break;
                    } else {
                        index++;
                    }
                }
            }
        }
    } catch (std::bad_alloc &) {
        throw;
    } catch (const XMLException& e) {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    } catch (const DOM_DOMException& e) {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

    return nodeToFind;

}


DOM_Node CdmwInitConfiguration::getActualChildNode(const DOM_Node& node, size_t childIndex)
    throw(XMLErrorException)
{
    DOM_Node actualChild;
    size_t actualChildIndex = 0;

    CDMW_ASSERT(!node.isNull());

    try
    {
        DOM_NodeList children = node.getChildNodes();

        size_t nbChildren = children.getLength();

        for (size_t i = 0; i < nbChildren; ++i)
        {
            DOM_Node child = children.item(i);

            if (child.getNodeType() == DOM_Node::ELEMENT_NODE)
            {
                if (childIndex != actualChildIndex)
                {
                    ++actualChildIndex;
                }
                else
                {
                   actualChild = child;
                   break;
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

    return actualChild;
}


CdmwInitConfiguration* 
CdmwInitConfiguration::ReadConfiguration(const std::string& XMLConfigurationFile,
                                          bool do_validate)
    throw(ConfigurationException, OutOfMemoryException, InternalErrorException)
{
    CdmwInitConfiguration* conf = 0;
    try {
        conf = new CdmwInitConfiguration(do_validate);
        conf->readConfigurationFile(XMLConfigurationFile);
    } catch (const std::bad_alloc &) {
        CDMW_THROW(OutOfMemoryException);
    } catch (const OutOfMemoryException & ) {
        throw;
    } catch (const ConfigurationException & ) {
        throw;
    } catch (const InternalErrorException & ) {
        throw;
    }

    return conf;
}


void CdmwInitConfiguration::readConfigurationFile(const std::string& XMLConfigurationFile)
    throw(XMLErrorException, OutOfMemoryException)
{
    try
    {
        m_parser->parse(XMLConfigurationFile.c_str());

        DOM_Document document = m_parser->getDocument();

        DOM_Element rootElement = document.getDocumentElement();

        m_servicesNode = findNode(rootElement, "services");

        m_lifeCycleServiceNode = findNode(m_servicesNode, "lifecycle");

        DOM_Node fault_tolerance = findNode(m_servicesNode, "fault-tolerance");
        if (!fault_tolerance.isNull())
            m_DataStoreServiceNode = findNode(fault_tolerance, "datastores");

    } catch (const std::bad_alloc & ) {
        CDMW_THROW(OutOfMemoryException);
    } catch (const XMLException& e) {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    } catch (const DOM_DOMException& e) {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

}


std::string 
CdmwInitConfiguration::getServiceAttributeValue(const std::string& service, 
                                                 const std::string& attribute)
    throw (BadParameterException, XMLErrorException, OutOfMemoryException)
{
    bool serviceFound = false;
    bool attributeFound = false;

    std::string attributeValue;

    CDMW_ASSERT(!m_servicesNode.isNull());

    try
    {
        // search the specified service among the children nodes
        DOM_Node serviceNode = findNode(m_servicesNode, service);

        if (!serviceNode.isNull())
        {
            serviceFound = true;

            // search the specified attribute
            DOM_NamedNodeMap attributes = serviceNode.getAttributes();
            DOM_Node attribName = attributes.getNamedItem(attribute.c_str());

            if (!attribName.isNull())
            {
                // attribute found
                attributeFound = true;

                attributeValue = to_string(attribName.getNodeValue());
            }
        }
    } catch (const std::bad_alloc & ) {
        CDMW_THROW(OutOfMemoryException);
    } catch (const OutOfMemoryException & ) {
        throw;
    } catch (const XMLErrorException & ) {
        throw;
    } catch (const XMLException& e) {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    } catch (const DOM_DOMException& e) {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

    if (!serviceFound)
        CDMW_THROW2(BadParameterException, service, " service not found");

    if (!attributeFound)
        CDMW_THROW2(BadParameterException, attribute, " attribute not found");

    return attributeValue;

}


std::string 
CdmwInitConfiguration::getFrameworkAttributeValue(const std::string& framework,
                                                   size_t frameworkInstance, 
//                                                 const std::string& property, 
                                                   const std::string& attribute)
    throw (BadParameterException, XMLErrorException, OutOfMemoryException)
{

    bool frameworkFound = false;
    bool frameworkInstanceFound = false;
//    bool propertyFound = false;
    bool attributeFound = false;

    std::string attributeValue;

    CDMW_ASSERT(!m_lifeCycleServiceNode.isNull());

    try
    {
        // search the specified service among the children nodes
        DOM_Node frameworkNode = findNode(m_lifeCycleServiceNode, framework);

        if (!frameworkNode.isNull())
        {
            frameworkFound = true;

            // search the specified framework instance
            DOM_Node instance = getActualChildNode(frameworkNode, frameworkInstance);

            if (!instance.isNull())
            {
                frameworkInstanceFound = true;

/*
                // search the specified property
                DOM_Node propertyNode = findNode(instance, property);

                if (!propertyNode.isNull())
                {
                    propertyFound = true;
*/                    

                    // search the specified attribute
                    //DOM_NamedNodeMap attributes = propertyNode.getAttributes();
                    DOM_NamedNodeMap attributes = instance.getAttributes();
                    DOM_Node attribName = attributes.getNamedItem(attribute.c_str());

                    if (!attribName.isNull())
                    {
                        // attribute found
                        attributeFound = true;

                        attributeValue = to_string(attribName.getNodeValue());
                    }
//                }
            }
        }
    } catch (const std::bad_alloc &) {
        CDMW_THROW(OutOfMemoryException);
    } catch (OutOfMemoryException &) {
        throw;
    } catch (XMLErrorException & ) {
        throw;
    } catch (const XMLException& e) {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    } catch (const DOM_DOMException& e) {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

    if (!frameworkFound)
        CDMW_THROW2(BadParameterException, framework, " framework not found");

    if (!frameworkInstanceFound)
        CDMW_THROW2(BadParameterException, "frameworkInstance" , " invalid index");

/*
    if (!propertyFound)
        CDMW_THROW2(BadParameterException, property, " property not found");
*/

    if (!attributeFound)
        CDMW_THROW2(BadParameterException, attribute, " attribute not found");

    return attributeValue;
}


std::string 
CdmwInitConfiguration::getFrameworkMultithreadAttribute(const std::string& framework,
                                                         size_t frameworkInstance, 
                                                         std::string& threadingPolicy, 
                                                         std::string& threadingAttribute)
    throw (BadParameterException, XMLErrorException, OutOfMemoryException)
{

    bool frameworkFound = false;
    bool frameworkInstanceFound = false;
    bool multithreadFound = false;
    bool policyFound = false;

    std::string attributeValue;

    CDMW_ASSERT(!m_lifeCycleServiceNode.isNull());

    try {
        // search the specified service among the children nodes
        DOM_Node frameworkNode;
        frameworkNode = findNode(m_lifeCycleServiceNode, framework);

        if (!frameworkNode.isNull())
        {
            frameworkFound = true;

            // search the specified factory instance
            DOM_Node instance = getActualChildNode(frameworkNode, frameworkInstance);

            if (!instance.isNull())
            {
                frameworkInstanceFound = true;

                DOM_Node multithreadNode = findNode(instance, "multithread");

                if (!multithreadNode.isNull())
                {
                    multithreadFound = true;

                    DOM_Node policyNode = getActualChildNode(multithreadNode, 0);
                    if (!policyNode.isNull())
                    {
                        policyFound = true;
                        threadingPolicy = to_string(policyNode.getNodeName());

                        // search the unique attribute
                        DOM_NamedNodeMap attributes = policyNode.getAttributes();
                        DOM_Node attribute = attributes.item(0);

                        if (!attribute.isNull())
                        {
                            threadingAttribute = to_string(attribute.getNodeName());
                            attributeValue = to_string(attribute.getNodeValue());
                        }
                        else
                        {
                            // the threading attribute is irrelevant
                            threadingAttribute = "";
                            attributeValue = "";
                        }
                    }
                }
            }
        }
    } catch (const std::bad_alloc & ) {
        CDMW_THROW(OutOfMemoryException);
    } catch (const OutOfMemoryException & ) {
        throw;
    } catch (const XMLErrorException & ) {
        throw;
    } catch (const XMLException& e) {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    } catch (const DOM_DOMException& e) {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

    if (!frameworkFound)
        CDMW_THROW2(BadParameterException, framework, " framework type not found");

    if (!frameworkInstanceFound)
        CDMW_THROW2(BadParameterException, "frameworkInstance" , " invalid index");

    if (!multithreadFound)
        CDMW_THROW2(BadParameterException, framework , " not multithreaded factory type");

    if (!policyFound)
        CDMW_THROW2(BadParameterException, "Threading policy" , " policy not found");

    return attributeValue;
}




std::string 
CdmwInitConfiguration::getDataStoreAttributeValue(const std::string& datastoreName,
                                                  int datastoreInstance,
                                                  const std::string& attribute)
    throw (BadParameterException, XMLErrorException, OutOfMemoryException)
{
    
    bool datastoreFound = false;
    bool attributeFound = false;

    std::string attributeValue;
    CDMW_ASSERT(!m_DataStoreServiceNode.isNull());

    try
    {
        // search the specified service among the children nodes
        DOM_Node datastoreNode = findNode(m_DataStoreServiceNode, datastoreName, datastoreInstance);
        if (!datastoreNode.isNull())
        {
            datastoreFound = true;

            DOM_NamedNodeMap attributes = datastoreNode.getAttributes();            
            DOM_Node attribName = attributes.getNamedItem(attribute.c_str());

            if (!attribName.isNull())
            {
               // attribute found
                attributeFound = true;
                
                attributeValue = to_string(attribName.getNodeValue());
            } else {
                DOM_Node parentNode = datastoreNode.getParentNode();
                DOM_NamedNodeMap parentAttributes = parentNode.getAttributes();    
                DOM_Node parentAttribName = parentAttributes.getNamedItem(attribute.c_str());

                if (!parentAttribName.isNull())
                {
                    // attribute found
                    attributeFound = true;
                    
                    attributeValue = to_string(parentAttribName.getNodeValue());
                }
            }
        }
    } catch (const std::bad_alloc &) {
        CDMW_THROW(OutOfMemoryException);
    } catch (OutOfMemoryException &) {
        throw;
    } catch (XMLErrorException & ) {
        throw;
    } catch (const XMLException& e) {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    } catch (const DOM_DOMException& e) {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

    if (!datastoreFound)
        CDMW_THROW2(BadParameterException, datastoreName, " datastore not found");

    if (!attributeFound)
        CDMW_THROW2(BadParameterException, attribute, " attribute not found");

    return attributeValue;
}


std::string 
CdmwInitConfiguration::getDataStoresAttributeValue
(const std::string& attribute)
    throw (BadParameterException, XMLErrorException, OutOfMemoryException)
{
    std::string attributeValue;
    CDMW_ASSERT(!m_DataStoreServiceNode.isNull());

    try
    {
        // search the specified service among the children nodes
        DOM_NamedNodeMap attributes = m_DataStoreServiceNode.getAttributes();
        DOM_Node attribName = attributes.getNamedItem(attribute.c_str());

        if (attribName.isNull())
        {
            CDMW_THROW2(BadParameterException, attribute,
                        " attribute not found");
        }

        attributeValue = to_string(attribName.getNodeValue());
    } catch (const std::bad_alloc &) {
        CDMW_THROW(OutOfMemoryException);
    } catch (OutOfMemoryException &) {
        throw;
    } catch (XMLErrorException & ) {
        throw;
    } catch (const XMLException& e) {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    } catch (const DOM_DOMException& e) {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }

    return attributeValue;    
}


}; // End namespace PlatformMngt
}; // End namespace Cdmw

