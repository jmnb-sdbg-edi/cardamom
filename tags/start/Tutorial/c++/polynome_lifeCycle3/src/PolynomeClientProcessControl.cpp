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


#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include "PolynomeClientProcessControl.hpp"


namespace Cdmw
{
namespace Tutorial
{


PolynomeClientProcessControl::PolynomeClientProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_polynome_client_thread (NULL),
      m_applicationName (""),
      m_processName ("")
{
}

PolynomeClientProcessControl::~PolynomeClientProcessControl()
    throw () 
{
    // ==================================================
    // if client thread is defined call on_stop
    //     to stop it if still running
    // ==================================================
    if (m_polynome_client_thread != NULL) 
    {
	    this->on_stop();
	}
}
  


// process initialisation called by platformmngt     
void PolynomeClientProcessControl::on_initialise (
                  const CdmwPlatformMngtBase::StartupKind& startup_kind)
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
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                     m_processName.c_str(), ">>> Initialisation requested by supervision");
    
    
    // ==================================================                                
    // create the hello client thread
    // ==================================================
    m_polynome_client_thread =
        new Cdmw::Tutorial::PolynomeClientThread (m_orb.in(),
                                                   m_applicationName,
                                                   m_processName);
                
}
  
// process to run called by platformmngt    
void PolynomeClientProcessControl::on_run()
    throw(CdmwPlatformMngt::Process::NotReadyToRun, 
          CORBA::SystemException)
{
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                     m_processName.c_str(), ">>> Run requested by supervision");
                                     
     
    // ======================================                                
    // start the client thread
    // ======================================
    if (m_polynome_client_thread != NULL) 
    {
	    m_polynome_client_thread->start();
	}
                
}


// process to stop called by platformmngt
void PolynomeClientProcessControl::on_stop()
    throw(CORBA::SystemException)
{
    // =================================================================
    // example of using the PlatformInterface for notifying a message
    // =================================================================
    Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                     m_processName.c_str(), ">>> Stop requested by supervision");
                                     
                                     
    if (m_polynome_client_thread != NULL) 
    {
	    // First signal termination to hello client thread if it was 
	    // at least requested to run
	    
        Cdmw::OsSupport::ThreadStatus status = m_polynome_client_thread -> get_status();
        
        if ((status == Cdmw::OsSupport::THREAD_RUN_REQUEST) ||
            (status == Cdmw::OsSupport::THREAD_RUNNING)) 
        {
            m_polynome_client_thread->stop();
            m_polynome_client_thread->join();
        }
        
	    delete m_polynome_client_thread;
	    m_polynome_client_thread = NULL;
    }
                                     
}


}; // End namespace Tutorial
}; // End namespace Cdmw
