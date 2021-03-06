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


#include "ConfAndPlug/cdmwinit/DefaultProcessControl.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"

namespace Cdmw
{
namespace CdmwInit
{

DefaultProcessControl::DefaultProcessControl() 
    throw ()
    : ProcessControl()
{
}

DefaultProcessControl::~DefaultProcessControl() 
    throw ()
{
}

void 
DefaultProcessControl::on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException) 
{
    using namespace Cdmw::PlatformMngt;
    // Default behaviour only informs of receipt of the message
    PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
				     PlatformInterface::Get_process_name(),
				     "Received 'initialisation' message");
}
    
void 
DefaultProcessControl::on_next_step()
    throw(CdmwPlatformMngt::ProcessDelegate::InvalidStep, 
	  CORBA::SystemException)
{
    using namespace Cdmw::PlatformMngt;
    // Default behaviour only informs of receipt of the message
    PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
				     PlatformInterface::Get_process_name(),
				     "Received 'next_step' message");
}

void 
DefaultProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
	  CORBA::SystemException)
{
    using namespace Cdmw::PlatformMngt;
    // Default behaviour only informs of receipt of the message
    PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
				     PlatformInterface::Get_process_name(),
				     "Received 'run' message");
}

void 
DefaultProcessControl::on_stop()
    throw(CORBA::SystemException) 
{
    using namespace Cdmw::PlatformMngt;
    // Default behaviour only informs of receipt of the message
    PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
				     PlatformInterface::Get_process_name(),
				     "Received 'stop' message");
}

}; // End namespace CdmwInit
}; // End namespace Cdmw


