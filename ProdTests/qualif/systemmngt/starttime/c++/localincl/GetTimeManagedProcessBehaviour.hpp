/* =========================================================================== *
 * Copyright (c) 2003-2005 THALES All rights reserved.
 * Software commonly developed by THALES and AMS.
 *
 * This file is part of CARDAMOM.
 *
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with CARDAMOM; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


#ifndef INCL_GET_TIME_MANAGED_PROCESS_BEHAVIOUR_HPP 
#define INCL_GET_TIME_MANAGED_PROCESS_BEHAVIOUR_HPP


#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>


namespace Cdmw {


class GetTimeManagedProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
{

public:
    GetTimeManagedProcessBehaviour(CORBA::ORB_ptr);

    virtual ~GetTimeManagedProcessBehaviour() throw ();
    


    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException);
    
    virtual void run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
              CORBA::SystemException);

    virtual void stop()
        throw(CORBA::SystemException);
              
private:
    CORBA::ORB_var m_orb;
    
    Cdmw::OsSupport::OS::Timeval m_ctor_timeval;
    Cdmw::OsSupport::OS::Timeval m_init_timeval;


};


};  // namespace

#endif
