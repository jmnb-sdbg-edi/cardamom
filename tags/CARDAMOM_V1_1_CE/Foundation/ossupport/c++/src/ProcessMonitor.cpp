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


#include <iostream>
#include <sstream>


#include "Foundation/common/MacroDefs.h"
#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/ossupport/ProcessCallback.hpp"


#include "ossupport/ProcessMonitor.hpp"



#if defined(CDMW_POSIX)

#include <sys/types.h>
#include <sys/wait.h>

#endif


namespace Cdmw
{

    namespace OsSupport
    {


        // ------------------------------------------------------
        // ProcessMonitorStarter class
        // ------------------------------------------------------

        //
        // Ctor
        //
        ProcessMonitorStarter::ProcessMonitorStarter (
            const std::string& path, const std::string& args,
            const std::string& home,
            ProcessCallback* creationCallback,
            ProcessCallback* endingCallback)
        throw()
                : m_creationCallback (creationCallback),
                m_processMonitor_condition (m_processMonitor_mutex)

        {
            // create the ProcessMonitor associated to the new process
            std::auto_ptr <ProcessMonitor> pProcessMonitor (
                new ProcessMonitor (
                    this,
                    path, args, home,
                    creationCallback,
                    endingCallback));

            m_processMonitor = pProcessMonitor.release();

        }



        //
        // Dtor
        //
        ProcessMonitorStarter::~ProcessMonitorStarter()
        throw()
        {}



        //
        // release ProcessCreation call
        //   by broadcasting the condition in createProcess
        //
        void ProcessMonitorStarter::releaseProcessCreation ()
        {
            CDMW_MUTEX_GUARD (m_processMonitor_mutex);

            // broadcast the condition
            m_processMonitor_condition.broadcast();
        }


        //
        // set process id
        //
        void ProcessMonitorStarter::set_processId (const OS::ProcessId processId)
        {
            m_processId = processId;
        }


        //
        // set process creation error
        //
        void ProcessMonitorStarter::set_createErr (const int createErr)
        {
            m_createErr = createErr;
        }


        //
        // Request to create a new process
        //
        OS::ProcessId ProcessMonitorStarter::createProcess ()
        throw (OutOfMemoryException,
               InternalErrorException)
        {
            try
            {
                CDMW_MUTEX_GUARD (m_processMonitor_mutex);

                try
                {
                    // start the thread in charge of process creation
                    m_processMonitor->start();
                }
                catch (...)
                {
                    // exception is detected :
                    // ProcessMonitor is not started so delete it
                    delete m_processMonitor;

                    throw;
                }


                // wait end of creation
                m_processMonitor_condition.wait();


                // check creation error code
                switch (m_createErr)
                {
                        // There is no error

                        case 0:
                        break;

                        // There is not enough memory

                        case 1:
                        CDMW_THROW(OutOfMemoryException);
                        break;

                        default:
                        CDMW_THROW(InternalErrorException);
                        break;
                }


                // if no error m_processId is set the new process ID

                // call creation callback
                if (m_creationCallback != NULL)
                {
                    m_creationCallback->execute(m_processId);
                }


                // return the pid of new process
                return m_processId;

            }
            catch (const ThreadSchedulingException&)
            {
                CDMW_THROW (InternalErrorException);
            }
        }



        // ------------------------------------------------------
        // ProcessMonitor class
        // ------------------------------------------------------


        //
        // Ctor
        //
        ProcessMonitor::ProcessMonitor(
            ProcessMonitorStarter* processMonitorStarter,
            const std::string& path, const std::string& args,
            const std::string& home,
            ProcessCallback* creationCallback,
            ProcessCallback* endingCallback)
        throw()
                : m_path (path),
                m_args (args),
                m_home (home),
                m_creationCallback (creationCallback),
                m_endingCallback (endingCallback),
                m_processMonitorStarter (processMonitorStarter)

        {}



        //
        // Dtor
        //
        ProcessMonitor::~ProcessMonitor()
        throw()
        {
        }


        //
        // get process id
        //
        const OS::ProcessId ProcessMonitor::get_processId()
        {
            return m_processId;
        }


        //
        // Call by the underlying thread system in a new thread
        //
        void ProcessMonitor::run()
        throw()
        {
            int createErr = 0;

            try
            {
                // create the process
                m_processId = OS::_create_process (
                                  m_path, m_args, m_home,
                                  m_creationCallback, NULL);

                // set new process id for ProcessMonitorStarter
                m_processMonitorStarter->set_processId (m_processId);

                // add the started object to garbage collector
                OS::addStartedProcessMonitor (this);

            }
            catch (const OutOfMemoryException&)
            {
                createErr = 1;
            }
            catch (const InternalErrorException&)
            {
                createErr = 2;
            }
            catch (...)
            {
                createErr = 3;
            }

            // set creation error for ProcessMonitorStarter
            m_processMonitorStarter->set_createErr (createErr);

            // release the process creation calling in ProcessMonitorStarter
            m_processMonitorStarter->releaseProcessCreation();


#if defined(CDMW_POSIX)

            // wait completion of process
            int status;

            pid_t pid = ::wait (&status);

            // If process is died
            if (pid != -1)
            {
                OS::ProcessId processPid = pid;

                // call the ending callback if defined

                if (m_endingCallback != NULL)
                {
                    m_endingCallback->execute(processPid);
                }
            }

#endif

            // add the terminated object to garbage collector
            OS::addTerminatedProcessMonitor (this);
        }




        // ------------------------------------------------------
        // MonitorGarbageCollector class
        // ------------------------------------------------------


        //
        // Ctor
        //
        MonitorGarbageCollector::MonitorGarbageCollector ()
                : m_threadToStop (false),
                m_garbage_condition (m_processMonitors_mutex)
        {}


        //
        // Dtor
        //
        MonitorGarbageCollector::~MonitorGarbageCollector()
        throw()
        {
            // Delete all ProcessMonitor remaining in the "started list"

            try
            {
                CDMW_MUTEX_GUARD (m_startedProcesses_mutex);

                for (StartedProcesses::iterator itr = m_startedProcesses.begin() ;
                        itr != m_startedProcesses.end() ; ++itr)
                {
                    try
                    {
                        ProcessMonitor* p_processMonitor = itr->second;

                        // kill the monitored process if not already done

                        try
                        {
                            OS::ProcessId pid = p_processMonitor->get_processId();
                            OS::kill_process (pid);
                        }
                        catch (...)
                        {}

                        // wait termination of ProcessMonitor
                        // then delete the object
                        p_processMonitor->join();

                        delete p_processMonitor;

                    }
                    catch (...)
                    {}

                }

            }

            // catch any exception
            catch (...)
            {}

        }


        //
        // Ask to the internal thread to stop its job.
        //
        void MonitorGarbageCollector::stop()
        throw (InternalErrorException)
        {
            {
                CDMW_MUTEX_GUARD (m_processMonitors_mutex);
            
                // Require the thread to terminate
                m_threadToStop = true;

                //  Awake the thread, if it is sleeping
                m_garbage_condition.broadcast();
            }

            try
            {
                // Wait the termination of internal thread
                this->join();
            }

            // No explanation, how is it possible : corrupted
            catch (const OsSupport::DeadlockException&)
            {
                CDMW_THROW (InternalErrorException);
            }
        }


        //
        // Insert terminated ProcessMonitor into garbage list
        //
        void MonitorGarbageCollector::addStartedProcessMonitor(
            ProcessMonitor* p_processMonitor)
        throw (OutOfMemoryException)
        {
            try
            {
                CDMW_MUTEX_GUARD (m_startedProcesses_mutex);

                // Insert ProcessMonitor in the list
                OS::ProcessId pid = p_processMonitor->get_processId();
                m_startedProcesses[pid] = p_processMonitor;
            }
            catch (const std::bad_alloc&)
            {
                CDMW_THROW (OutOfMemoryException);
            }

            // catch any exception from Mutex and assert it
            catch (...)
            {
                CDMW_ASSERT(false);
            }
        }




        //
        // Insert terminated ProcessMonitor into garbage list
        //
        void MonitorGarbageCollector::addTerminatedProcessMonitor(
            ProcessMonitor* p_processMonitor)
        throw (OutOfMemoryException)
        {
            {
                CDMW_MUTEX_GUARD (m_startedProcesses_mutex);

                // search the process monitor in started ProcessMonitor map
                // to erase it
                OS::ProcessId pid = p_processMonitor->get_processId();

                StartedProcesses::iterator it = m_startedProcesses.find(pid);

                if (it != m_startedProcesses.end())
                {
                    m_startedProcesses.erase(it);
                }
            }


            try
            {
                CDMW_MUTEX_GUARD (m_processMonitors_mutex);

                // Insert ProcessMonitor in the list
                m_processMonToDelete.push_back (p_processMonitor);

                // Awake the thread, if it is sleeping
                m_garbage_condition.broadcast();

            }
            catch (const std::bad_alloc&)
            {
                CDMW_THROW (OutOfMemoryException);
            }

            // catch any exception from Mutex and assert it
            catch (...)
            {
                CDMW_ASSERT(false);
            }
        }


        //
        // Call by the underlying thread system in a new thread
        //
        void MonitorGarbageCollector::run()
        throw()
        {
            try
            {
                // take the mutex
                CDMW_MUTEX_GUARD (m_processMonitors_mutex);
                    
                // Check, if someone has not ask the thread to terminate

                while (! m_threadToStop)
                {
                    // If there is ProcessMonitor to delete in the list,
                    // extract the next one

                    while (m_processMonToDelete.size() != 0)
                    {
                        ProcessMonitorCtr::iterator itr = m_processMonToDelete.begin();

                        // get and erase in list
                        ProcessMonitor* p_processMonitor = *itr;
                        m_processMonToDelete.erase (itr);

                        // delete the terminated ProcessMonitor Thread
                        p_processMonitor->join();

                        delete p_processMonitor;
                    }

                    // No more ProcessMonitor, wait they become available
                    m_garbage_condition.wait();
                }
            }

            // catch any exception and assert it
            catch (...)
            {
                CDMW_ASSERT(false);
            }
        }




    } // End namespace OsSupport
} // End namespace Cdmw

