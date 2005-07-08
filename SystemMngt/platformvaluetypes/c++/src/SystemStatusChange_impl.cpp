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
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         * SystemStatusChangeFactory
         */
        CORBA::ValueBase*
        SystemStatusChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::SystemStatusChange_var systemEv
            = new SystemStatusChange_impl();

            return systemEv._retn();
        }

        CdmwPlatformMngt::SystemStatusChange* SystemStatusChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            CdmwPlatformMngt::SystemStatus systemStatus,
            const char* statusInfo )
        {
            CdmwPlatformMngt::SystemStatusChange_var systemEv
            = new SystemStatusChange_impl(
                  aHeader,
                  systemName,
                  systemStatus,
                  statusInfo );

            return systemEv._retn();
        }

        /**
         * SystemStatusChange_impl
         */
        SystemStatusChange_impl::SystemStatusChange_impl()
        {
            event_kind( CdmwPlatformMngt::SYSTEM_STATUS_CHANGE );
        }

        SystemStatusChange_impl::SystemStatusChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            CdmwPlatformMngt::SystemStatus systemStatus,
            const char* statusInfo )
                : OBV_CdmwPlatformMngt::SystemStatusChange()
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::SYSTEM_STATUS_CHANGE );
            system_status( systemStatus );
            status_info( statusInfo );
        }

        CORBA::ValueBase* SystemStatusChange_impl::_copy_value()
        {
            CdmwPlatformMngt::SystemStatusChange_var systemEv
            = new SystemStatusChange_impl(
                  header(),
                  system_name(),
                  system_status(),
                  status_info() );

            return systemEv._retn();
        }

        char* SystemStatusChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " ";

            switch (system_status())
            {

                    case CdmwPlatformMngt::SYSTEM_INITIAL:
                    result_str += "INITIAL";
                    break;

                    case CdmwPlatformMngt::SYSTEM_STARTING:
                    result_str += "STARTING";
                    break;

                    case CdmwPlatformMngt::SYSTEM_STARTUP_SUSPENDED:
                    result_str += "STARTUP_SUSPENDED";
                    break;

                    case CdmwPlatformMngt::SYSTEM_STARTED:
                    result_str += "STARTED";
                    break;

                    case CdmwPlatformMngt::SYSTEM_STOPPING:
                    result_str += "STOPPING";
                    break;

                    case CdmwPlatformMngt::SYSTEM_STOPPED:
                    result_str += "STOPPED";
                    break;
            }

            result_str += " : ";
            result_str += status_info();

            CORBA::String_var str = CORBA::string_dup(result_str.c_str());
            return str._retn();

        }

    } // End namespace PlatformMngt
} // End namespace Cdmw

