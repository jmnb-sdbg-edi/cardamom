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


#ifndef INCL_PLATFORMADMIN_LIFECYCLEPARSER_HPP
#define INCL_PLATFORMADMIN_LIFECYCLEPARSER_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "platformadmin/ConfigElementParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class LifeCycleParser : public virtual ConfigElementParser
{
private:
    class InitStepParser : public virtual ConfigElementParser
    {
    public:
        InitStepParser( const DOM_Node& lifeCycleParser );

        std::string name() const;

        unsigned long timeout() const;
    };

    class RunStepParser : public virtual ConfigElementParser
    {
    public:
        RunStepParser( const DOM_Node& lifeCycleParser );

        unsigned long timeout() const;
    };

    class StopStepParser : public virtual ConfigElementParser
    {
    public:
        StopStepParser( const DOM_Node& lifeCycleParser );

        unsigned long timeout() const;
    };

    friend class InitStepParser;
    friend class RunStepParser;
    friend class StopStepParser;

public:
    LifeCycleParser( const DOM_Node& lifeCycleParser );

    unsigned int get_init_step_count() const;

    std::string get_init_step_name( unsigned int initStepIndex ) const;

    unsigned long get_init_step_timeout( unsigned int initStepIndex ) const;

    unsigned long get_run_timeout() const;

    unsigned long get_stop_timeout() const;
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_LIFECYCLEPARSER_HPP

