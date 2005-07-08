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


#include "eventchannelmanager/EventChannelConfiguration.hpp"
#include "Foundation/common/Assert.hpp"

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
namespace EventAdmin
{

/**
*<p>
*Code specific to Xerces XML Parser
*/

    
std::string to_string(const DOMString &s)
    throw (std::bad_alloc)
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


EventChannelConfiguration::EventChannelConfiguration(bool do_validate)
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


EventChannelConfiguration::~EventChannelConfiguration()
    throw()
{
    delete m_errorReporter;
    delete m_parser;

    XMLPlatformUtils::Terminate();

}

DOM_Node 
EventChannelConfiguration::findNode(const DOM_Node& startingNode, 
                                 const std::string& element)
    throw(XMLErrorException, std::bad_alloc)
{

    DOM_Node nodeToFind;

    CDMW_ASSERT(!startingNode.isNull());

    try
    {
        DOM_NodeList children = startingNode.getChildNodes();

        size_t nbChildren = children.getLength();

        for (size_t i = 0; i < nbChildren; ++i)
        {
            DOM_Node node = children.item(i);

            if (node.getNodeType() == DOM_Node::ELEMENT_NODE)
            {

                std::string nodeName(to_string(node.getNodeName()));
            
                if (nodeName.compare(element) == 0)
                {

                    nodeToFind = node;
                    break;
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


DOM_Node EventChannelConfiguration::getActualChildNode(const DOM_Node& node, size_t childIndex)
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

std::string 
EventChannelConfiguration::getNodeValue(const DOM_Node& node)
    throw(XMLErrorException)
{
    DOM_Node textNode;
    std::string nodeValue;
    
    CDMW_ASSERT(!node.isNull());

    textNode = node.getFirstChild();
    
   if (textNode.getNodeType() == DOM_Node::TEXT_NODE)
    {
        nodeValue = to_string(textNode.getNodeValue());
    }
    else
    {
        std::string reason(std::string("No value for the node ") + to_string(node.getNodeName()));
      CDMW_THROW1(XMLErrorException, reason);
    }
    
    return nodeValue;
}



EventChannelConfiguration* 
EventChannelConfiguration::readConfiguration(const std::string& XMLConfigurationFile,
                                                           bool do_validate)
    throw(ConfigurationException, OutOfMemoryException, InternalErrorException)
{
    EventChannelConfiguration* conf = 0;
    try {
        conf = new EventChannelConfiguration(do_validate);
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


void EventChannelConfiguration::readConfigurationFile(const std::string& XMLConfigurationFile)
    throw(XMLErrorException, OutOfMemoryException)
{

    try
    {
        if (m_parser)
          {
           m_parser->parse(XMLConfigurationFile.c_str());

           DOM_Document document = m_parser->getDocument();

           DOM_Element rootElement = document.getDocumentElement();

              if (!rootElement.isNull())
              {
                  std::string nodeName(to_string(rootElement.getNodeName()));
            
                if (nodeName.compare(CONF_ROOT_ELEMENT_NAME) == 0)
                {
                         m_eventChannelConfigurationNode = rootElement;
                     }
                     else
                     {
                         std::string reason("Invalid root element in xml file");
                   CDMW_THROW1(XMLErrorException, reason);
                     }
              }
              else
              {
                  std::string reason("Root element missing in xml file");
                  CDMW_THROW1(XMLErrorException, reason);
              }
          }
    } catch (const std::bad_alloc & ) {
        CDMW_THROW(OutOfMemoryException);
    } catch (const XMLException& e) {
        std::string reason(to_string(e.getMessage()));
        CDMW_THROW1(XMLErrorException, reason);
    } catch (const DOM_DOMException& e) {
        std::string reason(to_string(e.msg));
        CDMW_THROW1(XMLErrorException, reason);
    }
     catch (const std::exception& e) {
         std::string reason(to_string(e.what()));
        CDMW_THROW1(XMLErrorException, reason);
     }
     catch (...)
     {
         std::string reason(to_string("Unexpected exception"));
         reason = reason + "file:" + __FILE__ ;
         CDMW_THROW1(XMLErrorException, reason);
     }

}


/**
*<p>
* This function return the name of the next event channel in the event channel configuration file.
*/
std::string 
EventChannelConfiguration::getNextEventChannelName(size_t EventChannelIndex)
    throw (BadParameterException,XMLErrorException,OutOfMemoryException)
{
   std::string eventChannelName;
    bool eventChannelProfileNodeFound=false;
    bool eventChannelNameFound=false;
    DOM_Node eventChannelProfileNode;

   CDMW_ASSERT(!m_eventChannelConfigurationNode.isNull());

   try
   {
        eventChannelProfileNode = getActualChildNode(m_eventChannelConfigurationNode, 
                                                     EventChannelIndex);

      if (!eventChannelProfileNode.isNull())
      {
         eventChannelProfileNodeFound = true;

         // search the event channel name
            DOM_Node eventChannelNameNode = findNode(eventChannelProfileNode, "ChannelName");

         if (!eventChannelNameNode.isNull())
         {
            // attribute found
            eventChannelNameFound = true;

            eventChannelName = getNodeValue(eventChannelNameNode);
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

   if (!eventChannelProfileNodeFound)
      CDMW_THROW2(BadParameterException, "Event channel profile not found for channel", to_string(EventChannelIndex));

   if (!eventChannelNameFound)
       CDMW_THROW2(BadParameterException, "Event channel name not found", to_string(EventChannelIndex));

   return eventChannelName;
}

/**
*<p>
* This function return the number of eventchannelprofile in the event
* channel configuration file.
*/
size_t 
EventChannelConfiguration::getNbEventChannelProfile()
    throw (XMLErrorException)
{
    CDMW_ASSERT(!m_eventChannelConfigurationNode.isNull());

    size_t nbProfile = 0;

   try
   {
       DOM_NodeList children = m_eventChannelConfigurationNode.getChildNodes();

       size_t nbChildren = children.getLength();

       for (size_t i = 0; i < nbChildren; ++i)
       {
           DOM_Node child = children.item(i);

           if (child.getNodeType() == DOM_Node::ELEMENT_NODE)
           {
                    // this is a profile!
                    nbProfile++;
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

   return nbProfile;
}


/**
*<p>
* This function return the name and the value of the event channel in the event
* channel configuration file.
*/

ChannelProperties*
EventChannelConfiguration::getEventChannelProfileValue(const std::string& channelName)
    throw (BadParameterException,XMLErrorException,OutOfMemoryException)
{

    ChannelProperties* channelProfile = NULL;
    int i_channel = 0;
     std::string currentChannelName;

     bool eventchannelProfileFound = false;

     // get the number of Event channel profile in XML doc
     DOM_NodeList profiles = m_eventChannelConfigurationNode.getChildNodes();
    int nbProfiles = profiles.getLength();

    try
    {
        // search for the right event channel profile
        for (i_channel=0; i_channel<nbProfiles; i_channel++)
        {
            currentChannelName = getNextEventChannelName(i_channel);

            if (currentChannelName == channelName)
            {
                // eventchannelProfile found
                eventchannelProfileFound = true;
                break;
            }
        }

        if (eventchannelProfileFound)
        {
            DOM_Node eventChannelProfile = getActualChildNode(m_eventChannelConfigurationNode,
                i_channel);
            
            DOM_Node propertyNode = findNode(eventChannelProfile, "Property");
            DOM_Node name;  // property name
            DOM_Node value; // property value

            ChannelProperty propertyStruct;
            channelProfile = new ChannelProperties();

            if (!propertyNode.isNull())
            {
               do
               {
                   name = findNode(propertyNode, "Name");
                   value = findNode(propertyNode,"Value");

                    if ((!name.isNull()) && (!value.isNull()))
                    {
                      propertyStruct.name = getNodeValue(name);
                      propertyStruct.value = getNodeValue(value);
                    }

                   channelProfile->push_back(propertyStruct);

                    // find next property node
                    do 
                    {
                      propertyNode = propertyNode.getNextSibling();
                    }
                    while ((!propertyNode.isNull()) &&
                             (propertyNode.getNodeType() != DOM_Node::ELEMENT_NODE));
               } 
               while (!propertyNode.isNull());
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

    if (!eventchannelProfileFound)
    {
        // eventchannelProfile not found
        CDMW_THROW2(BadParameterException, channelName, " event channel profile not found");
    }

    return channelProfile;

}

}; // End namespace EventAdmin
}; // End namespace Cdmw

