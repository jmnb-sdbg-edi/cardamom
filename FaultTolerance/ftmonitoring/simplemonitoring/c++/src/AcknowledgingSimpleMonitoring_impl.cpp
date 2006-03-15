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
#include "simplemonitoring/AcknowledgingSimpleMonitoring_impl.hpp"

// include files
#include <Foundation/ossupport/OS.hpp>
#include "ftmonitoring/common/MacroDefs.hpp"

namespace CdmwFT
{
namespace MonitoringLibrary
{
namespace SimpleMonitoring
{

// constructor
AcknowledgingSimpleMonitoring_impl::AcknowledgingSimpleMonitoring_impl(CORBA::ORB_ptr orb, 
		        	     				       Cdmw::FT::Supervision::ArgvList argv)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
    std::string simplePullCorbaloc = std::string("");
    std::string simplePushCorbaloc = std::string("");

	Cdmw::FT::Supervision::ArgvList::iterator itList;

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
 	 }

	m_pMonitoring = 
	Cdmw::FT::Supervision::SimpleMonitoring::setup_acknowledgingMode(orb,
					     		      simplePullCorbaloc,
					     		      simplePushCorbaloc); 
	
}

// destructor
AcknowledgingSimpleMonitoring_impl::~AcknowledgingSimpleMonitoring_impl()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
    Cdmw::FT::Supervision::SimpleMonitoring::cleanup();
}

//uncomment here if you want a user copy constructor
//  AcknowledgingSimpleMonitoring_impl::AcknowledgingSimpleMonitoring_impl(const AcknowledgingSimpleMonitoring_impl& t)
//  {
//    //attribute=t.attribute
//  }

//  AcknowledgingSimpleMonitoring_impl& 
//  AcknowledgingSimpleMonitoring_impl::operator=(const AcknowledgingSimpleMonitoring_impl& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

  void AcknowledgingSimpleMonitoring_impl::start_monitoring ( )
  throw(CORBA::SystemException, AlreadyStarted)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMonitoring->start();
  }

  void AcknowledgingSimpleMonitoring_impl::stop_monitoring ( )
  throw(CORBA::SystemException, NotStarted)
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      m_pMonitoring->stop();
  }
  
}
}
}
