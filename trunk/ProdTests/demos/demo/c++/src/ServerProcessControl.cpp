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


#include "ServerProcessControl.hpp"

#include "Foundation/common/System.hpp"
#include "Foundation/common/Locations.hpp"
//#include "ossupport/OS.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

#include "Event/idllib/CosEventComm.stub.hpp"
#include "Event/eventinterface/EventChannel.hpp"
#include "Event/eventinterface/TypedEventChannel.hpp"
#include "Event/eventinterface/EventChannelInterface.hpp"

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "Echo_impl.hpp"

namespace
{  
    const char* INTERFACE_REF="IDL:TypedEcho:1.0";
}; // end of anonymous namespace

namespace Cdmw
{
namespace Resources
{ 


ServerProcessControl::ServerProcessControl(CORBA::ORB_ptr          orb,
                                           PortableServer::POA_ptr poa,
                                           const std::string &     server_name,
                                           const std::string &     channel_name,
                                           const std::string &     complete_channel_name,
                                           const std::string &     complete_typed_channel_name)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_poa (PortableServer::POA::_duplicate(poa)),
      m_server_name(server_name),
      m_channel_name(channel_name),
      m_complete_channel_name(complete_channel_name),
      m_complete_typed_channel_name(complete_typed_channel_name)
{
}

ServerProcessControl::~ServerProcessControl() 
  throw()
{
}
    
void 
ServerProcessControl::on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    
    // This call is legal since Cdmw services are initialised before calling
    // this operation.
    
    // Get NamingInterface object from RepositoryInterface
    using namespace Cdmw::CommonSvcs::Naming;
    using namespace Cdmw::NamingAndRepository;
    NamingInterface ni = RepositoryInterface::get_domain_naming_interface
        (Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN);
    
    // Get Event service naming context
    CORBA::Object_var obj = ni.resolve("EVENT");
    CosNaming::NamingContext_var nc =
        CosNaming::NamingContext::_narrow(obj.in());
    
#if CDMW_ORB_VDR == orbacus
    // Connect server1 to TypedEventChannel1 as consumer
    Cdmw::Event::TypedEventChannel* p_typedeventchannel1
        = new Cdmw::Event::TypedEventChannel
       (m_complete_typed_channel_name.c_str(), nc.in());
#else
    // Connect server1 to EventChannel2 as consumer
    Cdmw::Event::EventChannel* p_eventchannel2
        = new Cdmw::Event::EventChannel
       (m_complete_typed_channel_name.c_str(), nc.in());
#endif    
    
    Echo_impl* pcImpl = new Echo_impl(m_orb.in(), m_poa.in(),
                                      m_server_name,
                                      m_channel_name,
                                      m_complete_channel_name);
    try {
        TypedEcho_var ref = pcImpl->_this();

#if CDMW_ORB_VDR == orbacus
        p_typedeventchannel1->connect_typedPushConsumer(INTERFACE_REF, ref.in());
#else
       	p_eventchannel2->connect_pushConsumer(ref.in());
#endif    

    } catch (...) {
        std::cerr << "unexpected exception raised!" << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                              CORBA::COMPLETED_YES);
    }
}
    
void 
ServerProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
          CORBA::SystemException)
{
    // do nothing
}

void 
ServerProcessControl::on_stop()
    throw(CORBA::SystemException)
{
    // do nothing
}    
    

}; // End namespace Resources
}; // End namespace Cdmw



