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

#include "Foundation/common/Options.hpp"

#include "platformadmin/PlatformAdminUsage.hpp"
#include "platformadmin/PlatformAdmin.hpp"

using namespace std;

string CmdUsage::M_cmdUsage =
"\n"
"Sends a command to an element of the supervision.\n"
"\n"
"Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
"       command [parameters]\n"
"\n"
""CHECK_ONLY_OPTION"\n"
"  Allows to check the correctness of a command and the consistency of a\n"
"  configuration file  without actually sending the command to the supervision.\n"
"\n"
""SYSTEM_CORBALOC_OPTION"\n"
"  Specifies the supervisied system that must receive the command.\n"
"  Not mandatory for the command "CHECK_ONLY_OPTION", "HOST_SHUTDOWN" or "HOST_REBOOT".\n"
"\n"
"\n"
"command\n"
"  The command to be sent to an element of the supervision.\n"
"\n"
"  The available commands for a system\n"
"  "SYS_SNAPSHOT" "SYS_DEFINE" "SYS_ADD_HOST" "SYS_REMOVE_HOST"\n"
"  "SYS_ADD_APPLICATION" "SYS_REMOVE_APPLICATION" "SYS_ADD_ENTITY"\n"
"  "SYS_REMOVE_ENTITY" "SYS_ADD_SERVICE" "SYS_REMOVE_SERVICE"\n"
"  "SYS_ADD_PROPERTY" "SYS_REMOVE_PROPERTY"\n"
"  "SYS_SET_INIT_GRAPH" "SYS_SET_STOP_GRAPH"\n"
"  "SYS_START" "SYS_RESUME" "SYS_ACKNOWLEDGE" "SYS_STOP"\n"
"\n"
"  The available commands for an application\n"
"  "APP_SNAPSHOT" "APP_ADD_PROCESS" "APP_REMOVE_PROCESS"\n"
"  "APP_ADD_SERVICE" "APP_REMOVE_SERVICE" "APP_ADD_ENTITY"\n"
"  "APP_REMOVE_ENTITY" "APP_ADD_PROPERTY" "APP_REMOVE_PROPERTY"\n"
"  "APP_SET_INIT_GRAPH" "APP_SET_STOP_GRAPH"\n"
"  "APP_INITIALISE" "APP_RESUME" "APP_ACKNOWLEDGE" "APP_RUN"\n"
"  "APP_STOP"\n"
"\n"
"  The available commands for a process\n"
"  "PROC_SNAPSHOT" "PROC_ADD_SERVICE" "PROC_REMOVE_SERVICE"\n"
"  "PROC_ADD_ENTITY" "PROC_REMOVE_ENTITY" "PROC_ADD_PROPERTY"\n"
"  "PROC_REMOVE_PROPERTY"\n"
"  "PROC_INITIALISE" "PROC_NEXT_STEP" "PROC_RUN" "PROC_STOP"\n"
"  "PROC_SET_INFO" "PROC_SET_AUTOENDING" "PROC_RESET_AUTOENDING"\n"
"  "PROC_SET_LIFE_CYLE"\n"
"\n"
"  The available commands for a host\n"
"  "HOST_SHUTDOWN" "HOST_REBOOT"\n"
"  "HOST_ADD_PROPERTY" "HOST_REMOVE_PROPERTY"\n"
"\n"
"  The available commands for an entity\n"
"  "ENTITY_ADD_PROPERTY" "ENTITY_REMOVE_PROPERTY"\n"	
"\n" 	
"parameters\n"
"  The parameters associated to the command.\n"
"  (Re-excute with a command-name to get help on the command and the\n"
"  associated parameters).\n";

CmdInfo _cmdInfoTab[] =
{
    { HELP_OPTION, CmdUsage::M_cmdUsage },
    { SYS_SNAPSHOT,
      "Prints a snapshot of the entire system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_SNAPSHOT"\n" },
    { SYS_DEFINE,
      "Defines the initial configuration of a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_DEFINE" \"system_file_name\"\n" },
    { SYS_ADD_HOST,
      "Adds a host to a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_ADD_HOST" \"host_file_name\"\n" },
    { SYS_REMOVE_HOST,
      "Removes a host from a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_REMOVE_HOST" host_name\n" },
    { SYS_ADD_APPLICATION,
      "Adds an application to a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_ADD_APPLICATION" \"application_file_name\"\n" },
    { SYS_REMOVE_APPLICATION,
      "Removes an application from a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_REMOVE_APPLICATION" application_name\n" },
    { SYS_ADD_ENTITY,
      "Adds an entity to a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_ADD_ENTITY" \"entity_file_name\"\n" },
    { SYS_REMOVE_ENTITY,
      "Removes an entity from a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_REMOVE_ENTITY" entity_name\n" },
    { SYS_ADD_SERVICE,
      "Adds a service to an application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_ADD_SERVICE" \"service_file_name\"\n" },
    { SYS_REMOVE_SERVICE,
      "Removes a service from a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_REMOVE_SERVICE" service_name\n" },
    { SYS_ADD_PROPERTY,
      "Adds a property to a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_ADD_PROPERTY" \"new_propety_name\"\n" },
    { SYS_REMOVE_PROPERTY,
      "Removes a property from a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_REMOVE_PROPERTY" property_name\n" },
    { SYS_SET_INIT_GRAPH,
      "Sets the initialisation graph of application dependency.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_SET_INIT_GRAPH" \"application_dependency_graph_file\"\n" },
    { SYS_SET_STOP_GRAPH,
      "Sets a stop graph of application dependency.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_SET_STOP_GRAPH" \"application_dependency_graph_file\"\n" },
    { SYS_START,
      "Starts a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_START" [state_retrieval startup_mode specified_state_id]\n"
      "\n"
      "The following parameters are optional. They are meaningful only to the\n"
      "application software.\n"
      "\n"
      "state_retrieval must be one of the following values:\n"
      "  LAST_STATE      application software shall retrieve their last stable state.\n"
      "  RECOVER_STATE   application software shall recover their state from their last run.\n"
      "  SPECIFIED_STATE application software shall retrieve the specified state.\n"
      "startup_mode is a short integer meaningful only to application software.\n"
      "specified_state_id is a string meaningful only if state_retrieval is SPECIFIED_STATE.\n"},
    { SYS_ACKNOWLEDGE,
      ".\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_ACKNOWLEDGE"\n" },
    { SYS_RESUME,
      "Resumes a startup of a system.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_RESUME"\n" },
    { SYS_STOP,
      "Requests a system to stop.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "SYS_STOP" [EMERGENCY]\n"
      "\n"
      "EMERGENCY is specified if the system is required to stop with\n"
      "emergency otherwise it is required to stop properly according to\n"
      "the specified stop graph.\n" },


    { APP_SNAPSHOT,
      "Prints a snapshot of a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_SNAPSHOT" application_name\n" },
    { APP_ADD_PROCESS,
      "Adds a process to a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_ADD_PROCESS" application_name \"process_file_name\"\n" },
    { APP_REMOVE_PROCESS,
      "Removes a process from a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_REMOVE_PROCESS" application_name process_name host_name\n" },
    { APP_ADD_SERVICE,
      "Adds a service to a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_ADD_SERVICE" application_name \"service_file_name\"\n" },
    { APP_REMOVE_SERVICE,
      "Removes a service from a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_REMOVE_SERVICE" application_name service_name\n" },
    { APP_ADD_ENTITY,
      "Adds an entity to a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_ADD_ENTITY" application_name \"entity_file_name\"\n" },
    { APP_REMOVE_ENTITY,
      "Removes an entity from a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_REMOVE_ENTITY" application_name entity_name\n" },
    { APP_ADD_PROPERTY,
      "Adds a property to a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_ADD_PROPERTY" application_name \"new_property_name\"\n" },
    { APP_REMOVE_PROPERTY,
      "Removes a property from a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_REMOVE_PROPERTY" application_name property_name\n" },
    { APP_SET_INIT_GRAPH,
      "Sets the initialisation graph of process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_SET_INIT_GRAPH" application_name \"process_dependency_graph_file\"\n" },
    { APP_SET_STOP_GRAPH,
      "Sets the stop graph of process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_SET_STOP_GRAPH" application_name \"process_dependency_graph_file\"\n" },
    { APP_INITIALISE,
      "Starts the initialisation of a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_INITIALISE" application_name [state_retrieval startup_mode specified_state_id]\n"
      "\n"
      "The following parameters are optional. They are meaningful only to the\n"
      "application software.\n"
      "\n"
      "state_retrieval must be one of the following values:\n"
      "  LAST_STATE      application software shall retrieve their last stable state.\n"
      "  RECOVER_STATE   application software shall recover their state from their last run.\n"
      "  SPECIFIED_STATE application software shall retrieve the specified state.\n"
      "startup_mode is a short integer meaningful only to application software.\n"
      "specified_state_id is a string meaningful only if state_retrieval is SPECIFIED_STATE.\n"},
    { APP_RESUME,
      "Resumes the initialisation of a specified application.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_RESUME" application_name\n" },
    { APP_ACKNOWLEDGE, ".\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_ACKNOWLEDGE" application_name\n" },
    { APP_RUN,
      "Requests the application to run.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_RUN" application_name\n" },
    { APP_STOP,
      "Requests the application to stop.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "APP_STOP" application_name [EMERGENCY]\n"
      "\n"
      "EMERGENCY is specified if the system is required to stop with\n"
      "emergency otherwise it is required to stop properly according to\n"
      "the specified stop graph.\n" },


    { PROC_SNAPSHOT,
      "Prints a snapshot of a specified process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_SNAPSHOT" application_name process_name host_name\n" },
    { PROC_ADD_SERVICE,
      "Adds a service to a specified process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_ADD_SERVICE" application_name process_name host_name \\ \n"
      "       \"service_file_name\"\n" },
    { PROC_REMOVE_SERVICE,
      "Removes a service from a specified process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_REMOVE_SERVICE" application_name process_name host_name \\ \n"
      "       service_name\n" },
    { PROC_ADD_ENTITY,
      "Adds an entity to a specified process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_ADD_ENTITY" application_name process_name host_name \\ \n"
      "       \"entity_file_name\"\n" },
    { PROC_REMOVE_ENTITY,
      "Removes an entity from a specified process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_REMOVE_ENTITY" application_name process_name host_name \\ \n"
      "       entity_name\n" },
    { PROC_ADD_PROPERTY,
      "Adds a property to a specified process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_ADD_PROPERTY" application_name process_name host_name \\ \n"
      "       \"new_property_name\"\n" },
    { PROC_REMOVE_PROPERTY,
      "Removes a property from a specified process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_REMOVE_PROPERTY" application_name process_name host_name \\ \n"
      "       property_name\n" },
    { PROC_INITIALISE,
      "Starts the initialisation of a managed process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_INITIALISE" application_name process_name host_name [state_retrieval startup_mode specified_state_id]\n"
      "\n"
      "The following parameters are optional. They are meaningful only to the\n"
      "application software.\n"
      "\n"
      "state_retrieval must be one of the following values:\n"
      "  LAST_STATE      application software shall retrieve their last stable state.\n"
      "  RECOVER_STATE   application software shall recover their state from their last run.\n"
      "  SPECIFIED_STATE application software shall retrieve the specified state.\n"
      "startup_mode is a short integer meaningful only to application software.\n"
      "specified_state_id is a string meaningful only if state_retrieval is SPECIFIED_STATE.\n"},
    { PROC_NEXT_STEP,
      "Requests a managed process to perform its next step.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_NEXT_STEP" application_name process_name host_name\n" },
    { PROC_RUN,
      "Requests a managed process to run.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_RUN" application_name process_name host_name\n" },
    { PROC_STOP,
      "Requests a process to stop properly if it is a managed process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_STOP" application_name process_name host_name [EMERGENCY]\n"
      "\n"
      "EMERGENCY is specified if the system is required to stop with\n"
      "emergency otherwise it is required to stop properly according to\n"
      "the specified stop graph.\n" },
    { PROC_SET_INFO,
      "Changes a process's information.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_SET_INFO" application_name process_name host_name EXEC=\"executable_file\" PORT=process_port \\ \n"
      "       WDIR=\"working_directory\" ARGS=\"process_arguments\" \n" },
    { PROC_SET_AUTOENDING,
      "Requests a process to stop by itself.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_SET_AUTOENDING" application_name process_name host_name\n" },
    { PROC_RESET_AUTOENDING,
      "Requests a process to not stop by itself.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_RESET_AUTOENDING" application_name process_name host_name\n" },
    { PROC_SET_LIFE_CYLE,
      "Changes the life cycle definition of a process.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] ["CHECK_ONLY_OPTION"] ["SYSTEM_CORBALOC_OPTION"=corbaloc] \\ \n"
      "       "PROC_SET_LIFE_CYLE" application_name process_name host_name life_cycle_file\n" },


    { HOST_SHUTDOWN,
      "Shuts the specified host down.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "HOST_SHUTDOWN" host_name [system_port]\n"
      "\n"
      "system_port\n"
      "  Specifies the port of the system that must be shut down.\n"
      "  This argument is optional.\n" },
    { HOST_REBOOT,
      "Reboots the specified host.\n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "HOST_REBOOT" host_name [system_port]\n"
      "\n"
      "system_port\n"
      "  Specifies the port of the system that must be reboot.\n"
      "  This argument is optional.\n" },
	
    { HOST_ADD_PROPERTY,
      "adds a property to a specified Host. \n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "HOST_ADD_PROPERTY" hostname new_property   \n"
    },
     
    { HOST_REMOVE_PROPERTY,
      "removes a property to a specified Host. \n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "HOST_ADD_PROPERTY" hostname new_property \n"
    },
    
    { ENTITY_ADD_PROPERTY,
      "Adds a property to a specified property. \n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "ENTITY_ADD_PROPERTY" entity_name new_property (adds a property to System) \n" 
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "ENTITY_ADD_PROPERTY" entity_name application_name new_property (adds a property to Application) \n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "ENTITY_ADD_PROPERTY" entity_name application_name process_name host_name new_property (adds a property to Process) \n"
    },

    { ENTITY_REMOVE_PROPERTY,
      "removes a property to a specified property. \n"
      "\n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "ENTITY_REMOVE_PROPERTY" entity_name new_property (removes a property to System) \n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "ENTITY_REMOVE_PROPERTY" entity_name application_name host_name new_property (removes a property to Application) \n"
      "Usage: ["LOG_FILE_OPTION"=log_file_name] "ENTITY_REMOVE_PROPERTY" entity_name application_name process_name host_name new_property (removes a property to Process) \n"
    }
};

CmdInfo* CmdUsage::M_cmdInfoTab = _cmdInfoTab;

//----------------------//
// Function definitions //
//----------------------//
string usage( string cmdName )
{
    int nbCmd = sizeof( _cmdInfoTab ) / sizeof( CmdInfo );

    for( int i=0; i<nbCmd; i++ )
    {
        if( cmdName == CmdUsage::M_cmdInfoTab[i].cmdName )
            return CmdUsage::M_cmdInfoTab[i].cmdInfo;
    }

    return CmdUsage::M_cmdUsage;
}
 
