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
#include <testftinitsequencing/TestInterceptors.hpp>
#include <Foundation/ossupport/OS.hpp>


namespace Cdmw
{
    
    TestServerInterceptor::TestServerInterceptor (
        const char* where, const char* host)
        : m_host_name(host),
          m_init(false)
    {
        if (strcmp(where, "request") == 0) {
            m_event_to_intercept = kIntercept_RECEIVE_REQUEST;
        }
        else if (strcmp(where, "reply") == 0) {
            m_event_to_intercept = kIntercept_SEND_REPLY;
        }
    }

    //
    // IDL to C++ Mapping
    //
    char*
    TestServerInterceptor::name() throw (CORBA::SystemException)
    {
      return CORBA::string_dup("TestServerInterceptor");
    }

    void
    TestServerInterceptor::destroy() throw (CORBA::SystemException)
    {
      
    }

    void
    TestServerInterceptor::receive_request_service_contexts(
        PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {

    }

    void
    TestServerInterceptor::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
        std::string actual_method = ri->operation();
        std::cout << "[++++ " << m_host_name << "::TestServerInterceptor] receive_request(" << actual_method << "):" << std::endl;
        if (m_event_to_intercept == kIntercept_RECEIVE_REQUEST) {
            if (actual_method == "init_primary_data_stores") {
                Cdmw::OsSupport::OS::Timeval time =
                    Cdmw::OsSupport::OS::get_time();
                std::cout << "Start of Primary Insertion "
                          << time.seconds << "secs "
                          << time.microseconds << " usecs"
                          << std::endl << std::flush;
                
            }
            if (actual_method == "init_backup_data_stores") {
                Cdmw::OsSupport::OS::Timeval time =
                    Cdmw::OsSupport::OS::get_time();
                std::cout << "Start of Backup Insertion "
                          << time.seconds << "secs "
                          << time.microseconds << " usecs"
                          << std::endl << std::flush;
                
            }
            if (actual_method == "get_record_iterator") {
                Cdmw::OsSupport::OS::Timeval time =
                    Cdmw::OsSupport::OS::get_time();
                std::cout << "Start of Backup Insertion on the Primary side"
                          << time.seconds << "secs "
                          << time.microseconds << " usecs"
                          << std::endl << std::flush;
                if (m_init)
                {
                    CdmwReplicationManager::timestruct time_;
                    time_.second = time.seconds;
                    time_.microsecond = time.microseconds;
                    m_time_collector->set_get_record_iterator_time(time_);
                }
                
            }
        }


        
    }

    void
    TestServerInterceptor::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException)
    {
        std::string actual_method = ri->operation();
        std::cout << "[++++ " << m_host_name << "::TestServerInterceptor] send_reply(" << actual_method << "):" << std::endl;
            if (actual_method == "init_primary_data_stores") {
                Cdmw::OsSupport::OS::Timeval time =
                    Cdmw::OsSupport::OS::get_time();
                std::cout << "Start of Primary Insertion "
                          << time.seconds << "secs "
                          << time.microseconds << " usecs"
                          << std::endl << std::flush;
                if (m_init)
                {
                    CdmwReplicationManager::timestruct time_;
                    time_.second = time.seconds;
                    time_.microsecond = time.microseconds;
                    m_time_collector->set_init_primary_data_stores_time(time_);
                }
            }
    }

    void
    TestServerInterceptor::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      
    }

    void
    TestServerInterceptor::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      
    }

}; // End namespace Cdmw

