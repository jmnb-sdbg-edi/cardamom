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


#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "platforminterface/ProcessDelegate_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

/**
*Purpose: ProcessPullMonitorable implementation
*<p>
*
*
*/
class ProcessPullMonitorable_impl : virtual public POA_FT::PullMonitorable,
                                    virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        *
        */
        ProcessPullMonitorable_impl(ProcessBehaviour* behaviour)
                throw()
        {
           m_behaviour = behaviour;
        }

        /**
        * Purpose:
        * <p> Destructor
        *
        */
        virtual
        ~ProcessPullMonitorable_impl()
                throw()
        {
        }

        /**
                * Purpose:
                * <p> to call to check if object is still living
                *
                *@exception CORBA::SystemException
                *
                */
        virtual CORBA::Boolean is_alive ()
                throw(CORBA::SystemException)
        {
             return m_behaviour->is_alive();
        }

    protected:

    private:

    /**
     *Purpose:
     *<p> The actual behaviour of the process.
     */
    ProcessBehaviour *m_behaviour;

}; // End class ProcessPullMonitorable_impl

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate:1.0
//


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
//
CORBA::ULong ProcessDelegate_impl::nb_steps()
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    return m_behaviour->nb_steps();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
//
CORBA::Object_ptr ProcessDelegate_impl::get_service()
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    return m_behaviour->get_service();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_pull_monitorable:1.0
//
::FT::PullMonitorable_ptr ProcessDelegate_impl::get_pull_monitorable()
    throw(CORBA::SystemException)
{
   return ::FT::PullMonitorable::_duplicate(m_pullMonitorable.in()); 
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_push_monitorable:1.0
//
CdmwPlatformMngt::PushMonitorable_ptr ProcessDelegate_impl::get_push_monitorable()
    throw(CORBA::SystemException)
{
    return CdmwPlatformMngt::PushMonitorable::_nil();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
//
void ProcessDelegate_impl::initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    m_behaviour->initialise(startup_kind);
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
//
void ProcessDelegate_impl::next_step()
    throw(CdmwPlatformMngt::ProcessDelegate::BadOrder,
          CdmwPlatformMngt::ProcessDelegate::InvalidStep,
          CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    m_behaviour->next_step();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
//
void ProcessDelegate_impl::run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
          CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
          CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    m_behaviour->run();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
//
void ProcessDelegate_impl::stop()
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(m_behaviour != NULL);

    m_behaviour->stop();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/call_command:1.0
//
CdmwPlatformMngt::CommandResponse ProcessDelegate_impl::call_command (
                 const char * command_name,
                 const CdmwPlatformMngt::CmdParameterValues & command_args,
                 CORBA::String_out result_info)
     throw(CdmwPlatformMngt::CommandCallBackNotFound,
           CORBA::SystemException)
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/call_command:1.0
//
CdmwPlatformMngt::CommandResponse ProcessDelegate_impl::call_entity_command (
                const char * entity_name,
                const char * command_name,
                const CdmwPlatformMngt::CmdParameterValues & command_args,
                CORBA::String_out result_info)
        throw(CdmwPlatformMngt::CommandCallBackNotFound,
              CORBA::SystemException)
{
    // TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


ProcessDelegate_impl::ProcessDelegate_impl(CORBA::ORB_ptr orb, ProcessBehaviour *behaviour)
  : m_behaviour(behaviour)
{
    CDMW_ASSERT(behaviour != NULL);
    CDMW_ASSERT(orb != CORBA::ORB::_nil());

    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

    PortableServer::POAManager_var poaMgr = poa->the_POAManager();
    poaMgr->activate();

    PortableServer::ServantBase_var servant_var =
        new ProcessPullMonitorable_impl (behaviour);

    ProcessPullMonitorable_impl* p_PullMonitorableServant =
        dynamic_cast<ProcessPullMonitorable_impl*>(servant_var.in());

    // create the CORBA reference
    m_pullMonitorable = p_PullMonitorableServant->_this();
}

ProcessDelegate_impl::~ProcessDelegate_impl()
    throw()
{
    delete m_behaviour;
}


} // End namespace PlatformMngt
} // End namespace Cdmw


