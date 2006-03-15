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


#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <TestThread.hpp>

#define ECHO_HEADER() \
    "[Debug] "

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)
#define ENABLE_LB_DEBUG_DUMP 

#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment ;\
} while(0)

#endif


CcmLbTest::TestThread::TestThread()
{
    m_ctx = CcmLbTest::CCM_Client_Context::_nil();
}

CcmLbTest::TestThread::~TestThread() throw()
{
}
    
void 
CcmLbTest::TestThread::run() throw()
{
    DEBUG_DUMP("   ***DEMO STARTED***   ");    
    CORBA::String_var temp = CORBA::string_dup("");
    
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	DEBUG_DUMP("the process called is ==> " << process_name.in());
	DEBUG_DUMP("REQUEST " << i << "...check the RoundRobin policy...") ;
	if((strcmp(temp, process_name) == 0) && (!(i==5)))
	    DEBUG_DUMP(".....NOT OK!");
	else
	{
	    temp = process_name;
	    DEBUG_DUMP(".....OK!");
	}
    }
    
    std::string dummy;
    DEBUG_DUMP("Test the crash of a replica...");
    DEBUG_DUMP("kill process ServerProcess11 ==> Start the script command_proc1_stop.sh");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	DEBUG_DUMP("the process called is ==> " << process_name.in());
	DEBUG_DUMP("REQUEST " << i << "...check the RoundRobin policy...") ;
	if((strcmp(temp, process_name) == 0) && (!(i==5)) || (strcmp(process_name, "ServerProcess11") == 0))
	    DEBUG_DUMP(".....NOT OK!");
	else
	{
	    temp = process_name;
	    DEBUG_DUMP(".....OK!");
	}	   
    }
    
    DEBUG_DUMP("Test the crash of an application...");
    DEBUG_DUMP("kill the application CcmLbApplication1 ==> Start the scipt command_app_stop.sh");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	DEBUG_DUMP("the process called is ==> " << process_name.in());
	DEBUG_DUMP("REQUEST " << i << "...check the RoundRobin policy...") ;
	if((strcmp(temp, process_name) == 0) && (!(i==5)) || (strcmp(process_name, "ServerProcess12") == 0))
	    DEBUG_DUMP(".....NOT OK!");
	else
	{
	    temp = process_name;
	    DEBUG_DUMP(".....OK!");
	}
    }
    DEBUG_DUMP("Test the crash of an host...");
    DEBUG_DUMP("kill the HOST3 ==> type ^C in the daemon term on HOST3");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	DEBUG_DUMP("the process called is ==> " << process_name.in());
	DEBUG_DUMP("REQUEST " << i << "...check the RoundRobin policy...") ;
	if((strcmp(temp, process_name) == 0) && (!(i==5)) 
	   || (strcmp(process_name, "ServerProcess31") == 0) 
	   || (strcmp(process_name, "ServerProcess32") == 0))
	    DEBUG_DUMP(".....NOT OK!");
	
	else
	{
	    temp = process_name;
	    DEBUG_DUMP(".....OK!");
	    
	}
    }
    
    DEBUG_DUMP("Test the LoadBalancing Service with one replica alive in the component group...");
    DEBUG_DUMP("kill the ServerProcess21 ==> Start the script command_proc2_stop.sh");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    temp = CORBA::string_dup("");
    // perform 10 request with the RoundRobin policy
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	DEBUG_DUMP("the process called is ==> " << process_name.in());
	DEBUG_DUMP("REQUEST " << i << "...check the RoundRobin policy...there is only ServerProcess22 alive") ;
	if(strcmp(process_name, "ServerProcess22"))
	    DEBUG_DUMP(".....NOT OK!");
	else
	{
	    DEBUG_DUMP(".....OK!");
	}
    }
    DEBUG_DUMP("kill the Group Manager Supervision Observer ==> type ^C in the LBGroupManagerSupervisionObserver term");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin>>dummy;
    Cdmw::OsSupport::OS::sleep( 2000 );
    for (int i = 0; i<10; i++)
    {
	CORBA::String_var process_name =
	    CORBA::string_dup(test_connections());
	DEBUG_DUMP("the process called is ==> " << process_name.in());
	DEBUG_DUMP("REQUEST " << i << "...check the RoundRobin policy...there is only ServerProcess22 alive") ;
	if(strcmp(process_name, "ServerProcess22"))
	    DEBUG_DUMP(".....NOT OK!");
	else
	{
	    DEBUG_DUMP(".....OK!");
	}
    }

    DEBUG_DUMP("   ***DEMO STOPPED***   ");    
}
void
CcmLbTest::TestThread::set_context(::CcmLbTest::CCM_Client_Context_ptr ctx)
    throw(CORBA::SystemException)
{
    m_ctx = ::CcmLbTest::CCM_Client_Context::_duplicate(ctx);
};



char* 
CcmLbTest::TestThread::test_connections(void)
{
    CORBA::String_var process_name = CORBA::string_dup("");
    
    try {
	DEBUG_DUMP("Testing connections to ServerWithFacet group");
	
	// get the server HostInfo
	CcmLbTest::HostInfo_var hostInfo =
	    m_ctx->get_connection_serverHostInfo();
	
	if (CORBA::is_nil(hostInfo.in())) {
	    DEBUG_DUMP("error: hostInfo is NIL!");
	    return 0;
	}
	
	// read the hostname from the server HostInfo
	process_name = hostInfo->get_process_info();
	
	if (strcmp(process_name.in(), "") == 0) {
	    DEBUG_DUMP("error: hostname is empty string");
	} 
    } catch (const CORBA::SystemException& e) {
	    DEBUG_DUMP("Unexpected CORBA::SystemException: " << e);
    } catch (const CORBA::Exception& e) {
	DEBUG_DUMP("Unexpected CORBA::Exception: " << e);
    } catch (...) {
	DEBUG_DUMP("Unknown exception!");
    }
    
    return CORBA::string_dup(process_name);
    
}


