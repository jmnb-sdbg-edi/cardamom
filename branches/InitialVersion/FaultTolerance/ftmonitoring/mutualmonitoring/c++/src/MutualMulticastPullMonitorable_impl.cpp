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
#include "mutualmonitoring/MutualMulticastFactories.hpp"
#endif

#include "ftmonitoring/common/MacroDefs.hpp"
#include "mutualmonitoring/MutualMulticastPullMonitorable_impl.hpp"
#include "mutualmonitoring/MutualMonitoring.hpp"

// include files

namespace CdmwFT {
namespace MonitoringLibrary {

// constructor
MutualMulticastPullMonitorable_impl::MutualMulticastPullMonitorable_impl(CORBA::ORB_ptr orb, 
                                                                         Cdmw::FT::Supervision::MutualMonitoring* monitoring,
                                                                         std::string pullCorbaloc,
                                                                         std::string hostname)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc<<",hostname="<<hostname);
	m_pMonitoring = monitoring;
	m_localHostname = hostname;
	
#ifdef EXT_RELIABLE_MULTICAST      
        MutualMulticastPullServerFactory::setup(orb, pullCorbaloc, this);
#else
#ifdef EXT_UNRELIABLE_MULTICAST
        MutualMulticastPullServerFactoryUnreliable::setup(orb, pullCorbaloc, this);
#else
	//Create a group reference
	CORBA::String_var corbaStr = CORBA::string_dup(pullCorbaloc.c_str());
	CORBA::Object_var group = orb->string_to_object(corbaStr.in());
	//Get a poa
	CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
	m_poa = PortableServer::POA::_narrow(object.in());
	//Create an object id for the servant and associate it with the group reference
	  m_objectId = m_poa->create_id_for_reference(group.in());	
#endif
#endif
}

// destructor
MutualMulticastPullMonitorable_impl::~MutualMulticastPullMonitorable_impl()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

//uncomment here if you want a user copy constructor
//  MutualMulticastPullMonitorable_impl::MutualMulticastPullMonitorable_impl(const MutualMulticastPullMonitorable_impl& t)
//  {
//    //attribute=t.attribute
//  }

//  MutualMulticastPullMonitorable_impl& 
//  MutualMulticastPullMonitorable_impl::operator=(const MutualMulticastPullMonitorable_impl& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    


void 
MutualMulticastPullMonitorable_impl::is_alive (const char * hostname,
                                               CORBA::ULong counterVal) 
    throw (CORBA::SystemException)
 {
     CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"hostname="<<hostname<<",counterVal="<<counterVal);
     // Add your implementation here
     CDMW_DEBUG_3(::Cdmw::FT::FTLogger::GetLogger(),"MutualMulticastPullMonitorable_impl::is_alive : " << hostname);
     // Test if message comes from same host done into the monitoring
     // to verify no duplicate mutual monitoring on same host
     
     //Delegate the is_alive processing to the monitoring
     m_pMonitoring->is_alive(const_cast<char*>(hostname),
                             (unsigned int)counterVal);
  }
 
  void MutualMulticastPullMonitorable_impl::start()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
#ifdef EXT_RELIABLE_MULTICAST
      m_mPullMntrObject = MutualMulticastPullServerFactory::get_unique_object();
      MutualMulticastPullServerFactory::activate_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
      m_mPullMntrObject = MutualMulticastPullServerFactoryUnreliable::get_unique_object();
      MutualMulticastPullServerFactoryUnreliable::activate_unique_object();
#else
      m_poa->activate_object_with_id(m_objectId.in(), this);
#endif
#endif
  }	  

  void MutualMulticastPullMonitorable_impl::stop()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
#ifdef EXT_RELIABLE_MULTICAST
      MutualMulticastPullServerFactory::deactivate_unique_object();
#else
#ifdef EXT_UNRELIABLE_MULTICAST
      MutualMulticastPullServerFactoryUnreliable::deactivate_unique_object();
#else
      m_poa->deactivate_object(m_objectId.in());
#endif
#endif
  }


} // End of namespace MonitoringLibrary
} // End of namespace CdmwFT
