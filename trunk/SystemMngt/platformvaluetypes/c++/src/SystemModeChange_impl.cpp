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
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         * SystemModeChangeFactory
         */
        CORBA::ValueBase*
        SystemModeChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::SystemModeChange_var systemEv
            = new SystemModeChange_impl();

            return systemEv._retn();
        }

        CdmwPlatformMngt::SystemModeChange* SystemModeChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            CdmwPlatformMngt::FunctioningMode systemMode,
            const char* modeInfo )
        {
            CdmwPlatformMngt::SystemModeChange_var systemEv
            = new SystemModeChange_impl(
                  aHeader,
                  systemName,
                  systemMode,
                  modeInfo);

            return systemEv._retn();
        }

        /**
         * SystemModeChange_impl
         */
        SystemModeChange_impl::SystemModeChange_impl()
        {
            event_kind(CdmwPlatformMngt::SYSTEM_MODE_CHANGE);
        }

        SystemModeChange_impl::SystemModeChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            CdmwPlatformMngt::FunctioningMode systemMode,
            const char* modeInfo)
                : OBV_CdmwPlatformMngt::SystemModeChange()
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::SYSTEM_MODE_CHANGE );
            system_mode( systemMode );
            mode_info( modeInfo );
        }

        CORBA::ValueBase* SystemModeChange_impl::_copy_value()
        {
            CdmwPlatformMngt::SystemModeChange_var systemEv
            = new SystemModeChange_impl(
                  header(),
                  system_name(),
                  system_mode(),
                  mode_info() );

            return systemEv._retn();
        }

        char* SystemModeChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " ";

            switch (system_mode())
            {

                    case CdmwPlatformMngt::NORMAL_MODE:
                    result_str += "NORMAL";
                    break;

                    case CdmwPlatformMngt::DEGRADED_MODE:
                    result_str += "DEGRADED";
                    break;
            }

            result_str += " : ";
            result_str += mode_info();

            CORBA::String_var str = CORBA::string_dup(result_str.c_str());
            return str._retn();

        }

    } // End namespace PlatformMngt
} // End namespace Cdmw

