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


#include "testplatformfaultdetector/PullMonitorable_impl.hpp"

#include "Foundation/ossupport/OS.hpp"

#include <iostream>
#include <string>
#include <memory>

using namespace Cdmw::OsSupport;



PullMonitorable_impl::PullMonitorable_impl(std::string& pullName, unsigned int sleepTime, bool request, bool except)
      throw()
  : m_pullName(pullName), m_sleepTime(sleepTime), m_request(request), m_except(except), m_counter(2)
{
 
}



PullMonitorable_impl::~PullMonitorable_impl()
      throw()
{
}


CORBA::Boolean PullMonitorable_impl::is_alive ()
      throw(CORBA::SystemException)
{
    m_counter -= 1;

    OS::sleep (m_sleepTime);

    std::cout << m_pullName << " is alive " << m_counter << std::endl;

    if (m_except && (m_counter == 0))
    {
      int val = 10;
      std::cout<<"FAILURE BEFORE"<<val<<" "<<m_counter<<std::endl;
      val = val/m_counter;
      std::cout<<"FAILURE AFTER"<<val<<std::endl;

      //throw CORBA::NO_IMPLEMENT();
    }

    return m_request;
}



void PullMonitorable_impl::set_sleepTime (unsigned int sleepTime)
{
    m_sleepTime = sleepTime;
}



void PullMonitorable_impl::set_request (bool request)
{
    m_request = request;
}
