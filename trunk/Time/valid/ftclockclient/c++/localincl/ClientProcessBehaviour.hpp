/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#ifndef INCL_CLIENT_PROCESS_BEHAVIOUR_HPP 
#define INCL_CLIENT_PROCESS_BEHAVIOUR_HPP 


// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>

// Demo Files
#include "CosClockService.skel.hpp"
#include <ftclockclient/ClientThread.hpp>

namespace Cdmw {
    namespace clock {
        namespace valid {
            class ClientProcessBehaviour;
        }
    }
}

class Cdmw::clock::valid::ClientProcessBehaviour
    : public Cdmw::CdmwInit::ProcessControl
{

public:

    ClientProcessBehaviour(CORBA::ORB_ptr orb,
                           PortableServer::POA_ptr rootPOA);
    
    virtual ~ClientProcessBehaviour() throw ();

    
    virtual CORBA::ULong nb_steps()
        throw(CORBA::SystemException);

    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw(CORBA::SystemException);
        
    void on_next_step()
        throw(CdmwPlatformMngt::ProcessDelegate::InvalidStep,
              CORBA::SystemException);
    
    virtual void on_run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CORBA::SystemException);
    
    virtual void on_stop()
        throw(CORBA::SystemException);

private:
    CORBA::ORB_var m_orb;
    CosClockService::PeriodicExecution::Executor_var m_group;
    ClientThread* m_client_thread;
};


#endif // INCL_CLIENT_PROCESS_BEHAVIOUR_HPP


