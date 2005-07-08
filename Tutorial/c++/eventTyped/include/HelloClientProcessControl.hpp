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

#ifndef INCL_HELLO_CLIENT_PROCESS_CONTROL_HPP
#define INCL_HELLO_CLIENT_PROCESS_CONTROL_HPP

#include <iostream>

#include <Foundation/orbsupport/CORBA.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>

#include "HelloClientThread.hpp"


namespace Cdmw
{
namespace Tutorial
{

class HelloClientProcessControl : public Cdmw::CdmwInit::ProcessControl
{
    
  public:

    HelloClientProcessControl (CORBA::ORB_ptr orb)
        throw(CORBA::SystemException);
    
    ~HelloClientProcessControl()
        throw();
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/initialise:1.0
     * operation
     */
    virtual  
    void on_initialise (const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException);
    
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/run:1.0
     * operation
     */
    virtual 
    void on_run()
        throw(CdmwPlatformMngt::Process::NotReadyToRun, 
	          CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/stop:1.0
     * operation
     */
    virtual 
    void on_stop()
        throw(CORBA::SystemException);

  private:
  
    /**
    * The orb reference.
    */
    CORBA::ORB_var m_orb;
    
    /**
    * The client thread reference
    */
    Cdmw::Tutorial::HelloClientThread* m_hello_client_thread;
    
    /**
    * The application name.
    */
    std::string m_applicationName;
      
    /**
    * The process name.
    */
    std::string m_processName;
    
    
    
};  // End class HelloClientProcessControl

}; // End namespace Tutorial
}; // End namespace Cdmw

#endif // INCL_HELLO_CLIENT_PROCESS_CONTROL_HPP
