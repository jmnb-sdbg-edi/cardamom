/* =========================================================================== *
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
 * =========================================================================== */

#include "Foundation/ossupport/OS.hpp"

#include "testcdmwprocess/MyProcessControl.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
//#include "Repository/repositoryinterface/RepositoryInterface.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

/**
*Purpose:
*<p>    [class description]
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
MyProcessControl::MyProcessControl(CORBA::ORB_ptr      orb,
                                   std::ostream &      os,
                                   unsigned int sleepTime, 
                                   bool request, 
                                   bool except)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_os (os),
      m_sleepTime(sleepTime),
      m_request(request),
      m_except(except),
      m_counter(2)
{
}

MyProcessControl::~MyProcessControl() 
  throw()
{
    //if (m_client_thread != 0) 
	//this->on_stop();
}
    
/**
 * Purpose:
 * <p>    This operation informs the pull-based Fault Detector whether the object is able to
 *    accept requests and produce replies.
 *    This operation may be used to perform some user-defined sanity checks ...
 *
 * @return true if the object is alive and ready to take further requests, and false otherwise.
 *
 */
bool MyProcessControl::is_alive() throw(CORBA::SystemException)
{
    std::string processName = PlatformInterface::Get_process_name();

    m_counter -= 1;

    Cdmw::OsSupport::OS::sleep (m_sleepTime);

    m_os << processName << " is alive called, liveliness:" << m_request << std::endl;

    if (m_except && (m_counter == 0))
    {
        exit(0);
    }

    return m_request;
};

}; // End namespace PlatformMngt
}; // End namespace Cdmw

