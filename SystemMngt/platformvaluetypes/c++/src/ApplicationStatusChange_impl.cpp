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
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         * ApplicationStatusChangeFactory
         */
        CORBA::ValueBase*
        ApplicationStatusChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ApplicationStatusChange_var applicationEv
            = new ApplicationStatusChange_impl();
            return applicationEv._retn();
        }

        CdmwPlatformMngt::ApplicationStatusChange*
        ApplicationStatusChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* applicationName,
            CdmwPlatformMngt::ApplicationStatus applicationStatus,
            const char* statusInfo )
        {
            CdmwPlatformMngt::ApplicationStatusChange_var applicationEv
            = new ApplicationStatusChange_impl(
                  aHeader,
                  systemName,
                  applicationName,
                  applicationStatus,
                  statusInfo );

            return applicationEv._retn();
        }

        /**
         * ApplicationStatusChange_impl
         */
        ApplicationStatusChange_impl::ApplicationStatusChange_impl()
        {
            event_kind(CdmwPlatformMngt::APPLICATION_STATUS_CHANGE);
        }

        ApplicationStatusChange_impl::ApplicationStatusChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* applicationName,
            CdmwPlatformMngt::ApplicationStatus applicationStatus,
            const char* statusInfo )
                : OBV_CdmwPlatformMngt::ApplicationStatusChange()
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::APPLICATION_STATUS_CHANGE );
            application_name( applicationName );
            application_status( applicationStatus );
            status_info( statusInfo );
        }

        CORBA::ValueBase* ApplicationStatusChange_impl::_copy_value()
        {
            CdmwPlatformMngt::ApplicationStatusChange_var applicationEv
            = new ApplicationStatusChange_impl(
                  header(), system_name(),
                  application_name(), application_status(), status_info());

            return applicationEv._retn();
        }

        char* ApplicationStatusChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " A:'";
            result_str += application_name();
            result_str += "' ";

            switch (application_status())
            {

                    case CdmwPlatformMngt::APPLICATION_INITIAL:
                    result_str += "INITIAL";
                    break;

                    case CdmwPlatformMngt::APPLICATION_INITIALISING:
                    result_str += "INITIALISING";
                    break;

                    case CdmwPlatformMngt::APPLICATION_INITIALISATION_SUSPENDED:
                    result_str += "INITIALISATION_SUSPENDED";
                    break;

                    case CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE:
                    result_str += "INITIALISATION_INCOMPLETE";
                    break;

                    case CdmwPlatformMngt::APPLICATION_INITIALISED:
                    result_str += "INITIALISED";
                    break;

                    case CdmwPlatformMngt::APPLICATION_RUN_REQUEST:
                    result_str += "RUN_REQUEST";
                    break;

                    case CdmwPlatformMngt::APPLICATION_RUNNING:
                    result_str += "RUNNING";
                    break;

                    case CdmwPlatformMngt::APPLICATION_STOPPING:
                    result_str += "STOPPING";
                    break;

                    case CdmwPlatformMngt::APPLICATION_STOPPED:
                    result_str += "STOPPED";
                    break;

                    case CdmwPlatformMngt::APPLICATION_FAILED:
                    result_str += "FAILED";
                    break;
            }

            result_str += " : ";
            result_str += status_info();

            CORBA::String_var str = CORBA::string_dup(result_str.c_str());
            return str._retn();

        }

    } // End namespace PlatformMngt
} // End namespace Cdmw

