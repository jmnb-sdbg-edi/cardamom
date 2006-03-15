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
#include <sstream>
#include <cctype>
#include <algorithm>

#include "testeventsupport/ClientThread.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"

#include "Foundation/common/System.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "SystemMngt/tools/ProcessAdmin.hpp"

#include "testeventsupport/TestEventChannelFactory.hpp"
#include "testeventsupport/TestTypedEventChannelFactory.hpp"

namespace
{
}; // End anonymous namespace

namespace Cdmw
{
namespace Event
{

//----------------------------------------------------------------------------
ClientThread::ClientThread(CORBA::ORB_ptr orb, const char* nameServicePort, const char* callbackIOR,
                           const char* cdmwIdlPath) 
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_callbackIOR(callbackIOR),
      m_nameServicePort(nameServicePort),
      m_cdmwIdlPath(cdmwIdlPath)
{
}

//----------------------------------------------------------------------------
ClientThread::~ClientThread() throw()
{
}

//----------------------------------------------------------------------------
void ClientThread::run() throw()
{
    using namespace Cdmw::Common;
    using namespace CdmwEvent;
    using namespace Cdmw::OsSupport;

    // init main server of this test: PROC_000
    CdmwPlatformMngt::ProcessDelegate_var proc = CdmwPlatformMngt::ProcessDelegate::_nil();
    std::string proc_url("corbaname::localhost:");
    proc_url += m_nameServicePort;
    proc_url += "#CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_000";
    CORBA::Object_var obj = m_orb->string_to_object(proc_url.c_str());
    
    if (!CORBA::is_nil(obj.in())) 
    {
        proc = CdmwPlatformMngt::ProcessDelegate::_narrow(obj.in());
    }
    else
    {
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
            << " IN FILE: " << __FILE__ << std::endl;
        return;
    }
    
    Cdmw::Tools::ProcessAdmin admin(m_orb.in(),proc.in());
    
    int timescale = Cdmw::TestUtils::Testable::get_timescale();

    // send initialise order
    std::string commands("init LAST 2\n exit \n");
    std::istringstream commands_is(commands);
    admin.run(commands_is,std::cout);
    OS::sleep(timescale*5000);
    
    // send run order
    commands = "run\n exit \n";
    commands_is.str(commands);
    admin.run(commands_is,std::cout);
    OS::sleep(timescale*3000);
  
    //
    // Get NamingInterface object from RepositoryInterface
    //
    Cdmw::CommonSvcs::Naming::NamingInterface ni
        = Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(Locations::CDMW_SERVICES_NAME_DOMAIN);
    
    // I) TEST UNTYPED EVENT CHANNEL FACTORY
    //
    //Get a reference on the EventChannelFactory
    //
    obj = ni.resolve("EVENT/FACTORIES/DefaultEventChannelFactory");
    EventChannelFactory_var factory = EventChannelFactory::_narrow(obj.in());
    
    if (CORBA::is_nil(factory.in())) 
    {
        std::cerr << "defaultEventChannelFactory is a nil object reference" << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        return;
    }

    // Testing TestEventChannelFactory 
    TestEventChannelFactory testFactory(factory.in());
    testFactory.start();
    
    // Starting the EventChannelManager
    std::string profileManagerService="cdmw_event_channel_manager";
#if (CDMW_ORB_VDR == orbacus )
    std::string managerArgs=" -ORBconfig obtest.conf ";        
#else
    std::string managerArgs="";        
#endif
    managerArgs += Options::CALLBACK_IOR_OPTION;
    managerArgs += "=" + m_callbackIOR + " " 
                + Options::PROCESS_PORT_OPTION + "=15327 " 
                + Options::LOCALISATION_SERVICE_OPTION+ "=15328 " 
                + " --ProfileXMLFile=EventChannelConf1.xml " 
                + Options::PROCESS_XML_FILE_OPTION + "=Server.xml "
                    + Options::VALIDATE_XML_OPTION;
    std::string dummy;
    
    std::cout << " launch EventChannelManager " << managerArgs << std::endl;
    OS::ProcessId idProfileManager;
    idProfileManager = OS::create_process(profileManagerService, managerArgs);
    //std::cin >> dummy;
     
    OS::sleep(timescale*10000);
    std::cout << "EventChannelManager started..." << std::endl;
    
    // EventChannelManager Proc URL is CDMW Test Application/PROC_001 while main 
    // of this test is PROC_000
    std::cout << " Getting eventchannelmanager ProcessCallback" << std::endl;
    CdmwPlatformMngt::ProcessDelegate_var proc_ec_mng = CdmwPlatformMngt::ProcessDelegate::_nil();
    proc_url = "corbaname::localhost:" + m_nameServicePort;
    proc_url += "#CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_001";
    obj = m_orb->string_to_object(proc_url.c_str());
    
    if (!CORBA::is_nil(obj.in())) 
    {
       proc_ec_mng  = CdmwPlatformMngt::ProcessDelegate::_narrow(obj.in());
    }
    else
    {
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
            << " IN FILE: " << __FILE__ << std::endl;
        return;
    }
    
    std::cout << " Getting Process admin" << std::endl;
    Cdmw::Tools::ProcessAdmin admin1(m_orb.in(),proc_ec_mng.in());
    
    // send initialise order
    commands = "init LAST 2\n exit \n";
    commands_is.str(commands);
    admin1.run(commands_is,std::cout);
    OS::sleep(timescale*5000);
    
    // send run order
    commands = "run\n exit \n";
    commands_is.str(commands);
    admin1.run(commands_is,std::cout);
    OS::sleep(timescale*3000);
        
    // Testing TestEventChannelFactory continue
    testFactory.next_test();

#if (CDMW_ORB_VDR == orbacus )
    // Starting Eventservice
    OS::ProcessId idEventServer;
    idEventServer = OS::create_process("eventserv", "-ORBconfig obtest.conf");
    OS::sleep(timescale*10000);
    std::cout << "Eventserv started" << std::endl;
#endif

    // Testing TestEventChannelFactory ending
    testFactory.next_test();

    if (!CORBA::is_nil(m_orb.in()))
    {
        try
        {
            // send stop order
            commands = "stop\n exit \n";
            commands_is.str(commands);
            std::cout << "Stop cdmw_event_channel_manager" << std::endl;
            admin1.run(commands_is,std::cout);
            
#if (CDMW_ORB_VDR == orbacus )
            OS::kill_process(idEventServer);
#endif
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }
    }   
     OS::sleep(timescale*5000);

#if (CDMW_ORB_VDR == orbacus )
    // II) TEST TYPED EVENT CHANNEL FACTORY
    //
    // Get a reference on the TypedEventChannelFactory
    //
    obj = ni.resolve("EVENT/FACTORIES/DefaultTypedEventChannelFactory");
    TypedEventChannelFactory_var typedFactory = TypedEventChannelFactory::_narrow(obj.in());
    
    if (CORBA::is_nil(typedFactory.in())) 
    {
        std::cerr << "defaultTypedEventChannelFactory is a nil object reference" << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                  << " IN FILE: " << __FILE__ << std::endl;
        return;
    }

    // Testing TestEventChannelFactory 
    TestTypedEventChannelFactory testTypedFactory(typedFactory.in());
    testTypedFactory.start();
    
    // Starting the EventChannelManager
    idProfileManager
        = OS::create_process(profileManagerService, managerArgs);
    std::cout << " launch EventChannelManager " << managerArgs;
    //std::cin >> dummy;
    OS::sleep(timescale*10000);
    std::cout << "EventChannelManager started" << std::endl;

    proc = CdmwPlatformMngt::ProcessDelegate::_nil();
    // EventChannelManager Proc URL shall be incremented (stoppped, then restarted)
    // and is now CDMW Test Application/PROC_002
    proc_url = "corbaname::localhost:" + m_nameServicePort;
    proc_url += "#CDMW.I/ProcessCallback.simulated/CDMW Test Application/PROC_002";
    obj = m_orb->string_to_object(proc_url.c_str());
    
    if (!CORBA::is_nil(obj.in())) 
    {
        proc = CdmwPlatformMngt::ProcessDelegate::_narrow(obj.in());
    }
    else
    {
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
            << " IN FILE: " << __FILE__ << std::endl;
        return;
    }
    
    Cdmw::Tools::ProcessAdmin admin2(m_orb.in(),proc.in());
    
    // send initialise order
    commands = "init LAST 2\n exit \n";
    commands_is.str(commands);
    admin2.run(commands_is,std::cout);
    OS::sleep(timescale*5000);
    
    // send run order
    commands = "run\n exit \n";
    commands_is.str(commands);
    admin2.run(commands_is,std::cout);
          
    // Testing TestTypedEventChannelFactory continue
    testTypedFactory.next_test();

    // Starting typed Eventservice
    // Launch irserv needed by eventserv for typed event
    std::string irservArgs = "-ORBconfig obtest.conf -I";        
    irservArgs += m_cdmwIdlPath;
    irservArgs += " Echo.idl";
     
    OS::ProcessId idIRServer = OS::create_process("irserv", irservArgs);
    
    idEventServer = OS::create_process("eventserv", "-t -ORBconfig obtest.conf ");
    
    OS::sleep(timescale*10000);
    std::cout << "Eventserv started" << std::endl;


    // Testing TestEventChannelFactory ending
    testTypedFactory.next_test();

    if (!CORBA::is_nil(m_orb.in()))
    {
        try
        {
            // send stop order
            commands = "stop\n exit \n";
            commands_is.str(commands);
            std::cout << "Stop cdmw_event_channel_manager" << std::endl;
            admin2.run(commands_is,std::cout);

            OS::kill_process(idEventServer);
            OS::kill_process(idIRServer);
            
            m_orb -> shutdown(false);
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }
    }   
#else
     if (!CORBA::is_nil(m_orb.in()))
    {
        try
        {
            std::cout << "shutdown called" << std::endl;
            m_orb -> shutdown(false);
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }
    }   

#endif
}

}; // namespace Event
}; // namespace Cdmw


