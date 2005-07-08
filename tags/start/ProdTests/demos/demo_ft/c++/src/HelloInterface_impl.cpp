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


#include "HelloInterface_impl.hpp"

#include <iostream>
#include <Foundation/ossupport/OS.hpp> 



namespace Cdmw {
namespace Test {


HelloInterface_impl::HelloInterface_impl(std::string name,
                                         PortableServer::POA_ptr poa)
:  m_nb_hello(0),
   m_name(name),
   m_poa(poa),
   m_state_changed(false)
{
}

PortableServer::POA_ptr HelloInterface_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}



HelloInterface_impl::~HelloInterface_impl()
throw()
{
}


void HelloInterface_impl::hello ()
throw( CORBA::SystemException )
{
    // simulate server processing during 1 second
    // (server could "crash" with Ctrl-C during this period) 
    Cdmw::OsSupport::OS::sleep(1000);

    m_nb_hello++;
    
    m_state_changed = true;

    std::cout << "         " << m_name 
              << " : <####### HELLO " << m_nb_hello <<"  ######>"
              << std::endl;
}


FT::State* 
HelloInterface_impl::get_state()
    throw (FT::NoStateAvailable)
{
    // Hello servant state is a sequence of 1 byte.
    //  - byte 0: m_nb_hello 
    FT::State_var s = new FT::State(1);
    s->length(1);
    s[0] = m_nb_hello;

    std::cout << "           . current state is " << m_nb_hello << std::endl;
    
    m_state_changed = false;
    
    return s._retn();
}



void 
HelloInterface_impl::set_state(const FT::State& s)
    throw (FT::InvalidState)
{
    // Hello servant state is a sequence of 1 byte.
    //  - byte 0: m_nb_hello 
    m_nb_hello = s[0];
    
    std::cout << "           . set new state to " << m_nb_hello << std::endl;
}


bool 
HelloInterface_impl::has_state_changed() {
    return m_state_changed;
}


}; // End namespace Test
}; // End namespace Cdmw

