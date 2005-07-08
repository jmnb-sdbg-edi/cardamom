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


#include <sstream>
#include "platformadmin/PlatformAdminSnapshot.hpp"

using namespace std;

//----------------------//
// Function definitions //
//----------------------//
string snapShotToString( CdmwPlatformMngt::SystemSnapshot* snapshot )
{
    ostringstream strSnapshot;

    strSnapshot << endl;
    strSnapshot << "|  " << endl;
    strSnapshot << "+- System" << endl;
    switch( snapshot->system_status )
    {
         case (CdmwPlatformMngt::SYSTEM_INITIAL) :
            strSnapshot << "|  status:      SYSTEM_INITIAL" << endl;
            break;
         case (CdmwPlatformMngt::SYSTEM_STARTING) :
            strSnapshot << "|  status:      SYSTEM_STARTING" << endl;
            break;
         case (CdmwPlatformMngt::SYSTEM_STARTUP_SUSPENDED) :
            strSnapshot << "|  status:      SYSTEM_STARTUP_SUSPENDED" << endl;
            break;
         case (CdmwPlatformMngt::SYSTEM_STARTED) :
            strSnapshot << "|  status:      SYSTEM_STARTED" << endl;
            break;
         case (CdmwPlatformMngt::SYSTEM_STOPPING) :
            strSnapshot << "|  status:      SYSTEM_STOPPING" << endl;
            break;
         case (CdmwPlatformMngt::SYSTEM_STOPPED) :
            strSnapshot << "|  status:      SYSTEM_STOPPED" << endl;
            break;
    }
    strSnapshot << "|  status info: " << snapshot->status_info.in() << endl;
    switch( snapshot->system_mode )
    {
         case (CdmwPlatformMngt::NORMAL_MODE) :
            strSnapshot << "|  mode:        NORMAL_MODE" << endl;
            break;
         case (CdmwPlatformMngt::DEGRADED_MODE) :
            strSnapshot << "|  mode:        DEGRADED_MODE" << endl;
            break;
         default:
            strSnapshot << "|  mode:        " << endl;
    }
    strSnapshot << "|  mode info:   " << snapshot->mode_info.in() << endl;

    // Get the host snapshots
    int hostCount = snapshot->system_hosts.length();

    for( int hostIndex = 0;
         hostIndex < hostCount;
         hostIndex++ )
    {
        CdmwPlatformMngt::HostSnapshot hostSnapshot
            = snapshot->system_hosts[ hostIndex ];

        strSnapshot << "|  |  " << endl;
        strSnapshot << "|  +- Host" << endl;
        strSnapshot << "|  |  name:   " << hostSnapshot.host_name.in() << endl;
        switch( hostSnapshot.host_status )
        {
             case (CdmwPlatformMngt::HOST_RESPONDING) :
                strSnapshot << "|  |  status: HOST_RESPONDING" << endl;
                break;
             case (CdmwPlatformMngt::HOST_NOT_RESPONDING) :
                strSnapshot << "|  |  status: HOST_NOT_RESPONDING" << endl;
                break;
        }
    }

    // Get the application snapshots
    int applicationCount = snapshot->system_applications.length();

    for( int applicationIndex = 0;
         applicationIndex < applicationCount;
         applicationIndex++ )
    {
        CdmwPlatformMngt::ApplicationSnapshot applicationSnapshot
            = snapshot->system_applications[ applicationIndex ];

        strSnapshot << snapShotToString( &applicationSnapshot, "|  " ).c_str();
    }

    // Get the entities snapshots
    int entityCount = snapshot->system_entities.length();

    for( int entityIndex = 0;
         entityIndex < entityCount;
         entityIndex++ )
    {
        CdmwPlatformMngtEntity::Entity entitySnapshot
            = snapshot->system_entities[ entityIndex ];

        strSnapshot << snapShotToString( &entitySnapshot, "|  " ).c_str();
    }

    return string( strSnapshot.str() );
}

string snapShotToString(
    CdmwPlatformMngt::ApplicationSnapshot* snapshot,
    const char* sIndent )
{
    ostringstream strSnapshot;

    strSnapshot << sIndent << "|  " << endl;
    strSnapshot << sIndent << "+- Application" << endl;
    strSnapshot << sIndent << "|  name:        " << snapshot->application_name.in() << endl;
    switch( snapshot->application_status )
    {
         case (CdmwPlatformMngt::APPLICATION_INITIAL) :
            strSnapshot << sIndent << "|  status:      APPLICATION_INITIAL" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_INITIALISING) :
            strSnapshot << sIndent << "|  status:      APPLICATION_INITIALISING" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_INITIALISATION_SUSPENDED) :
            strSnapshot << sIndent << "|  status:      APPLICATION_INITIALISATION_SUSPENDED" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_INITIALISATION_INCOMPLETE) :
            strSnapshot << sIndent << "|  status:      APPLICATION_INITIALISATION_INCOMPLETE" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_INITIALISED) :
            strSnapshot << sIndent << "|  status:      APPLICATION_INITIALISED" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_RUN_REQUEST) :
            strSnapshot << sIndent << "|  status:      APPLICATION_RUN_REQUEST" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_RUNNING) :
            strSnapshot << sIndent << "|  status:      APPLICATION_RUNNING" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_STOPPING) :
            strSnapshot << sIndent << "|  status:      APPLICATION_STOPPING" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_STOPPED) :
            strSnapshot << sIndent << "|  status:      APPLICATION_STOPPED" << endl;
            break;
         case (CdmwPlatformMngt::APPLICATION_FAILED) :
            strSnapshot << sIndent << "|  status:      APPLICATION_FAILED" << endl;
            break;
    }
    strSnapshot << sIndent << "|  status info: " << snapshot->status_info.in() << endl;
    switch( snapshot->application_mode )
    {
         case (CdmwPlatformMngt::NORMAL_MODE) :
            strSnapshot << sIndent << "|  mode:        NORMAL_MODE" << endl;
            break;
         case (CdmwPlatformMngt::DEGRADED_MODE) :
            strSnapshot << sIndent << "|  mode:        DEGRADED_MODE" << endl;
            break;
         default:
            strSnapshot << sIndent << "|  mode:" << endl;
    }
    strSnapshot << sIndent << "|  mode info:   " << snapshot->mode_info.in() << endl;

    // Get the process snapshots
    int processCount = snapshot->application_processes.length();

    for( int processIndex = 0;
         processIndex < processCount;
         processIndex++ )
    {
        CdmwPlatformMngt::ProcessSnapshot processSnapshot
            = snapshot->application_processes[ processIndex ];

        strSnapshot << snapShotToString(
                            &processSnapshot,
                            (string( sIndent ) + "|  ").c_str()
                        ).c_str();
    }

    // Get the entities snapshots
    int entityCount = snapshot->application_entities.length();

    for( int entityIndex = 0;
         entityIndex < entityCount;
         entityIndex++ )
    {
        CdmwPlatformMngtEntity::Entity entitySnapshot
            = snapshot->application_entities[ entityIndex ];

        strSnapshot << snapShotToString(
                            &entitySnapshot,
                            (string( sIndent ) + "|  ").c_str()
                        ).c_str()    ;
    }

    return string( strSnapshot.str() );
}

string snapShotToString(
    CdmwPlatformMngt::ProcessSnapshot* snapshot,
    const char* sIndent )
{
    ostringstream strSnapshot;

    strSnapshot << sIndent << "|  " << endl;
    strSnapshot << sIndent << "+- Process" << endl;
    strSnapshot << sIndent << "|  name:        " << snapshot->process_name.in() << endl;
    switch( snapshot->process_status )
    {
         case (CdmwPlatformMngt::PROCESS_INITIAL) :
            strSnapshot << sIndent << "|  status:      PROCESS_INITIAL" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_INITIALISING) :
            strSnapshot << sIndent << "|  status:      PROCESS_INITIALISING" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_STEP_PERFORMED) :
            strSnapshot << sIndent << "|  status:      PROCESS_STEP_PERFORMED" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_INITIALISED) :
            strSnapshot << sIndent << "|  status:      PROCESS_INITIALISED" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_RUN_REQUEST) :
            strSnapshot << sIndent << "|  status:      PROCESS_RUN_REQUEST" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_RUNNING) :
            strSnapshot << sIndent << "|  status:      PROCESS_RUNNING" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_STOPPING) :
            strSnapshot << sIndent << "|  status:      PROCESS_STOPPING" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_STOPPED) :
            strSnapshot << sIndent << "|  status:      PROCESS_STOPPED" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_ENDED) :
            strSnapshot << sIndent << "|  status:      PROCESS_ENDED" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_FAILED_ERROR) :
            strSnapshot << sIndent << "|  status:      PROCESS_FAILED_ERROR" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT) :
            strSnapshot << sIndent << "|  status:      PROCESS_FAILED_TIMEOUT" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY) :
            strSnapshot << sIndent << "|  status:      PROCESS_FAILED_DISCREPANCY" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_FAILED_INVALID) :
            strSnapshot << sIndent << "|  status:      PROCESS_FAILED_INVALID" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE) :
            strSnapshot << sIndent << "|  status:      PROCESS_FAILED_NO_RESPONSE" << endl;
            break;
         case (CdmwPlatformMngt::PROCESS_FAILED_DEATH) :
            strSnapshot << sIndent << "|  status:      PROCESS_FAILED_DEATH" << endl;
            break;
    }
    strSnapshot << sIndent << "|  status info: " << snapshot->process_status_info.in() << endl;

    // Get the activity points
    int pointCount = snapshot->activity_points.length();

    for( int pointIndex = 0;
         pointIndex < pointCount;
         pointIndex++ )
    {
        CdmwPlatformMngt::ActivityPoint activityPoint
            = snapshot->activity_points[ pointIndex ];

        strSnapshot << sIndent << "|  |" << endl;
        strSnapshot << sIndent << "|  +- Activity point" << endl;
        strSnapshot << sIndent << "|  |  name:   " << activityPoint.point_name.in() << endl;
        switch( activityPoint.point_status )
        {
             case (CdmwPlatformMngt::POINT_RESPONDING) :
                strSnapshot << sIndent << "|  |  status: POINT_RESPONDING" << endl;
                break;
             case (CdmwPlatformMngt::POINT_NOT_RESPONDING) :
                strSnapshot << sIndent << "|  |  status: POINT_NOT_RESPONDING" << endl;
                break;
             case (CdmwPlatformMngt::POINT_UNKNOWN) :
                strSnapshot << sIndent << "|  |  status: POINT_UNKNOWN" << endl;
                break;
        }
    }

    // Get the entities snapshots
    int entityCount = snapshot->process_entities.length();

    for( int entityIndex = 0;
         entityIndex < entityCount;
         entityIndex++ )
    {
        CdmwPlatformMngtEntity::Entity entitySnapshot
            = snapshot->process_entities[ entityIndex ];

        strSnapshot << snapShotToString(
                            &entitySnapshot,
                            (string( sIndent )  + "|  ").c_str()
                       ).c_str();
    }

    return string( strSnapshot.str() );
}

string snapShotToString(
    CdmwPlatformMngtEntity::Entity* entity,
    const char* sIndent )
{
    ostringstream strSnapshot;

    strSnapshot << sIndent << "|  " << endl;
    strSnapshot << sIndent << "+- Entity" << endl;
    strSnapshot << sIndent << "|  name:  " << entity->entity_name.in() << endl;
    switch( entity->entity_status )
    {
         case (CdmwPlatformMngtEntity::ENTITY_FUNCTIONING) :
            strSnapshot << sIndent << "|  status: ENTITY_FUNCTIONING" << endl;
            break;
         case (CdmwPlatformMngtEntity::ENTITY_DYSFUNCTION) :
            strSnapshot << sIndent << "|  status: ENTITY_DYSFUNCTION" << endl;
            break;
         case (CdmwPlatformMngtEntity::ENTITY_FAILED_NO_RESPONSE) :
            strSnapshot << sIndent << "|  status: ENTITY_FAILED_NO_RESPONSE" << endl;
            break;
         case (CdmwPlatformMngtEntity::ENTITY_FAILED_DEATH) :
            strSnapshot << sIndent << "|  status: ENTITY_FAILED_DEATH" << endl;
            break;
    }
    strSnapshot << sIndent << "|  info:   " << entity->entity_info.in() << endl;

    return string( strSnapshot.str() );
}

