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

#include <iostream>


#include <Foundation/common/Assert.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>


#include "Snapshot.hpp"
#include "MyObserver_impl.hpp"

void MyObserver_impl::notify( CdmwPlatformMngt::Event* event )
throw( CORBA::SystemException )
{
    // PROCESS_MESSAGE: print the message events of the processes
    if( event->event_kind() == CdmwPlatformMngt::PROCESS_MESSAGE )
    {
        CdmwPlatformMngt::ProcessMessage* message
            = CdmwPlatformMngt::ProcessMessage::_downcast( event );

        std::cout
            << ((message->process_id()).process_name).in()
            << " <- "
            << message->info()
            << std::endl;
    }

    // USER_EVENT: print the system snapshot in this case
    if( event->event_kind() == CdmwPlatformMngt::USER_EVENT )
    {
        // Get the data part of the user event
        CdmwPlatformMngt::UserEvent* user_event
            = CdmwPlatformMngt::UserEvent::_downcast( event );

        CdmwPlatformMngt::SystemSnapshot* snapshot;
        CDMW_ASSERT( user_event->data() >>= snapshot );

        // Print out the snapshot of the system 
        std::cout << snapShotToString( snapshot ).c_str();
    }
}
