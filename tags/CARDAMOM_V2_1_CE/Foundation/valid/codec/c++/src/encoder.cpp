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

//#include <sstream>
#include <iostream>

#include <Foundation/common/System.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "Foundation/common/String.hpp"
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"

#include "testcodec_lib/TestEncoder.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"

namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;

    class  MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
    {
        
    public:
            MyProcessBehaviour(CORBA::ORB_ptr orb) : m_testEncoder(orb)
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
            std::cout << "TestEncoder initialize" << std::endl;
            m_testEncoder.init();
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
            std::cout << "TestEncoder start" << std::endl;
            m_testEncoder.start();
            std::cout << "TestEncoder ended" << std::endl;
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
        Cdmw::TestCodec::TestEncoder m_testEncoder;
    };

}; // End anonymous namespace


int
run(CORBA::ORB_ptr orb, int argc, char* argv[], const std::string& callbackIOR)
{
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

    if (!callbackIOR.empty())
    {   // If it is launched by supervision callbackIOR is automaticaly put
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
    using namespace Cdmw::TestCodec;
    
    int status = SUCCESS;
    // agent IOR
    std::string callbackIOR = OS::get_option_value( argc, argv,
                                                    Options::CALLBACK_IOR_OPTION);
    if ( callbackIOR == "no" )
    {
        callbackIOR = "";
    }
   
    std::cout << "Start of encoder" << std::endl;
    

    CORBA::ORB_var orb;                        // orb reference

    // We allow the user to specify a different port number for the decoder
    std::string option("--CdmwDecoderPort");
    std::string portNumber = OS::get_option_value(argc, argv, option) ;

    if (( portNumber != "no" )&&( portNumber != "yes" )) {
        TestEncoder::CDMW_LOCALISATION_PORT = atoi(portNumber.c_str());
    }
    std::cout << "encoder will retrieve decoder via port: " << TestEncoder::CDMW_LOCALISATION_PORT << std::endl;

    std::string strArgv = "valid_encoder";
    int myArgc(1);
    if (callbackIOR != "no" && callbackIOR != "yes")  {
        strArgv += " ";
        strArgv += Options::CALLBACK_IOR_OPTION;
        strArgv += "=" + callbackIOR;
        myArgc++;
    }
    
    char** myArgv = String::to_char_array( String::to_strings(strArgv) );

    try 
    {        
        // ===================================================
        // Initialize the ORB
        // ===================================================
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(myArgc, myArgv, orb_strategies);
        status = run(orb.in(), myArgc, myArgv, callbackIOR);        
        
        std::cout << "Encoder Started" << std::endl;
    } 
    catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex)
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    }
    catch (const Cdmw::Exception & ex)
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } 
    catch(const CORBA::Exception& ex)  
    {        
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
    catch(...)  
    {        
        std::cerr << "unexpected exception" << std::endl;
        status = FAILURE;
    }


    // ===================================================
    // destroy orb
    // ===================================================
    
    if (!CORBA::is_nil(orb.in()))
    {
        try 
        {
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb -> destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }      
  
    // We free memory allocated by String::to_char_array
    for (int iArgc=0; iArgc < myArgc; ++iArgc) {
        delete[] myArgv[iArgc];
    }
    delete[] myArgv;

    return status;    
}

