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


#ifndef INCL_PLATFORMADMIN_PROCESS_HPP
#define INCL_PLATFORMADMIN_PROCESS_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtProcessProxy.stub.hpp"
#include "platformadmin/Service.hpp"
#include "platformadmin/Entity.hpp"
#include "platformadmin/Property.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class ConfigException;
class ProcessParser;

class Process : public ServiceContainer, public EntityContainer, public PropertyContainer
{
private:
    CdmwPlatformMngt::ProcessProxy_var m_process;
    
public:
    Process(
        CdmwPlatformMngt::ProcessProxy_ptr process );

    ~Process();

    CdmwPlatformMngt::ProcessSnapshot* get_snapshot();

    void initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind );

    void next_step();

    void run();

    void stop(
        bool emergency );

    static
    CdmwPlatformMngt::ProcessDef* get_definition(
        const ProcessParser& processParser );

    void set_info(
        const char* process_exe,
        CdmwPlatformMngt::Port process_port,
        const char* working_directory,
        const char* process_args );

    void set_autoending();

    void set_host(
        const char* host_name );

    void set_life_cycle(
        const char* lifeCycleFileName );
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_PROCESS_HPP

