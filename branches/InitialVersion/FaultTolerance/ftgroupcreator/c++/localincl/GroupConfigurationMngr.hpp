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


#ifndef INCL_FTGROUPCREATOR_GROUP_CONFIGURATIONMNGR_IMPL_HPP
#define INCL_FTGROUPCREATOR_GROUP_CONFIGURATIONMNGR_IMPL_HPP 

#include <string>
#include <list>
#include <map>

#include "Foundation/common/Exception.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"


namespace Cdmw
{
namespace FT
{


class ConfigurationException : public Exception
{
public:

    ConfigurationException(const std::string& message, size_t lineNumber)
        : Exception( Exception::FATAL, "Invalid configuration file" ),
          m_message(message), m_lineNumber(lineNumber)
    {
    }

    ~ConfigurationException() throw() {};

    const char* getMessage() const
    {
        return m_message.c_str();
    }

    size_t getLineNumber() const
    {
        return m_lineNumber;
    }

private:
    std::string m_message;
    size_t m_lineNumber;

};

/**
*Purpose:
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class GroupConfigurationMngr
{

public:

    /**
     * Purpose:
     * <p> Constructor. Sets default values.
     * 
     */ 
    GroupConfigurationMngr(bool validate)
        throw();

    /**
     * Purpose:
     * <p> Constructor. Sets default values.
     * 
     */ 
    GroupConfigurationMngr(::CdmwFT::ReplicationManager_ptr replication_manager)
        throw();


    /**
     * Purpose:
     * <p> validate the configuration from an XML file.
     * 
     * @param xml_conf The XML file path
     * @param validate Validate the XML file before processing
     */ 
    void validate(const std::string& xml_conf)
        throw(OutOfMemoryException, ConfigurationException);

    /**
     * Purpose:
     * <p> Creates group the configuration from an XML file.
     * 
     * @param xml_conf The XML file path
     * @param validate Validate the XML file before processing
     */ 
    void parse(const std::string& xml_conf, bool validate)
        throw(OutOfMemoryException, ConfigurationException, CORBA::SystemException);


    /**
     * Purpose:
     * <p> Destructor
     * 
     */ 
    virtual 
    ~GroupConfigurationMngr()
        throw();

private:
    CdmwFT::ReplicationManager_var m_replication_manager;

}; // End class ConfigurationMngr 

} // End namespace FT
} // End namespace Cdmw
#endif // INCL_FTGROUPCREATOR_GROUP_CONFIGURATIONMNGR_IMPL_HPP

