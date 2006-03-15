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


#ifndef _INCL_TRACECLIENTTHREAD_HPP_
#define _INCL_TRACECLIENTTHREAD_HPP_


#include <iostream>
#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>


/**
 *
 */
class TraceClientThread : public Cdmw::OsSupport::Thread
{
    public:
        /**
         * Constructor.
         */
        TraceClientThread(CORBA::ORB_ptr,
                          std::string& applicationName,
                          std::string& processName,
                          std::string& traceMessage,
                          unsigned long traceCount)
            throw(CORBA::SystemException);

        /**
         * Destructor.
         */
        ~TraceClientThread() throw();

        /**
         *
         */
        void
        stop()
        {
            m_exit_required = true;
        }

    protected:
        /**
         *
         */
        void
        run()
            throw();

    private:
        /**
         * Default constructor.
         */
        TraceClientThread();

        /**
         * Copy constructor.
         */
        TraceClientThread(const TraceClientThread&);

        /**
         * Assignment operator.
         */
        TraceClientThread&
        operator=(const TraceClientThread&);

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

        // flag to break the thread loop
        bool m_exit_required;
};


#endif // _INCL_TRACECLIENTTHREAD_HPP_
