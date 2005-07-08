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

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <Foundation/common/System.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <TraceAndPerf/tracelibrary/Trace.hpp>

#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>


#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include <Event/eventinterface/EventChannelInterface.hpp>
#include <Event/idllib/CdmwEvent.stub.hpp>

#include "HelloServerProcessControl.hpp"
#include "HelloPushConsumer_impl.hpp"





namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;
    
    const char* CHANNEL_NAME = "EventChannel1";
 
}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    int status = SUCCESS;
    
    std::cout << "Start Hello Server" << std::endl;
    
    
    CORBA::ORB_var orb;                        // orb reference
    Cdmw::Trace::Stream stream;               // stream used by trace service
    Cdmw::Event::EventChannelInterface* p_eventchannel;
                                               // event channel interface
   
    try 
    {
    	// ===================================================
        // Initialize the ORB
        // ===================================================
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
        
        // ===================================================
        // Get the root POA 
        // ===================================================
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        // ===================================================
        // Activate the root POA manager
        // ===================================================
        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
        poaManager->activate();
        
        
        // ===================================================
        // create the process control for platform management
        // ===================================================
	    Cdmw::CdmwInit::ProcessControl_var process_ctrl =
              new Cdmw::Tutorial::HelloServerProcessControl(orb.in());


        // ===================================================
        // Call generated cdmwinit
        // ===================================================
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, process_ctrl.in());
        
        
        // ===================================================
        // Get the process name and application name
        // ===================================================
        std::string processName = Cdmw::CdmwInit::InitUtils::get_cdmw_process_name();
        
        std::string applicationName = Cdmw::CdmwInit::InitUtils::get_cdmw_application_name();
        

        std::cout << "Hello Server Started" << std::endl;
        
        std::string baseMsg = "process <" + applicationName + "/" + processName + ">" ;
        
        
        // ========================================================
        // Get the naming and repository from Repository interface
        // ========================================================
        CdmwNamingAndRepository::Repository_var repository =
                                  CdmwNamingAndRepository::Repository::_nil();  
                                  
        repository = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
          
          
          
        // ========================================================
        // Get the event channel factory from Event Channel Manager
        // ========================================================

        // Get NamingInterface object from RepositoryInterface
        Cdmw::NamingAndRepository::NamingInterface ni =
              Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(
                            Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN);


        // Get a reference on the EventChannelFactory
        std::cout << "Find a factory to create a generic channel" << std::endl;
        
        CORBA::Object_var obj2 = ni.resolve("EVENT/FACTORIES/DefaultEventChannelFactory");
        
        CdmwEvent::EventChannelFactory_var factory = 
            CdmwEvent::EventChannelFactory::_narrow(obj2.in());
        
        if (CORBA::is_nil(factory.in())) 
        {
            std::cerr << "DefaultEventChannelFactory is a nil object reference" << std::endl;
                
            return FAILURE;
        }


        // ========================================================
        // Create a new generic event channel using factory
        // ========================================================
        std::cout << "Creating a new generic channel EventChannel" << std::endl;

        CosEventChannelAdmin::EventChannel_var channel = 
                             factory->create_event_channel(CHANNEL_NAME);


        // ========================================================
        // Create the associated event channel interface
        // ========================================================
        p_eventchannel =
               new Cdmw::Event::EventChannelInterface(channel.in());   
               
               
        // ========================================================
        // Create and connect a push consumer to the channel
        // ========================================================       
        std::cout << "Connect local consumer to EventChannel1" << std::endl;
        
        Cdmw::Tutorial::HelloPushConsumer_impl* pcImpl = 
                       new Cdmw::Tutorial::HelloPushConsumer_impl(orb.in(), rootPOA.in());

        CosEventComm::PushConsumer_var ref = pcImpl->_this();
        p_eventchannel->connect_pushConsumer(ref.in());


        // ===================================================
        // trace a message (don't use CDMW_TRACE in main)
        // ===================================================
        std::string msg = baseMsg + "started and waiting CORBA messages";  
                                       
        CDMW_TRACE_ST(stream,"TUTOR", 1, msg.c_str());
        
        
        
        // ===================================================
        // wait for CORBA message
        // ===================================================
        orb->run();

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
    catch (const CdmwEvent::CreationError& ex)
    {
        std::cerr << ex << ": " << ex.reason << std::endl;
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


    // ========================================================
    // program stopping
    // ========================================================
    
    // ===================================================
    // Event clean up
    // ===================================================
    p_eventchannel->destroy();
    delete p_eventchannel;
                       
    // ===================================================
    // Call generated Cardamom cleanup
    // ===================================================
    Cdmw::CdmwInit::CDMW_cleanup(orb.in());
    
    // ===================================================
    // Call ORB cleanup
    // ===================================================
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
       
    // =====================================================
    // Destroy orb
    // =====================================================
    if (!CORBA::is_nil(orb.in()))
    {
        try 
        {
            orb -> destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}

