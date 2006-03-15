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


#ifndef _CDMW_FT_MUTUAL_MONITORING_OBSERVER_HPP_
#define _CDMW_FT_MUTUAL_MONITORING_OBSERVER_HPP_

#include "idllib/CdmwFTMutualMonitoring.skel.hpp"
#include "idllib/FT.stub.hpp"
#include "ftmanager/FTManager_impl.hpp"
#include "idllib/CdmwFTFaultDetector.stub.hpp"
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <string>
#include <ostream>

namespace Cdmw {
namespace FT {


    class MutualMonitoringObserver_impl:
        public virtual CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoringObserver,
        public virtual Cdmw::OrbSupport::RefCountLocalObject
    {
    public:

        MutualMonitoringObserver_impl(::FT::FaultNotifier_ptr fault_notifier, Cdmw::FT::FTManager_impl* ftmanager);
        
        ~MutualMonitoringObserver_impl();        
        
        virtual void on_failure (const char * the_location)
            throw(CORBA::SystemException);
        
        virtual void on_switch_to_primary ()
            throw(CORBA::SystemException);
        
        virtual void on_switch_to_backup ()
            throw(CORBA::SystemException);
        
        virtual void on_start (const char * the_location)
            throw(CORBA::SystemException);
    private:
        /**
         * The fault notifier reference;
         */
        ::FT::FaultNotifier_var m_faultNotifier;

        /**
         * The fault tolerance manager
         */
        Cdmw::FT::FTManager_impl* m_ftmanager;
        
        /**
         * The mutex protecting concurrent access to m_processes.
         */
        OsSupport::Mutex m_mutex;

        /**
         * the scheduler
         */
        Cdmw::OsSupport::SchedulerBase m_scheduler;


    }; // End class   MonitoringObserver_impl
    
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_MUTUAL_MONITORING_OBSERVER_HPP_

