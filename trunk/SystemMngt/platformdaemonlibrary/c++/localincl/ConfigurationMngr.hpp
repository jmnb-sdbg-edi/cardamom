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


#ifndef INCL_PLATFORMMNGT_CONFIGURATIONMNGR_IMPL_HPP 
#define INCL_PLATFORMMNGT_CONFIGURATIONMNGR_IMPL_HPP 

#include "Foundation/common/Exception.hpp"
#include <string>

namespace Cdmw
{
namespace PlatformMngt
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
class ConfigurationMngr
{

    public:

        /**
        * Purpose:
        * <p> Constructor. Sets default values.
        * 
        */ 
        ConfigurationMngr(long port)
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
        * <p> Returns the user id.
        */ 
        int getUID() const throw();

        /**
        * Purpose:
        * <p> Returns the group id.
        */ 
        int getGID() const throw();

        /**
        * Purpose:
        * <p> Returns the full path to the log file.
        */ 
        const char* getLogFile() const throw();
		
        /**
        * Purpose:
        * <p> Returns the full path to the file containing the IOR of the daemon.
        */ 
        const char* getIorFile() const throw();

        /**
        * Purpose:
        * <p> Returns the timeout allowed for notifying a platform information.
        */ 
        unsigned long getNotificationCallTimeout() const throw();
        
        /**
        * Purpose:
        * <p> Returns the pull monitoring interval for process started by daemon.
        */ 
        unsigned long getPullInterval() const throw();
        
        /**
        * Purpose:
        * <p> Returns the pull monitoring timeout allowed for process started by daemon.
        */ 
        unsigned long getPullTimeout() const throw();

        /**
        * Purpose:
        * <p> Returns the Orb Service Configuration file
        */ 
        const char* getSvcConf() const throw();
		
        /**
        * Purpose:
        * <p> Returns the directory into which file containing pid of launched processes must be saved. 
        */
        const char* getProcessesIdDirectory() throw();
        

        /**
        * Purpose:
        * <p> return the request duration time (used in a fault tolerance context)
         */
        size_t getRequestDurationTime() throw();
    

        /**
        * Purpose:
        * <p> Returns the pull corbalocfor the simple monitoring (used in a fault tolerance context)
        */
        const char* getPullCorbaloc() throw();


        /**
        * Purpose:
        * <p> Returns the push corbalocfor the simple monitoring (used in a fault tolerance context)
        */
        const char* getPushCorbaloc() throw();

        /**
         * Purpose:
         * <p> Returns the address of the locator service
         */
        const char* getLocationServiceAddress() throw();

        /**
         * Purpose:
         * <p> Returns the address of the locator service
         */
        const char* getRegistrationServiceAddress() throw();
    
        /**
         * Purpose:
         * <p> Returns the number of retires to find a service
         */
        int getLocationServiceRetries() throw();
    
        /**
         * Purpose:
         * <p> Returns the time between two reties
         */
        int getLocationServiceTimeInterval() throw();
    


    private:
        
        /**
        * The port identifying the system.
        */
        std::string m_systemPort;

        /**
        * The user id used for creating processes without privileges.
        */
        int m_UID;

        /**
        * The group id used for creating processes without privileges.
        */
        int m_GID;

        /**
        * The full path to the log file.
        */
        std::string m_logFile;

        /**
        * The full path to the file containing the IOR of the daemon.
        */
        std::string m_iorFile;

        /**
        * The timeout allowed for notifying a platform information.
        */ 
        unsigned long m_notificationCallTimeout;
        
        /**
        * The pull monitoring interval for simple processes.
        */ 
        unsigned long m_pullInterval;
        
        /**
        * The pull monitoring timeout for simple processes.
        */ 
        unsigned long m_pullTimeout;

        /**
        * The path to the Orb Service Configuration file
        */
        std::string m_svcConf;

        /**
        * The directory into which file containing pid of launched processes must be saved. 
        */
        std::string m_pidDirectory;

        /**
         * the request duration time (only used in a fault tolerance context)
         */
        size_t m_duration_time;

        /**
         * the pull corbaloc (only used in a fault tolerance context)
         */
        std::string m_pull_corbaloc;    
    
        /**
         * the push corbaloc (only used in a fault tolerance context)
         */
        std::string m_push_corbaloc;

        /**
         * the locator service address
         */
        std::string m_locator_service_address;
    
        /**
         * the registration service address
         */
        std::string m_registration_service_address;

        /**
         * the locator service reties
         */
        int m_locator_service_reties;
    
    
        /**
         * the locator service time
         */
        int m_locator_service_time;
    
    
         
}; // End class ConfigurationMngr 

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif // INCL_PLATFORMMNGT_CONFIGURATIONMNGR_IMPL_HPP

