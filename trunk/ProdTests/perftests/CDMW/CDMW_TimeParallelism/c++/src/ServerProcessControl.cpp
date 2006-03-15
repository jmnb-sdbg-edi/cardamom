/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

#include "ServerProcessControl.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "ConfAndPlug/cdmwinit/InitUtils.hpp"


namespace TimeInvocation
{
  
  ServerProcessControl::ServerProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName ("")
  {
  }
  
  
  ServerProcessControl::~ServerProcessControl()
    throw()
  {
  }
  
  // process initialisation called by platformmngt 
  void ServerProcessControl::on_initialise (const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
  {
    // ===================================================
    // Get the process name and application name
    // ===================================================
    m_processName = Cdmw::CdmwInit::InitUtils::get_cdmw_process_name();
    
    m_applicationName = Cdmw::CdmwInit::InitUtils::get_cdmw_application_name();
    
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Initialisation requested by supervision");
    // do nothing
  }
  
  
  // process to run called by platformmngt    
  void ServerProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(),
                                                         ">>>>>>>>>>>>>> Run requested by supervision");
                
    // do nothing
  }


  // process to stop called by platformmngt 
  void ServerProcessControl::on_stop()
    throw(CORBA::SystemException)
  {
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                                         m_processName.c_str(), 
                                                         ">>>>>>>>>>>>>> Stop requested by supervision");
    // do nothing
  }    
  

}; // End namespace TimeInvocation
