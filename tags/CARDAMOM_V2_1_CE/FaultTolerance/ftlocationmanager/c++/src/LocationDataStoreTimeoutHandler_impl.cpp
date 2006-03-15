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


#include <FaultTolerance/ftlocationmanager/LocationDataStoreTimeoutHandler_impl.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <SystemMngt/platformlibrary/EventHeaderFactory.hpp>
#include <iostream>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>


namespace Cdmw {
namespace FT {

    //
    // Constructor
    //
    LocationDataStoreTimeoutHandler::LocationDataStoreTimeoutHandler(::FT::FaultNotifier_ptr fault_notifier)
        : m_fault_notifier(::FT::FaultNotifier::_duplicate(fault_notifier))
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }
    
    
    //
    // Destructor
    //
    LocationDataStoreTimeoutHandler::~LocationDataStoreTimeoutHandler() 
        throw()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }
    
    
    //
    // on_freeze_timeout
    //
    void LocationDataStoreTimeoutHandler::on_freeze_timeout(const std::string& location) 
        throw()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location="<<location);
        CDMW_INFO(FTLogger::GetLogger(),"Freeze timeout for location '"<<location <<"'");
        //
        ::FT::Location* ft_location = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(location);

        CDMW_DEBUG(FTLogger::GetLogger(),
                   "got ft_location");

        // Prepare the event for the fault notification
        CosNotification::StructuredEvent fault_event;
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup( "FT_CORBA" );
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup( "ObjectCrashFault" );
        fault_event.filterable_data.length( 3);
        fault_event.filterable_data[0].name = CORBA::string_dup( "FTDomainId" );
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup( "Location" );
        fault_event.filterable_data[1].value <<= *ft_location;


        CdmwPlatformMngtBase::TimeStamp time_stamp =
        PlatformMngt::EventHeaderFactory::createTimeStamp();
        fault_event.filterable_data[2].name = CORBA::string_dup( "TimeStamp" );
        fault_event.filterable_data[2].value <<= time_stamp;                


        CDMW_DEBUG(FTLogger::GetLogger(),"about to push_structured_fault");
        m_fault_notifier->push_structured_fault( fault_event );
        CDMW_DEBUG(FTLogger::GetLogger(),"after to push_structured_fault");        
    }
    


}  // End namespace FT
}  // End namespace Cdmw
