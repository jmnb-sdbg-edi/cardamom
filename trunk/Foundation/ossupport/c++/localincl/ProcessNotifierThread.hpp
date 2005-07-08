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


#ifndef INCL_OSSUPPORT_PROCESSNOTIFIERTHREAD_IMPL_HPP
#define INCL_OSSUPPORT_PROCESSNOTIFIERTHREAD_IMPL_HPP

#ifdef _WIN32
#   include <windows.h>
#endif

#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include "Foundation/ossupport/ProcessCallback.hpp"

#include <map>

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Purpose:
        *<p> 
        *
        *Features:
        *<p> Thread safe
        *
        *@see  
        *
        */

        class ProcessNotifierThread : public OsSupport::Thread
        {

            public:

                /**
                * Purpose:
                * <p> Constructor
                * 
                */
                ProcessNotifierThread()
                throw();



                /**
                * Purpose:
                * <p> Destructor
                * 
                */
                virtual
                ~ProcessNotifierThread()
                throw();




                /**
                * Purpose:
                * <p> Add the specified process to the list of processes
                *   to be monitored
                * 
                */
#       if defined(CDMW_POSIX)

            public:

                void
                add_process(OS::ProcessId processInformation, ProcessCallback* endingCallback)
                throw();

            private:

                ProcessCallback* wait_creation(OS::ProcessId processInformation);


#       elif defined(_WIN32)

            public:

                void
                add_process(PROCESS_INFORMATION processInformation, ProcessCallback* endingCallback)
                throw();

            private:

                ProcessCallback* wait_creation(PROCESS_INFORMATION processInformation);

#       else
#           error "Cdmw: Not supported platform"
#       endif


            protected:



                virtual
                void
                run()
                throw();



                /**
                * Container used to store the list of started processes
                */
#       ifdef _WIN32

                typedef std::map<PROCESS_INFORMATION, ProcessCallback*> Processes;

#       elif defined (CDMW_POSIX)

                typedef std::map<OS::ProcessId, ProcessCallback*> Processes;

#       else
#           error "Cdmw: Not supported platform"
#       endif


                /**
                * The list of processes whose ending must be notified.
                */
                Processes m_processes;

                /**
                * Mutex protecting concurrent acces to m_processes.
                */
                OsSupport::Mutex m_processes_mutex;

                /**
                * Condition associated with m_processes_mutex indicating that a process
                * has been added.
                */
                Condition m_processes_condition;


                /**
                * Indicates whether the thread must exit. Only sets to true
                * in the destructor. No protection is needed.
                */
                bool m_thread_must_exit;


            private:

                /**
                * Purpose:
                * <p>  Copy constructor
                * 
                */
                ProcessNotifierThread (const ProcessNotifierThread& rhs)
                throw();



                /**
                * Purpose:
                * <p> Assignment operator
                * 
                */
                ProcessNotifierThread&
                operator=(const ProcessNotifierThread& rhs)
                throw();

#       ifdef _WIN32

                /**
                * Event used to notify the thread their is new process to be
                * monitored.
                */
                HANDLE m_event;

                /**
                * Purpose:
                * <p> Return the list of handle to be monitored associated to the
                *   started process.
                *
                * @return return the index in the processId, of the first invalide
                *       process.
                *
                */
                void
                build_handle_list( HANDLE*& pHandle,
                                   PROCESS_INFORMATION*& pProcessInformation,
                                   size_t& nbHandle)
                throw();



                /**
                * Purpose:
                * <p> Free a list of handle allocated by build_handle_list_from_processId
                */
                void
                free_handle_list( HANDLE* pHandle, PROCESS_INFORMATION* pProcessInformation, size_t nbHandle )
                throw();

#       endif


        }

        ; // End class ProcessNotifierThread

    } // End namespace OsSupport
} // End namespace Cdmw

#endif // INCL_OSSUPPORT_PROCESSNOTIFIERTHREAD_IMPL_HPP

