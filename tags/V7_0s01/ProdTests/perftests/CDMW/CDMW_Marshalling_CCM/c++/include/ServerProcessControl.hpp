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
 
 

#ifndef INCL_SERVER_PROCESS_CONTROL_HPP 
#define INCL_SERVER_PROCESS_CONTROL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include <iostream>
#include <string>




namespace Marshalling
{
  
  class ServerProcessControl : public Cdmw::CdmwInit::ProcessControl
  {
  public:
    
    ServerProcessControl(CORBA::ORB_ptr orb)
      throw(CORBA::SystemException);
        
    ~ServerProcessControl()
      throw();
    
    virtual void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
      throw(CORBA::SystemException);
  
    virtual void on_run()
      throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
	    CORBA::SystemException);
    
    virtual void on_stop()
      throw(CORBA::SystemException);

    static void ccm_activated()
      throw(CORBA::SystemException);

  private:
    

    /**
     * The orb reference.
     */
    CORBA::ORB_var m_orb;
    
    /**
     * Mutex protecting the following condition
     */
    static Cdmw::OsSupport::Mutex M_mutex;

    /**
     * Condition on which the thread is sleeping to wait for ccm_activate
     */
    static Cdmw::OsSupport::Condition M_ccmactivated_condition;

    /**
     * The application name.
     */
    std::string m_applicationName;
    
    /**
     * The process name.
     */
    std::string m_processName;
    
    
  };  // End class ServerProcessControl
  
  
}; // End namespace Marshalling

#endif // INCL_SERVER_PROCESS_CONTROL_HPP
