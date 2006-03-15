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
#include "ftmonitoring/common/MacroDefs.hpp"
#include "simplemonitoring/InterrogatingSimpleMonitoring_impl.hpp"

// include files
#include <Foundation/ossupport/OS.hpp>
#include "ftmonitoring/common/MacroDefs.hpp"


namespace CdmwFT
{
namespace MonitoringLibrary
{
namespace SimpleMonitoring
{

  const int DEFAULT_DAEMON_ACKNOWLEDGEMENT  = 160;

  //Constructor 
  InterrogatingSimpleMonitoring_impl::InterrogatingSimpleMonitoring_impl (
      CORBA::ORB_ptr orb,
      Cdmw::FT::Supervision::ArgvList argv,
      const Locations & locations )
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      std::string simplePullCorbaloc = std::string("");
      std::string simplePushCorbaloc = std::string("");
      
      Cdmw::FT::Supervision::ArgvList::iterator itList;
      
      int daemonAcknowledgement = DEFAULT_DAEMON_ACKNOWLEDGEMENT;

      for (itList=argv.begin(); itList!=argv.end();itList++)
          {
              
              if ((*itList) == std::string("--SIMPLE_PULL_CORBALOC"))
                  {
                      simplePullCorbaloc = *(++itList);
                  }
              else if ((*itList) == std::string("--SIMPLE_PUSH_CORBALOC"))
                  {
                      simplePushCorbaloc = *(++itList);
                  }
              else if ((*itList) == std::string("--DAEMON_ACKNOWLEDGEMENT"))
                  {
                      daemonAcknowledgement = atoi((*(++itList)).c_str());;
                  }
          }      
      
      CDMW_DEBUG(::Cdmw::FT::FTLogger::GetLogger(),"InterrogatingSimpleMonitoring_impl:: call setupInterrogatingMode..");

      m_pMonitoring =
          Cdmw::FT::Supervision::SimpleMonitoring::setup_interrogatingMode(
                                                                           orb,
                                                                           simplePullCorbaloc,
                                                                           simplePushCorbaloc,
                                                                           daemonAcknowledgement);
      
      set_locations(locations); 
  }

  //Destructor 
    InterrogatingSimpleMonitoring_impl::~InterrogatingSimpleMonitoring_impl (void)  {
        CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
        Cdmw::FT::Supervision::SimpleMonitoring::cleanup();
    }

  void InterrogatingSimpleMonitoring_impl::add_location (
      const char * the_location
    ) throw (CORBA::SystemException, AlreadyExists)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"the_location="<<the_location);
      m_pMonitoring->add_location(const_cast<char*>(the_location));
  }
      
  void InterrogatingSimpleMonitoring_impl::remove_location (
      const char * the_location
    ) throw (CORBA::SystemException, NotFound)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"the_location="<<the_location);
      // This operation is implemented (uncomment below) but considering the note
      // written in header file i choose to throw NO_IMPLEMENT
      // m_pMonitoring->remove_location((Location)the_location);
      CORBA::NO_IMPLEMENT ex;
      CDMW_LOG_FUNCTION_EXCEPTION(ex);
      throw ex;
  }

  void InterrogatingSimpleMonitoring_impl::set_locations (
      const Locations & the_locations
    ) throw (CORBA::SystemException)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMonitoring->set_locations(the_locations);
  }
      
  void InterrogatingSimpleMonitoring_impl::reset_locations( )
    throw (CORBA::SystemException)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMonitoring->reset_locations();
  }
      
  void InterrogatingSimpleMonitoring_impl::attach_observer (
      MonitoringObserver_ptr the_observer
    ) throw (CORBA::SystemException, AlreadyExists)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMonitoring->attach_observer(the_observer);    
  }
      
  void InterrogatingSimpleMonitoring_impl::detach_observer (
      MonitoringObserver_ptr the_observer
    ) throw (CORBA::SystemException, NotFound)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMonitoring->detach_observer(the_observer);    
  }

  void InterrogatingSimpleMonitoring_impl::start_monitoring ()
  throw(CORBA::SystemException, AlreadyStarted)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMonitoring->start();
  }

  void InterrogatingSimpleMonitoring_impl::stop_monitoring ()
  throw(CORBA::SystemException, NotStarted)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMonitoring->stop();
  }

}; // end namespace Simple Monitoring

}; // end namespace MonitoringLibrary

}; // end namespace CdmwFT
