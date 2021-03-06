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


#ifndef INCL_GROUP_CREATOR_CONFIGURATIONMNGR_IMPL_HPP 
#define INCL_GROUP_CREATOR_CONFIGURATIONMNGR_IMPL_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "ftgroupcreator/GroupConfigurationMngr.hpp"
#include <string>
#include <list>
#include <map>

namespace Cdmw
{
namespace FT
{

typedef std::map< std::string, std::list< std::string > > SupportedInterfacesMap;
    


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
class ConfigurationMngr
{

public:

    /**
     * Purpose:
     * <p> Constructor. Sets default values.
     * 
     */ 
    ConfigurationMngr(const char * name)
        throw(OutOfMemoryException);



    /**
     * Purpose:
     * <p> Initializes the configuration from an XML file.
     * 
     * @param xml_conf The XML file path
     * @param validate Validate the XML file before processing
     */ 
    void parse(const std::string& xml_conf, bool validate)
        throw(OutOfMemoryException, ConfigurationException);


    /**
     * Purpose:
     * <p> Destructor
     * 
     */ 
    virtual 
    ~ConfigurationMngr()
        throw();


    /**
     * Purpose:
     * <p> return the request duration time (used in a fault tolerance context)
     */
    size_t getRequestDurationTime() throw();
    

    /**
     * Purpose:
     * <p> Returns the port for the Daemon
     */ 
    const char* getSystemDaemonPort() throw();
    
    
    
private:
        

    std::list<std::string> tokenize(std::string str, std::string delims);

    /**
     * The port identifying the system.
     */
    std::string m_systemDaemonPort;

    /**
     * the request duration time 
     */
    size_t m_duration_time;


    
}; // End class ConfigurationMngr 

} // End namespace FT
} // End namespace Cdmw
#endif // INCL_GROUP_CREATOR_CONFIGURATIONMNGR_IMPL_HPP

