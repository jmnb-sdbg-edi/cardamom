/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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


#ifndef INCL_GROUP_MANAGER_CONFIGURATIONMNGR_IMPL_HPP 
#define INCL_GROUP_MANAGER_CONFIGURATIONMNGR_IMPL_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <string>
#include <list>
#include <map>

namespace Cdmw
{
namespace LB
{

typedef std::map< std::string, std::list< std::string > > SupportedInterfacesMap;
    

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
class ConfigurationMngr
{

public:

    /**
     * Purpose:
     * <p> Constructor. Sets default values.
     * 
     */ 
    ConfigurationMngr()
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
     * <p> Returns the system port as a string.
     */ 
    const char* getSystemPortString() const throw();

    /**
     * Purpose:
     * <p> Returns the register options.
     */ 
    const char* getRegisterOption() const throw();
    /**
     * Purpose:
     * <p> Returns the unregister options.
     */ 
    const char* getUnregisterOption() const throw();

    /**
     * Purpose:
     * <p> Returns the fault tolerance domain id.
     */ 
    const char* getDomainID() throw();
 
    /**
     * Purpose:
     * <p> Returns the port used for state transfer.
     */ 
    const char* getStateTransferPort() throw();


    /**
     * Purpose:
     * <p> Returns the observer name.
     */ 
    const char* getObserverName() throw();

    /**
     * Purpose:
     * <p> return the cohort timeout
     */
    OsSupport::OS::Timeval getCohortTimeOut() throw();

    /**
     * Purpose:
     * <p> return the coordinator timeout
     */
    OsSupport::OS::Timeval getCoordinatorTimeOut() throw();

    /**
     * Purpose:
     * <p> return the max_transaction_in_progress
     */
    int getMaxTransactionInProgress() throw();
    
    /**
     * Purpose:
     * <p> return the max_transaction_done
     */
    int getMaxTransactionDone() throw();
    
    /**
     * Purpose:
     * <p> return the map of supported interfaces
     */
    SupportedInterfacesMap getSupportedInterfaceMap() throw();
    
private:
        

    std::list<std::string> tokenize(std::string str, std::string delims);

    /**
     * The port identifying the system.
     */
    std::string m_systemPort;

    /**
     * The port used for state transfer.
     */
    std::string m_StateTransferPort;

    /**
     * The register option.
     */
    std::string m_registerOption;

   /**
     * The unregister option.
     */
    std::string m_unregisterOption;

    /**
     * the fault tolerance domain id
     */
    std::string m_domain_id;    

    /**
     * the observer_name
     */
    std::string m_observer_name;    

    /**
     * the cohort timeout
     */
    OsSupport::OS::Timeval m_cohort_timeout;

    /**
     * the cordinator timeout
     */
    OsSupport::OS::Timeval m_coordinator_timeout;

    /** 
     * max number of transaction in progress
     */ 
    int m_transaction_in_progress;
    
    /**
     * max transaction done
     */
    int m_transaction_done;
    
    /**
     * supported interfaces map
     */
    SupportedInterfacesMap m_supportedInterfacesMap;
    
    
}; // End class ConfigurationMngr 

} // End namespace LB
} // End namespace Cdmw
#endif // INCL_GROUP_MANAGER_CONFIGURATIONMNGR_IMPL_HPP

