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


#ifndef INCL_PLATFORMMNGT_ACKNOWLEDGEMENT_CALLBACK_IMPL_HPP 
#define INCL_PLATFORMMNGT_ACKNOWLEDGEMENT_CALLBACK_IMPL_HPP 

#include "osthreads/Mutex.hpp"
#include "osthreads/Condition.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


enum AcknowledgementStatus
{
    /**
    * The acknowledgement succeeded.
    */
    ACK_SUCCEED,

    /**
    * The timeout allowed for the acknowledgement has expired.
    */
    ACK_TIMEOUT,

    /**
    * An error has been reported during the acknowledgement.
    */
    ACK_ERROR
};

/**
* Purpose:
* <p>
* The base class of all acknowledgement callbacks.
*/
class AcknowledgementCallback_impl
{

public:

        /**
        * Purpose:
        * <p> Lock the Ack Callback mutex.
        * 
        */ 
        void lock();
        
        /**
        * Purpose:
        * <p> Unlock the Ack Callback mutex.
        * 
        */ 
        void unlock();
        
        
        /**
        * Purpose:
        * <p> Waits for the acknowledgement during the allowed timeout.
        * 
        * @param allowedTimeout the timeout in ms.
        * @return the status of the acknowledgement.
        */ 
        AcknowledgementStatus wait(size_t allowedTimeout);


        /**
        * Purpose:
        * <p> Returns the information about the error.
        */ 
        const char* getErrorInfo();


        /**
        * Purpose:
        * <p> Destructor
        */ 
        virtual ~AcknowledgementCallback_impl() throw();


protected:

        /**
        * Purpose:
        * <p> Constructor
        */ 
        AcknowledgementCallback_impl();

        /**
        * Purpose:
        * <p> Signals the acknowledgement.
        * 
        * @param error indicates whether an error has been reported during the
        * acknowledgement.
        * @return true if the acknowledgement has been done before the timeout.
        * Otherwise return false.
        */ 
        bool signal(bool error = false);

        /**
        * The information about the error.
        * Should be updated if needed before calling signal().
        */ 
        std::string m_errorInfo;

private:

        /**
        * The mutex used by m_acknowlegement_condition and protecting
        * m_timeout_expired.
        */
        OsSupport::Mutex m_mutex;

        /**
        * The condition on the acknowledgement.
        */
        OsSupport::Condition m_acknowlegement_condition;
                
        /**
        * Indicates whether mutex is locked.
        */
        bool m_mutexLocked;

        /**
        * Indicates whether the timeout has expired.
        */
        bool m_timeoutExpired;

        /**
        * Indicates whether an error has been reported.
        */
        bool m_errorReported;


};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_ACKNOWLEDGEMENT_CALLBACK_IMPL_HPP

