/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include <string>
#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"

namespace Cdmw
{

    namespace PlatformMngt
    {

        /**
         * UserEventFactory
         */
        CORBA::ValueBase* UserEventFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::UserEvent_var event = new UserEvent_impl();
            return event._retn();
        }

        CdmwPlatformMngt::UserEvent* UserEventFactory::create(
            const CdmwPlatformMngtBase::EventHeader& header,
            const char* system_name,
            const char* application_name,
            const char* process_name,
            const char* issuer,
            const CORBA::Any& data )
        {
            CdmwPlatformMngt::UserEvent_var event
            = new UserEvent_impl(
                  header,
                  system_name,
                  application_name,
                  process_name,
                  issuer,
                  data );

            return event._retn();
        }

        /**
         * UserEvent_impl
         */
        UserEvent_impl::UserEvent_impl()
                : OBV_CdmwPlatformMngt::UserEvent()
        {
            event_kind( CdmwPlatformMngt::USER_EVENT );
        }

        UserEvent_impl::UserEvent_impl(
            const CdmwPlatformMngtBase::EventHeader& theHeader,
            const char* systemName,
            const char* applicationName,
            const char* processName,
            const char* theIssuer,
            const CORBA::Any& theData )
                : OBV_CdmwPlatformMngt::UserEvent()
        {
            CORBA::Any data_ = theData;

            event_kind( CdmwPlatformMngt::USER_EVENT );
            header( theHeader );
            system_name( systemName );
            application_name( applicationName );
            process_name( processName );
            issuer( theIssuer );
            data( data_ );
        }

        CORBA::ValueBase* UserEvent_impl::_copy_value()
        {
            CdmwPlatformMngt::UserEvent_var event
            = new UserEvent_impl(
                  header(),
                  system_name(),
                  application_name(),
                  process_name(),
                  issuer(),
                  data() );
            return event._retn();
        }

        //
        // IDL:thalesgroup.com/CdmwPlatformMngt/Event/get_issuer:1.0
        //
        char* UserEvent_impl::get_issuer()
        {
            CORBA::String_var theIssuer = CORBA::string_dup( issuer() );
            return theIssuer._retn();
        }

        //
        // IDL:thalesgroup.com/CdmwPlatformMngt/Event/to_string:1.0
        //
        char* UserEvent_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " A:'";
            result_str += application_name();
            result_str += "' P:'";
            result_str += process_name();
            result_str += "' ";

            CORBA::String_var corba_str = CORBA::string_dup( result_str.c_str() );
            return corba_str._retn();
        }

    } // End namespace PlatformMngt
} // End namespace Cdmw

