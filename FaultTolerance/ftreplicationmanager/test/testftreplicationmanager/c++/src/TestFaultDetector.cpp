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


#include <Foundation/orbsupport/CORBA.hpp>
#include "idllib/CdmwFTFaultDetector.stub.hpp"
#include <Foundation/ossupport/OS.hpp>
#include <exception>
#include <iostream>
#include "testftreplicationmanager/TestFaultDetector.hpp"

namespace Cdmw {
    namespace FT {
        
        // constructor
        TestFaultDetector::TestFaultDetector( )
        {
        }
        
        // destructor
        TestFaultDetector::~TestFaultDetector()
        {
        }
    
        // start_monitoring ( called by the replication manager )
        CdmwFT::FaultManagement::MonitoringId 
        TestFaultDetector::start_monitoring (::FT::PullMonitorable_ptr monitorable_object,
                                             const ::FT::Location & location,
                                             const ::FT::FaultMonitoringIntervalAndTimeoutValue & timeout)
            throw(CORBA::SystemException,
                  ::FT::ObjectNotFound)
        {
            std::cout<<"Start Monitoring"<<std::endl;
            return 1;
        }
        
        // stop_monitoring ( called by the replication manager )
        void
        TestFaultDetector::stop_monitoring (CdmwFT::FaultManagement::MonitoringId monitoring_id)
            throw(CORBA::SystemException,
                  CdmwFT::FaultManagement::InvalidMonitoringId)
        {
            std::cout<<"Stop Monitoring"<<std::endl;
        }

        void TestFaultDetector::set_fault_notifier(::FT::FaultNotifier_ptr fault_notifier)
            throw (CORBA::SystemException)
        {
            std::cout<<"Set fault notifier"<<std::endl;            
        }
        
        
        
    }; // namespace FT
}; // namespace Cdmw

