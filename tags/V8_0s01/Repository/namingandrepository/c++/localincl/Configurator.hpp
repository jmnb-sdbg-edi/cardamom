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


#ifndef INCL_NAMINGANDREPOSITORY_CONFIGURATOR_HPP 
#define INCL_NAMINGANDREPOSITORY_CONFIGURATOR_HPP 

#include "dom/DOM.hpp"

#include "namingandrepository/Repository_impl.hpp"
#include "namingandrepository/RootNamingContext_impl.hpp"
#include "namingandrepository/NameDomain_impl.hpp"
#include "Foundation/common/Exception.hpp"

class DOMParser;
class ErrorHandler;


namespace Cdmw
{
namespace NamingAndRepository
{

/**
* Purpose:
* ConfigurationException.
*/
class ConfigurationException  : public Exception {
    
    public:
        ConfigurationException(const std::string& reason) 
            : Exception( Exception::SAFE, "Invalid configuration" )
        {
            m_what = std::string(Exception::what()) + ":" + reason;
        }


        ~ConfigurationException() throw() {};

        virtual const char* what() const throw()
        {
            return m_what.c_str();
        }

    protected:

    private:
       std::string m_what;

};


/**
* Purpose:
* XMLErrorException.
*/
class XMLErrorException  : public ConfigurationException {
    
    public:
        XMLErrorException(const std::string& reason) 
            : ConfigurationException("XML Error" )
        {
            m_what = std::string(ConfigurationException::what()) + ":" + reason;
        }

        ~XMLErrorException() throw() {}; 

        virtual const char* what() const throw()
        {
            return m_what.c_str();
        }

    protected:

    private:
       std::string m_what;

};


/**
*Purpose:
*Abstract base configurator.
*Provides the ability to configure each child node.
*
*Features:
*<p>
*Thread safe
*
*/
class Configurator 
{

public:

    /**
    * The destructor
    */ 
    virtual 
    ~Configurator() { }

    /**
    * Purpose:
    * <p>
    * Configures each child of the given node 
    *
    *@param node The node to configure
    *@param element The element type of the child nodes 
    */ 
    void configure(const DOM_Node& node, const std::string& element)
        throw(ConfigurationException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Configures the specified child.
    * This method must be overriden.
    */ 
    virtual void configureChild(const DOM_Node& node) = 0;

}; // End class Configurator 




/**
*Purpose:
*The configurator for a root naming context.
*
*Features:
*<p>
*Thread safe
*
*/
class RootContextConfigurator : public Configurator 
{

public:

    /**
    * Purpose:
    * The constructor
    */ 
    RootContextConfigurator(CosNaming::NamingContextExt_var rootContext)
        throw(InternalErrorException);

    /**
    * Purpose:
    * The destructor
    */ 
    virtual 
    ~RootContextConfigurator() { }

    /**
    * Purpose:
    * <p>
    * Configures the specified child.
    */ 
    virtual void configureChild(const DOM_Node& node);

private:

    /**
    * The root context to configure
    */ 
    CosNaming::NamingContextExt_var m_rootContext;

}; // End class RootContextConfigurator



/**
*Purpose:
*The configurator for a name domain.
*
*Features:
*<p>
*Thread safe
*
*/
class NameDomainConfigurator : public Configurator 

{

public:

    /**
    * Purpose:
    * The constructor
    */ 
    NameDomainConfigurator(Cdmw::NamingAndRepository::NameDomain_impl* nameDomain_i)
        throw(InternalErrorException);


    /**
    * Purpose:
    * The destructor
    */ 
    virtual 
    ~NameDomainConfigurator() { }

    /**
    * Purpose:
    * <p>
    * Configures the specified child.
    */ 
    virtual void configureChild(const DOM_Node& node);

private:

    /**
    * The name domain to configure
    */ 
    Cdmw::NamingAndRepository::NameDomain_impl* m_nameDomain_i;

}; // End class NameDomainConfigurator


/**
*Purpose:
*The configurator for the repository.
*/
class RepositoryConfigurator : public Configurator 
{

public:

    /**
    * Purpose:
    * The constructor
    *
    *@param validate Indicates if the XML configuration file will be validated
    */ 
    RepositoryConfigurator(CdmwNamingAndRepository::Repository_ptr repository, bool validate)
        throw(InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Starts the configuration of the repository from the XML configuration file. 
    */ 
    void configure(const std::string& XMLConfigurationFile)
        throw(ConfigurationException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    * Configures the specified child.
    */ 
    virtual void configureChild(const DOM_Node& node);


    /**
    * Purpose:
    * The destructor
    */ 
    virtual 
    ~RepositoryConfigurator();


protected:

    /**
    * The DOM parser
    */ 
    DOMParser *m_parser;

    /**
    * The DOM error handler
    */ 
    ErrorHandler *m_errorReporter;

    /**
    * The repository to configure
    */ 
    CdmwNamingAndRepository::Repository_var m_repository;

private:

}; // End class RepositoryConfigurator

} // End namespace NamingAndRepository
} // End namespace Cdmw
#endif // INCL_NAMINGANDREPOSITORY_CONFIGURATOR_HPP

