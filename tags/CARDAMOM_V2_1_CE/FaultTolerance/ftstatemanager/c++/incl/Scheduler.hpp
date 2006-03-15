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

#ifndef _CDMW_FT_SCHEDULER_HPP_
#define _CDMW_FT_SCHEDULER_HPP_

#include <Foundation/osthreads/SchedulerBase.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <FaultTolerance/ftstatemanager/TransactionEngineCoordinatorBase.hpp>
#include <FaultTolerance/ftstatemanager/Method_Request_Stop.hpp>
#include <FaultTolerance/ftstatemanager/Method_Request_Prepare.hpp>
#include <FaultTolerance/ftstatemanager/Method_Request_Commit.hpp>
#include <FaultTolerance/ftstatemanager/Method_Request_Rollback.hpp>
#include <FaultTolerance/ftstatemanager/Method_Request_Get_Transaction_Status.hpp>
#include <FaultTolerance/ftstatemanager/Transaction_Result.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <Foundation/orbsupport/TypeTraits.hpp>
#include <ace/Future.h>

#if CDMW_ORB_VDR == tao
#include <tao/Messaging/Messaging.h>
#else
#error "Unsupported ORB"
#endif

namespace Cdmw {
namespace FT {
namespace Scheduler {


class Scheduler : public Cdmw::OsSupport::SchedulerBase
{
public:
    Scheduler(CORBA::ORB_ptr orb, 
              Cdmw::FT::TransactionEngineCoordinatorBase* coordinator, 
              unsigned short nbthreads);
    
    ~Scheduler()
        throw() ;        
    
    void prepare(CdmwFT::Transaction::TPCCohort_ptr cohort, 
                 const CdmwFT::Transaction::StateChanges& change,
                 CdmwFT::Transaction::TransactionIdentification id,
                 const std::string & location);
    
    void commit(CdmwFT::Transaction::TPCCohort_ptr cohort, 
                CdmwFT::Transaction::TransactionIdentification id,
                const std::string & location);
    
    void get_transaction_status(CdmwFT::Transaction::TPCCohort_ptr cohort, 
                                CdmwFT::Transaction::TransactionIdentification id,
                                const std::string & location);
    
    void rollback(CdmwFT::Transaction::TPCCohort_ptr cohort, 
                  CdmwFT::Transaction::TransactionIdentification id,
                  const std::string & location);
    
private:
    Cdmw::FT::TransactionEngineCoordinatorBase* m_coordinator;
    CORBA::ORB_var m_orb;
};
 
}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_TIMER_HPP_
