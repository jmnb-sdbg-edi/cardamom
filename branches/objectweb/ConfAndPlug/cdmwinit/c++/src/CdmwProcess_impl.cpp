/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "ConfAndPlug/cdmwinit/CdmwProcess_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"

namespace Cdmw
{
namespace CdmwInit
{

CdmwProcess_impl::CdmwProcess_impl(CORBA::ORB_ptr           orb,
                                     PortableServer::POA_ptr  poa,
                                     PortableServer::POA_ptr  CDMW_rootPOA,
                                     ProcessControl*          ctrl)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_CDMW_rootPOA(PortableServer::POA::_duplicate(CDMW_rootPOA)),
      m_process_ctrl(ctrl),
      m_cdmw_init_exception_raised(false),
      m_cdmw_init_done(false)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CDMW_ASSERT(ctrl != 0);
    m_process_ctrl->_add_ref();

    // Creating a Condition associated with the member Mutex
    m_condition = new OsSupport::Condition(m_mutex);

}

CdmwProcess_impl::~CdmwProcess_impl()
    throw()
{
    delete m_condition;
}


void 
CdmwProcess_impl::set_cdmw_init_exception_raised() 
{
    m_mutex.lock();
    m_cdmw_init_exception_raised = true;
    m_mutex.unlock();
}

void 
CdmwProcess_impl::set_cdmw_init_done() 
{
    m_mutex.lock();
    m_cdmw_init_done = true;
    m_mutex.unlock();
}


OsSupport::Condition& 
CdmwProcess_impl::get_condition() 
{
    return *m_condition;
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
//
CORBA::ULong 
CdmwProcess_impl::nb_steps()
    throw(CORBA::SystemException)
{
    return m_process_ctrl->nb_steps();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_activity_points:1.0
//
CORBA::ULong 
CdmwProcess_impl::nb_activity_points()
    throw(CORBA::SystemException)
{
    return 0;
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
//
CORBA::Object_ptr 
CdmwProcess_impl::get_service()
    throw(CORBA::SystemException)
{
    return m_process_ctrl->get_service();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_pull_monitorable:1.0
//
CdmwPlatformMngt::PullMonitorable_ptr 
CdmwProcess_impl::get_pull_monitorable()
    throw(CORBA::SystemException)
{
    return CdmwPlatformMngt::PullMonitorable::_nil();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_push_monitorable:1.0
//
CdmwPlatformMngt::PushMonitorable_ptr 
CdmwProcess_impl::get_push_monitorable()
    throw(CORBA::SystemException)
{
    return CdmwPlatformMngt::PushMonitorable::_nil();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_activity_point:1.0
//
CdmwPlatformMngt::Process::ActivityPointInfo* 
CdmwProcess_impl::get_activity_point(CORBA::ULong point_index)
    throw(CdmwPlatformMngt::Process::OutOfRange, CORBA::SystemException)
{
    throw CdmwPlatformMngt::Process::OutOfRange();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_all_activity_points:1.0
//
CdmwPlatformMngt::Process::ActivityPointInfos* 
CdmwProcess_impl::get_all_activity_points()
    throw(CORBA::SystemException)
{
    try {
        CdmwPlatformMngt::Process::ActivityPointInfos_var pointInfos =
            new CdmwPlatformMngt::Process::ActivityPointInfos;
        pointInfos->length(0);

        return pointInfos._retn();

    } catch(const std::bad_alloc&) {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
//
void CdmwProcess_impl::initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    using namespace Cdmw::OsSupport;

    m_mutex.lock();
   
    while (!(m_cdmw_init_exception_raised || m_cdmw_init_done))
    {
        // Wait for cdmw_init end
        m_condition->wait();
    }

    if (m_cdmw_init_exception_raised)
    {
            m_mutex.unlock();
            PlatformMngt::PlatformInterface::notifyFatalError(PlatformMngt::PlatformInterface::getProcessName(),
                        "Exception raised during CDMW_init!");
    }
    else
    {
        if (m_cdmw_init_done)
        {
            m_mutex.unlock();
            // CDMW POA Managers is in the discarding state. So we have to re-activate it
            // NOTA: We do activate the POA manager before on_initialise()
            //       to enable nested callbacks are needed (user receives an incomming request
            //       while making an outgoing call).
            PortableServer::POAManager_var CDMWpoaManager = m_CDMW_rootPOA -> the_POAManager();
            CDMWpoaManager->activate();
            
            m_process_ctrl->on_initialise(startup_kind);
        }
        else
        {
            m_mutex.unlock();
            PlatformMngt::PlatformInterface::notifyFatalError(PlatformMngt::PlatformInterface::getProcessName(),
                        "Unexpected statement reached!");
        }
    }
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
//
void CdmwProcess_impl::next_step()
    throw(CdmwPlatformMngt::Process::InvalidStep,
          CORBA::SystemException)
{
    // TODO: Add CDMW specific Init if necessary
    m_process_ctrl->on_next_step();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
//
void CdmwProcess_impl::run()
    throw(CdmwPlatformMngt::Process::NotReadyToRun,
          CORBA::SystemException)
{
    // TODO: Add CDMW specific stuff if necessary
    m_process_ctrl->on_run();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
//
void CdmwProcess_impl::stop()
    throw(CORBA::SystemException)
{
    m_process_ctrl->on_stop();
    // TODO: Add CDMW specific stuff if necessary
    // then initiate ORB shutdown
    m_orb->shutdown(false);
}

}; // End namespace CdmwInit
}; // End namespace Cdmw


