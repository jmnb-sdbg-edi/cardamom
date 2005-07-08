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


#ifndef INCL_PLATFORMADMIN_SYSTEM_HPP
#define INCL_PLATFORMADMIN_SYSTEM_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "platformadmin/Service.hpp"
#include "platformadmin/Entity.hpp"
#include "platformadmin/Property.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class ConfigException;
class SystemParser;

class System : public virtual ServiceContainer, public virtual EntityContainer, public virtual PropertyContainer
{
private:
    CdmwPlatformMngt::System_var m_system;
    
public:
    System(
        CdmwPlatformMngt::System_ptr system );

    ~System();

    CdmwPlatformMngt::SystemSnapshot* get_snapshot();
    
    void start(
        const CdmwPlatformMngtBase::StartupKind& startup_kind );

    void stop(
        bool emergency );

    void resume();

    void acknowledge();

    static
    CdmwPlatformMngt::SystemDef* get_definition(
        const SystemParser& sytemParser );

    void define(
        const char* sytemConfigFileName );

    void add_host(
        const char* hostConfigFileName );

    void remove_host( const char* hostConfigFileName );

    void add_application(
        const char* applicationConfigFileName );

    void remove_application(
        const char* applicationName );

    void set_init_graph(
        const char* graphConfigFileName );

    void set_stop_graph(
        const char* graphConfigFileName );
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_SYSTEM_HPP

