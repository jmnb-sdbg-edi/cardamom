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


#ifndef INCL_CDMW_PLATFORMMNGT_CDMWINITCONFIGURATION_HPP 
#define INCL_CDMW_PLATFORMMNGT_CDMWINITCONFIGURATION_HPP 

#include <string>
#include "Foundation/common/Exception.hpp"

#include <dom/DOM.hpp>

class DOMParser;
class ErrorHandler;


namespace Cdmw
{
namespace PlatformMngt
{


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
*Purpose:
*<p>
* This class serves as a facility for accessing to configuration information
* that is provided in an XML file and that is needed by the CDMW init.
*/
class CdmwInitConfiguration 
{

public:

        /**
        * Purpose:
        * <p> Constructs a CdmwInitConfiguration
        * by initializing the underlying XML parser.
        * 
        */ 
//        CdmwInitConfiguration()
//            throw(ConfigurationException, InternalErrorException);


        /**
        * Purpose:
        * <p>
        * Cleans up the underlying XML parser.
        */ 
        virtual 
        ~CdmwInitConfiguration()
                throw();


        /**
        * Purpose:
        * <p> Sets up the init configuration with the
        * specified XML configuration file
        * 
        *@param XMLConfigurationFile the XML configuration file
        */ 
        static 
        CdmwInitConfiguration * 
        ReadConfiguration(const std::string& XMLConfigurationFile,
                          bool do_validate)
            throw(ConfigurationException, OutOfMemoryException, InternalErrorException);


        /**
        * Purpose:
        * <p> Gets the value of the specified service's attribute
        * 
        *@param service   The service that owns the attribute
        *@param attribute The attribute whose value is queried
        *@return The value of the specified attribute
        */ 
        std::string 
        getServiceAttributeValue(const std::string& service, const std::string& attribute)
            throw (BadParameterException,XMLErrorException,OutOfMemoryException);


        /**
        * Purpose:
        * <p> Gets the value of the specified factory's attribute corresponding to :
        * - the specified factory type
        * - specified service's attribute
        * 
        *@param factoryType     The factory type
        *@param factoryInstance The index of the factory instance
        *@param property        The property
        *@param attribute       The attribute
        *@return The value of the specified attribute
        */ 
        std::string 
        getFrameworkAttributeValue(const std::string& framework,
                                   size_t frameworkInstance,
                                   const std::string& attribute)
            throw (BadParameterException, XMLErrorException, OutOfMemoryException);


        /**
        * Purpose:
        * <p> Gets the property (the unique attributeof the specified factory.
        value of the specified factory's attribute corresponding to :
        * - the specified factory type
        * - specified service's attribute
        * 
        *@param factoryType     The factory type
        *@param factoryInstance The index of the factory instance
        *@param property        The property
        *@param attribute       The attribute
        *@return The value of the specified attribute
        */ 
        std::string 
        getFrameworkMultithreadAttribute(const std::string& factoryType,
                                         size_t factoryInstance, 
                                         std::string& threadingPolicy, 
                                         std::string& threadingAttribute)
            throw (BadParameterException, XMLErrorException, OutOfMemoryException);

        /**
        * Purpose:
        * <p> Gets the value of the specified datastore's attribute corresponding to :
        * - the specified datastore type
        * - specified service's attribute
        * 
        *@param datastoreName     The datastore name
        *@param datastoreInstance The index of the datastore instance
        *@param attribute       The attribute
        *@return The value of the specified attribute
        */ 
        std::string 
        getDataStoreAttributeValue(const std::string& datastoreName,
                                   int datastoreInstance,
                                   const std::string& attribute)
            throw (BadParameterException, XMLErrorException, OutOfMemoryException);

        /**
        * Purpose:
        * <p> Gets the value of the specified datastores attribute.
        * 
        *@param attribute       The attribute
        *@return The value of the specified attribute
        */ 
        std::string 
        getDataStoresAttributeValue(const std::string& attribute)
            throw (BadParameterException, XMLErrorException,
                   OutOfMemoryException);


protected:
       /**
        * Purpose:
        * <p> Constructs a CdmwInitConfiguration
        * by initializing the underlying XML parser.
        * 
        */
        CdmwInitConfiguration(bool do_validate = false)
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
    DOM_Node findNode(const DOM_Node& startingNode, const std::string& element, int occurrence = 1)
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
    * The DOM parser
    */ 
    DOMParser *m_parser;

    /**
    * The DOM error handler
    */ 
    ErrorHandler *m_errorReporter;

    /**
    * The parent node for all services. This node
    * is the starting point for the getServiceAttribute method.
    */ 
    DOM_Node m_servicesNode;

    /**
    * The parent node for all factories. This node
    * is the starting point for the getFactoryAttribute method.
    */ 
    DOM_Node m_lifeCycleServiceNode;

    /**
    * The parent node for all datastores. This node
    * is the starting point for the getDataStoreAttribute method.
    */ 
    DOM_Node m_DataStoreServiceNode;

private:

}; // End class CdmwInitConfiguration 

}; // End namespace PlatformMngt
}; // End namespace Cdmw

#endif // INCL_CDMW_PLATFORMMNGT_CDMWINITCONFIGURATION_HPP

