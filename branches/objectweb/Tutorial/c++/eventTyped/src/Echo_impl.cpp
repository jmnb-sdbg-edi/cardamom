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


#include <Foundation/common/System.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/ossupport/OS.hpp>


#include "Echo_impl.hpp"



namespace Cdmw
{
namespace Tutorial
{ 
    
    
Echo_impl::Echo_impl(CORBA::ORB_ptr          orb, 
                     PortableServer::POA_ptr poa)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{

}
    
//
// IDL to C++ mapping
//
void Echo_impl::echo_string(const char* str)
    throw(CORBA::SystemException)
{
    
    std::cout << "Consumer in server has received: " << str << std::endl;
    
}




void Echo_impl::disconnect_push_consumer()
    throw(CORBA::SystemException)
{
    std::cout << "Disconnect" << std::endl;
    PortableServer::ObjectId_var oid = m_poa->servant_to_id(this);
    m_poa-> deactivate_object(oid.in());
}


void Echo_impl::push(const CORBA::Any& data)
    throw(CORBA::SystemException)
{
    std::cout << "PushConsumer in server : push any not implemented" << std::endl;
}

}; // End namespace Tutorial
}; // End namespace Cdmw
