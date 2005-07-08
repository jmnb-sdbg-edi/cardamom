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


#ifndef INCL_PLATFORMMNGT_PROCESSEVENTDISPATCHER_HPP 
#define INCL_PLATFORMMNGT_PROCESSEVENTDISPATCHER_HPP 

#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"

#include <map>
#include <string>
#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtPlatformObserver.stub.hpp"

#include "Foundation/ossupport/ProcessObserver.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/ossupport/OS.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


class PlatformNotifier_impl;

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
class ProcessEventDispatcher : public OsSupport::ProcessObserver
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        ProcessEventDispatcher()
                throw();



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~ProcessEventDispatcher()
                throw();

        /**
        * Purpose:
        * <p> Call by the Process sub-system, when a process is created
        * 
        */ 
        virtual 
        void
        onProcessStarted( OsSupport::OS::ProcessId)
                throw();

        
        /**
        * Purpose:
        * <p> Call by the Process sub-system, when a process died
        * 
        */ 
        virtual 
        void
        onProcessExited( OsSupport::OS::ProcessId)
                throw();


        /**
        * Define the set of informations identifying a process.
        */
        struct ProcessInformation
        {
            std::string processName;
            std::string applicationName;
            
            enum Status {
                STARTED,
                DEAD
            } status;

        };



        /**
        * Purpose:
        * <p> Start and register information about a newly created process
        * 
        */    
        OsSupport::OS::ProcessId 
        create_process( const std::string& exe, 
                        const std::string& args, 
                        const std::string& run_directory,
                        const ProcessInformation& processInformation)
            throw( BadParameterException, OutOfMemoryException, InternalErrorException );

        /**
        * DELETED -- Purpose:
        * <p> Register information about a newly created process
        * 
        */    
        /*void 
        add_process( OsSupport::OS::ProcessId pid, 
                     const ProcessInformation& processInformation
                   ) 
              throw();*/


        
        /**
        * Purpose:
        * <p> Inform the ProcessEventDispatcher of its associated
        * PlatformNotifier
        * 
        */    
        void 
        set_PlatformNotifier( PlatformNotifier_impl* pPlatformNotifier_impl);


        /**
        * Purpose:
        * <p> Returns the state of the platform in a Corba structure
        * 
        */    
        CdmwPlatformMngt::PlatformInfoSeq* 
        get_platformInfoSeq();
          

              
    protected:


    private:


    
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        ProcessEventDispatcher (const ProcessEventDispatcher& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        ProcessEventDispatcher&
        operator=(const ProcessEventDispatcher& rhs)
                throw();


        /**
        * Container used to store the process to be monitored
        */
        typedef  std::map<OsSupport::OS::ProcessId, ProcessInformation> ProcessInformationCtr;



        /**
        * Mutex protecting m_processInformation
        */
        OsSupport::Mutex m_processInformation_mtx;

        

        /**
        * Information about the process to be monitored
        */
        ProcessInformationCtr m_processInformation;


        /**
        * Pointer on the object used to communicate with CORBA client
        */
        PlatformNotifier_impl* m_pPlatformNotifier_impl;


}; // End class PlatformNotifier 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROCESSEVENTDISPATCHER_HPP

