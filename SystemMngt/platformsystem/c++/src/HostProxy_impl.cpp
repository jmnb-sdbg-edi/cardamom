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


#include "platformsystem/HostProxy_impl.hpp"
#include "platformsystem/System_impl.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

class HostProxy_impl;
class System_impl;



// ----------------------------------------------------------------------
// HostRespondingCallback class.
// ----------------------------------------------------------------------
HostRespondingCallback::HostRespondingCallback (HostProxy_impl* hostProxy)
    : Callback()
{
    m_hostProxy = hostProxy;
}


HostRespondingCallback::~HostRespondingCallback ()
{
}


void HostRespondingCallback::execute()
  throw()
{
    
    m_hostProxy->setStatus (CdmwPlatformMngt::HOST_RESPONDING);
    
}

// ----------------------------------------------------------------------
// HostNotRespondingCallback class.
// ----------------------------------------------------------------------
HostNotRespondingCallback::HostNotRespondingCallback (HostProxy_impl* hostProxy)
    : Callback()
{
    m_hostProxy = hostProxy;
}


HostNotRespondingCallback::~HostNotRespondingCallback ()
{
}


void HostNotRespondingCallback::execute()
  throw()
{
   
    m_hostProxy->setStatus (CdmwPlatformMngt::HOST_NOT_RESPONDING);
    
}




// ----------------------------------------------------------------------
// HostProxy_impl class.
// ----------------------------------------------------------------------


#ifdef CDMW_USE_FAULTTOLERANCE
HostProxy_impl::HostProxy_impl (PortableServer::POA_ptr poa,
                                const CdmwPlatformMngt::HostDef& hostDef,
                                System_impl* system,
                                CdmwFT::FaultManagement::FaultDetector_ptr faultDetector)
#else
HostProxy_impl::HostProxy_impl (PortableServer::POA_ptr poa,
                                const CdmwPlatformMngt::HostDef& hostDef,
                                System_impl* system)
#endif
    throw (BadParameterException,
           OutOfMemoryException)
    : DeactivableServant_impl (poa)                         
{
    try
    { 
        if (system == NULL)
        {
            CDMW_THROW2 (BadParameterException, "system", "NULL");
        }
       
        // Set the internal data
        m_system = system;
        
        m_hostName = hostDef.host_name;
    
        m_platform_application_exe = hostDef.platform_application_exe;

        m_application_agent_exe = hostDef.application_agent_exe;
                             
        m_status = CdmwPlatformMngt::HOST_RESPONDING;
    
        m_monitor = NULL;

#ifdef CDMW_USE_FAULTTOLERANCE
        m_faultDetector = CdmwFT::FaultManagement::FaultDetector::_duplicate(faultDetector);

        m_monitoringID = 0;        
#else

        // create Daemon PullMonitor
        std::auto_ptr <DaemonMonitor> 
               monitor (new DaemonMonitor (   
                                    m_system, m_hostName.in(),                   
                                    hostDef.daemon_monitoring_info.monitoring_interval,
                                    hostDef.daemon_monitoring_info.monitoring_timeout));
        
        
        // create Callbacks                        
        std::auto_ptr <HostRespondingCallback>  responseCallback (
                                new HostRespondingCallback (this));
                                
        std::auto_ptr <HostNotRespondingCallback>  noResponseCallback (
                                new HostNotRespondingCallback (this));

    
    
                                
        m_monitor = monitor.release();
        
        // set callbacks in monitor
        m_monitor->setResponseCallback (responseCallback.release());
        m_monitor->setNoResponseCallback (noResponseCallback.release());
#endif
    }
     
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    } 

}



HostProxy_impl::~HostProxy_impl ()
{
#ifndef CDMW_USE_FAULTTOLERANCE
    if (m_monitor != NULL)
    {
        // terminate pull monitor thread if not stopped
        m_monitor->stop();
        
        delete m_monitor;
    }
#endif
}


CdmwPlatformMngt::HostName HostProxy_impl::name()
throw( CORBA::SystemException )
{
    CORBA::String_var name = m_hostName;

    return name._retn();
}



CdmwPlatformMngt::HostStatus HostProxy_impl::status()
throw( CORBA::SystemException )
{
    return m_status;
}
    
    

char* HostProxy_impl::platform_application_exe()
throw( CORBA::SystemException )
{
    CORBA::String_var exe = m_platform_application_exe;

    return exe._retn();
}
    
    
char* HostProxy_impl::application_agent_exe()
throw( CORBA::SystemException )
{
    CORBA::String_var exe = m_application_agent_exe;

    return exe._retn();    
}
 


void HostProxy_impl::setStatus (CdmwPlatformMngt::HostStatus status)
{
    // if status change
    if (status != m_status)
    {
        // notify status change
        m_system->notifyHostStatusChangeEvent (
                               get_hostName(),
                               status);
        
        // store new status                       
        m_status = status;
        
        if (status == CdmwPlatformMngt::HOST_NOT_RESPONDING)
        {
            // update applications of system with unavailable host name
            m_system->updateApplications (get_hostName());
        }
    }
}


void HostProxy_impl::start()
{
    // start pull monitor thread
#ifdef CDMW_USE_FAULTTOLERANCE
    ::FT::Location location;
    location.length(1);
    location[0].kind="hostname";
    location[0].id=get_hostName();
    
    ::FT::FaultMonitoringIntervalAndTimeoutValue time;
    time.monitoring_interval=0;
    time.timeout=0;    
      
    m_monitoringID = m_faultDetector->start_monitoring(::FT::PullMonitorable::_nil(),
                                                       location,
                                                       time);
#else
    m_monitor->start();
#endif
}
    
    

void HostProxy_impl::stop()
{
    // terminate pull monitor thread
#ifdef CDMW_USE_FAULTTOLERANCE
    m_faultDetector->stop_monitoring(m_monitoringID);    
#else
    m_monitor->stop();
#endif
}


CORBA::ULong HostProxy_impl::get_number_of_properties()
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);       
}

void HostProxy_impl::get_all_property_names (
                        CORBA::ULong how_many,
                        CosPropertyService::PropertyNames_out property_names,
                        CosPropertyService::PropertyNamesIterator_out rest)
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CORBA::Any* HostProxy_impl::get_property_value (const char* property_name)    
        throw (CosPropertyService::PropertyNotFound,
               CosPropertyService::InvalidPropertyName,
               CORBA::SystemException)
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

CORBA::Boolean HostProxy_impl::get_properties (
                         const CosPropertyService::PropertyNames& property_names,
                         CosPropertyService::Properties_out nproperties)
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

void HostProxy_impl::get_all_properties (
                         CORBA::ULong how_many,
                         CosPropertyService::Properties_out nproperties,
                         CosPropertyService::PropertiesIterator_out rest)
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

void HostProxy_impl::set_property (
                         const char* property_name,
                         const CORBA::Any& property_value)
throw( CORBA::SystemException )
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);


    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}

void HostProxy_impl::remove_property (const char* property_name)    
        throw (CosPropertyService::PropertyNotFound,
               CORBA::SystemException)
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);


    // TODO: Notify the configuration change

    // TODO: Trace to the log manager
}


const char* HostProxy_impl::get_hostName()
{
    return m_hostName.in();
}


} // End namespace PlatformMngt
} // End namespace Cdmw


