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

#include "ftstatemanager/TransactionEngineCoordinator.hpp"

namespace Cdmw {
namespace FT {
namespace Scheduler {

Scheduler::Scheduler
(CORBA::ORB_ptr orb, 
 Cdmw::FT::TransactionEngineCoordinatorBase* coordinator, 
 unsigned short nbthreads)
    : SchedulerBase(nbthreads),
      m_coordinator(coordinator),
      m_orb(CORBA::ORB::_duplicate(orb))
{
}

Scheduler::~Scheduler()
    throw()
{}


void Scheduler::prepare(CdmwFT::Transaction::TPCCohort_ptr cohort, 
                        const CdmwFT::Transaction::StateChanges& change, 
                        CdmwFT::Transaction::TransactionIdentification id,
                        const std::string & location)
{
    this->enqueue(new Method_Request_Prepare (cohort, m_coordinator, change,
                                              id, location));
}

void Scheduler::commit(CdmwFT::Transaction::TPCCohort_ptr cohort, 
                       CdmwFT::Transaction::TransactionIdentification id,
                       const std::string & location)
{
    this->enqueue(new Method_Request_Commit (cohort, m_coordinator, id, location));
}

void Scheduler::get_transaction_status(CdmwFT::Transaction::TPCCohort_ptr cohort, 
                                       CdmwFT::Transaction::TransactionIdentification id,
                                       const std::string & location)
{
    this->enqueue(new Method_Request_Get_Transaction_Status (cohort, 
                                                             m_coordinator,
                                                             id, location));
}

void Scheduler::rollback(CdmwFT::Transaction::TPCCohort_ptr cohort, 
                         CdmwFT::Transaction::TransactionIdentification id,
                         const std::string & location)
{
    this->enqueue(new Method_Request_Rollback (cohort, m_coordinator, id, location));
}

}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw
