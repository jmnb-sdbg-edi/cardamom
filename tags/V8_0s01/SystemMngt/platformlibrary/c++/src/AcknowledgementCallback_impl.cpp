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


#include "SystemMngt/platformlibrary/AcknowledgementCallback_impl.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


AcknowledgementCallback_impl::AcknowledgementCallback_impl()
    : m_acknowlegement_condition(m_mutex),
    m_mutexLocked(false),
    m_timeoutExpired(false), m_errorReported(false)
{

}


AcknowledgementCallback_impl::~AcknowledgementCallback_impl() throw()
{
    // if mutex already locked, unlock it
    if (m_mutexLocked)
    {
        m_mutex.unlock();
    }
}



void AcknowledgementCallback_impl::lock()
{
    if (!m_mutexLocked)
    {
        m_mutex.lock();
        m_mutexLocked = true;
    }
}

void AcknowledgementCallback_impl::unlock()
{
    if (m_mutexLocked)
    {
        m_mutex.unlock();
        m_mutexLocked = false;
    }
}

AcknowledgementStatus AcknowledgementCallback_impl::wait(size_t allowedTimeout)
{
    AcknowledgementStatus ret;

    // if mutex not already locked, lock it
    if (!m_mutexLocked)
    {
        m_mutex.lock();
        m_mutexLocked = true;
    }
    
    OsSupport::Condition::WaitReturn statusWait;
    
    try
    {
        statusWait = 
            m_acknowlegement_condition.wait(allowedTimeout);
    }
    catch (...)
    {
        m_mutex.unlock();
        m_mutexLocked = false;
        throw;
    }
    

    if (statusWait == OsSupport::Condition::TIMEDOUT)
    {
        m_timeoutExpired = true;
        ret = ACK_TIMEOUT;
    }
    else if (m_errorReported)
    {
        ret = ACK_ERROR;
    }
    else
    {
        ret = ACK_SUCCEED;
    }
    
    // unlock the mutex
    m_mutex.unlock();
    m_mutexLocked = false;

    return ret;

}


const char* AcknowledgementCallback_impl::getErrorInfo()
{
    return m_errorInfo.c_str();
}


bool AcknowledgementCallback_impl::signal(bool error)
{
    bool ret = true;

    CDMW_MUTEX_GUARD(m_mutex);

    if (!m_timeoutExpired)
    {
        m_errorReported = error;
        m_acknowlegement_condition.broadcast();
    }
    else
    {
        ret = false;
    }

    return ret;

}


} // End namespace PlatformMngt
} // End namespace Cdmw


