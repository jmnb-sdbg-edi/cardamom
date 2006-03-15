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


#ifndef INCL_EVENTADMIN_EVENTCHANNELCONFIGURATION_HPP 
#define INCL_EVENTADMIN_EVENTCHANNELCONFIGURATION_HPP 

#include <string>
#include <vector>
#include "Foundation/common/Exception.hpp"

#include <dom/DOM.hpp>

class DOMParser;
class ErrorHandler;


namespace Cdmw
{
namespace EventAdmin
{

/**
* Purpose:
* rootElement Name
*/
const std::string CONF_ROOT_ELEMENT_NAME="EventChannelsConfiguration";

/**
* Purpose:
* ConfigurationException.
*/
class ConfigurationException  : public Exception {
    
    public:
        ConfigurationException(const std::string& reason) 
            : Exception( Exception::SAFE, "Invalid configuration :" + reason )
        {
        }

    protected:

    private:

};

/**
* Purpose:
* XMLErrorException.
*/
class XMLErrorException  : public ConfigurationException {
    
    public:
        XMLErrorException(const std::string& reason) 
            : ConfigurationException("XML Error :" + reason)
        {
        }


    protected:

    private:

};

/**
* Purpose:
* ChannelProperties
*/
struct ChannelProperty {
    std::string name;
    std::string value;
};
        
typedef std::vector<ChannelProperty> ChannelProperties;
    

/**
*Purpose:
*<p>    
* This class serves as a facility for accessing to configuration information
* that is provided in an XML file and that is needed by the CDMW init.
*
*/
class EventChannelConfiguration 
{

    public:
    
      /**
      * Purpose:
      * <p>
      * Cleans up the underlying XML parser.
      */ 
      virtual 
      ~EventChannelConfiguration()
          throw();


      /**
      * Purpose:
      * <p> Sets up the init configuration with the
      * specified XML configuration file
      * 
      *@param XMLConfigurationFile the XML configuration file
      */ 
      static 
      EventChannelConfiguration * 
      readConfiguration(const std::string& XMLConfigurationFile,
                              bool         do_validate)
          throw(ConfigurationException, OutOfMemoryException, InternalErrorException);

      /**
      * Purpose:
      * <p>
      * Returns the event channel name at EventChannelIndex index 
      *
      *@param childIndex The index of the event channel name to get
        *
        *@exception BadParameterException if no name at index
      */ 
        std::string getNextEventChannelName(size_t EventChannelIndex)
            throw (BadParameterException,XMLErrorException,OutOfMemoryException);

        /**
        * Purpose:
      * <p>
      * Returns the number of event channel profile
      *
        */
        size_t getNbEventChannelProfile()
            throw (XMLErrorException);

      /**
      * Purpose:
      * <p>
      * Returns the event channel name at EventChannelIndex index 
      *
      *@param childIndex The index of the event channel name to get
        *
      *@return a NULL object if there is no profile for channelName event channel.
      */ 
        ChannelProperties* getEventChannelProfileValue(const std::string& channelName)
            throw (BadParameterException,XMLErrorException,OutOfMemoryException);



protected:
       /**
        * Purpose:
        * <p> Constructs a EventChannelConfiguration
        * by initializing the underlying XML parser.
        * 
        */
        EventChannelConfiguration(bool do_validate = false)
          throw(ConfigurationException, InternalErrorException);

        /**
        * Purpose:
        * <p> Sets up the init configuration with the
        * specified XML configuration file
        * 
        *@param XMLConfigurationFile the XML configuration file
        */ 
        void 
        readConfigurationFile(const std::string& XMLConfigurationFile)
            throw(XMLErrorException, OutOfMemoryException);

        /**
        * Purpose:
        * <p>
        * Finds the actual child node that is of the specified element type
        *
        *@param startingNode The node where to find the child
        *@param element      The element type
        *@return a NULL node if childIndex doesn't denote a valid child
        */ 
        DOM_Node findNode(const DOM_Node& startingNode, const std::string& element)
            throw(XMLErrorException, std::bad_alloc);

        /**
        * Purpose:
        * <p>
        * Returns the actual child node (ELEMENT TYPE) of the specified node.
        *
        *@param node       The parent of the child to get
        *@param childIndex The index of the child to get
        *@return a NULL node if childIndex doesn't denote a valid child
        */ 
        DOM_Node getActualChildNode(const DOM_Node& node, size_t childIndex)
            throw(XMLErrorException);

          
        /**
        * Purpose:
        * <p>
        * Returns the text value of the specified node (ELEMENT TYPE).
        *
        *@param node       The node containing a text node
        *@return the string value
          *@exception XMLErrorException if the specified node doesn't contain a text Node
        */ 
          std::string getNodeValue(const DOM_Node& node)
              throw(XMLErrorException);


    /**
    * The DOM parser
    */ 
    DOMParser *m_parser;

    /**
    * The DOM error handler
    */ 
    ErrorHandler *m_errorReporter;


    /**
    * The Parent Node for all EventChannelProfile nodes. This node
    * is the starting point for the getEventChannelName and
    * getEventChannelProperties methods.
    */ 
    DOM_Node m_eventChannelConfigurationNode;


private:

}; // End class EventChannelConfiguration 


}; // End namespace EventAdmin
}; // End namespace Cdmw
#endif // INCL_EVENTADMIN_EVENTCHANNELCONFIGURATION_HPP


