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


#include "testmonitoring/PullMonitorable_impl.hpp"

#include "Foundation/ossupport/OS.hpp"

#include <iostream>
#include <string>
#include <memory>

using namespace Cdmw::OsSupport;



PullMonitorable_impl::PullMonitorable_impl(unsigned int sleepTime)
      throw()
     : m_sleepTime(sleepTime),
       m_counter(0)
{            
}



PullMonitorable_impl::~PullMonitorable_impl()
      throw()
{
}


void PullMonitorable_impl::is_alive ()
      throw(CORBA::SystemException)
{
	OS::sleep (m_sleepTime);
    m_counter++;
}



void PullMonitorable_impl::set_sleepTime (unsigned int sleepTime)
{
    m_sleepTime = sleepTime;
}

unsigned long PullMonitorable_impl::get_counter ()
{
    return m_counter;
}

