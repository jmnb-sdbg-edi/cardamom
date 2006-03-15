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


#include <ftmanager/SimpleMonitoringObserver_impl.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>
#include <SystemMngt/platformlibrary/EventHeaderFactory.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

namespace Cdmw {
namespace FT {
    
    SimpleMonitoringObserver_impl::SimpleMonitoringObserver_impl(CORBA::ORB_ptr orb,
                                                                 PortableServer::POA_ptr poa,
                                                                 ::FT::FaultNotifier_ptr fault_notifier)
        :m_orb(CORBA::ORB::_duplicate(orb)),
         m_poa(PortableServer::POA::_duplicate(poa)),
         m_faultNotifier(::FT::FaultNotifier::_duplicate(fault_notifier))
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
		m_hostStatusStorageHome = new HostStatusStorageHome(HOST_DATA_STORE2);
    }    
    
    SimpleMonitoringObserver_impl::~SimpleMonitoringObserver_impl()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }    
    
    void SimpleMonitoringObserver_impl::on_failure (const char * the_location)
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        CDMW_INFO(FTLogger::GetLogger(),"========> on_failure : "<<the_location);
        Cdmw::OsSupport::OS::Timeval time = Cdmw::OsSupport::OS::get_time();
        std::cout << "########## Detect Daemon " << the_location << " death: " 
                  << time.seconds << "sec " << time.microseconds << "microsec" 
                  << std::endl;
        
        try
        {            
            if (status_is(the_location))
            {
                CDMW_DEBUG(FTLogger::GetLogger(),"--> FAILURE on the Simple Monitoring");        
                
                ::FT::Location location;              
                location.length(1);
                location[0].kind = CORBA::string_dup( "hostname" );
                location[0].id = CORBA::string_dup(the_location);

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

                m_faultNotifier->push_structured_fault( fault_event ); 
				set_status(the_location,false);            
            } 
        }
        catch(const ::FT::MemberNotFound& e)
        {
            CDMW_WARN(FTLogger::GetLogger()," New unknown host has crashed : "<<the_location);
        }
                
    }
     void SimpleMonitoringObserver_impl::on_start (const char * the_location)
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        try
        {            
            if (!status_is(the_location))
            {
                CDMW_DEBUG(FTLogger::GetLogger(),"--> START on the Simple Monitoring");        
                
                ::FT::Location location;              
                location.length(1);
                location[0].kind = CORBA::string_dup( "hostname" );
                location[0].id = CORBA::string_dup(the_location);
                
                // Prepare the event for the fault notification
                CosNotification::StructuredEvent fault_event;
                fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup( "FT_CORBA" );
                fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup( "ObjectResponding" );
                fault_event.filterable_data.length( 3 );
                fault_event.filterable_data[0].name = CORBA::string_dup( "FTDomainId" );
                fault_event.filterable_data[0].value <<= "thalesgroup.com";
                fault_event.filterable_data[1].name = CORBA::string_dup( "Location" );
                fault_event.filterable_data[1].value <<= location;
                CdmwPlatformMngtBase::TimeStamp time_stamp =
                PlatformMngt::EventHeaderFactory::createTimeStamp();
                fault_event.filterable_data[2].name = CORBA::string_dup( "TimeStamp" );
                fault_event.filterable_data[2].value <<= time_stamp;                

                m_faultNotifier->push_structured_fault( fault_event ); 
                set_status(the_location,true);
            } 
        }
        catch(const ::FT::MemberNotFound& e)
        {
            CDMW_WARN(FTLogger::GetLogger()," New unknown host is found : "<<the_location);
        }
        
    }

	void SimpleMonitoringObserver_impl::set_status(const char * the_location, bool status)
		throw(::FT::MemberNotFound)
	{
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location<<",status="<<status);
		try
        {         
			HostStatusStorageObject obj = m_hostStatusStorageHome->find_by_oid(the_location);
			obj.set(status);
		}
        catch(const Cdmw::FT::NotFoundException &)
        {
            ::FT::MemberNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
		}
	}


    bool SimpleMonitoringObserver_impl::status_is(const char * the_location)
        throw(::FT::MemberNotFound)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
		try
        {         
			HostStatusStorageObject obj = m_hostStatusStorageHome->find_by_oid(the_location);
			return obj.get();
		}
        catch(const Cdmw::FT::NotFoundException &)
        {
            CDMW_DEBUG(FTLogger::GetLogger(),"status_is -> ::FT::()NotFoundException;");   
            ::FT::MemberNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
		}
		
    }
    
    
    void SimpleMonitoringObserver_impl::add_location(const char * the_location)
        throw(::FT::MemberAlreadyPresent)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);        
 		try
        {         
			m_hostStatusStorageHome->create(the_location, true);
		}
        catch(const Cdmw::FT::AlreadyExistException &)
        {
            ::FT::MemberAlreadyPresent ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
		}

        
    }
    
    
    void SimpleMonitoringObserver_impl::remove_location(const char * the_location)
        throw(::FT::MemberNotFound)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);  
		try
        {         
			m_hostStatusStorageHome->remove(the_location);
		}
        catch(const Cdmw::FT::NotFoundException &)
        {
            ::FT::MemberNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
		}
    }
    
}; // namespace FT
}; // namespace Cdmw

