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

#include <string>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "testami/Server_impl.hpp"


namespace 
{
    CORBA::Short EXPECTED_RETURN_VALUE = 10;
    CORBA::Short EXPECTED_A_VALUE = 20;
    CORBA::Short EXPECTED_B_VALUE = 30;
}



namespace Cdmw
{
namespace TestAMI
{


Server_impl::Server_impl(CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
{ 
}

Server_impl::~Server_impl()
    throw()
{
}


            
CORBA::Short Server_impl::request_sleeping (CORBA::Short time_to_sleep, 
                                            CORBA::Short& a, 
                                            CORBA::Short& b)
     throw(CORBA::SystemException)                  
{
    std::cout << "  receive request request_sleeping()" << std::endl;
    CORBA::Short ret = EXPECTED_RETURN_VALUE;
    a = EXPECTED_A_VALUE;
    b = EXPECTED_B_VALUE;

    std::cout << "  server sleeping during " 
              << time_to_sleep << " ms..." 
              << std::endl;
    Cdmw::OsSupport::OS::sleep(time_to_sleep);
    
    return ret;
    
}

void Server_impl::request_sending_exception()
    throw (CORBA::SystemException, TestException)
{
    std::cout << "  receive request request_sending_exception()" << std::endl;
    throw TestException();
}


}; // End namespace TestAMI
}; // End namespace Cdmw
