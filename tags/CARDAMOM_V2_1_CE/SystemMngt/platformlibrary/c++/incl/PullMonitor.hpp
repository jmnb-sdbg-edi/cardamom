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


#ifndef INCL_PLATFORMMNGT_PULLMONITOR_HPP 
#define INCL_PLATFORMMNGT_PULLMONITOR_HPP 

#include <string>
#include <iostream>
#include <fstream>


#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "SystemMngt/platformlibrary/Callback.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"


#include "SystemMngt/idllib/CdmwPlatformMngtMonitoring.stub.hpp"



/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt.
*/
namespace PlatformMngt
{


/**
*Purpose:
*<p>    This class implement the Pull Monitoring of Corba Object
*
*Features:
*<p> Thread safe, exception safe
*
*
*/
class PullMonitor : public OsSupport::Thread
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param pullMonitorable    reference to PullMonitorable object 
        *                          (can be nil)
        *@param monitoringInterval monitoring interval used to reactivate the thread in msec
        *@param monitoringTimeout  timeout used by PullMonitoringCall object
        *                          to wait response of PullMonitorable object during polling
        *
        */ 
        PullMonitor (CdmwPlatformMngt::PullMonitorable* p_pullMonitorable,
                     unsigned long monitoringInterval,
                     unsigned long monitoringTimeout);


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~PullMonitor()
                throw();




        /**
        * Purpose:
        * <p> Ask to the internal thread to stop its job, 
        * When this function return the internal thread have stopped.
        *
        *
        */ 
        void stop()
            throw (InternalErrorException);


        
        
        /**
        * Purpose:
        * <p> Set the response callback
        *
        *@param p_callback pointer to callback object
        *
        *@exception BadParameterException
        *
        */ 
        void setResponseCallback (Callback* p_callback)
            throw (BadParameterException);
        
        
        /**
        * Purpose:
        * <p> Set the no response callback
        *
        *@param p_callback pointer to callback object
        *
        *@exception BadParameterException
        *
        */ 
        void setNoResponseCallback (Callback* p_callback)
            throw (BadParameterException);
        
        

    protected:


        /**
        * Purpose:
        * <p> Call by the underlying thread system in a new thread
        * 
        */ 
        virtual
        void run()
            throw();


        /**
        * Purpose:
        * <p> return the pullMonitorable object
        * 
        */ 
        virtual
        const CdmwPlatformMngt::PullMonitorable_ptr get_pullMonitorable()
            throw();
        
        
        
    private:

        //
        // Copy ctor
        //
        PullMonitor (const PullMonitor& rhs)
                throw();




        //
        // Assignement operator
        //
        PullMonitor& operator=(const PullMonitor& rhs)
                 throw();


        /**
        * At each iteration the thread test this variable, to
        * see if it must stop it's processing. If true
        * the thread terminates.
        */
        bool m_stopped;    
        
        
        
        /**
        * Mutex protecting stop flag and associated to condition
        */
        OsSupport::Mutex m_mutex;



        /**
        * Condition on which the thread is sleeping
        * if there is no more job.
        */
        OsSupport::Condition m_timerCondition;


        /**
        * monitoring interval used to reactivate the thread in msec
        */
        unsigned long m_monitoringInterval;


        /**
        * monitoring timeout used by PullMonitoringCall object
        * for polling in msec
        */
        unsigned long m_monitoringTimeout;

        
        /**
        * response callback
        */
        Callback* m_pResponseCallback;
        
        
        /**
        * no response callback
        */
        Callback* m_pNoResponseCallback;
        
        
        /**
        * BoundSyncCallFactory reference
        */
        BoundSyncCallFactory* m_pBoundSyncCallFactory;
        
        /**
        * PullMonitorable object reference
        */
        CdmwPlatformMngt::PullMonitorable_var m_pullMonitorable;


}; // End class PullMonitor 

} // End namespace PlatformMngt
} // End namespace Cdmw


#endif // INCL_PLATFORMMNGT_PULLMONITOR_HPP

