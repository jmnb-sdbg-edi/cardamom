/* =========================================================================== *
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
 * =========================================================================== */


#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>


#include <Foundation/common/System.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <SystemMngt/platformlibrary/EventHeaderFactory.hpp>
#include <SystemMngt/platformvaluetypes/UserEvent_impl.hpp>

#include "Menu.hpp"


#define ECHO_TRACELIB_TEST if (m_verbose) os

#define PRINT_EXCEPTION(f,l,e,os) \
do { \
    os << "File : " << f << " Line : " << l << "\n";\
    print_exception(e,os);\
} while(0)

#define PRINT_EXCEPTION_MSG(f,l,msg,os) \
do { \
    os << "File : " << f << " Line : " << l << "\n";\
    os << msg << "\n";\
    CDMW_ASSERT(0); \
} while(0)

const char* INFO_MESSAGE = "Type h or help for a list of available commands.";
const char* PROMPT_MESSAGE = "\n%>";
const char* UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

const CORBA::Long EXIT_ORDERED(-10);
const CORBA::Long OP_SUCCESS = 0;
const CORBA::Long OP_FAILURE = -1;
 

void print_exception(const CORBA::SystemException & ex, std::ostream & os)
{
    os << "CORBA system Exception : \n" << ex << std::endl;
}

void print_exception(const CORBA::Exception & ex, std::ostream & os)
{
    os << "CORBA Exception : \n" << ex << std::endl;
}

void print_exception(const Cdmw::Exception & ex, std::ostream & os)
{
    os << "CARDAMOM Exception : \n" << ex.what() << std::endl;
}


bool not_space(char c)
{
    return !isspace(c);
};


//----------------------------------------------------------------------------
Menu::Menu(
    CORBA::ORB_ptr orb,
    std::istream & is,
    std::ostream & os )
: m_orb( CORBA::ORB::_duplicate( orb ) ),
  m_verbose( false ),
  m_istream( is ),
  m_ostream( os )
{
    // Get the CORBA reference of the system of the CARDAMOM Supervision
    CORBA::Object_var system_obj
        = m_orb->string_to_object(
            "corbaloc::localhost:21871/CdmwPlatformMngtSupervision" );

    m_system = CdmwPlatformMngt::System::_narrow( system_obj.in() );
    if( CORBA::is_nil( m_system.in() ) )
        CDMW_ASSERT( 0 );

    // Get the CORBA reference of the application
    m_application = m_system->get_application( "A1" );

}

//----------------------------------------------------------------------------
Menu::~Menu()
throw()
{
}

//----------------------------------------------------------------------------
void Menu::run()
throw()
{
    run (m_istream,m_ostream);
}

//----------------------------------------------------------------------------
// List of commands must be declared in the alphabetic order
const Menu::command_def Menu::commands[] =
{
    { "exit"               ,&Menu::exit               },
    { "h"                  ,&Menu::print_help         },
    { "help"               ,&Menu::print_help         },
    { "initialise_process" ,&Menu::initialise_process },
    { "run_process"        ,&Menu::run_process        },
    { "snapshot"           ,&Menu::snapshot_system    },
    { "start_system"       ,&Menu::start_system       },
    { "stop_process"       ,&Menu::stop_process       },
    { "stop_system"        ,&Menu::stop_system        },
    { "v"                  ,&Menu::toggle_verbose     },
    { "verbose"            ,&Menu::toggle_verbose     },
    { "x"                  ,&Menu::exit               }
};

const CORBA::ULong Menu::nb_commands =
    sizeof( Menu::commands ) / sizeof( Menu::command_def );

 
//----------------------------------------------------------------------------
CORBA::Long Menu::runOperation(
    const std::string &op,
    const std::string &arg,
    std::ostream &os)
{
    CORBA::Long l = 0;
    CORBA::Long r = (CORBA::Long) nb_commands;
    CORBA::Long m;
    CORBA::Long res;
 
    while(l < r)
    {
        m = (l + r) / 2;
        res = strcmp(commands[m].name, op.c_str());

        if(res == 0)
        {
            // run command
            return (this->*commands[m].op)(arg,os);
        }

        if(res > 0)
        {
            r = m;
        }
        else
        {
            l = m + 1;
        }
    }

    if(op != "") unknown_command(os);
 
    return OP_FAILURE;
}

//----------------------------------------------------------------------------
void Menu::print_info(std::ostream& os)
{
    os << "\n\n"
       << "\t\t--------------------------------------------------\n"
       << "\t\t  Cardamom Platform Supervision Management Tutorial  \n"
       << "\t\t--------------------------------------------------\n"
       << "\n\n"
       << INFO_MESSAGE
       << "\nInfo: (verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
}

//----------------------------------------------------------------------------
void Menu::print_prompt(std::ostream& os)
{
    os << PROMPT_MESSAGE << std::flush;
}

//----------------------------------------------------------------------------
void Menu::unknown_command( std::ostream & os)
{
    os << UNKNOWN_COMMAND_MESSAGE << std::endl;
}

//----------------------------------------------------------------------------
void Menu::run(std::istream& is,std::ostream& os)
{
    print_info(os);
    print_prompt(os);

    std::string s;

    while(std::getline(is,s))
	{
        // ===============
        // Extract command
        // ===============
        // skip leading whitespaces
        std::string::const_iterator i = s.begin();
        std::string::const_iterator end = s.end();
        i = std::find_if(i,end,not_space);
        // find end of command
        std::string::const_iterator j = std::find_if(i,end,isspace);
        // copy the characters in [i,j)
        std::string command;
        if (i != end)
            command = std::string(i,j);

        // =================
        // Extract arguments
        // =================
        // skip leading whitespaces
        i = std::find_if(j,end,not_space);
        std::string arg;
        // copy arguments
        if (i != end)
        {
          arg = std::string(i,end);
        }
 
		ECHO_TRACELIB_TEST << "@cmd = <" << command << "> @args = <" << arg << ">" << std::endl;


        if (runOperation(command,arg,os) == EXIT_ORDERED) break;
        print_prompt(os);
    }
}

//----------------------------------------------------------------------------
CORBA::Long Menu::exit(const std::string & arg, std::ostream & os)
{
    try
   	{
        if (m_verbose) os << "Initiating ORB shutdown." << std::endl;
        m_orb->shutdown(false);
    }
	catch (const CORBA::SystemException & ex)
	{
        PRINT_EXCEPTION(__FILE__,__LINE__,ex,os);
    }
    return EXIT_ORDERED;
}

//----------------------------------------------------------------------------
CORBA::Long Menu::toggle_verbose(const std::string & arg, std::ostream & os)
{
    m_verbose = !m_verbose;
    os << "(verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
// Available commands:
//
CORBA::Long Menu::print_help(const std::string & arg, std::ostream & os)
{
    os << " +===================================+========================================+" << "\n"
       << " | COMMAND                           | DESCRIPTION                            |" << "\n"
       << " +===================================+========================================+" << "\n"
       << " | x | exit                          | exit Menu mode                         |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n"
       << " | h | help                          | print available commands               |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n" 
       << " | v | verbose                       | toggle verbose mode on/off             |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n"
	   << " | snapshot                          | print the snapshot of the system       |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n"
	   << " | start_system                      | start the system                       |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n"
	   << " | stop_system                       | stop the system                        |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n"
	   << " | initialise_process <process_name> | initialise a process                   |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n"
	   << " | run_process <process_name>        | run a process                          |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n"
	   << " | stop_process <process_name>       | stop a process                         |" << "\n"
       << " +-----------------------------------+----------------------------------------+" << "\n"
       << std::endl;

     return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long Menu::snapshot_system(const std::string &arg, std::ostream &os)
{
    // Get the snapshot of the system
    CdmwPlatformMngt::SystemSnapshot_var snapshot
        = m_system->get_snapshot();

    // Get the the CORBA reference of the observer from its CORBALOC
    CORBA::Object_var observer_obj = m_orb->string_to_object(
        "corbaloc::localhost:21872/user_observer" );

    CdmwPlatformMngt::SupervisionObserver_var observer
        = CdmwPlatformMngt::SupervisionObserver::_narrow(
            observer_obj.in() );
    if( CORBA::is_nil( observer.in() ) )
    {
        std::cout
            << "the observer could not be contacted"
            << std::endl;

        return OP_FAILURE;
    }

    // Put the snapshot in a CORBA::Any in order to be placed in a user event
    CORBA::Any data;
    data <<= snapshot.in();

    // Create the user event with the snapshot of the system
    CdmwPlatformMngtBase::EventHeader header
        = Cdmw::PlatformMngt::EventHeaderFactory::createHeader(
            CdmwPlatformMngtBase::INF );
    Cdmw::PlatformMngt::UserEventFactory factory;
    CdmwPlatformMngt::UserEvent* event = factory.create(
        header, "SystemName", "ApplicationName", "ProcessName",
        "issuer", data ) ;

    // Directly notify the observer with out passing by the supervision
    observer->notify( event );

    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long Menu::start_system(const std::string &arg, std::ostream &os)
{
    // Request the system to start
    CdmwPlatformMngtBase::StartupKind_var startupKind
        = new CdmwPlatformMngtBase::StartupKind();

    startupKind->startup_mode = 0;
    startupKind->state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
    startupKind->specified_state_id = CORBA::string_dup( "" );

    try
    {
        m_system->start( startupKind.in() );
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        std::cout
            << "operation not available in the current status of the system"
            << std::endl;

        return OP_FAILURE;
    }

    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long Menu::stop_system(const std::string &arg, std::ostream &os)
{
    // Request the system to stop
    CORBA::Boolean emergency = false;

    m_system->stop( emergency );

    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long Menu::initialise_process(const std::string &arg, std::ostream &os)
{
    // Get the CORBA reference of the specified process
    CdmwPlatformMngt::ProcessProxy_var process;

    try
    {
        process = m_application->get_process( arg.c_str() );
    }
    catch( const CdmwPlatformMngt::ProcessNotFound& e )
    {
        std::cout
            << arg.c_str() << " -> "
            << "Unknown process"
            << std::endl;

        return OP_FAILURE;
    }

    // Request the process to perform its initialisation step
    CdmwPlatformMngtBase::StartupKind_var startupKind
        = new CdmwPlatformMngtBase::StartupKind();

    startupKind->startup_mode = 0;
    startupKind->state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
    startupKind->specified_state_id = CORBA::string_dup( "" );

    try
    {
        process->initialise( startupKind.in() );
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        std::cout
            << "Initialisation of the process '"
            << arg.c_str()
            << "' is not allowed in its current status"
            << std::endl;

        return OP_FAILURE;
    }

    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long Menu::run_process(const std::string &arg, std::ostream &os)
{
    // Get the CORBA reference of the specified process
    CdmwPlatformMngt::ProcessProxy_var process;

    try
    {
        process = m_application->get_process( arg.c_str() );
    }
    catch( const CdmwPlatformMngt::ProcessNotFound& e )
    {
        std::cout
            << arg.c_str() << " -> "
            << "Unknown process"
            << std::endl;

        return OP_FAILURE;
    }

    // Request the process to run
    try
    {
        process->run();
    }
    catch( const CdmwPlatformMngt::IncompatibleStatus& e )
    {
        std::cout
            << "Run of the process '"
            << arg.c_str()
            << "' is not allowed in its current status"
            << std::endl;

        return OP_FAILURE;
    }

    return OP_SUCCESS;
}

//----------------------------------------------------------------------------
CORBA::Long Menu::stop_process(const std::string &arg, std::ostream &os)
{
    // Get the CORBA reference of the specified process
    CdmwPlatformMngt::ProcessProxy_var process;

    try
    {
        process = m_application->get_process( arg.c_str() );
    }
    catch( const CdmwPlatformMngt::ProcessNotFound& e )
    {
        std::cout
            << arg.c_str() << " -> "
            << "Unknown process"
            << std::endl;

        return OP_FAILURE;
    }

    // Request the process to stop
    CORBA::Boolean emergency = false;

    process->stop( emergency );

    return OP_SUCCESS;
}

