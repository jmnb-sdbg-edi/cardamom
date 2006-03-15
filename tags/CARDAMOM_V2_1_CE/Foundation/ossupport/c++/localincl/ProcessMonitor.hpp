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


#ifndef INCL_OSSUPPORT_PROCESS_MONITOR_HPP
#define INCL_OSSUPPORT_PROCESS_MONITOR_HPP


#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/common/Exception.hpp"


#include <list>
#include <map>


/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW OsSupport
    */

    namespace OsSupport
    {

        class ProcessCallback;

        class MonitorGarbageCollector;

        class ProcessMonitor;


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

        class ProcessMonitorStarter
        {

            public:

                /**
                * Purpose:
                * <p> Constructor
                *
                * @param path path to the executable
                * @param args arguments to pass to the process
                * @param home directory where the process must be started
                * @param creationCallback the callback that will be executed just after
                *                         the process creation.
                * @param endingCallback   the callback that will be executed just after
                *                         the process ending.
                * 
                */
                ProcessMonitorStarter(
                    const std::string& path, const std::string& args,
                    const std::string& home = "",
                    ProcessCallback* creationCallback = NULL,
                    ProcessCallback* endingCallback = NULL)
                throw();



                /**
                * Purpose:
                * <p> Destructor
                * 
                */
                virtual
                ~ProcessMonitorStarter()
                throw();




                /**
                * Purpose:
                * <p> Request to create a new process
                *
                * @return the process id of the newly created process
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException
                */
                OS::ProcessId createProcess ()
                throw (OutOfMemoryException,
                       InternalErrorException);




                /**
                * Purpose:
                * <p> release ProcessCreation call
                *
                */
                void releaseProcessCreation ();


                /**
                * Purpose:
                * <p> set process id
                *
                */
                void set_processId (const OS::ProcessId processId);


                /**
                * Purpose:
                * <p> set process creation error
                *
                */
                void set_createErr (const int createErr);



            private:

                /**
                * pointer to creation callback
                */
                ProcessCallback* m_creationCallback;


                /**
                * creation code error
                */
                int m_createErr;


                /**
                * id of created process.
                */
                OS::ProcessId m_processId;

                /**
                * Mutex associated to the processMonitor condition.
                */
                OsSupport::Mutex m_processMonitor_mutex;

                /**
                * Condition for waiting the end of process creation
                */
                Condition m_processMonitor_condition;


                /**
                * pointer to associated ProcessMonitor
                */
                ProcessMonitor* m_processMonitor;


        }

        ; // End class ProcessMonitorStarter


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

        class ProcessMonitor : public OsSupport::Thread
        {

            public:

                /**
                * Purpose:
                * <p> Constructor
                *
                * @param processMonitorStarter the associated ProcessMonitorStarter
                * @param path path to the executable
                * @param args arguments to pass to the process
                * @param home directory where the process must be started
                * @param creationCallback the callback that will be executed just after
                *                         the process creation.
                * @param endingCallback   the callback that will be executed just after
                *                         the process ending.
                * 
                */
                ProcessMonitor(ProcessMonitorStarter* processMonitorStarter,
                               const std::string& path, const std::string& args,
                               const std::string& home,
                               ProcessCallback* creationCallback,
                               ProcessCallback* endingCallback)
                throw();



                /**
                * Purpose:
                * <p> Destructor
                * 
                */
                virtual
                ~ProcessMonitor()
                throw();


                /**
                * Purpose:
                * <p> get process id
                *
                */
                const OS::ProcessId get_processId();



            protected:

                virtual
                void
                run()
                throw();


            private:


                /**
                * executable path of process
                */
                std::string m_path;

                /**
                * arguments of process
                */
                std::string m_args;

                /**
                * home directory of process
                */
                std::string m_home;


                /**
                * pointer to creation callback
                */
                ProcessCallback* m_creationCallback;

                /**
                * pointer to ending callback
                */
                ProcessCallback* m_endingCallback;


                /**
                * id of created process.
                */
                OS::ProcessId m_processId;


                /**
                * pointer to associated ProcessMonitorStarter
                */
                ProcessMonitorStarter* m_processMonitorStarter;


        }

        ; // End class ProcessMonitor




        class MonitorGarbageCollector : public OsSupport::Thread
        {

            public:

                /**
                * Purpose:
                * <p> Constructor
                * 
                */
                MonitorGarbageCollector ();

                /**
                * Purpose:
                * <p> Destructor
                * 
                */
                virtual
                ~MonitorGarbageCollector()
                throw();

                /**
                * Purpose:
                * <p> Ask to the internal thread to stop its job.
                *     When this function return the internal thread have stopped.
                *
                *@exception InternalErrorException
                *
                */
                void stop()
                throw (InternalErrorException);



                /**
                * Purpose:
                * <p> Insert started ProcessMonitor into garbage list
                * 
                */
                void addStartedProcessMonitor(ProcessMonitor* p_processMonitor)
                throw (OutOfMemoryException);



                /**
                * Purpose:
                * <p> Insert terminated ProcessMonitor into garbage list
                * 
                */
                void addTerminatedProcessMonitor(ProcessMonitor* p_processMonitor)
                throw (OutOfMemoryException);


            protected:

                /**
                * Purpose:
                * <p> Call by the underlying thread system in a new thread
                * 
                */
                virtual
                void run()
                throw();



            private:


                //
                // Copy ctor
                //
                MonitorGarbageCollector (const MonitorGarbageCollector& rhs)
                throw();

                //
                // Assignement operator
                //
                MonitorGarbageCollector& operator=(const MonitorGarbageCollector& rhs)
                throw();


                /**
                * At each iteration the thread test this variable, to
                * see if it must stop it's processing. If true
                * the thread terminates.
                */
                bool m_threadToStop;

                /**
                * Mutex protecting the ProcessMonitor containers
                */
                OsSupport::Mutex m_processMonitors_mutex;

                /**
                * Condition on which the thread is sleeping
                * if there is no more job.
                */
                OsSupport::Condition m_garbage_condition;


                /**
                * Define the container used to storea all ProcessMonitor
                * currently managed by the MonitorGarbageCollector (flushed out or
                * not).
                */
                typedef std::list <ProcessMonitor*> ProcessMonitorCtr;


                /**
                * Contains the list of all terminated ProcessMonitor to delete
                * This list mut be manipulated with the m_processMonitors_mutex
                * Mutex acquired.
                */
                ProcessMonitorCtr m_processMonToDelete;


                /**
                * Mutex protecting the ProcessMonitor containers
                */
                OsSupport::Mutex m_startedProcesses_mutex;

                /**
                * Container used to store the list of started processes
                */
                typedef std::map<OS::ProcessId, ProcessMonitor*> StartedProcesses;

                /**
                * The map of started processes.
                */
                StartedProcesses m_startedProcesses;


        }

        ; // End class MonitorGarbageCollector





    }  // End namespace OsSupport
}  // End namespace Cdmw



#endif // INCL_OSSUPPORT_PROCESS_MONITOR_HPP

