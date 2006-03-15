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


#ifndef INCL_FAULT_MANAGER_CONFIGURATIONMNGR_IMPL_HPP 
#define INCL_FAULT_MANAGER_CONFIGURATIONMNGR_IMPL_HPP 

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
     * <p> return the request duration time (used in a fault tolerance context)
     */
    size_t getRequestDurationTime() throw();
    

    /**
     * Purpose:
     * <p> Returns the fault tolerance domain id.
     */ 
    const char* getDomainID() throw();


    /**
     * Purpose:
     * <p> Returns the pull corbaloc for the simple monitoring 
     */
    const char* getSimplePullCorbaloc() throw();


    /**
     * Purpose:
     * <p> Returns the push corbaloc for the simple monitoring 
     */
    const char* getSimplePushCorbaloc() throw();


    /**
     * Purpose:
     * <p> Returns the pull corbaloc for the mutual monitoring 
     */
    const char* getMutualPullCorbaloc() throw();


    /**
     * Purpose:
     * <p> Returns the localisation service address 
     */
    const char* getLocalisationServiceAddress() throw();

    /**
     * Purpose:
     * <p> Returns the push corbaloc for the mutual monitoring 
     */
    const char* getMutualPushCorbaloc() throw();

    /**
     * Purpose:
     * <p> Returns master interrogation time
     */
    const char* getMasterInterrogationTime() throw();

    /**
     * Purpose:
     * <p> Returns slave acknowledgement time
     */
    const char* getSlaveAcknowledgementTime() throw();

    /**
     * Purpose:
     * <p> Returns master confirmation time
     */
    const char* getMasterConfirmationTime() throw();

    /**
     * Purpose:
     * <p> Returns master election time
     */
    const char* getMasterElectionTime() throw();

    /**
     * Purpose:
     * <p> Returns daemon acknowledgement time
     */
    const char* getDaemonAcknowledgementTime() throw();

    /**
     * Purpose:
     * <p> Returns the Orb Service Configuration file
     */ 
    const char* getSvcConf() const throw();

    /**
     * Purpose:
     * <p> Returns the state transfert domain
     */
    const char* getStateTransfertDomain() throw();

    /**
     * Purpose:
     * <p> Returns the state transfert IP address
     */
    const char* getStateTransfertIPAddress() throw();

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
     * <p> return the replication manager security level
     */
    const char* getSecurityLevel() throw();
    
    /**
     * Purpose:
     * <p> return the replication manager replication level
     */
    const char* getReplicationLevel() throw();

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
     * the request duration time 
     */
    size_t m_duration_time;

    /**
     * the fault tolerance domain id
     */
    std::string m_domain_id;    

    /**
     * the pull corbaloc for the simple monitoring
     */
    std::string m_simple_pull_corbaloc;    
    
    /**
     * the push corbaloc for the simple monitoring
     */
    std::string m_simple_push_corbaloc;
    
    /**
     * the pull corbaloc for the mutual monitoring
     */
    std::string m_mutual_pull_corbaloc;    
    
    /**
     * the push corbaloc for the mutual monitoring
     */
    std::string m_mutual_push_corbaloc;

    /**
     * the localisation service address
     */
    std::string m_localisation_service_corbaloc;

    /**
     * the master interrogation time
     */
    std::string m_master_interrogation;
         
    /**
     * the slave acknowledgement time
     */
    std::string m_slave_acknowledgement;
         
    /**
     * the master confirmation time
     */
    std::string m_master_confirmation;
         
    /**
     * the master election time
     */
    std::string m_master_election;
         
    /**
     * the daemon acknowledgement time
     */
    std::string m_daemon_acknowledgement;
         
    /**
     * The path to the Orb Service Configuration file
     */
    std::string m_svcConf;

    /**
     * the state tranfert domain
     */
    std::string m_state_tranfert_domain;
    
    /**
     * the state transfert IP address
     */
    std::string m_state_tranfert_IP_address;

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
     * replication manager security level
     */
    std::string m_security_level;
    
    /**
     * replication manager replication level
     */
    std::string m_replication_level;


    /**
     * supported interfaces map
     */
    SupportedInterfacesMap m_supportedInterfacesMap;
    
    
}; // End class ConfigurationMngr 

} // End namespace FT
} // End namespace Cdmw
#endif // INCL_FAULT_MANAGER_CONFIGURATIONMNGR_IMPL_HPP

