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
#include "mutualmonitoring/MutualMonitoring_impl.hpp"
#include "mutualmonitoring/MutualMonitoringType.hpp"

// include files
#include <Foundation/ossupport/OS.hpp>

#include "ftmonitoring/common/CommonTypeDefinitions.hpp"
#include "ftmonitoring/common/MacroDefs.hpp"

namespace CdmwFT
{
namespace MonitoringLibrary
{
namespace PrimaryBackup
{

  const std::string DEFAULT_MUTUAL_PULL_CORBALOC = "corbaloc:miop:1.0@1.0-testmonitoringlib-3/225.1.1.9:5000";
  const std::string DEFAULT_MUTUAL_PUSH_CORBALOC = "corbaloc:miop:1.0@1.0-testmonitoringlib-4/225.1.1.9:5001";

  const int DEFAULT_MASTER_INTERROGATION  = 210;
  const int DEFAULT_SLAVE_ACKNOWLEDGEMENT = 140;
  const int DEFAULT_MASTER_CONFIRMATION   = 50;
  const int DEFAULT_MASTER_ELECTION       = 80;

// constructor
MutualMonitoring_impl::MutualMonitoring_impl(CORBA::ORB_ptr orb, 
		 			     Cdmw::FT::Supervision::ArgvList argv,
					     const Locations & locations)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
  
    std::string mutualPullCorbaloc = DEFAULT_MUTUAL_PULL_CORBALOC;
    std::string mutualPushCorbaloc = DEFAULT_MUTUAL_PUSH_CORBALOC;
    
    Cdmw::FT::Supervision::ElectionStrategyType strategy;
    std::string electionStrategy = std::string("");
    
    Cdmw::FT::Supervision::ArgvList::iterator itList;
    Cdmw::FT::Supervision::MutualMonitoring::TimeoutMap timeoutMap;
    // set default values for timeouts
    timeoutMap[Cdmw::FT::Supervision::MASTER_INTERROGATION]  = DEFAULT_MASTER_INTERROGATION;
    timeoutMap[Cdmw::FT::Supervision::SLAVE_ACKNOWLEDGEMENT] = DEFAULT_SLAVE_ACKNOWLEDGEMENT;
    timeoutMap[Cdmw::FT::Supervision::MASTER_CONFIRMATION]   = DEFAULT_MASTER_CONFIRMATION;
    timeoutMap[Cdmw::FT::Supervision::MASTER_ELECTION]       = DEFAULT_MASTER_ELECTION;
    
    for (itList=argv.begin(); itList!=argv.end();itList++)
        {
            
            if ((*itList) == std::string("--MUTUAL_PULL_CORBALOC"))
                {
                    mutualPullCorbaloc = *(++itList);
                }
            else if ((*itList) == std::string("--MUTUAL_PUSH_CORBALOC"))
                {
                    mutualPushCorbaloc = *(++itList);
                }
            else if ((*itList) == std::string("--ELECTION_STRATEGY"))
                {
                    electionStrategy = *(++itList);
                }
            else if ((*itList) == std::string("--MASTER_INTERROGATION"))
                {
                    timeoutMap[Cdmw::FT::Supervision::MASTER_INTERROGATION] = atoi((*(++itList)).c_str());
                }
            else if ((*itList) == std::string("--SLAVE_ACKNOWLEDGEMENT"))
                {
                    timeoutMap[Cdmw::FT::Supervision::SLAVE_ACKNOWLEDGEMENT] = atoi((*(++itList)).c_str());
                }
            else if ((*itList) == std::string("--MASTER_CONFIRMATION"))
                {
                    timeoutMap[Cdmw::FT::Supervision::MASTER_CONFIRMATION] = atoi((*(++itList)).c_str());
                }
            else if ((*itList) == std::string("--MASTER_ELECTION"))
                {
                    timeoutMap[Cdmw::FT::Supervision::MASTER_ELECTION] = atoi((*(++itList)).c_str());
                }
        }
    
    if (electionStrategy == "HOST_PRIORITY")
        {
            strategy = Cdmw::FT::Supervision::HOST_PRIORITY;
        }
    else if (electionStrategy =="CPU_LOAD")
        {
            strategy = Cdmw::FT::Supervision::CPU_LOAD;
        }
    else strategy = Cdmw::FT::Supervision::HOST_PRIORITY;			
    
    CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"MutualMonitoring setup call");
    
    m_pMonitoring = 
        Cdmw::FT::Supervision::MutualMonitoring::setup(orb,
                                                       mutualPullCorbaloc,
                                                       mutualPushCorbaloc,
                                                       strategy,
                                                       locations,
                                                       timeoutMap);	     
}

// destructor
MutualMonitoring_impl::~MutualMonitoring_impl()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
        Cdmw::FT::Supervision::MutualMonitoring::cleanup();
        m_pMonitoring = NULL;
}

//uncomment here if you want a user copy constructor
//  MutualMonitoring_impl::MutualMonitoring_impl(const MutualMonitoring_impl& t)
//  {
//    //attribute=t.attribute
//  }

//  MutualMonitoring_impl& 
//  MutualMonitoring_impl::operator=(const MutualMonitoring_impl& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

void MutualMonitoring_impl::add_location (
    const char * the_location
  )
  throw(CORBA::SystemException, AlreadyExists)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"the_location="<<the_location);
	m_pMonitoring->add_location(const_cast<char*>(the_location));
}

void MutualMonitoring_impl::remove_location (
    const char * the_location
  )
  throw(CORBA::SystemException, NotFound)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"the_location="<<the_location);
	m_pMonitoring->remove_location(const_cast<char*>(the_location));
}

void MutualMonitoring_impl::set_locations (
    const Locations & the_locations
  )
  throw(CORBA::SystemException, NotEmpty)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
	m_pMonitoring->set_locations(the_locations);
}

void MutualMonitoring_impl::reset_locations (
  )
  throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
	m_pMonitoring->reset_locations();
}

Locations * MutualMonitoring_impl::get_started_locations (
  )
  throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
    try
        {
            return m_pMonitoring->get_started_locations();
        }
    catch (const Cdmw::OutOfMemoryException&)
        {
            CORBA::NO_RESOURCES ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
}

void MutualMonitoring_impl::attach_observer (
    PrimaryBackup::MutualMonitoringObserver_ptr the_observer
  )
    throw(CORBA::SystemException, AlreadyExists)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
	m_pMonitoring->attach_observer(the_observer);
}

void MutualMonitoring_impl::detach_observer (
    PrimaryBackup::MutualMonitoringObserver_ptr the_observer
  )
    throw(CORBA::SystemException, NotFound)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
	m_pMonitoring->detach_observer(the_observer);
}

void MutualMonitoring_impl::start_monitoring ( )
  throw(CORBA::SystemException, AlreadyStarted)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
	m_pMonitoring->start();
}

void MutualMonitoring_impl::stop_monitoring ( )
  throw(CORBA::SystemException, NotStarted)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
	m_pMonitoring->stop();
}
  
}
}
}
