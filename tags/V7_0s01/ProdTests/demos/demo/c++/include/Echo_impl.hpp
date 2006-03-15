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


#ifndef INCL_CDMW_RESOURCES_ECHO_IMPL_HPP 
#define INCL_CDMW_RESOURCES_ECHO_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Echo.skel.hpp"
#include "Foundation/orbsupport/RefCountServantBase.hpp"


#include <string>

namespace Cdmw
{
namespace Resources
{
// ----------------------------------------------------------------------
// Echo_impl implementation
// ----------------------------------------------------------------------
class Echo_impl : virtual public POA_TypedEcho,
                  virtual public Cdmw::OrbSupport::RefCountServantBase
{
public:
    std::string m_rcvMsg_array[10];
    unsigned short m_nbRcvdMsg;
    
    Echo_impl(CORBA::ORB_ptr          orb, 
              PortableServer::POA_ptr poa,
              const std::string &     server_name,
              const std::string &     channel_name,
              const std::string &     complete_channel_name);
    
    //
    // IDL to C++ mapping
    //
    void
    echo_string(const char* str)
        throw(CORBA::SystemException);

    void
    disconnect_push_consumer()
        throw(CORBA::SystemException);

    void
    push(const CORBA::Any& data)
        throw(CORBA::SystemException);
    
    PortableServer::POA_ptr
    _default_POA()
    {
        return PortableServer::POA::_duplicate(m_poa.in());
    }
private:
    CORBA::ORB_var                  m_orb; // The ORB
    PortableServer::POA_var         m_poa; // My POA.
    CosEventComm::PushConsumer_var  m_pushConsumer; // ref of the consumer to
                                             // which msg received is pushed.
    std::string                     m_server_name;
    std::string                     m_channel_name;
    std::string                     m_complete_channel_name;
}; // End class Echo_impl

}; // End namespace Resources
}; // End namespace Cdmw

#endif // INCL_CDMW_RESOURCES_ECHO_IMPL_HPP 

