/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "simrepository/SimProcessMessageBroker_impl.hpp"
#include <string>
#include <sstream>
#include <iomanip>

namespace
{
    std::string
    get_time_stamp(const CdmwPlatformMngtBase::TimeStamp& time_stamp)
    {
        using namespace std;
        ostringstream ostrm;
        ostrm << setw(4) << setfill('0') << time_stamp.year   << "/"
              << setw(2) << setfill('0') << time_stamp.month  << "/"
              << setw(2) << setfill('0') << time_stamp.day    << " "
              << setw(2) << setfill('0') << time_stamp.hour   << ":"
              << setw(2) << setfill('0') << time_stamp.minute << ":"
              << setw(2) << setfill('0') << time_stamp.second;
        return ostrm.str();
    }
};

namespace Cdmw
{
namespace Tools
{


SimProcessMessageBroker_impl::SimProcessMessageBroker_impl (PortableServer::POA_ptr             parent,
                                                            CdmwSimulatedRepository::MessageLogger_ptr logger) 
    throw (CORBA::SystemException)
    : m_poa(PortableServer::POA::_duplicate(parent)),
      m_logger(CdmwSimulatedRepository::MessageLogger::_duplicate(logger))
{
}

SimProcessMessageBroker_impl::~SimProcessMessageBroker_impl() throw()
{
}
    
PortableServer::POA_ptr SimProcessMessageBroker_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_message:1.0
//
void
SimProcessMessageBroker_impl::notify_message(const CdmwPlatformMngtBase::EventHeader& header,
                                             const char* issuer,
                                             const char* message)
    throw(CORBA::SystemException)
{
    std::ostringstream ostrm;
    ostrm << "( {" << get_time_stamp(header.time_stamp) << ";" << header.level << "},"
          << issuer << "," << message << ")";
    
    m_logger->log("ProcessMessageBroker::notify_message",ostrm.str().c_str());
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_error:1.0
//
void
SimProcessMessageBroker_impl::notify_error(const CdmwPlatformMngtBase::TimeStamp& time_stamp,
                                           const char* issuer,
                                           const char* error_info)
    throw(CORBA::SystemException)
{    
    std::ostringstream ostrm;
    ostrm << "(" << get_time_stamp(time_stamp) << ","
          << issuer << "," << error_info << ")";
    
    m_logger->log("ProcessMessageBroker::notify_error",ostrm.str().c_str());
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_user_event:1.0
//
void
SimProcessMessageBroker_impl::notify_user_event(const char* issuer,
                                                const CORBA::Any& data)
    throw(CORBA::SystemException)
{
    std::ostringstream ostrm;
    ostrm << "("<< issuer << ",data)";
    
    m_logger->log("ProcessMessageBroker::notify_user_event",ostrm.str().c_str());
}



} // namespace Tools
} // namespace Cdmw

