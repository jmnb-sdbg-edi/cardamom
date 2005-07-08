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
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt


        /**
         * SystemEntityStatusChangeFactory
         */
        CORBA::ValueBase*
        SystemEntityStatusChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::SystemEntityStatusChange_var entityEv
            = new SystemEntityStatusChange_impl();

            return entityEv._retn();
        }


        CdmwPlatformMngt::SystemEntityStatusChange*
        SystemEntityStatusChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* theInfo )
        {
            CdmwPlatformMngt::SystemEntityStatusChange_var entityEv
            = new SystemEntityStatusChange_impl(
                  aHeader,
                  systemName,
                  entityName,
                  entityStatus,
                  theInfo );

            return entityEv._retn();
        }

        /**
         * SystemEntityStatusChange_impl
         */
        SystemEntityStatusChange_impl::SystemEntityStatusChange_impl()
        {
            event_kind(CdmwPlatformMngt::SYSTEM_ENTITY_STATUS_CHANGE);
        }

        SystemEntityStatusChange_impl::SystemEntityStatusChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* theInfo )
                : OBV_CdmwPlatformMngt::SystemEntityStatusChange()
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::SYSTEM_ENTITY_STATUS_CHANGE );
            entity_name( entityName );
            entity_status( entityStatus );
            info( theInfo );
        }

        CORBA::ValueBase* SystemEntityStatusChange_impl::_copy_value()
        {
            CdmwPlatformMngt::SystemEntityStatusChange_var entityEv
            = new SystemEntityStatusChange_impl(
                  header(),
                  system_name(),
                  entity_name(),
                  entity_status(),
                  info() );

            return entityEv._retn();
        }

        std::string SystemEntityStatusChange_impl::endString()
        {
            std::string endString("E:'");
            endString += entity_name();
            endString += "' ";

            switch (entity_status())
            {

                    case CdmwPlatformMngtEntity::ENTITY_FUNCTIONING:
                    endString += "FUNCTIONING";
                    break;

                    case CdmwPlatformMngtEntity::ENTITY_DYSFUNCTION:
                    endString += "DYSFUNCTION";
                    break;

                    case CdmwPlatformMngtEntity::ENTITY_FAILED_NO_RESPONSE:
                    endString += "FAILED_NO_RESPONSE";
                    break;

                    case CdmwPlatformMngtEntity::ENTITY_FAILED_DEATH:
                    endString += "FAILED_DEATH";
                    break;
            }

            endString += " : ";
            endString += info();

            return endString;
        }

        char* SystemEntityStatusChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " ";

            result_str += endString();

            CORBA::String_var str = CORBA::string_dup(result_str.c_str());
            return str._retn();
        }


        /**
         * ApplicationEntityStatusChangeFactory
         */
        CORBA::ValueBase*
        ApplicationEntityStatusChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ApplicationEntityStatusChange_var entityEv
            = new ApplicationEntityStatusChange_impl();

            return entityEv._retn();
        }


        CdmwPlatformMngt::ApplicationEntityStatusChange*
        ApplicationEntityStatusChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* applicationName,
            const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* theInfo )
        {
            CdmwPlatformMngt::ApplicationEntityStatusChange_var entityEv
            = new ApplicationEntityStatusChange_impl(
                  aHeader,
                  systemName,
                  applicationName,
                  entityName,
                  entityStatus,
                  theInfo );

            return entityEv._retn();
        }

        /**
         * ApplicationEntityStatusChange_impl
         */
        ApplicationEntityStatusChange_impl::ApplicationEntityStatusChange_impl()
        {
            event_kind(CdmwPlatformMngt::APPLICATION_ENTITY_STATUS_CHANGE);
        }

        ApplicationEntityStatusChange_impl::ApplicationEntityStatusChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* applicationName,
            const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* theInfo )
                : OBV_CdmwPlatformMngt::ApplicationEntityStatusChange()
        {
            header( aHeader ),
            system_name( systemName ),
            event_kind( CdmwPlatformMngt::APPLICATION_ENTITY_STATUS_CHANGE ),
            entity_name( entityName ),
            entity_status( entityStatus ),
            info( theInfo ),
            application_name( applicationName );
        }

        CORBA::ValueBase* ApplicationEntityStatusChange_impl::_copy_value()
        {
            CdmwPlatformMngt::ApplicationEntityStatusChange_var entityEv
            = new ApplicationEntityStatusChange_impl(
                  header(),
                  system_name(),
                  application_name(),
                  entity_name(),
                  entity_status(),
                  info() );

            return entityEv._retn();
        }


        char* ApplicationEntityStatusChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " A:'";
            result_str += application_name();
            result_str += "' ";

            result_str += endString();

            CORBA::String_var str = CORBA::string_dup(result_str.c_str());
            return str._retn();
        }


        /**
         * ProcessEntityStatusChangeFactory
         */
        CORBA::ValueBase*
        ProcessEntityStatusChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ProcessEntityStatusChange_var entityEv
            = new ProcessEntityStatusChange_impl();

            return entityEv._retn();
        }


        CdmwPlatformMngt::ProcessEntityStatusChange*
        ProcessEntityStatusChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* applicationName,
            const char* processName,
            const char* hostName,
            const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* theInfo )
        {
            CdmwPlatformMngt::ProcessEntityStatusChange_var entityEv
            = new ProcessEntityStatusChange_impl(
                  aHeader,
                  systemName,
                  applicationName,
                  processName,
                  hostName,
                  entityName,
                  entityStatus,
                  theInfo );

            return entityEv._retn();
        }

        /**
         * ProcessEntityStatusChange_impl
         */
        ProcessEntityStatusChange_impl::ProcessEntityStatusChange_impl()
        {
            event_kind(CdmwPlatformMngt::PROCESS_ENTITY_STATUS_CHANGE);
        }

        ProcessEntityStatusChange_impl::ProcessEntityStatusChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const char* applicationName,
            const char* processName,
            const char* hostName,
            const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* theInfo )
                : OBV_CdmwPlatformMngt::ProcessEntityStatusChange()
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::PROCESS_ENTITY_STATUS_CHANGE );
            entity_name( entityName );
            entity_status( entityStatus );
            info( theInfo );
            application_name( applicationName );
            process_name( processName );
            host_name( hostName );
        }

        CORBA::ValueBase* ProcessEntityStatusChange_impl::_copy_value()
        {
            CdmwPlatformMngt::ProcessEntityStatusChange_var entityEv
            = new ProcessEntityStatusChange_impl(
                  header(),
                  system_name(),
                  application_name(),
                  process_name(),
                  host_name(),
                  entity_name(),
                  entity_status(),
                  info() );

            return entityEv._retn();
        }


        char* ProcessEntityStatusChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " A:'";
            result_str += application_name();
            result_str += "' P:'";
            result_str += process_name();
            result_str += "' ";

            result_str += endString();

            CORBA::String_var str = CORBA::string_dup(result_str.c_str());
            return str._retn();
        }

    } // End namespace PlatformMngt
} // End namespace Cdmw

