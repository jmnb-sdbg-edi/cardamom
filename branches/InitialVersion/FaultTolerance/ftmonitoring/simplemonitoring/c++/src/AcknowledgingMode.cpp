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
#include "simplemonitoring/AcknowledgingMode.hpp"

#include <iostream>

// include files
#include "ftmonitoring/common/MacroDefs.hpp" //for debug only

namespace Cdmw {
namespace FT {
namespace Supervision
{
	
// constructor
AcknowledgingMode::AcknowledgingMode(CORBA::ORB_ptr orb, 
		    		     std::string pullCorbaloc, 
		    		     std::string pushCorbaloc)
    : m_pPullMonitorableImpl(NULL)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc<<",pushCorbaloc="<<pushCorbaloc);
	m_pPullMonitorableImpl = 
        new CdmwFT::MonitoringLibrary::SimpleMulticastPullMonitorable_impl(orb,
                                                                           pullCorbaloc,
                                                                           pushCorbaloc);
}

// destructor
AcknowledgingMode::~AcknowledgingMode()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (m_pPullMonitorableImpl != NULL) {
        m_pPullMonitorableImpl->_remove_ref();
        m_pPullMonitorableImpl = NULL;
    }
}

//uncomment here if you want a user copy constructor
//  AcknowledgingMode::AcknowledgingMode(const AcknowledgingMode& t)
//  {
//    //attribute=t.attribute
//  }

//  AcknowledgingMode& 
//  AcknowledgingMode::operator=(const AcknowledgingMode& t)
//  {
//    if (this != &t) {
//      //clean all attributes (delete[] attribute...)
//      //attribute=t.attribute
//   }
//   return *this;
//  }    

  SimpleMonitoringTimeoutType AcknowledgingMode::wake_up()
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
      CDMW_LOG_FUNCTION_RETURN(DAEMON_ACKNOWLEDGEMENT);
      return DAEMON_ACKNOWLEDGEMENT;
  }

  void AcknowledgingMode::start()
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
      m_pPullMonitorableImpl->start();
  }

  void AcknowledgingMode::stop()
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
      m_pPullMonitorableImpl->stop();
  }
  
  void AcknowledgingMode::attach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer) throw (CdmwFT::MonitoringLibrary::AlreadyExists)
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
      CDMW_WARN(FTLogger::GetLogger(), "AcknowledgingMode::attach_observer not implemented" );
  }

  void AcknowledgingMode::detach_observer(CdmwFT::MonitoringLibrary::MonitoringObserver_ptr observer) throw (CdmwFT::MonitoringLibrary::NotFound)
  {
      CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
      CDMW_WARN(FTLogger::GetLogger(), "AcknowledgingMode::detach_observer not implemented" );
  }
  
} // End namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw
