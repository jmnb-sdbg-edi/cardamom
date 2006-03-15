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

// private include files
#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST)
#include "simplemonitoring/SimpleMulticastFactories.hpp"
#endif

#include "simplemonitoring/SimpleMulticastPullMonitorable_impl.hpp"

// include files
#include <Foundation/ossupport/OS.hpp>

#include "ftmonitoring/common/MacroDefs.hpp" 

namespace CdmwFT {
namespace MonitoringLibrary {
	
// constructor
SimpleMulticastPullMonitorable_impl::SimpleMulticastPullMonitorable_impl(CORBA::ORB_ptr orb,
                                                                         std::string pullCorbaloc,
                                                                         std::string pushCorbaloc)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc<<",pushCorbaloc="<<pushCorbaloc);
	m_localHostname = Cdmw::OsSupport::OS::get_hostname();
	
#ifdef EXT_RELIABLE_MULTICAST        
        SimpleMulticastPullServerFactory::setup(orb, pullCorbaloc, this);
#else
#ifdef EXT_UNRELIABLE_MULTICAST
        SimpleMulticastPullServerFactoryUnreliable::setup(orb, pullCorbaloc, this);
#else
	orb = CORBA::ORB::_duplicate(orb);
	//Create a pull group reference
	CORBA::String_var corbaStrPull = CORBA::string_dup(pullCorbaloc.c_str());
	CORBA::Object_var groupPull = orb->string_to_object(corbaStrPull.in());
	//Get poa
	CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
	m_poa = PortableServer::POA::_narrow(object.in());
	//Create an object id for the servant and associate it with the group reference
	m_pullObjectId = m_poa->create_id_for_reference(groupPull.in());	
#endif
#endif

    CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMulticastPullMonitorable_impl: set m_pushRef");
#ifdef EXT_RELIABLE_MULTICAST 
	SimpleMulticastPushClientFactory::setup(orb, pushCorbaloc);
        m_pushRef = SimpleMulticastPushClientFactory::get_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
	SimpleMulticastPushClientFactoryUnreliable::setup(orb, pushCorbaloc);
        m_pushRef = SimpleMulticastPushClientFactoryUnreliable::get_unique_object();
#else
	//Create a push group reference
	CORBA::String_var corbaStrPush = CORBA::string_dup(pushCorbaloc.c_str());
	CORBA::Object_var groupPush = orb->string_to_object(corbaStrPush.in());
	m_pushRef = MulticastPushMonitorable::_unchecked_narrow(groupPush.in());
#endif
#endif
}

// destructor
SimpleMulticastPullMonitorable_impl::~SimpleMulticastPullMonitorable_impl()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

//uncomment here if you want a user copy constructor
//  SimpleMulticastPullMonitorable_impl::SimpleMulticastPullMonitorable_impl(const SimpleMulticastPullMonitorable_impl& t)
//  {
//    //attribute=t.attribute
//  }

//  SimpleMulticastPullMonitorable_impl& 
//  SimpleMulticastPullMonitorable_impl::operator=(const SimpleMulticastPullMonitorable_impl& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

void 
SimpleMulticastPullMonitorable_impl::is_alive (const char * hostname,
                                               CORBA::ULong counterVal) 
    throw (CORBA::SystemException)
 {
     CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",counterVal="<<counterVal);
     // Add your implementation here
     CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMulticastPullMonitorable_impl::is_alive called, hostname:"
                << hostname );
        
     m_pushRef.in()->i_am_alive(m_localHostname.c_str(), 
                                ACKNOWLEDGEMENT,
                                counterVal);
	
  }
 
  void SimpleMulticastPullMonitorable_impl::start()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
#ifdef EXT_RELIABLE_MULTICAST
      m_mPullMntrObject = SimpleMulticastPullServerFactory::get_unique_object();
      SimpleMulticastPullServerFactory::activate_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
      m_mPullMntrObject = SimpleMulticastPullServerFactoryUnreliable::get_unique_object();
      SimpleMulticastPullServerFactoryUnreliable::activate_unique_object();
#else
      m_poa->activate_object_with_id(m_pullObjectId.in(), this);
#endif
#endif
  }	  

  void SimpleMulticastPullMonitorable_impl::stop()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
#ifdef EXT_RELIABLE_MULTICAST
      SimpleMulticastPullServerFactory::deactivate_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
      SimpleMulticastPullServerFactoryUnreliable::deactivate_unique_object();
#else
      m_poa->deactivate_object(m_pullObjectId.in());
#endif
#endif
  }


} // End of namespace MonitoringLibrary
} // End of namespace CdmwFT
