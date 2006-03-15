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


#ifndef _INCL_MYPROCESSBEHAVIOUR_HPP_
#define _INCL_MYPROCESSBEHAVIOUR_HPP_


#include <string>

#include "Foundation/common/Exception.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "traceclient2/TraceClientThread.hpp"


/**
 *
 */
class MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
{
    public:
        /**
         * Constructor.
         */
        MyProcessBehaviour(CORBA::ORB_ptr orb,
                           std::string& traceMessage,
                           unsigned long traceCount);

        /**
         * Destructor.
         */
        virtual
        ~MyProcessBehaviour()
            throw();

        /**
         * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
         */
        virtual CORBA::ULong
        nb_steps()
            throw(CORBA::SystemException)
        {
            return 1;
        }

        /**
         * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
         */
        virtual CORBA::Object_ptr
        get_service()
            throw(CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
         */
        virtual void
        initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
         */
        virtual void
        next_step()
            throw(CdmwPlatformMngt::ProcessDelegate::InvalidStep,
                  CORBA::SystemException)
        {
        }

        /**
         * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
         */
        virtual void
        run()
            throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
                  CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
                  CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
         */
        virtual void
        stop()
            throw(CORBA::SystemException);

        /**
         *
         */
        void
        set_init_done();

        /**
         *
         */
        Cdmw::OsSupport::Condition&
        get_condition()
        {
            return *m_condition;
        }

    private:
        // the ORB
        CORBA::ORB_var m_orb;

        // name of the application
        std::string m_applicationName;

        // name of the process
        std::string m_processName;

        // message to trace
        std::string m_traceMessage;

        // number of times the message must be traced
        unsigned long m_traceCount;

        // reference of the client thread
        TraceClientThread* m_trace_client_thread;

        // init flag
        bool m_init_done;

        // the condition and its associated mutex
        Cdmw::OsSupport::Condition* m_condition;
        Cdmw::OsSupport::Mutex      m_mutex;
};


#endif // _INCL_MYPROCESSBEHAVIOUR_HPP_
