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


#include <ftmanager/MutualMonitoringObserver_impl.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include "FaultTolerance/ftstatemanager/DataStoreBase.hpp"
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <ftmanager/Method_Request_On_Failure.hpp>
#include <ftmanager/Method_Request_On_Start.hpp>
#include <ftmanager/Method_Request_On_Switch_To_Primary.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>





namespace 
{


    const int NB_SCHEDULER_THREADS = 1;

} // end anonymous namespace

namespace Cdmw {
namespace FT {

    MutualMonitoringObserver_impl::MutualMonitoringObserver_impl(::FT::FaultNotifier_ptr fault_notifier, Cdmw::FT::FTManager_impl* ftmanager)
        :m_faultNotifier(::FT::FaultNotifier::_duplicate(fault_notifier)),
         m_ftmanager(ftmanager),
         m_scheduler(NB_SCHEDULER_THREADS)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_scheduler.start();
    }    
    
    MutualMonitoringObserver_impl::~MutualMonitoringObserver_impl()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_scheduler.stop();
    }    
    
    void MutualMonitoringObserver_impl::on_failure (const char * the_location)
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        m_scheduler.enqueue(new Scheduler::Method_Request_On_Failure(m_ftmanager, the_location));
    }
    
    void MutualMonitoringObserver_impl::on_switch_to_primary ()
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_scheduler.enqueue(new Scheduler::Method_Request_On_Switch_To_Primary(m_ftmanager)); 
    }
    
        
    void MutualMonitoringObserver_impl::on_switch_to_backup ()
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try
        {
            CDMW_MUTEX_GUARD(m_mutex);
            
            CDMW_DEBUG(FTLogger::GetLogger(),"--> on_switch_to_backup : deactivate");
            m_ftmanager->deactivate();    
        }
        catch(const CORBA::SystemException & ex)
        {
            CDMW_ERROR(FTLogger::GetLogger(),"CORBA System excepion raised in on_switch_to_backup() ex: '" 
                       << ex << "'");
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }

    }    
    
    void MutualMonitoringObserver_impl::on_start (const char * the_location)
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        m_scheduler.enqueue(new Scheduler::Method_Request_On_Start(m_ftmanager, the_location));
    }
    

}; // namespace FT
}; // namespace Cdmw
