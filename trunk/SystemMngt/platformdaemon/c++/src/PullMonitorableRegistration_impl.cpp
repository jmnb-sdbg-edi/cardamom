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


#include "platformdaemon/Daemon_impl.hpp"
#include "platformdaemon/PullMonitorableRegistration_impl.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "idllib/CdmwPlatformMngtLocalFaultDetector.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


PullMonitorableRegistration_impl::PullMonitorableRegistration_impl(
                CORBA::ORB_ptr orb,
                CdmwPlatformMngt::LocalFaultDetector_ptr faultDetector,
                Daemon_impl* daemon,
                unsigned long pullInterval,
                unsigned long pullTimeout)
      : m_daemon (daemon),
        m_pull_interval(pullInterval),
        m_pull_timeout (pullTimeout)
{
    // set fault detector
    m_localFaultDetector = 
       CdmwPlatformMngt::LocalFaultDetector::_duplicate(faultDetector);
    
    // gets the POA Current reference
    CORBA::Object_var current = orb->resolve_initial_references("POACurrent");
    m_current = PortableServer::Current::_narrow(current.in());
    
    // get the host name
    m_localHostName = OsSupport::OS::get_hostname();
}


PullMonitorableRegistration_impl::~PullMonitorableRegistration_impl()
  throw()
{

}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorableRegistration_impl/register_pull_monitorable:1.0
//
void PullMonitorableRegistration_impl::register_pull_monitorable(
              ::FT::PullMonitorable_ptr pull_monitorable)
    throw(CORBA::SystemException)
{
    if (CORBA::is_nil(pull_monitorable))
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference,
                               CORBA::COMPLETED_NO);
    }
    
    m_pull_monitorable = ::FT::PullMonitorable::_duplicate (pull_monitorable);
    
    // get the process name from the current object id
    
    std::string process_name;
    
    try
    {
        // get the current oid
        PortableServer::ObjectId_var oid = m_current->get_object_id();
        CORBA::String_var oid_str = PortableServer::ObjectId_to_string(oid.in());

        process_name = oid_str.in();
    }
    catch(const PortableServer::Current::NoContext&)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }


    // start monitoring
    try
    {
        ::FT::Location location;                
        location.length(2);
        location[0].kind = CORBA::string_dup ("hostname");
        location[0].id = CORBA::string_dup (m_localHostName.c_str());
        location[1].kind = CORBA::string_dup ("processname");
        location[1].id = CORBA::string_dup (process_name.c_str());

        FT::FaultMonitoringIntervalAndTimeoutValue timeout;
        timeout.monitoring_interval = m_pull_interval;
        timeout.timeout = m_pull_timeout;
        
        ::CdmwPlatformMngt::MonitoringId id =
            m_localFaultDetector->start_monitoring(m_pull_monitorable.in(),
                                                   location, timeout);
                                               
        m_daemon->set_process_monitoringId(process_name, id);
        
        std::cout << "<<PullMonitorableRegistration_impl::register_pull_monitorable()>> "
                  << process_name.c_str()
                  << " Monitoring started" << std::endl;
    }
    catch(const ::FT::ObjectNotFound &)
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference,
                               CORBA::COMPLETED_NO);
    }
    catch(const CdmwPlatformMngt::BadLocation &)
    {
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidLocation,
                               CORBA::COMPLETED_NO);
    }
    catch(const BadParameterException &)
    {
        std::cout << "<<PullMonitorableRegistration_impl::register_pull_monitorable()>> "
                  << process_name.c_str()
                  << " process not found to set monitoring id" << std::endl;
        // do nothing
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw


