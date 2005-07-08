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


#ifndef INCL_PLATFORMADMIN_MONITORINGPARSER_HPP
#define INCL_PLATFORMADMIN_MONITORINGPARSER_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "platformadmin/ConfigElementParser.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

class MonitoringParser : public ConfigElementParser
{
protected:
    MonitoringParser( const DOM_Node& monitoringParser );

public:
    virtual ~MonitoringParser() {};

    virtual CdmwPlatformMngt::MonitoringModel model() const;

    virtual unsigned long pull_interval() const;

    virtual unsigned long pull_timeout() const;

    virtual unsigned long push_interval() const;

    virtual unsigned long push_timeout() const;
};

class HostMonitoringParser : public virtual MonitoringParser
{
public:
    HostMonitoringParser( const DOM_Node& monitoringParser );

    CdmwPlatformMngt::MonitoringModel model() const
    {
        CDMW_ASSERT( false );
        return CdmwPlatformMngt::NO_MONITORING;
    };

    unsigned long push_interval() const
    {
        CDMW_ASSERT( false );
        return 0;
    };

    unsigned long push_timeout() const
    {
        CDMW_ASSERT( false );
        return 0;
    };
};

class ApplicationMonitoringParser : public virtual MonitoringParser
{
public:
    ApplicationMonitoringParser( const DOM_Node& monitoringParser );

    CdmwPlatformMngt::MonitoringModel model() const
    {
        CDMW_ASSERT( false );
        return CdmwPlatformMngt::NO_MONITORING;
    };

    unsigned long push_interval() const
    {
        CDMW_ASSERT( false );
        return 0;
    };

    unsigned long push_timeout() const
    {
        CDMW_ASSERT( false );
        return 0;
    };
};

class ProcessMonitoringParser : public MonitoringParser
{
public:
    ProcessMonitoringParser( const DOM_Node& monitoringParser );
};

class ActivityPointParser : public virtual ConfigElementParser
{
public:
    ActivityPointParser( const DOM_Node& activityPointParser );

    std::string name() const;

    unsigned long monitoring_interval() const;

    unsigned long monitoring_timeout() const;
};

class ActivityPointContainerParser : public virtual ConfigElementParser
{
protected:
    ActivityPointContainerParser( const DOM_Node& activityPointContainerParser );

public:
    unsigned int get_activity_point_count() const;

    ActivityPointParser get_activity_point( unsigned int activityPointIndex ) const;
};

} // End namespace PlatformAdmin
} // End namespace Cdmw

#endif //INCL_PLATFORMADMIN_MONITORINGPARSER_HPP

