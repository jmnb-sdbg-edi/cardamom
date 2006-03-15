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
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         * ApplicationModeChangeFactory
         */
        CORBA::ValueBase*
        ApplicationModeChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ApplicationModeChange_var applicationEv
            = new ApplicationModeChange_impl();
            return applicationEv._retn();
        }

        CdmwPlatformMngt::ApplicationModeChange*
        ApplicationModeChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* applicationName,
            CdmwPlatformMngt::FunctioningMode applicationMode,
            const char* modeInfo )
        {
            CdmwPlatformMngt::ApplicationModeChange_var applicationEv
            = new ApplicationModeChange_impl(
                  aHeader,
                  systemName,
                  applicationName,
                  applicationMode,
                  modeInfo );

            return applicationEv._retn();
        }

        /**
         * ApplicationModeChange_impl
         */
        ApplicationModeChange_impl::ApplicationModeChange_impl()
        {
            event_kind(CdmwPlatformMngt::APPLICATION_MODE_CHANGE);
        }

        ApplicationModeChange_impl::ApplicationModeChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* applicationName,
            CdmwPlatformMngt::FunctioningMode applicationMode,
            const char* modeInfo )
                : OBV_CdmwPlatformMngt::ApplicationModeChange()
        {
            header( aHeader );
            system_name( systemName );
            event_kind(CdmwPlatformMngt::APPLICATION_MODE_CHANGE);
            application_name( applicationName );
            application_mode( applicationMode );
            mode_info( modeInfo );
        }

        CORBA::ValueBase* ApplicationModeChange_impl::_copy_value()
        {
            CdmwPlatformMngt::ApplicationModeChange_var applicationEv
            = new ApplicationModeChange_impl(
                  header(),
                  system_name(),
                  application_name(),
                  application_mode(),
                  mode_info() );

            return applicationEv._retn();
        }

        char* ApplicationModeChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " A:'";
            result_str += application_name();
            result_str += "' ";

            switch (application_mode())
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

