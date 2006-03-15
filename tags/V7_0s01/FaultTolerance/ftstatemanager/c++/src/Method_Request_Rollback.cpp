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

#include "ftstatemanager/Method_Request_Rollback.hpp"

namespace Cdmw {
namespace FT {
namespace Scheduler {

Method_Request_Rollback::Method_Request_Rollback
(CdmwFT::Transaction::TPCCohort_ptr cohort,
 Cdmw::FT::TransactionEngineCoordinatorBase * coordinator,
 CdmwFT::Transaction::TransactionIdentification id,
 const std::string & location)
    : m_cohort(CdmwFT::Transaction::TPCCohort::_duplicate(cohort)),
      m_coordinator(coordinator),
      m_id(id),
      m_location(location)
{
}
Method_Request_Rollback::~Method_Request_Rollback (void) throw()
{
}


void Method_Request_Rollback::run () throw()
{
    try
    {
        CdmwFT::Transaction::TransactionSender_var sender;
        m_cohort->rollback(m_id, sender.out());
        m_coordinator->send_end_i(m_location.c_str(), m_id);
    }
    catch (const CORBA::TIMEOUT &)
    {
        // ignore for now
    }
    catch (...)
    {
        // ignore for now
    }
}

}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw