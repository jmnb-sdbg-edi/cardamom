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

        strSnapshot << snapShotToString( &hostSnapshot, "|  " ).c_str();
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
        CdmwPlatformMngt::EntitySnapshot entitySnapshot
            = snapshot->system_entities[ entityIndex ];

        strSnapshot << snapShotToString( &entitySnapshot, "|  " ).c_str();
    }

    // Get the properties snapshots
    int propertyCount = snapshot->system_properties.length();
     
    for(int propIndex = 0;
	propIndex < propertyCount;
	propIndex++ )
    {
        CdmwPlatformMngt::PropertySnapshot propertySnapshot
	    = snapshot->system_properties[propIndex];

	strSnapshot << snapShotToString(&propertySnapshot,"|  ").c_str();
    }		

    return string( strSnapshot.str() );
}


string snapShotToString(
    CdmwPlatformMngt::HostSnapshot* snapshot,
    const char* sIndent )
{
    ostringstream strSnapshot;

    strSnapshot << sIndent << "|  " << endl;
    strSnapshot << sIndent << "+- Host" << endl;
    strSnapshot << sIndent << "|  name:        " << snapshot->host_name.in() << endl;
    
    switch( snapshot->host_status )
    {
    case (CdmwPlatformMngt::HOST_RESPONDING) :
	strSnapshot << sIndent << "|  status: HOST_RESPONDING" << endl;
	break;
    case (CdmwPlatformMngt::HOST_UNREACHABLE) :
	strSnapshot << sIndent << "|  status: HOST_UNREACHABLE" << endl;
	break;
    }

    // Get the entities snapshots
    int entityCount = snapshot->host_entities.length();

    for( int entityIndex = 0;
         entityIndex < entityCount;
         entityIndex++ )
    {
        CdmwPlatformMngt::EntitySnapshot entitySnapshot
            = snapshot->host_entities[ entityIndex ];

        strSnapshot << snapShotToString(
	    &entitySnapshot,
	    (string( sIndent ) + "|  ").c_str()
	    ).c_str()    ;
    }

                                                                                                                                 
    // Get the properties snapshots
    int propertyCount = snapshot->host_properties.length();
                                                                                                                             
    for(int propIndex = 0;
	propIndex < propertyCount;
	propIndex++ )
    {
        CdmwPlatformMngt::PropertySnapshot propertySnapshot
	    = snapshot->host_properties[propIndex];
        strSnapshot << snapShotToString(&propertySnapshot,(string( sIndent ) + "|  ").c_str()).c_str();
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
        CdmwPlatformMngt::EntitySnapshot entitySnapshot
            = snapshot->application_entities[ entityIndex ];

        strSnapshot << snapShotToString(
	    &entitySnapshot,
	    (string( sIndent ) + "|  ").c_str()
	    ).c_str()    ;
    }

    // Get the properties snapshots
    int propertyCount = snapshot->application_properties.length();
    for(int propIndex = 0;
	propIndex < propertyCount;
	propIndex++ )
    {
        CdmwPlatformMngt::PropertySnapshot propertySnapshot
	    = snapshot->application_properties[propIndex];
   	strSnapshot << snapShotToString(&propertySnapshot,(string( sIndent ) + "|  ").c_str()).c_str();
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
    strSnapshot << sIndent << "|  host:        " << snapshot->host_name.in() << endl;
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

    // Get the entities snapshots
    int entityCount = snapshot->process_entities.length();

    for( int entityIndex = 0;
         entityIndex < entityCount;
         entityIndex++ )
    {
        CdmwPlatformMngt::EntitySnapshot entitySnapshot
            = snapshot->process_entities[ entityIndex ];

        strSnapshot << snapShotToString(
	    &entitySnapshot,
	    (string( sIndent )  + "|  ").c_str()
	    ).c_str();
    }
    
    // Get the properties snapshots
    int propertyCount = snapshot->process_properties.length();
    
    for(int propIndex = 0;
	propIndex < propertyCount;
	propIndex++ )
    {
        CdmwPlatformMngt::PropertySnapshot propertySnapshot
	    = snapshot->process_properties[propIndex];
        strSnapshot << snapShotToString(&propertySnapshot,(string( sIndent ) + "|  ").c_str()).c_str();
    }		
    
    return string( strSnapshot.str() );
}

string snapShotToString(
    CdmwPlatformMngt::EntitySnapshot* snapshot,
    const char* sIndent )
{
    ostringstream strSnapshot;
    
    strSnapshot << sIndent << "|  " << endl;
    strSnapshot << sIndent << "+- Entity" << endl;
    strSnapshot << sIndent << "|  name  :  " << snapshot->entity_name.in() << endl;
    strSnapshot << sIndent << "|  status:  " << snapshot->entity_status.in() << endl;
    strSnapshot << sIndent << "|  info  :  " << snapshot->entity_info.in() << endl;
    
    // Get the properties snapshots
    int propertyCount = snapshot->entity_properties.length();
    
    for(int propIndex = 0;
	propIndex < propertyCount;
	propIndex++ )
    {
        CdmwPlatformMngt::PropertySnapshot propertySnapshot
	    = snapshot->entity_properties[propIndex];
//	strSnapshot << snapShotToString(&propertySnapshot,"|  ").c_str();
	strSnapshot << snapShotToString(&propertySnapshot,(string( sIndent ) + "|  ").c_str()).c_str();
    }		
    
    return string( strSnapshot.str() );
}

string snapShotToString( 
    CdmwPlatformMngt::PropertySnapshot* snapshot, 
    const char* sIndent)
{
    ostringstream strSnapshot;
    
/*
  std::string mode;
  if (snapshot->property_mode == 0) mode="Read/Write";
  else  mode="Read-Only";
    
*/	                                                                                                                             
    strSnapshot << sIndent << "|  " << endl;
    strSnapshot << sIndent << "+- Property" << endl;
    strSnapshot << sIndent << "|  name  :  " << snapshot->property_name.in() << endl;
    strSnapshot << sIndent << "|  type  :  " << snapshot->property_type.in() << endl;
    strSnapshot << sIndent << "|  mode  :  " << snapshot->property_mode.in() << endl;
    
    

    // Preparing the output for the Properties 	
    CORBA::Any prop_value = snapshot -> property_value;      
    CORBA::TypeCode_var tc;
    tc = prop_value.type();
    switch (tc->kind())
    {
    default: 
	break;	
    case CORBA::tk_string:  
    {
	snapshot -> property_type = CORBA::string_dup("String");
	const char* value;
	prop_value >>= value ;
    	strSnapshot << sIndent << "|  value :  " << value << endl;
    }
    break;	
    
    case CORBA::tk_float: 
    { 
	snapshot -> property_type = CORBA::string_dup("Float");
	CORBA::Float value;
	prop_value >>= value;
        strSnapshot << sIndent << "|  value :  " << value << endl;
	
    }
    break;
    case (CORBA::tk_short):
    {  
	snapshot -> property_type = CORBA::string_dup("Short");
	short value;
	prop_value >>= value;
        strSnapshot << sIndent << "|  value :  " << value << endl;

    }	
    break;
    }		        
	

     
		
    //TO DO OTHER FIELD OF THE PROPERTY (UAD)
    return string(strSnapshot.str() ); 		
}





