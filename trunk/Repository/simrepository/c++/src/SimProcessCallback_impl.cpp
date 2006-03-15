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


#include "simrepository/SimProcessCallback_impl.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegate.stub.hpp"
#include <sstream>
#include <iomanip>

namespace
{
    const char* PROCESS_PREFIX="PROC_";
};

namespace Cdmw
{
namespace Tools
{


SimProcessCallback_impl::SimProcessCallback_impl(
    PortableServer::POA_ptr                     parent,
    CosNaming::NamingContext_ptr                root,
    CdmwPlatformMngt::ProcessMessageBroker_ptr message_broker,
    CdmwPlatformMngtService::ServiceBroker_ptr service_broker,
    CdmwSimulatedRepository::MessageLogger_ptr message_logger,
    const std::string &                        application_name)
    throw (CORBA::SystemException)
    : m_poa(PortableServer::POA::_duplicate(parent)),
      m_root(root),
      m_message_broker(CdmwPlatformMngt::ProcessMessageBroker::_duplicate(message_broker)),
      m_service_broker(CdmwPlatformMngtService::ServiceBroker::_duplicate(service_broker)),
      m_logger(CdmwSimulatedRepository::MessageLogger::_duplicate(message_logger)),
      m_application_name(application_name),
      m_process_number(0L)
{
}

SimProcessCallback_impl::~SimProcessCallback_impl() throw()
{
}
    
PortableServer::POA_ptr SimProcessCallback_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/get_starting_info:1.0
//
CdmwPlatformMngt::ProcessCallback::ProcessStartingData * SimProcessCallback_impl::get_starting_info (
             CdmwPlatformMngt::ProcessDelegate_ptr the_process)
      throw(CORBA::SystemException)
{
    CdmwPlatformMngt::ProcessCallback::ProcessStartingData starting_data;

    // The name of the application owning the process.
    starting_data.application_name= CORBA::string_dup(m_application_name.c_str());
    
    // The process name.
    {
        using namespace std;
        ostringstream ostrm;
        ostrm << PROCESS_PREFIX << setw(3) << setfill('0') << m_process_number;
        ++m_process_number;
        starting_data.process_name =  CORBA::string_dup(ostrm.str().c_str());        
    }


    // The broker used by the managed process to notify its
    // applicative messages and fatal errors.
    starting_data.process_message_broker = m_message_broker;

    std::string body(m_application_name);
    body += "/";
    body += starting_data.process_name.in();
    m_logger->log("ProcessCallback::set_ready",body.c_str());
    
    // Register the_process in the naming context
    try {
        m_root.bind(body,the_process,true);
    } catch (...) {
        throw CORBA::INTERNAL();
    }

    return new CdmwPlatformMngt::ProcessCallback::ProcessStartingData(starting_data);
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_ready:1.0
//
CdmwPlatformMngt::ProcessCallback::ProcessStartingData*
SimProcessCallback_impl::set_ready(CdmwPlatformMngt::ProcessDelegate_ptr the_process)
    throw(CORBA::SystemException)
{
    return get_starting_info (the_process);
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_creation_done:1.0
//     
void SimProcessCallback_impl::set_creation_done ()
        throw(CORBA::SystemException)
{

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/notify_error:1.0
//
void
SimProcessCallback_impl::notify_error(const char* issuer,
                                      const char* error_info)
    throw(CORBA::SystemException)
{
    std::string body("(");
    body += issuer;
    body += ",";
    body += error_info;
    body += ")";
    m_logger->log("ProcessCallback::notify_error",body.c_str());
}

} // namespace Tools
} // namespace Cdmw

