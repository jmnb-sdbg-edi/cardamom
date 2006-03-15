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


#include <string>
#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         * ApplicationAgentDeathFactory
         */
        CORBA::ValueBase*
        ApplicationAgentDeathFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ApplicationAgentDeath_var event
            = new ApplicationAgentDeath_impl();

            return event._retn();
        }

        CdmwPlatformMngt::ApplicationAgentDeath*
        ApplicationAgentDeathFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* hostName,
            const char* applicationName )
        {
            CdmwPlatformMngt::ApplicationAgentDeath_var event
            = new ApplicationAgentDeath_impl(
                  aHeader,
                  systemName,
                  hostName,
                  applicationName );

            return event._retn();
        }

        /**
         * ApplicationAgentDeath_impl
         */
        ApplicationAgentDeath_impl::ApplicationAgentDeath_impl()
        {
            event_kind(CdmwPlatformMngt::APPLICATION_AGENT_DEATH);
        }

        ApplicationAgentDeath_impl::ApplicationAgentDeath_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* hostName,
            const char* applicationName )
                : OBV_CdmwPlatformMngt::ApplicationAgentDeath()
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::APPLICATION_AGENT_DEATH );
            host_name( hostName );
            application_name( applicationName );
        }

        CORBA::ValueBase* ApplicationAgentDeath_impl::_copy_value()
        {
            CdmwPlatformMngt::ApplicationAgentDeath_var event
            = new ApplicationAgentDeath_impl(
                  header(),
                  system_name(),
                  host_name(),
                  application_name() );

            return event._retn();
        }

        char* ApplicationAgentDeath_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " A:'";
            result_str += application_name();
            result_str += "' The agent has crashed on host '";
            result_str += host_name();
            result_str += "'";

            CORBA::String_var str = CORBA::string_dup( result_str.c_str() );
            return str._retn();
        }

    } // End namespace PlatformMngt
} // End namespace Perc

