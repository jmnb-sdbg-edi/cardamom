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


#ifndef INCL_PLATFORMADMIN_APPLICATION_HPP
#define INCL_PLATFORMADMIN_APPLICATION_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "platformadmin/Service.hpp"
#include "platformadmin/Entity.hpp"
#include "platformadmin/Property.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class ConfigException;
class ApplicationParser;

class Application : public ServiceContainer, public EntityContainer, public PropertyContainer
{
private:
    CdmwPlatformMngt::Application_var m_application;
    
public:
    Application(
        CdmwPlatformMngt::Application_ptr application );

    ~Application();

    CdmwPlatformMngt::ApplicationSnapshot* get_snapshot();

    void initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind );

    CdmwPlatformMngt::LongRequestStatus initialise_and_wait(
        const CdmwPlatformMngtBase::StartupKind& startup_kind );

    void run();

    void stop(
        bool emergency );

    void resume();

    CdmwPlatformMngt::LongRequestStatus resume_and_wait();

    void acknowledge();

    static CdmwPlatformMngt::ApplicationDef* get_definition(
        const ApplicationParser& applicationParser,
        CdmwPlatformMngt::PlatformApplicationDef_out platformApplicationDef );

    void add_process(
        const char* processConfigFileName );

    void remove_process(
        const char* processName );

    void set_init_graph(
        const char* graphConfigFileName );

    void set_stop_graph(
        const char* graphConfigFileName );
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_APPLICATION_HPP

