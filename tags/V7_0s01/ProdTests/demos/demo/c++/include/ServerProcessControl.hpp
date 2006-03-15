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


#ifndef INCL_CDMW_RESOURCES_SERVER_PROCESS_CONTROL_HPP 
#define INCL_CDMW_RESOURCES_SERVER_PROCESS_CONTROL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"

#include <iostream>


#include <string>

namespace Cdmw
{
namespace Resources
{

class ServerProcessControl : public Cdmw::CdmwInit::ProcessControl
{
public:

    ServerProcessControl(CORBA::ORB_ptr          orb,
                         PortableServer::POA_ptr poa,
                         const std::string &     server_name,
                         const std::string &     channel_name,
                         const std::string &     complete_channel_name,
                         const std::string &     complete_typed_channel_name)
        throw(CORBA::SystemException);
    
    ~ServerProcessControl() throw();
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    virtual 
    void 
    on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException);
    
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    virtual 
    void 
    on_run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
	      CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    virtual 
    void 
    on_stop()
        throw(CORBA::SystemException);

private:
    CORBA::ORB_var           m_orb;
    PortableServer::POA_var  m_poa;
    std::string              m_server_name;
    std::string              m_channel_name;
    std::string              m_complete_channel_name;
    std::string              m_complete_typed_channel_name;
};  // End class MyProcessControl


}; // End namespace Resources
}; // End namespace Cdmw

#endif // INCL_CDMW_RESOURCES_SERVER_PROCESS_CONTROL_HPP

