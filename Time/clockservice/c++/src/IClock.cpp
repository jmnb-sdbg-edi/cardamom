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

#include <Time/clockservice/CosClockService.stub.hpp>
#include "clockservice/IClock.hpp"
#include <Foundation/common/Assert.hpp>

using namespace Cdmw::clock;

IClock::IClock()
{
    m_rate = 1.0;
    m_status = RUNNING;
}

///////////////////////////////////////////////////////////////////////////////

IClock::~IClock()
{
    // No-Op
}

///////////////////////////////////////////////////////////////////////////////

IClock::ControlledClockState_t
IClock::get_state() const
{
    return m_status;
}

///////////////////////////////////////////////////////////////////////////////

CORBA::Float  
IClock::retrieve_rate() const
{
    return m_rate;
}

///////////////////////////////////////////////////////////////////////////////

void  
IClock::set_rate(CORBA::Float rate)
{
    m_rate = rate;
}

///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////

