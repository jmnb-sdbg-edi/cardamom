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


#include <iostream>
#include <string>
#include <sstream>
#include <sax/SAXParseException.hpp>
#include <util/PlatformUtils.hpp>

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "platformadmin/AdminLogMngr.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"

#include "platformadmin/ConfigElementParser.hpp"
#include "platformadmin/System.hpp"
#include "platformadmin/Application.hpp"
#include "platformadmin/Process.hpp"
#include "platformadmin/Host.hpp"
#include "platformadmin/AdminLogMessageId.hpp"

#include "platformadmin/PlatformAdmin.hpp"
#include "platformadmin/PlatformAdminUsage.hpp"
#include "platformadmin/PlatformAdminSnapshot.hpp"

using namespace std;
using namespace Cdmw;
using namespace PlatformAdmin;
using namespace OsSupport;

//------------------
// Global variables 
//------------------
bool check_only = false;


//---------------------//
// Function prototypes //
//---------------------//
int getCmdPos( int argc, char** );

void performCmd( CORBA::ORB_ptr orb, int, char** );
void performSysCmd( System*, string, int, char** );
void performAppCmd( Application*, string, int, char** );
void performProcCmd( Process*, string, int, char** );
void performHostCmd( CORBA::ORB_ptr, string, int, char** );

CdmwPlatformMngtBase::StartupKind* getStartupKind( int, char** );

CdmwPlatformMngt::Daemon_ptr getDaemon(
    CORBA::ORB_ptr,
    const char*,
    const CdmwPlatformMngt::Port )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::HostNotReachable,
       CORBA::SystemException );

//-----------------//
// Function bodies //
//-----------------//
int getCmdPos( int argc, char** argv )
{
    for( int argIndex = argc-1; argIndex > -1; argIndex-- )
    {
        string arg = argv[ argIndex ];

        if( arg.find( "--" ) == 0 )
            return argIndex;
    }

    return -1;
}

void performCmd( CORBA::ORB_ptr orb, int argc, char** argv )
{
    int    cmdPos  = getCmdPos( argc, argv );
    string cmdName = argv[ cmdPos ];

    // Look for the existence of the command
    if( usage( cmdName ) == CmdUsage::M_cmdUsage )
        throw CmdUsageException( cmdName );

    // Perform a host command
    if( cmdName.find( "--host-" ) != string::npos )
    {
        performHostCmd(
            orb,
            cmdName,
            argc - (cmdPos + 1),
            &argv[ cmdPos + 1 ] );
    }
    // Perform an other command
    else
    {
        // Get the system specified with the corbaloc
        CdmwPlatformMngt::System_var system;

        if( !check_only )
        {
            if( OS::get_option_value( argc, argv, SYSTEM_CORBALOC_OPTION ) == "no" )
                throw CmdUsageException( cmdName );

            string system_corbaloc
                = OS::get_option_value( argc, argv, SYSTEM_CORBALOC_OPTION );

            CORBA::Object_var obj_system
                = orb->string_to_object( system_corbaloc.c_str() );
            if( CORBA::is_nil( obj_system.in() ) )
                CDMW_THROW2(
                    Cdmw::Exception,
                    Cdmw::Exception::FATAL,
                    "Invalid system reference" );

            system = CdmwPlatformMngt::System::_narrow( obj_system.in() );
            if( CORBA::is_nil( system.in() ) )
                CDMW_THROW2(
                    Cdmw::Exception,
                    Cdmw::Exception::FATAL,
                    "Invalid system" );
        }

        System admSystem( system.in() );

        // Perform a system command
        if( cmdName.find( "--sys-" ) != string::npos )
        {
            performSysCmd(
                &admSystem,
                cmdName,
                argc - (cmdPos + 1),
                &argv[ cmdPos + 1 ] );
        }
        // Perform an application command
        else if( cmdName.find( "--app-" ) != string::npos )
        {
            if( (cmdPos+1) >= argc )
                throw CmdUsageException( cmdName );

            string applicationName = argv[ cmdPos + 1 ];
            CdmwPlatformMngt::Application_var application;

            if( !check_only )
                application = system->get_application( applicationName.c_str() );

            Application admApplication( application.in() );

            performAppCmd(
                &admApplication,
                cmdName,
                argc - (cmdPos + 2),
                &argv[ cmdPos + 2 ] );
        }
        // Perform a process command
        else if( cmdName.find( "--proc-" ) != string::npos )
        {
            if( (cmdPos+2) >= argc )
                throw CmdUsageException( cmdName );

            string applicationName = argv[ cmdPos + 1 ];
            string processName = argv[ cmdPos + 2 ];
            CdmwPlatformMngt::Application_var application;
            CdmwPlatformMngt::ProcessProxy_var process;

            if( !check_only )
            {
                application = system->get_application( applicationName.c_str() );
                process = application->get_process( processName.c_str() );
            }

            Process admProcess( process.in() );

            performProcCmd(
                &admProcess,
                cmdName,
                argc - (cmdPos + 3),
                &argv[ cmdPos + 3 ] );
        }
        // Not existing command
        else throw CmdUsageException( cmdName );
    }
}

void performSysCmd(
    System* admSystem,
    string cmdName,
    int paramCnt,
    char** param )
{
    if( cmdName == SYS_SNAPSHOT )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        cout << snapShotToString( admSystem->get_snapshot() ).c_str();
    }
    else if( cmdName == SYS_DEFINE )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->define( param[0] );
    }
    else if( cmdName == SYS_ADD_HOST )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->add_host( param[0] );
    }
    else if( cmdName == SYS_REMOVE_HOST )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->remove_host( param[0] );
    }
    else if( cmdName == SYS_ADD_APPLICATION )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->add_application( param[0] );
    }
    else if( cmdName == SYS_REMOVE_APPLICATION )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->remove_application( param[0] );
    }
    else if( cmdName == SYS_ADD_ENTITY )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->add_entity( param[0] );
    }
    else if( cmdName == SYS_REMOVE_ENTITY )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->remove_entity( param [0] );
    }
    else if( cmdName == SYS_ADD_SERVICE )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->add_service( param[0] );
    }
    else if( cmdName == SYS_REMOVE_SERVICE )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->remove_service( param[0] );
    }
    else if( cmdName == SYS_SET_INIT_GRAPH )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->set_init_graph( param[0] );
    }
    else if( cmdName == SYS_SET_STOP_GRAPH )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admSystem->set_stop_graph( param[0] );
    }
    else if( cmdName == SYS_START )
    {
        if( (paramCnt != 0) && (paramCnt != 3) )
            throw CmdUsageException( cmdName );

        CdmwPlatformMngtBase::StartupKind_var startupKind
            = getStartupKind( paramCnt, param );

        admSystem->start( startupKind.in() );
    }
    else if( cmdName == SYS_RESUME )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        admSystem->resume();
    }
    else if( cmdName == SYS_ACKNOWLEDGE )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        admSystem->acknowledge();
    }
    else if( cmdName == SYS_STOP )
    {
        CORBA::Boolean emergencyStop;

        if( paramCnt == 0)
            emergencyStop = false;
        else if( (paramCnt == 1) && (string( param[0] ) == "EMERGENCY") )
            emergencyStop = true;
        else
            throw CmdUsageException( cmdName );

        admSystem->stop( emergencyStop );
    }
    else throw CmdUsageException( cmdName );
}

void performAppCmd(
    Application* admApplication,
    string cmdName,
    int paramCnt,
    char** param )
{
    if( cmdName == APP_SNAPSHOT )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        cout << snapShotToString( admApplication->get_snapshot(), "" ).c_str();
    }
    else if( cmdName == APP_ADD_PROCESS )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admApplication->add_process( param[0] );
    }
    else if( cmdName == APP_REMOVE_PROCESS )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admApplication->remove_process( param[0] );
    }
    else if( cmdName == APP_ADD_SERVICE )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admApplication->add_service( param[0] );
    }
    else if( cmdName == APP_REMOVE_SERVICE )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admApplication->remove_service( param[0] );
    }
    else if( cmdName == APP_ADD_ENTITY )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admApplication->add_entity( param[0] );
    }
    else if( cmdName == APP_REMOVE_ENTITY )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admApplication->remove_entity( param [0] );
    }
    else if( cmdName == APP_SET_INIT_GRAPH )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admApplication->set_init_graph( param[0] );
    }
    else if( cmdName == APP_SET_STOP_GRAPH )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admApplication->set_stop_graph( param[0] );
    }
    else if( cmdName == APP_INITIALISE )
    {
        if( (paramCnt != 0) && (paramCnt != 3) )
            throw CmdUsageException( cmdName );

        CdmwPlatformMngtBase::StartupKind_var startupKind
            = getStartupKind( paramCnt, param );

        admApplication->initialise( startupKind.in() );
    }
    else if( cmdName == APP_RESUME )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        admApplication->resume();
    }
    else if( cmdName == APP_ACKNOWLEDGE )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        admApplication->acknowledge();
    }
    else if( cmdName == APP_RUN )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        admApplication->run();
    }
    else if( cmdName == APP_STOP )
    {
        CORBA::Boolean emergencyStop;

        if( paramCnt == 0)
            emergencyStop = false;
        else if( (paramCnt == 1) && (string( param[0] ) == "EMERGENCY") )
            emergencyStop = true;
        else
            throw CmdUsageException( cmdName );

        admApplication->stop( emergencyStop );
    }
    else throw CmdUsageException( cmdName );
}

void performProcCmd(
    Process* admProcess,
    string cmdName,
    int paramCnt,
    char** param )
{
    if( cmdName == PROC_SNAPSHOT )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        cout << snapShotToString( admProcess->get_snapshot(), "" ).c_str();
    }
    else if( cmdName == PROC_ADD_SERVICE )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admProcess->add_service( param[0] );
    }
    else if( cmdName == PROC_REMOVE_SERVICE )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admProcess->remove_service( param[0] );
    }
    else if( cmdName == PROC_ADD_ENTITY )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admProcess->add_entity( param[0] );
    }
    else if( cmdName == PROC_REMOVE_ENTITY )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admProcess->remove_entity( param [0] );
    }
    else if( cmdName == PROC_INITIALISE )
    {
        if( (paramCnt != 0) && (paramCnt != 3) )
            throw CmdUsageException( cmdName );

        CdmwPlatformMngtBase::StartupKind_var startupKind
            = getStartupKind( paramCnt, param );

        admProcess->initialise( startupKind.in() );
    }
    else if( cmdName == PROC_NEXT_STEP )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        admProcess->next_step();
    }
    else if( cmdName == PROC_RUN )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        admProcess->run();
    }
    else if( cmdName == PROC_STOP )
    {
        CORBA::Boolean emergencyStop;

        if( paramCnt == 0)
            emergencyStop = false;
        else if( (paramCnt == 1) && (string( param[0] ) == "EMERGENCY") )
            emergencyStop = true;
        else
            throw CmdUsageException( cmdName );

        admProcess->stop( emergencyStop );
    }
    else if( cmdName == PROC_SET_INFO )
    {
        if( paramCnt < 2 || paramCnt > 4) throw CmdUsageException( cmdName );

        typedef char* char_ptr;
        char_ptr paramTmp[5];
        
        paramTmp[0] = (char*)"";
        
        for (int i=0 ; i < 4 ; i++)
        {
            if (i < paramCnt) 
            {
                paramTmp[i+1] = param[i];
            }
            else
            {
                paramTmp[i+1] = (char*)"";
            } 
        }

        string exec = OS::get_option_value( 5, paramTmp, "EXEC" );
        string port = OS::get_option_value( 5, paramTmp, "PORT" );
        string wdir = OS::get_option_value( 5, paramTmp, "WDIR" );
        string args = OS::get_option_value( 5, paramTmp, "ARGS" );
        
        if (exec == "no" || exec == "yes")
        {
            throw CmdUsageException( cmdName );
        }
        
        int portNbr;
        if (port != "no") 
        {	        
	        std::istringstream portNbr_strm(port);
	        portNbr_strm >> portNbr;
	        if (portNbr_strm.fail()) 
	        {
	          throw CmdUsageException( cmdName );
	        }
	    }
        
        if (wdir == "no" || wdir == "yes")
        {
            wdir = "";
        }
        
        if (args == "no" || args == "yes")
        {
            args = "";
        }

        admProcess->set_info(
            exec.c_str(),
            portNbr,
            wdir.c_str(),
            args.c_str() );
    }
    else if( cmdName == PROC_SET_AUTOENDING )
    {
        if( paramCnt != 0 ) throw CmdUsageException( cmdName );
        admProcess->set_autoending();
    }
    else if( cmdName == PROC_SET_HOST )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admProcess->set_host( param[0] );
    }
    else if( cmdName == PROC_SET_LIFE_CYLE )
    {
        if( paramCnt != 1 ) throw CmdUsageException( cmdName );
        admProcess->set_life_cycle( param[0] );
    }
    else throw CmdUsageException( cmdName );
}


CdmwPlatformMngtBase::StartupKind* getStartupKind( int paramCnt, char** param )
{
    CdmwPlatformMngtBase::StateRetrieval state_retrieval
        = CdmwPlatformMngtBase::LAST_STATE;
    unsigned long startup_mode = 0;
    string specified_state_id = "";

    if( paramCnt == 3 )
    {
        // Get the retrieval state
        if( string( param[0] ) == "LAST_STATE" )
            state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
        else if( string( param[0] ) == "RECOVER_STATE" )
            state_retrieval = CdmwPlatformMngtBase::RECOVER_STATE;
        else if( string( param[0] ) == "SPECIFIED_STATE" )
            state_retrieval = CdmwPlatformMngtBase::SPECIFIED_STATE;
        else
            throw ;

        // Get the startup mode
        startup_mode = atol( param[ 1 ] );

        // Get the state id
        specified_state_id = param[ 2 ];
    }

    CdmwPlatformMngtBase::StartupKind_var startupKind
        = new CdmwPlatformMngtBase::StartupKind();

    startupKind->state_retrieval = state_retrieval;
    startupKind->startup_mode = startup_mode;
    startupKind->specified_state_id
        = CORBA::string_dup( specified_state_id.c_str() );

    return startupKind._retn();
}


void performHostCmd(
    CORBA::ORB_ptr orb,
    string cmdName,
    int paramCnt,
    char** param )
{
    // Get the parameters
    CdmwPlatformMngt::Port systemPort = -1;
    string hostName;

    if( paramCnt == 1)
    {
        hostName = param[0];
        systemPort = -1;
    }
    else if( paramCnt == 2 )
    {
        hostName = param[0];
        systemPort = ::atoi( param[1] );
    }
    else
        throw CmdUsageException( cmdName );

    if( check_only )
        return;

    // Get the daemon reference
    CdmwPlatformMngt::Daemon_var daemon
        = getDaemon( orb, hostName.c_str(), systemPort );

    // Perform the command on the daemon
    if( cmdName == HOST_REBOOT )
    {
        daemon->reboot();
    }
    else if( cmdName == HOST_SHUTDOWN )
    {
        daemon->shutdown();
    }
    else throw CmdUsageException( cmdName );
}


CdmwPlatformMngt::Daemon_ptr getDaemon(
    CORBA::ORB_ptr orb,
    const char* hostName,
    const CdmwPlatformMngt::Port systemPort )
throw( CdmwPlatformMngt::HostNotFound,
       CdmwPlatformMngt::HostNotReachable,
       CORBA::SystemException )
{
    try
    {

        CdmwPlatformMngt::Port daemonPort; 

        if( systemPort < 0 )
        {
            daemonPort = CdmwPlatformMngt::DEFAULT_SYSTEM_PORT;
        }
        else
        {
            daemonPort = systemPort;
        }

        std::ostringstream daemonCorbaloc;

        daemonCorbaloc 
            << "corbaloc::" << hostName
            <<  ":" << daemonPort
            <<  "/" << CdmwPlatformMngt::DAEMON_NAME;

        CORBA::Object_var obj =
            orb->string_to_object( daemonCorbaloc.str().c_str() );
            
        if( CORBA::is_nil( obj.in() ) )
        {
            throw CdmwPlatformMngt::HostNotFound();
        }
        
        CdmwPlatformMngt::Daemon_var daemon =
            CdmwPlatformMngt::Daemon::_narrow( obj.in() );
            
        if( CORBA::is_nil( daemon.in() ) )
        {
            throw CdmwPlatformMngt::HostNotFound();
        }

        return daemon._retn();
    }
    catch( const CORBA::SystemException& )
    {
        throw CdmwPlatformMngt::HostNotReachable();
    }
}


//----------------------------------------//
// Main loop (entry point of the process) //
//----------------------------------------//
int main( int argc, char* argv[] )
{
    CORBA::ORB_var orb;
    int ret_code = 0;

    XMLPlatformUtils::Initialize();
    
    AdminLogMngr::initialise();

    try
    {
        // Open the log file
        string logFile = OS::get_option_value( argc, argv, LOG_FILE_OPTION );
        if( logFile != "no" )
        {
            PlatformMngt::LogMngr::logMessage(
                PlatformMngt::INF,
                MSG_ID_ADMIN_LOG_FILE,
                logFile );
            PlatformMngt::LogMngr::open( logFile );
        }

        if( OS::get_option_value( argc, argv, CHECK_ONLY_OPTION ) == "yes" )
            check_only = true;

        // Log the command line
        ostringstream message;

        message << "Requested command \"";
        for( int i=1; i<argc; i++ )
            message << argv[i] << " ";
        message << "\"";

        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::INF,
            message.str().c_str() );

        // Initialize the ORB
        if( !check_only )
            orb = CORBA::ORB_init( argc, argv );

        // Perform the command
        performCmd( orb.in(), argc, argv );

        if( check_only )
        {
            PlatformMngt::LogMngr::logMessage(
                PlatformMngt::INF,
                "Check successful" );
        }
    }
    catch( const LoggedException& )
    {
        ret_code = -1;
    }
    catch( const CmdUsageException& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_COMMAND_USAGE );

        cout << usage( e.m_cmdName ) << endl;
        ret_code = -1;
    }
    catch( const SAXParseException& e )
    {
        ostringstream message;
        
        message << to_string( e.getMessage() ) << endl;
        message << "in file: " << to_string( e.getSystemId() ) << endl;
        message << "   line: " << e.getLineNumber() << endl;
        message << "   char: " << e.getColumnNumber() << endl;

        PlatformMngt::LogMngr::logMessage( PlatformMngt::ERR, message.str().c_str() );
        ret_code = -1;
    }
    catch( const Cdmw::Exception& e)
    {
        PlatformMngt::LogMngr::logMessage( PlatformMngt::ERR, e.what() );
        ret_code = -1;
    }
    catch( const CORBA::Exception& e )
    {
        PlatformMngt::LogMngr::logMessage( PlatformMngt::ERR, e._name() );
        ret_code = -1;
    }
    catch( ... )
    {
        PlatformMngt::LogMngr::logMessage( PlatformMngt::ERR, MSG_ID_ADMIN_UNKNOWN_ERROR );
        ret_code = -1;
    }

    PlatformMngt::LogMngr::destroy();

    XMLPlatformUtils::Terminate();

    if( !CORBA::is_nil( orb.in() ) )
      {
	  orb->destroy();
      }
  
    return ret_code;
}

