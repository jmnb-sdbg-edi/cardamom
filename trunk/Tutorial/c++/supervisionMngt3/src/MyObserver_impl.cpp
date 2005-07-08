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
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>

#include "MyObserver_impl.hpp"

#include "Data.stub.hpp"

void MyObserver_impl::notify( CdmwPlatformMngt::Event* event )
throw( CORBA::SystemException )
{
    // Only treat the user event
    if( event->event_kind() == CdmwPlatformMngt::USER_EVENT )
    {
        // Get the data part of the user event
        CdmwPlatformMngt::UserEvent* user_event
            = CdmwPlatformMngt::UserEvent::_downcast( event );

        CORBA::ValueBase* value_base;
        CDMW_ASSERT( user_event->data() >>= value_base );

        BaseData* base_data = BaseData::_downcast( value_base );

        // downcast the data into the appropriate type
        switch( base_data->data_type() )
        {
            case TYPE_A:
            {
                DataA* data = DataA::_downcast( base_data );
                std::cout
                    << "Received a user event with a data of type TYPE_A" << std::endl
                    << "DataA.message = " << data->message() << std::endl
                    << std::endl;
                break;
            }

            case TYPE_B:
            {
                DataB* data = DataB::_downcast( base_data );
                std::cout
                    << "Received a user event with a data of type TYPE_B" << std::endl
                    << "DataB.message = " << data->message() << std::endl
                    << "DataB.number  = " << data->number() << std::endl
                    << std::endl;
                break;
            }
        }
    }
}
