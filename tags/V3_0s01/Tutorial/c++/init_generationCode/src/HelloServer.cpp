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
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


#include "HelloServerProcessControl.hpp"
#include "HelloInterface_impl.hpp"


namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;
 
}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    int status = SUCCESS;
    
    std::cout << "Start Hello Server" << std::endl;
    
    
    CORBA::ORB_var orb;                        // orb reference
    Cdmw::Trace::Stream stream;               // stream used by trace service
   
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
        
        
        // ========================================================
        // Get the naming and repository from Repository interface
        // ========================================================
        CdmwNamingAndRepository::Repository_var repository =
                                  CdmwNamingAndRepository::Repository::_nil();  
                                  
        repository = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
          
                                                                  
        // ===================================================
        // create the HelloInterface servant
        // ===================================================
        HelloModule::HelloInterface_var helloInterface;
        
        {
            Cdmw::Tutorial::HelloInterface_impl* pHelloInterfaceServant = 
                            new Cdmw::Tutorial::HelloInterface_impl();
    
		    // create an object var to take pointer ownership
		    // (ref count will be decremented when var will be destroyed at the block end)
		    PortableServer::ServantBase_var servant_var = pHelloInterfaceServant;
		
		    // activate servant on POA (ref count is incremented)
		    PortableServer::ObjectId_var oid = 
		                   rootPOA->activate_object(pHelloInterfaceServant);
		                   
		    CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
		    helloInterface = HelloModule::HelloInterface::_narrow(object.in());
	    }
	    
	    
	    // ======================================================
        // Get HELLO name domain from repository
        // ======================================================
        CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("TUTORIAL/HELLO");
             
             
        // ========================================================
        // Register the HelloInterface object in the HELLO name domain
        // ========================================================
        // reserve the name "Hello1" in the domain
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = 
                                    helloDomain->new_name ("Hello1");
        // register the object
        helloDomain->register_object (regId.in(), helloInterface.in());
       
        
       

        std::cout << "Hello Server Started" << std::endl;
        
        std::string baseMsg = "process <" + applicationName + "/" + processName + ">" ;
        
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
    // Call generated Cardamom cleanup
    // ===================================================
    Cdmw::CdmwInit::CDMW_cleanup(orb.in());
    
    // ===================================================
    // Call ORB cleanup
    // ===================================================
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            
    // ===================================================
    // destroy orb
    // ===================================================
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

