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


#include "testplatformdaemon/SIM_ProcessObserver.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/


//
// Ctor
//
SIM_ProcessObserver::SIM_ProcessObserver(const char* logFile)
    : m_fs(logFile, std::ios::out)
{

}


//
//
//
void SIM_ProcessObserver::stop()
{
    m_fs.close();
}


//
// Dtor
//
SIM_ProcessObserver::~SIM_ProcessObserver()
    throw()
{

}


void SIM_ProcessObserver::notify_user_event(const char* process_name,
    const char* issuer,
    const CORBA::Any& data)
throw(CORBA::SystemException)
{

    m_fs << "process \"" << process_name << "\" reported a user event" << std::endl;

}


void SIM_ProcessObserver::notify_message(const char* process_name,
    const CdmwPlatformMngtBase::EventHeader& header, const char* issuer, const char* message) 
    throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
{

    m_fs << "Process \"" << process_name << "\" reported the following message : "
        << message << std::endl;

}


void SIM_ProcessObserver::notify_error(const char* process_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp, const char* issuer, const char* error_info) 
    throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
{

    m_fs << "Process \"" << process_name << "\" reported the following error : "
        << error_info << std::endl;

}


void SIM_ProcessObserver::notify_ending(const char* process_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp)
    throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
{

    m_fs << "Process \"" << process_name << "\" has ended" << std::endl;

}


void SIM_ProcessObserver::notify_monitoring_failure(const char* process_name,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp)
    throw(CdmwPlatformMngt::ProcessNotFound, CORBA::SystemException)
{

    m_fs << "Process \"" << process_name << "\" doesn't respond" << std::endl;

}

void SIM_ProcessObserver::notify_point_monitoring_failure(const char* process_name,
    CORBA::ULong point_index,
    const CdmwPlatformMngtBase::TimeStamp& time_stamp)
throw(CdmwPlatformMngt::ProcessNotFound,
    CdmwPlatformMngt::ActivityPointNotFound,
    CORBA::SystemException)
{

    m_fs << "Activity point " << point_index << " of process \"" << process_name << "\" doesn't respond" << std::endl;

}


} // End namespace PlatformMngt
} // End namespace Cdmw


