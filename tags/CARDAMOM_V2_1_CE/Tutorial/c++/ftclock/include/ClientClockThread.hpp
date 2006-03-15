/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * =========================================================================== */


#ifndef INCL_CLIENT_CLOCK_THREAD_HPP
#define INCL_CLIENT_CLOCK_THREAD_HPP

// Cdmw Files
#include <Foundation/osthreads/Thread.hpp>

#include <CosClockService.stub.hpp>



class ClientClockThread : public Cdmw::OsSupport::Thread
{
public:
    
    ClientClockThread(CosClockService::Clock_ptr clock):
        clock_(CosClockService::Clock::_duplicate(clock)),
        m_stopped(false)
    {
    }

    virtual ~ClientClockThread()
        throw()
    {
        
    }
    
    void stop() throw()
    {
        m_stopped = true;
    }
    
protected:

    void run() throw();
    
    
private:
    CosClockService::Clock_var clock_;

    bool m_stopped;

};

#endif // INCL_CLIENT_CLOCK_THREAD_HPP

