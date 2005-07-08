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


#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "platforminterface/Process_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process:1.0
//


//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
//
CORBA::ULong Process_impl::nb_steps()
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    return m_behaviour->nb_steps();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_activity_points:1.0
//
CORBA::ULong Process_impl::nb_activity_points()
    throw(CORBA::SystemException)
{
    return 0;
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
//
CORBA::Object_ptr Process_impl::get_service()
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    return m_behaviour->get_service();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_pull_monitorable:1.0
//
CdmwPlatformMngt::PullMonitorable_ptr Process_impl::get_pull_monitorable()
    throw(CORBA::SystemException)
{
    return CdmwPlatformMngt::PullMonitorable::_nil();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_push_monitorable:1.0
//
CdmwPlatformMngt::PushMonitorable_ptr Process_impl::get_push_monitorable()
    throw(CORBA::SystemException)
{
    return CdmwPlatformMngt::PushMonitorable::_nil();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_activity_point:1.0
//
CdmwPlatformMngt::Process::ActivityPointInfo* Process_impl::get_activity_point(CORBA::ULong point_index)
    throw(CdmwPlatformMngt::Process::OutOfRange, CORBA::SystemException)
{
    throw CdmwPlatformMngt::Process::OutOfRange();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_all_activity_points:1.0
//
CdmwPlatformMngt::Process::ActivityPointInfos* Process_impl::get_all_activity_points()
    throw(CORBA::SystemException)
{
    try
    {

        CdmwPlatformMngt::Process::ActivityPointInfos_var pointInfos =
            new CdmwPlatformMngt::Process::ActivityPointInfos;
        pointInfos->length(0);

        return pointInfos._retn();

    }
    catch(const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY(OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }

}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
//
void Process_impl::initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::Process::BadOrder, CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    m_behaviour->initialise(startup_kind);
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
//
void Process_impl::next_step()
    throw(CdmwPlatformMngt::Process::BadOrder,
          CdmwPlatformMngt::Process::InvalidStep,
          CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    m_behaviour->next_step();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
//
void Process_impl::run()
    throw(CdmwPlatformMngt::Process::NotReadyToRun,
          CdmwPlatformMngt::Process::AlreadyDone,
          CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    m_behaviour->run();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
//
void Process_impl::stop()
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    m_behaviour->stop();
}


Process_impl::Process_impl(ProcessBehaviour *behaviour)
{
    CDMW_ASSERT(behaviour != NULL);
    m_behaviour = behaviour;
}

Process_impl::~Process_impl()
    throw()
{
    delete m_behaviour;
}


} // End namespace PlatformMngt
} // End namespace Cdmw


