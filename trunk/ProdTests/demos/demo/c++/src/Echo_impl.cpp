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


#include "Echo_impl.hpp"

#include "Foundation/common/System.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#include <iostream>

#include "Event/idllib/CosEventComm.stub.hpp"
#include "Event/eventinterface/EventChannel.hpp"
#include "Event/eventinterface/TypedEventChannel.hpp"


namespace Cdmw
{
namespace Resources
{ 
    
Echo_impl::Echo_impl(CORBA::ORB_ptr          orb, 
                     PortableServer::POA_ptr poa,
                     const std::string &     server_name,
                     const std::string &     channel_name,
                     const std::string &     complete_channel_name) 
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_pushConsumer(CosEventComm::PushConsumer::_nil()),
      m_server_name(server_name),
      m_channel_name(channel_name),
      m_complete_channel_name(complete_channel_name)
{
    m_nbRcvdMsg = 0;
}
    
//
// IDL to C++ mapping
//
void
Echo_impl::echo_string(const char* str)
    throw(CORBA::SystemException)
{
    std::cout << "Consumer in " << m_server_name << " has received: " << str << std::endl;
    
    m_nbRcvdMsg++;
    
    if (m_nbRcvdMsg == 1) {
        // Get NamingInterface object from RepositoryInterface
        using namespace Cdmw::NamingAndRepository;
        NamingInterface ni = RepositoryInterface::get_domain_naming_interface
            (Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN);
        
        // Get Event service naming context
        CORBA::Object_var obj = ni.resolve("EVENT");
        CosNaming::NamingContext_var nc =
            CosNaming::NamingContext::_narrow(obj.in());
        
        if (CORBA::is_nil(nc.in())) {
            std::cerr << "NamingContext is a nil object reference"
                      << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
            return ;
        }
        
        // Connect server1 to EventChannel1 as supplier
        Cdmw::Event::EventChannel* p_eventchannel1
            = new Cdmw::Event::EventChannel(m_complete_channel_name.c_str(),
                                             nc.in());
        
//         for (int i=0; i<200; i++) {
//             try  {
//                 p_eventchannel1 = new Cdmw::Event::EventChannel(m_complete_channel_name.c_str(),
//                                                                  nc.in());
//                 break;
//             } catch (const CORBA::BAD_PARAM& e) {
//                 // try again, maybe event channel is still not created!
//             }
//             Cdmw::OsSupport::OS::sleep(20);
//         }
        
//         if (p_eventchannel1 == NULL) {
//             std::cerr << "eventchannel1 not found!" << std::endl;
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl;
//             return ;
//         }
        
        try {
            m_pushConsumer = p_eventchannel1->get_pushConsumer(CosEventComm::PushSupplier::_nil());
        } catch (...) {
            std::cerr << "impossible to get a push consumer on " << m_channel_name << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
            return ;
        }
    }

    CORBA::Any data;
    data <<= str;
    if (!CORBA::is_nil(m_pushConsumer.in())) {
        m_pushConsumer->push(data);
    }
}

void
Echo_impl::disconnect_push_consumer()
    throw(CORBA::SystemException)
{
    std::cout << "Disconnect" << std::endl;
    PortableServer::ObjectId_var oid = m_poa-> servant_to_id(this);
    m_poa-> deactivate_object(oid.in());
}

void
Echo_impl::push(const CORBA::Any& data)
    throw(CORBA::SystemException)
{
#if CDMW_ORB_VDR == orbacus
    std::cout << "PushConsumer in " << m_server_name << ": push any not implemented" << std::endl;
#else
    
    m_nbRcvdMsg++;
    
    if (m_nbRcvdMsg == 1) {
        // Get NamingInterface object from RepositoryInterface
        using namespace Cdmw::NamingAndRepository;
        NamingInterface ni = RepositoryInterface::get_domain_naming_interface
            (Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN);
        
        // Get Event service naming context
        CORBA::Object_var obj = ni.resolve("EVENT");
        CosNaming::NamingContext_var nc =
            CosNaming::NamingContext::_narrow(obj.in());
        
        if (CORBA::is_nil(nc.in())) {
            std::cerr << "NamingContext is a nil object reference"
                      << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
            return ;
        }
        
        // Connect server1 to EventChannel1 as supplier
        Cdmw::Event::EventChannel* p_eventchannel1
            = new Cdmw::Event::EventChannel(m_complete_channel_name.c_str(),
                                             nc.in());

        try {
            m_pushConsumer = p_eventchannel1->get_pushConsumer(CosEventComm::PushSupplier::_nil());
        } catch (...) {
            std::cerr << "impossible to get a push consumer on " << m_channel_name << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
            return ;
        }
    }


    const char* str;

    if (data >>= str)
        {
           std::cout <<  "Consumer in " << m_server_name << " has received: " << str << std::endl;
           m_rcvMsg_array[m_nbRcvdMsg] = str;
        }

    CORBA::Any data_tmp;
    data_tmp <<= str;
    if (!CORBA::is_nil(m_pushConsumer.in())) {
        m_pushConsumer->push(data_tmp);
    }

#endif
}

}; // End namespace Resources
}; // End namespace Cdmw

