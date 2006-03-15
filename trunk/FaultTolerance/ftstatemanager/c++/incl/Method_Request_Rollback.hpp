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

#ifndef _CDMW_FT_METHOD_REQUEST_ROLLBACK_HPP_
#define _CDMW_FT_METHOD_REQUEST_ROLLBACK_HPP_
#include <FaultTolerance/ftstatemanager/TransactionEngineCoordinatorBase.hpp>
#include <Foundation/osthreads/Runnable.hpp>

namespace Cdmw {
namespace FT {
namespace Scheduler {


class Method_Request_Rollback : public Cdmw::OsSupport::Runnable
{
public:
    Method_Request_Rollback (CdmwFT::Transaction::TPCCohort_ptr cohort,
                           Cdmw::FT::TransactionEngineCoordinatorBase* coordinator,
                           CdmwFT::Transaction::TransactionIdentification id,
                           const std::string & location);
    
    virtual ~Method_Request_Rollback (void) throw();
    
    virtual void run() throw();
    
private:
    CdmwFT::Transaction::TPCCohort_var m_cohort;
    Cdmw::FT::TransactionEngineCoordinatorBase * m_coordinator;
    CdmwFT::Transaction::TransactionIdentification m_id;
    std::string m_location;
};
 

}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_METHOD_REQUEST_ROLLBACK_HPP_
