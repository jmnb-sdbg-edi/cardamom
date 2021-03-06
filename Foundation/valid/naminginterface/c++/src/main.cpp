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


#include <iostream>
#include <cstdlib>
#include <string>

#include "Foundation/common/System.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/testutils/Testable.hpp"


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

#include "test_lib/TestObject_impl.hpp"
#include "test_lib/TestNameParserToString.hpp"
#include "test_lib/TestNameParserToName.hpp"
#include "test_lib/TestNamingInterfaceResolve.hpp"
#include "test_lib/TestNamingInterfaceBindContext.hpp"
#include "test_lib/TestNamingInterfaceBind.hpp"
#include "test_lib/TestNamingInterfaceUnbind.hpp"
#include "test_lib/TestNamingInterfaceList.hpp"
#include "test_lib/TestNamingInterfaceDestroy.hpp"
#include "test_lib/TestMacroDefs.hpp"

#include "Foundation/common/Options.hpp"

#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"

using namespace Cdmw::CommonSvcs::Naming;

namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;


    class  MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
    {
        
    public:
        MyProcessBehaviour(CORBA::ORB_ptr orb)
        {
            m_orb = CORBA::ORB::_duplicate(orb);
        }
        
        virtual
        ~MyProcessBehaviour() throw ()
        {            
        }
                
        // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
        virtual
        void
        initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CORBA::SystemException)
        {
            using namespace Cdmw::PlatformMngt;
            // example of using the PlatformInterface for notifying a message
            PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                              PlatformInterface::Get_process_name(),
                                              "Initialisation requested");
        }
                
        // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
        virtual
        void run()
            throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, CORBA::SystemException)
        {
            using namespace Cdmw::PlatformMngt;
            PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                              PlatformInterface::Get_process_name(),
                                              "Run requested");
            // The naming context used for the tests
            CosNaming::NamingContextExt_var nc;

            //
            // Resolve the Root POA
            //
            CORBA::Object_var obj = m_orb -> resolve_initial_references("RootPOA");
            PortableServer::POA_var root = PortableServer::POA::_narrow(obj.in());

            //
            // Get naming service
            //
            try
            {
                obj = m_orb -> resolve_initial_references("NameService");
            }
            catch(const CORBA::ORB::InvalidName&)
            {
                NI_TEST_CERR("can't resolve `NameService'");
                return;
            }

            if(CORBA::is_nil(obj.in()))
            {
                NI_TEST_CERR("`NameService' is a nil object reference");
                return;
            }
    
            nc = CosNaming::NamingContextExt::_narrow(obj.in());

            if(CORBA::is_nil(nc.in()))
            {
                NI_TEST_CERR(": `NameService' is not a NamingContextExt object reference");
                return;
            }

            // Create TestObject implementation
            TestObject_impl* test = new TestObject_impl(root.in());
            test::TestObject_var test_ref = test->_this();
            test->_remove_ref(); // it is reference counted and the RootPOA holds a reference to it
            
            //Testing TestNamingInterfaceResolve
            TestNamingInterfaceResolve resolveDriver(nc.in(),test_ref.in());
            resolveDriver.start();    
            
            // Testing TestNamingInterfaceBindContext
            TestNamingInterfaceBindContext bind_contextDriver(nc.in(),test_ref.in());
            bind_contextDriver.start();    
            
            // Testing TestNamingInterfaceBind
            TestNamingInterfaceBind bindDriver(nc.in(),test_ref.in());
            bindDriver.start();
            
            // Testing TestNamingInterfaceUnbind
            TestNamingInterfaceUnbind unbindDriver(nc.in(),test_ref.in());
            unbindDriver.start();
            
            // Testing TestNamingInterfaceList
            TestNamingInterfaceList listDriver(nc.in(),test_ref.in());
            listDriver.start();
            
            // Testing TestNamingInterfaceDestroy
            TestNamingInterfaceDestroy destroyDriver(nc.in(),test_ref.in());
            destroyDriver.start();

            m_orb->shutdown(false);
        }
        
        // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
        virtual
        void stop()
            throw(CORBA::SystemException)
        {
            using namespace Cdmw::PlatformMngt;
            PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                                              PlatformInterface::Get_process_name(),
                                              "Stop requested");
            
            m_orb->shutdown(false);
        }
                
    private:
        /**
         * The ORB.
         */
        CORBA::ORB_var m_orb;        
    };
    
}


int
run(CORBA::ORB_ptr orb, int argc, char* argv[], const std::string& callbackIOR)
{
    // Testing NameParser
    TestNameParserToName to_nameNPdriver;
    to_nameNPdriver.start();
    
    
    TestNameParserToString to_stringNPdriver;
    to_stringNPdriver.start();
    
    //
    // Resolve the Root POA
    //
    CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
    PortableServer::POA_var root = PortableServer::POA::_narrow(obj.in());
    PortableServer::POAManager_var poaManager = root -> the_POAManager();

    //
    // Initialize the server so that incoming requests are
    // handled from this point on
    //
    poaManager -> activate();


    // The naming context used for the tests
    //CosNaming::NamingContextExt_var nc;


    // use the actual naming and repository provided by the platform agent
    if (!callbackIOR.empty())
    {
        std::cout << "Getting repository from ProcessCallbak" << std::endl;
        using namespace Cdmw::PlatformMngt;
        PlatformInterface::Setup(orb,argc,argv);

        // acknowledge the creation of the process
        PlatformInterface::Acknowledge_creation(new MyProcessBehaviour(orb));
        
        orb->run();

        // cleanup the platform interface
        PlatformInterface::Cleanup();
    }
    else
    {
        std::cout << "callbackIOR is empty !" << std::endl;
	return FAILURE;
    }
    
    return SUCCESS;
}

int main(int argc, char* argv[])
{

    using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;

    // agent IOR
    std::string callbackIOR = OS::get_option_value( argc, argv,
                                                    Options::CALLBACK_IOR_OPTION);
    if ( callbackIOR == "no" )
    {
        callbackIOR = "";
    }

    // We allow the user to specify a different port number for the Name Service
    std::string option("--CdmwNamePort");
    std::string portNumber = OS::get_option_value(argc, argv, option) ;

    if ( portNumber == "no" ) {
        portNumber="5000";
    } 

#if (CDMW_ORB_VDR == orbacus )
    std::string namingservice="nameserv";
    OS::ProcessId idNaming = OS::create_process( namingservice, "-OAport " + portNumber );
#elif (CDMW_ORB_VDR == tao )
    std::string namingservice="Naming_Service";
    OS::ProcessId idNaming = OS::create_process( namingservice, "-ORBEndpoint iiop://localhost:" + portNumber );
#endif

	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale*3000);

#if (CDMW_ORB_VDR == orbacus )
    std::string strArgv = "testNI -ORBnaming corbaloc::localhost:" + portNumber + "/NameService";
#elif (CDMW_ORB_VDR == tao )
    std::string strArgv = "testNI -ORBInitRef NameService=corbaloc:iiop:localhost:" + portNumber + "/NameService";
#endif
    
    int myArgc(3);
    if (callbackIOR != "no" && callbackIOR != "yes")  {
        strArgv += " ";
        strArgv += Options::CALLBACK_IOR_OPTION;
        strArgv += "=" + callbackIOR;
        myArgc++;
    }
        
    char** myArgv = String::to_char_array( String::to_strings(strArgv) );

    int status = SUCCESS;
    CORBA::ORB_var orb;

    try
    {
        orb = CORBA::ORB_init(myArgc, myArgv);
        status = run(orb.in(), myArgc, myArgv, callbackIOR);
    }
    catch(const CORBA::Exception& ex)
    {
	    NI_TEST_CERR(ex._name());
	    status = FAILURE;
    } catch (const Cdmw::Exception& ex)
    {
	    NI_TEST_CERR(ex.what());
	    status = FAILURE;
    }

    if(!CORBA::is_nil(orb.in()))
    {
    	try
	    {
	        orb -> destroy();
    	}
    	catch(const CORBA::Exception& ex)
    	{
    	    NI_TEST_CERR(ex._name());
	        status = FAILURE;
    	}
    }    

    // We free memory allocated by String::to_char_array
    for (int iArgc=0; iArgc < myArgc; ++iArgc) {
        delete[] myArgv[iArgc];
    }
    delete[] myArgv;

    // And we kill the Name Server
    OS::kill_process( idNaming );


    return status;    
}


