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

#include "FaultTolerance/ftstatemanager/Timer.hpp"
#include "ace/Timer_Hash.h"
#include "ace/Timer_List.h"

namespace Cdmw {
namespace FT {
    TimerBase::TimerBase()
    {
      m_stop = false;
      m_timer_queue = new ACE_Timer_List();
        
    }
    
    TimerBase::~TimerBase()
        throw ()
    {
        // stop threads if not already done
        if (! m_stop)
        {
            this->stop();
        }
        
        // wait all threads have been joined
        this->wait();
        
        delete m_timer_queue;
    }

    void TimerBase::stop()
    {
        m_stop = true;
    }
    
    int TimerBase::cancel_timer (int timer_id, const void **act)
    {
        int res = m_timer_queue->cancel(timer_id, act);
        return res;
    }
 
/*  could be use to factorize deletion processing but it is better to have
    new and delete in the same class (TransactionEngineCohort)

    int TimerBase::cancel_timer (int timer_id)
    {
        const void* p_tidToDelete;
        int res = m_timer_queue->cancel(timer_id, &p_tidToDelete);
        if (p_tidToDelete != NULL)
        {
            delete (CdmwFT::Transaction::TransactionIdentification*)p_tidToDelete;
        }
        return res;
    }
*/
 
    int TimerBase::start()
    {
        this->activate();
        return 1;
    }
    
    int TimerBase::svc (void)
    {
        while(!m_stop)
        {
            Cdmw::OsSupport::OS::sleep( 400 );    
            m_timer_queue->expire();
        }
        return 1;
    }
    

}; // namespace FT
}; // namespace Cdmw
