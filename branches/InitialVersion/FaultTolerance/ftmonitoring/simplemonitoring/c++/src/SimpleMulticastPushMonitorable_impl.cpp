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

#include "ftmonitoring/common/MacroDefs.hpp"
#include "simplemonitoring/SimpleMulticastPushMonitorable_impl.hpp"
#include "simplemonitoring/InterrogatingMode.hpp"

// include files

namespace CdmwFT {
namespace MonitoringLibrary {

// constructor

SimpleMulticastPushMonitorable_impl::SimpleMulticastPushMonitorable_impl(CORBA::ORB_ptr orb,
                                                                         std::string pushCorbaloc,
                                                                         Cdmw::FT::Supervision::InterrogatingMode* interrogatingMode)
/*
SimpleMulticastPushMonitorable_impl::SimpleMulticastPushMonitorable_impl(CORBA::ORB_ptr orb,
				                                         std::string pushCorbaloc,
									 Cdmw::FT::Supervision::LivelinessRepository* repository)
*/
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pushCorbaloc="<<pushCorbaloc);
        // m_pRepository = repository;
	m_pInterrogatingMode = interrogatingMode;

#ifdef EXT_RELIABLE_MULTICAST        
	SimpleMulticastPushServerFactory::setup(orb, pushCorbaloc, this);
#else
#ifdef EXT_UNRELIABLE_MULTICAST
	SimpleMulticastPushServerFactoryUnreliable::setup(orb, pushCorbaloc, this);
#else
	// Create a group reference
	CORBA::String_var corbaStr = CORBA::string_dup(pushCorbaloc.c_str());
	CORBA::Object_var group = orb->string_to_object(corbaStr.in());
	// Get poa
	CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
	m_poa = PortableServer::POA::_narrow(object.in());
	// Create an object id for the servant and associate it with the group reference
	m_pushObjectId = m_poa->create_id_for_reference(group.in());        
#endif
#endif
}

// destructor
SimpleMulticastPushMonitorable_impl::~SimpleMulticastPushMonitorable_impl()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

//uncomment here if you want a user copy constructor
//  SimpleMulticastPushMonitorable_impl::SimpleMulticastPushMonitorable_impl(const SimpleMulticastPushMonitorable_impl& t)
//  {
//    //attribute=t.attribute
//  }

//  SimpleMulticastPushMonitorable_impl& 
//  SimpleMulticastPushMonitorable_impl::operator=(const SimpleMulticastPushMonitorable_impl& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

  void SimpleMulticastPushMonitorable_impl::i_am_alive (const char * hostname,
                                                        RequestType request_type,
                                                        CORBA::ULong counterVal) throw (CORBA::SystemException)  
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",request_type="<<request_type
                        <<",counterVal="<<counterVal);
      // Add your implementation here
      CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"SimpleMulticastPushMonitorable_impl::i_am_alive called, hostname:"
                 << hostname );
      /*
      //Update liveliness repository with the current daemon acknowledgement
      m_pRepository->updateLiveliness(hostname,
      request_type,
      (unsigned int)counterVal);
      */
      // Delegate the i_am_alive processing to the interrogating mode
      m_pInterrogatingMode->i_am_alive(const_cast<char*>(hostname),
                                       request_type,
                                       (unsigned int)counterVal);
  }

  void SimpleMulticastPushMonitorable_impl::start()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
#ifdef EXT_RELIABLE_MULTICAST
      m_mPushMntrObject = SimpleMulticastPushServerFactory::get_unique_object();
      SimpleMulticastPushServerFactory::activate_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
      m_mPushMntrObject = SimpleMulticastPushServerFactoryUnreliable::get_unique_object();
      SimpleMulticastPushServerFactoryUnreliable::activate_unique_object();
#else
      m_poa->activate_object_with_id(m_pushObjectId.in(), this);
#endif
#endif
  }	  

  void SimpleMulticastPushMonitorable_impl::stop()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
#ifdef EXT_RELIABLE_MULTICAST
      SimpleMulticastPushServerFactory::deactivate_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
      SimpleMulticastPushServerFactoryUnreliable::deactivate_unique_object();
#else
      m_poa->deactivate_object(m_pushObjectId.in());
#endif
#endif
  }

} // End of namespace MonitoringLibrary
} // End of namespace CdmwFT
