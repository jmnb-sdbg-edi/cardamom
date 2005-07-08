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
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         * HostStatusChangeFactory
         */
        CORBA::ValueBase*
        HostStatusChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::HostStatusChange_var hostEv
            = new HostStatusChange_impl();

            return hostEv._retn();
        }

        CdmwPlatformMngt::HostStatusChange*
        HostStatusChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* hostName,
            CdmwPlatformMngt::HostStatus hostStatus )
        {
            CdmwPlatformMngt::HostStatusChange_var hostEv
            = new HostStatusChange_impl(
                  aHeader,
                  systemName,
                  hostName,
                  hostStatus );

            return hostEv._retn();
        }

        /**
         * HostStatusChange_impl
         */
        HostStatusChange_impl::HostStatusChange_impl()
        {
            event_kind(CdmwPlatformMngt::HOST_STATUS_CHANGE);
        }

        HostStatusChange_impl::HostStatusChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* hostName,
            CdmwPlatformMngt::HostStatus hostStatus )
                : OBV_CdmwPlatformMngt::HostStatusChange()
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::HOST_STATUS_CHANGE );
            host_name( hostName );
            host_status( hostStatus );
        }

        CORBA::ValueBase* HostStatusChange_impl::_copy_value()
        {
            CdmwPlatformMngt::HostStatusChange_var hostEv
            = new HostStatusChange_impl(
                  header(),
                  system_name(),
                  host_name(),
                  host_status() );

            return hostEv._retn();
        }

        char* HostStatusChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " H:'";
            result_str += host_name();
            result_str += "' ";

            switch (host_status())
            {

                    case CdmwPlatformMngt::HOST_RESPONDING:
                    result_str += "RESPONDING";
                    break;

                    case CdmwPlatformMngt::HOST_NOT_RESPONDING:
                    result_str += "NOT_RESPONDING";
                    break;
            }


            CORBA::String_var str = CORBA::string_dup(result_str.c_str());
            return str._retn();
        }

    } // End namespace PlatformMngt
} // End namespace Perc

