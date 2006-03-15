/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#ifndef ANEXECUTION_HPP
#define ANEXECUTION_HPP

#include "Time/clockservice/CdmwTime.skel.hpp"
#include "Time/clockservice/Util.hpp"
#include <iostream>

struct  anExecutionCallback 
: std::unary_function<TimeBase::TimeT,bool> 
{
    virtual bool operator()(TimeBase::TimeT)=0;
};

template <class T>
class   anExecution 
: public POA_CosClockService::PeriodicExecution::Periodic
{
    T*
        ref_;
        
    anExecutionCallback*
        m_cbk;
        
public: 
   
    anExecution(T* aRef,
                anExecutionCallback* cbk=0)
    {
        ref_ = aRef;
        m_cbk =cbk;
    }
    
    ////////////////////////////////////////////////////////////////////////////
    
    CORBA::Boolean 
    do_work (const CORBA::Any&)
        throw (CORBA::SystemException)
    {
        if(m_cbk)
    {
            return  (*m_cbk)(ref_->current_time());
        }
        return 1;
    }
};    

#endif /*ANEXECUTION_HPP*/
