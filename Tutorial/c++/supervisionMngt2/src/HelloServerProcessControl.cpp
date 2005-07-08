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


#include <SystemMngt/platforminterface/PlatformInterface.hpp>

#include "HelloServerProcessControl.hpp"


namespace Cdmw
{
namespace Tutorial
{ 


HelloServerProcessControl::HelloServerProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_applicationName (""),
      m_processName ("")
{
}


HelloServerProcessControl::~HelloServerProcessControl()
    throw()
{
}
    
    
    
// process initialisation called by platformmngt 
void HelloServerProcessControl::on_initialise (
                const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    // ==============================================================
    // get application and process names   
    // ==============================================================                 
    m_applicationName = Cdmw::PlatformMngt::PlatformInterface::getApplicationName();
    m_processName = Cdmw::PlatformMngt::PlatformInterface::getProcessName();
    
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                     m_processName.c_str(), 
                                     ">>>>>>>>>>>>>> Initialisation requested by supervision");
                
    // do nothing
}
  
  
// process to run called by platformmngt    
void HelloServerProcessControl::on_run()
    throw(CdmwPlatformMngt::Process::NotReadyToRun, 
          CORBA::SystemException)
{
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                     m_processName.c_str(), 
                                     ">>>>>>>>>>>>>> Run requested by supervision");
                
    // do nothing
}


// process to stop called by platformmngt 
void HelloServerProcessControl::on_stop()
    throw(CORBA::SystemException)
{
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                     m_processName.c_str(), 
                                     ">>>>>>>>>>>>>> Stop requested by supervision");
                
    // do nothing
}    
    

}; // End namespace Tutorial
}; // End namespace Cdmw

