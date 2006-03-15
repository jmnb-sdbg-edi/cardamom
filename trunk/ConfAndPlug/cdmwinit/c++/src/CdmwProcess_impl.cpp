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


#include "ConfAndPlug/cdmwinit/CdmwProcess_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "Foundation/idllib/FTPullMonitorable.skel.hpp"

// Anonymous namespace
namespace {
    /**
     *Purpose: Default PullMonitorable object
     *<p> 
     *
     *
     */
    class DefaultPullMonitorable_impl : virtual public POA_FT::PullMonitorable,
                                        virtual public Cdmw::OrbSupport::RefCountServantBase
    {
    public:
        /**
         * Purpose:
         * <p> Constructor
         *
         */ 
        DefaultPullMonitorable_impl(PortableServer::POA_ptr                  poa,
                                    Cdmw::CdmwInit::ProcessControl*          ctrl)
            throw(CORBA::SystemException)
            : m_process_ctrl(ctrl),
              m_poa(PortableServer::POA::_duplicate(poa))
        {
            CDMW_ASSERT(!CORBA::is_nil(poa));
            CDMW_ASSERT(ctrl != 0);
            m_process_ctrl->_add_ref();
        }
        
        /**
         * Purpose:
         * <p> Destructor
         * 
         */ 
        virtual 
        ~DefaultPullMonitorable_impl()
            throw()
        {
        }

        virtual PortableServer::POA_ptr _default_POA() {
            return PortableServer::POA::_duplicate(m_poa.in());
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
            bool alive = m_process_ctrl->is_alive();
            return (alive ? 1:0);
        }
        
    private:
        
        /**
         * Purpose:
         * <p>  Copy constructor
         * 
         */ 
        DefaultPullMonitorable_impl (const DefaultPullMonitorable_impl& rhs)
            throw();
        
        /**
         * Purpose:
         * <p> Assignment operator
         * 
         */ 
        DefaultPullMonitorable_impl&
        operator=(const DefaultPullMonitorable_impl& rhs)
            throw();
        
        /**
         * The actual behaviour of the process
         */  
        Cdmw::CdmwInit::ProcessControl_var      m_process_ctrl;
        
        /**
         * Parent POA
         */  
        PortableServer::POA_var m_poa;
        
    }; // End class DefaultPullMonitorable_impl

};


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
      m_default_pull_monitorable(::FT::PullMonitorable::_nil()),
      m_cdmw_init_exception_raised(false),
      m_cdmw_init_done(false)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CDMW_ASSERT(ctrl != 0);
    m_process_ctrl->_add_ref();

    // Creating a Condition associated with the member Mutex
    m_condition = new OsSupport::Condition(m_mutex);

    // Create a default PullMonitorableObject
    DefaultPullMonitorable_impl * pm = new DefaultPullMonitorable_impl(poa,ctrl);
    m_default_pull_monitorable = pm->_this();
    // Release a ref since the POA is holding one.
    pm->_remove_ref();
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
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
//
CORBA::ULong 
CdmwProcess_impl::nb_steps()
    throw(CORBA::SystemException)
{
    return m_process_ctrl->nb_steps();
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
//
CORBA::Object_ptr 
CdmwProcess_impl::get_service()
    throw(CORBA::SystemException)
{
    return m_process_ctrl->get_service();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_pull_monitorable:1.0
//
::FT::PullMonitorable_ptr 
CdmwProcess_impl::get_pull_monitorable()
    throw(CORBA::SystemException)
{
    return ::FT::PullMonitorable::_duplicate(m_default_pull_monitorable.in());
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_push_monitorable:1.0
//
CdmwPlatformMngt::PushMonitorable_ptr 
CdmwProcess_impl::get_push_monitorable()
    throw(CORBA::SystemException)
{
    return CdmwPlatformMngt::PushMonitorable::_nil();
}


//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
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
            PlatformMngt::PlatformInterface::Notify_fatal_error(PlatformMngt::PlatformInterface::Get_process_name(),
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
            PlatformMngt::PlatformInterface::Notify_fatal_error(PlatformMngt::PlatformInterface::Get_process_name(),
                        "Unexpected statement reached!");
        }
    }
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
//
void CdmwProcess_impl::next_step()
    throw(CdmwPlatformMngt::ProcessDelegate::InvalidStep,
          CORBA::SystemException)
{
    // TODO: Add CDMW specific Init if necessary
    m_process_ctrl->on_next_step();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
//
void CdmwProcess_impl::run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
          CORBA::SystemException)
{
    // TODO: Add CDMW specific stuff if necessary
    m_process_ctrl->on_run();
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
//
void CdmwProcess_impl::stop()
    throw(CORBA::SystemException)
{
    m_process_ctrl->on_stop();
    // TODO: Add CDMW specific stuff if necessary
    // then initiate ORB shutdown
    m_orb->shutdown(false);
}

//
// IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/call_command:1.0
//
CdmwPlatformMngt::CommandResponse CdmwProcess_impl::call_command (
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
CdmwPlatformMngt::CommandResponse CdmwProcess_impl::call_entity_command (
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


}; // End namespace CdmwInit
}; // End namespace Cdmw


