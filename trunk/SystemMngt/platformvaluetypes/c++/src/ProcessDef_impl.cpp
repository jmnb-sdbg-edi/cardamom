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
#include "SystemMngt/platformvaluetypes/ProcessDef_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         * ProcessDefFactory
         */
        CORBA::ValueBase*
        ProcessDefFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ProcessDef_var processDef = new ProcessDef_impl();
            return processDef._retn();
        }

        CdmwPlatformMngt::ProcessDef* ProcessDefFactory::create(
            const char* name)
        {
            CdmwPlatformMngt::ProcessDef_var processDef = new ProcessDef_impl(
                        name, CdmwPlatformMngt::UNMANAGED_PROCESS);
            return processDef._retn();
        }

        /**
         * ProcessDef_impl
         */
        ProcessDef_impl::ProcessDef_impl()
        {
            type(CdmwPlatformMngt::UNMANAGED_PROCESS);
        }

        ProcessDef_impl::ProcessDef_impl(
            const char* name,
            CdmwPlatformMngt::ProcessType type )
        {
            OBV_CdmwPlatformMngt::ProcessDef::name(name);
            OBV_CdmwPlatformMngt::ProcessDef::type(type);
        }

        CORBA::ValueBase* ProcessDef_impl::_copy_value()
        {
            CdmwPlatformMngt::ProcessDef_var processDef =
                new ProcessDef_impl(name(), type());

            processDef->auto_ending(auto_ending());
            processDef->host_name(host_name());
            processDef->process_info(process_info());

            return processDef._retn();
        }

        /**
         * ManagedProcessDefFactory
         */
        CORBA::ValueBase*
        ManagedProcessDefFactory::create_for_unmarshal()
        {
            CdmwPlatformMngt::ManagedProcessDef_var processDef
            = new ManagedProcessDef_impl();

            return processDef._retn();
        }

        CdmwPlatformMngt::ManagedProcessDef* ManagedProcessDefFactory::create(
            const char* name, CdmwPlatformMngt::ProcessType type )
        {
            CdmwPlatformMngt::ManagedProcessDef_var processDef
            = new ManagedProcessDef_impl(name, type);

            return processDef._retn();
        }

        /**
         * ManagedProcessDef_impl
         */
        ManagedProcessDef_impl::ManagedProcessDef_impl()
        {
            type(CdmwPlatformMngt::MANAGED_PROCESS);
        }

        ManagedProcessDef_impl::ManagedProcessDef_impl(
            const char* name,
            CdmwPlatformMngt::ProcessType type )
        {
            OBV_CdmwPlatformMngt::ManagedProcessDef::name(name);

            if (type == CdmwPlatformMngt::UNMANAGED_PROCESS)
                // error => force the type
                OBV_CdmwPlatformMngt::ManagedProcessDef::type(CdmwPlatformMngt::MANAGED_PROCESS);
            else
                OBV_CdmwPlatformMngt::ManagedProcessDef::type(type);

        }

        CORBA::ValueBase* ManagedProcessDef_impl::_copy_value()
        {
            CdmwPlatformMngt::ManagedProcessDef_var processDef
            = new ManagedProcessDef_impl(name(), type());

            // the other attributes of the base value type
            processDef->auto_ending(auto_ending());
            processDef->host_name(host_name());
            processDef->process_info(process_info());

            // the attributes of this value type
            processDef->entities(entities());
            processDef->service_definitions(service_definitions());
            processDef->life_cycle(life_cycle());
            processDef->monitoring_info(monitoring_info());

            return processDef._retn();
        }

    } // End namespace PlatformMngt
} // End namespace Perc

