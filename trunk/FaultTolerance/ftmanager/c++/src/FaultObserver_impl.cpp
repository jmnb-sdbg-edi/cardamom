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


#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "ftmanager/FaultObserver_impl.hpp"
#include <SystemMngt/platformlibrary/EventHeaderFactory.hpp>

#include <sstream>

namespace Cdmw {
namespace FT {
namespace FaultManagement {

FaultObserver_impl::FaultObserver_impl(
	CORBA::ORB_ptr orb )
throw( CORBA::SystemException )
{
    if( CORBA::is_nil( orb ) )
        throw CORBA::BAD_PARAM(
            OrbSupport::BAD_PARAMNilObjectReference,
            CORBA::COMPLETED_NO );

    m_orb = CORBA::ORB::_duplicate( orb );
    m_fault_notifier = ::FT::FaultNotifier::_nil();
}

FaultObserver_impl::~FaultObserver_impl()
throw( CORBA::SystemException )
{
}

//
// IDL:thalesgroup.com/FT/PullMonitorable/is_alive:1.0
//
CORBA::Boolean FaultObserver_impl::is_alive ()
throw( CORBA::SystemException )
{
    return true;
}

//
// IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector/start_monitoring:1.0
//
CdmwFT::FaultManagement::MonitoringId FaultObserver_impl::start_monitoring (
    ::FT::PullMonitorable_ptr monitorable_object,
    const ::FT::Location& location,
    const ::FT::FaultMonitoringIntervalAndTimeoutValue & timeout )
throw( CORBA::SystemException,
       ::FT::ObjectNotFound )
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

//
// IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector/stop_monitoring:1.0
//
void FaultObserver_impl::stop_monitoring (
    CdmwFT::FaultManagement::MonitoringId monitoring_id )
throw( CORBA::SystemException,
       CdmwFT::FaultManagement::InvalidMonitoringId )
{
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/PlatformObserver/notify:1.0
//
void FaultObserver_impl::notify (
    CdmwPlatformMngt::Event* an_event )
throw( CORBA::SystemException )
{
    // Filter supervision observer events
    ::FT::Location location;

    switch( an_event->event_kind() )
    {
        case CdmwPlatformMngt::HOST_STATUS_CHANGE :
            {
                CdmwPlatformMngt::HostStatusChange* event
                    = CdmwPlatformMngt::HostStatusChange::_downcast(
                        an_event );

                if( event->host_status() == CdmwPlatformMngt::HOST_UNREACHABLE )
                {
                    location.length( 1 );
                    location[0].kind = CORBA::string_dup( "hostname" );
                    location[0].id = CORBA::string_dup( event->host_name() );
                }
                else
                    return;
            }
            break;

        case CdmwPlatformMngt::APPLICATION_AGENT_DEATH :
            {
                CdmwPlatformMngt::ApplicationAgentDeath* event
                    = CdmwPlatformMngt::ApplicationAgentDeath::_downcast(
                        an_event );

                location.length( 2 );
                location[0].kind = CORBA::string_dup( "hostname" );
                location[0].id = CORBA::string_dup( event->host_name() );
                location[1].kind = CORBA::string_dup( "applicationname" );
                location[1].id = CORBA::string_dup( event->application_name() );
            }
            break;

        case CdmwPlatformMngt::PROCESS_STATUS_CHANGE :
            {
                CdmwPlatformMngt::ProcessStatusChange* event
                    = CdmwPlatformMngt::ProcessStatusChange::_downcast(
                        an_event );

                switch( event->process_status() )
                {
                    case CdmwPlatformMngt::PROCESS_STOPPED:
                    case CdmwPlatformMngt::PROCESS_ENDED:
                    case CdmwPlatformMngt::PROCESS_FAILED_ERROR:
                    case CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT:
                    case CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY:
                    case CdmwPlatformMngt::PROCESS_FAILED_INVALID:
                    case CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE:
                    case CdmwPlatformMngt::PROCESS_FAILED_DEATH:
                        location.length( 3 );
                        location[0].kind = CORBA::string_dup( "hostname" );
                        location[0].id = CORBA::string_dup( event->host_name() );
                        location[1].kind = CORBA::string_dup( "applicationname" );
                        location[1].id = CORBA::string_dup(  event->process_id().application_name );
                        location[2].kind = CORBA::string_dup( "processname" );
                        location[2].id = CORBA::string_dup( event->process_id().process_name );
                        break;

                    default:
                        return;
                }
            }
            break;

        default:
            return;
    }

    // Prepare the event for the fault notification
    CosNotification::StructuredEvent fault_event;
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup( "FT_CORBA" );
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup( "ObjectCrashFault" );
        fault_event.filterable_data.length( 3 );
        fault_event.filterable_data[0].name = CORBA::string_dup( "FTDomainId" );
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup( "Location" );
        fault_event.filterable_data[1].value <<= location;

        CdmwPlatformMngtBase::TimeStamp time_stamp =
        PlatformMngt::EventHeaderFactory::createTimeStamp();
        fault_event.filterable_data[2].name = CORBA::string_dup( "TimeStamp" );
        fault_event.filterable_data[2].value <<= time_stamp;                

	// Get the fault notifier
    if( CORBA::is_nil( m_fault_notifier.in() ) )
	{
		CORBA::Object_var objref
			= m_orb->resolve_initial_references( "ReplicationManager" );
		::FT::ReplicationManager_var rm
			= ::FT::ReplicationManager::_narrow( objref.in() );

	    CDMW_ASSERT( !CORBA::is_nil( rm.in() ) )
		m_fault_notifier = rm->get_fault_notifier();
	}

    // Notifies to the fault notifier
    CDMW_ASSERT( !CORBA::is_nil( m_fault_notifier.in() ) )
    m_fault_notifier->push_structured_fault( fault_event );
}


//
// IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultObserver/set_fault_notifier:1.0
//
void FaultObserver_impl::set_fault_notifier (
    ::FT::FaultNotifier_ptr fault_notifier )
throw( CORBA::SystemException )
{
	// Set the fault notifier
    if( CORBA::is_nil( fault_notifier ) )
        throw CORBA::BAD_PARAM(
            OrbSupport::BAD_PARAMNilObjectReference,
            CORBA::COMPLETED_NO );

    m_fault_notifier = ::FT::FaultNotifier::_duplicate( fault_notifier );
}


}  // End namespace FaultManagement
}  // End namespace FT
}  // End namespace Cdmw

