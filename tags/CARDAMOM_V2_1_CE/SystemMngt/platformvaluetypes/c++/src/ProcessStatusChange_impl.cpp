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
#include <sstream>
#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt


        /**
         * ProcessMessageFactory
         */
        CORBA::ValueBase*
        ProcessMessageFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ProcessMessage_var messageEv
            = new ProcessMessage_impl();

            return messageEv._retn();
        }

        CdmwPlatformMngt::ProcessMessage*
        ProcessMessageFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const CdmwPlatformMngt::ProcessID& processId,
            const char* theIssuer,
            const char* theInfo )
        {
            CdmwPlatformMngt::ProcessMessage_var messageEv
            = new ProcessMessage_impl(
                  aHeader,
                  systemName,
                  processId,
                  theIssuer,
                  theInfo );

            return messageEv._retn();
        }

        /**
         * ProcessMessage_impl
         */
        ProcessMessage_impl::ProcessMessage_impl()
        {
            event_kind(CdmwPlatformMngt::PROCESS_MESSAGE);
        }

        ProcessMessage_impl::ProcessMessage_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const CdmwPlatformMngt::ProcessID& processId,
            const char* theIssuer,
            const char* theInfo )
                : OBV_CdmwPlatformMngt::ProcessMessage()
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::PROCESS_MESSAGE );
            process_id( processId );
            issuer( theIssuer );
            info( theInfo );
        }

        CORBA::ValueBase*
        ProcessMessage_impl::_copy_value()
        {
            CdmwPlatformMngt::ProcessMessage_var messageEv
            = new ProcessMessage_impl(
                  header(),
                  system_name(),
                  process_id(),
                  issuer(),
                  info() );

            return messageEv._retn();
        }


        char* ProcessMessage_impl::get_issuer()
        {
            CORBA::String_var theIssuer = CORBA::string_dup(issuer());
            return theIssuer._retn();
        }

        char* ProcessMessage_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " A:'";
            result_str += (process_id().application_name).in();
            result_str += "' H:'";
            result_str += (process_id().host_name).in();
            result_str += "' P:'";
            result_str += (process_id().process_name).in();
            result_str += "' : ";

            result_str += info();

            CORBA::String_var str = CORBA::string_dup(result_str.c_str());
            return str._retn();

        }


        /**
         * ProcessStatusChangeFactory
         */
        CORBA::ValueBase*
        ProcessStatusChangeFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ProcessStatusChange_var processEv
            = new ProcessStatusChange_impl();

            return processEv._retn();
        }

        CdmwPlatformMngt::ProcessStatusChange*
        ProcessStatusChangeFactory::create(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const CdmwPlatformMngt::ProcessID& processId,
            const char* theIssuer,
            CdmwPlatformMngt::ProcessStatus processStatus,
            CORBA::Short stepNumber,
            const char* stepLabel,
            const char* theInfo )
        {
            CdmwPlatformMngt::ProcessStatusChange_var processEv
            = new ProcessStatusChange_impl(
                  aHeader,
                  systemName,
                  processId,
                  theIssuer,
                  processStatus,
                  stepNumber,
                  stepLabel,
                  theInfo );

            return processEv._retn();
        }

        /**
         * ProcessStatusChange_impl
         */
        ProcessStatusChange_impl::ProcessStatusChange_impl()
        {
            event_kind( CdmwPlatformMngt::PROCESS_STATUS_CHANGE );
        }

        // Nota:
        // Segmentation fault problem with AIX for multi inheritance with ": OBV_CdmwPlatformMngt::ProcessStatusChange()",
        // ": OBV_CdmwPlatformMngt::ProcessStatusChange(), ProcessMessage_impl()" leads also to same problem
        // With TAO, the default valuetype constructors OBV_* are not generated by the idl compiler
        //

        ProcessStatusChange_impl::ProcessStatusChange_impl(
            const CdmwPlatformMngtBase::EventHeader& aHeader,
            const char* systemName,
            const CdmwPlatformMngt::ProcessID& processId,
            const char* theIssuer,
            CdmwPlatformMngt::ProcessStatus processStatus,
            CORBA::Short stepNumber,
            const char* stepLabel,
            const char* theInfo )
#if CDMW_ORB_VDR != tao

#ifndef __IBMCPP__
                :
                OBV_CdmwPlatformMngt::ProcessStatusChange()
#endif
#endif
        {
            header( aHeader );
            system_name( systemName );
            event_kind( CdmwPlatformMngt::PROCESS_STATUS_CHANGE );
            process_id( processId );
            issuer( theIssuer );
            info( theInfo );
            process_status( processStatus );
            step_number( stepNumber );
            step_label( stepLabel );
        }

        CORBA::ValueBase*
        ProcessStatusChange_impl::_copy_value()
        {
            CdmwPlatformMngt::ProcessStatusChange_var processEv
            = new ProcessStatusChange_impl(
                  header(),
                  system_name(),
                  process_id(),
                  issuer(),
                  process_status(),
                  step_number(),
                  step_label(),
                  info() );

            return processEv._retn();
        }

        char* ProcessStatusChange_impl::to_string()
        {
            std::string result_str(beginString());

            result_str += " A:'";
            result_str += (process_id().application_name).in();
            result_str += "' H:'";
            result_str += (process_id().host_name).in();
            result_str += "' P:'";
            result_str += (process_id().process_name).in();
            result_str += "' ";

            CdmwPlatformMngt::ProcessStatus processStatus = process_status();

            switch (processStatus)
            {

                    case CdmwPlatformMngt::PROCESS_INITIAL:
                    result_str += "INITIAL";
                    break;

                    case CdmwPlatformMngt::PROCESS_INITIALISING:
                    result_str += "INITIALISING";
                    break;

                    case CdmwPlatformMngt::PROCESS_STEP_PERFORMED:
                    result_str += "STEP_PERFORMED";
                    break;

                    case CdmwPlatformMngt::PROCESS_INITIALISED:
                    result_str += "INITIALISED";
                    break;

                    case CdmwPlatformMngt::PROCESS_RUN_REQUEST:
                    result_str += "RUN_REQUEST";
                    break;

                    case CdmwPlatformMngt::PROCESS_RUNNING:
                    result_str += "RUNNING";
                    break;

                    case CdmwPlatformMngt::PROCESS_STOPPING:
                    result_str += "STOPPING";
                    break;

                    case CdmwPlatformMngt::PROCESS_STOPPED:
                    result_str += "STOPPED";
                    break;

                    case CdmwPlatformMngt::PROCESS_ENDED:
                    result_str += "ENDED";
                    break;

                    case CdmwPlatformMngt::PROCESS_FAILED_ERROR:
                    result_str += "FAILED_ERROR";
                    break;

                    case CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT:
                    result_str += "FAILED_TIMEOUT";
                    break;

                    case CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY:
                    result_str += "FAILED_DISCREPANCY";
                    break;

                    case CdmwPlatformMngt::PROCESS_FAILED_INVALID:
                    result_str += "FAILED_INVALID";
                    break;

                    case CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE:
                    result_str += "FAILED_NO_RESPONSE";
                    break;

                    case CdmwPlatformMngt::PROCESS_FAILED_DEATH:
                    result_str += "FAILED_DEATH";
                    break;
            }

            if (processStatus == CdmwPlatformMngt::PROCESS_INITIALISING
                    || processStatus == CdmwPlatformMngt::PROCESS_STEP_PERFORMED)
            {

                std::ostringstream temp;
                temp << " step ";
                temp.fill('0');
                temp.width(2);
                temp << step_number() << " '" << step_label() << "'";
                result_str += temp.str();
            }
            else
            {
                result_str += " : ";
                result_str += info();
            }

            CORBA::String_var str = CORBA::string_dup( result_str.c_str() );
            return str._retn();
        }

    } // End namespace PlatformMngt
} // End namespace Cdmw

