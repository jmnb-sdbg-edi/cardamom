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

#include <Foundation/common/System.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>

#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


#include "PrinterModule.stub.hpp"
#include "PrinterSessionFactory.stub.hpp"


namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;
 
}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    int status = SUCCESS;
    
    std::cout << "Start Printer Client" << std::endl;
    

    CORBA::ORB_var orb;                        // orb reference
   
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
        // Call generated cdmwinit
        // ===================================================
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, NULL);
        
        
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
        
        
        // ========================================================
        // Get the Default Root context from Repository
        // ========================================================
        CORBA::Object_var obj2 = repository->resolve_root_context (
                                   CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
        CosNaming::NamingContextExt_var rootCtx = 
                                   CosNaming::NamingContextExt::_narrow(obj2.in());
                                   
                                   
        // ========================================================
        // Get the Factory Finder from Default Root Context
        // ========================================================
        CORBA::Object_var obj3 = rootCtx->resolve_str (
                                   CdmwNamingAndRepository::FACTORY_FINDER);
        CdmwLifeCycle::FactoryFinder_var factoryFinder = 
                                   CdmwLifeCycle::FactoryFinder::_narrow(obj3.in());
                                   
         
        // ======================================================
        // find the Printer factory object from repository
        // ======================================================
        CdmwLifeCycle::FactoryBase_var factory = factoryFinder->find_factory (
                        "Printer.Domain/Printer.Factories/PrinterSession.factory");
                        
                        
	    PrinterFactories::PrinterSessionFactory_var printer_factory = 
	             PrinterFactories::PrinterSessionFactory::_narrow(factory.in());

        // ======================================================
	    // get the Printer proxy from the factory
	    // ======================================================
	    PrinterModule::Printer_var printer = 
	             PrinterModule::Printer::_nil();
	             	    
	    printer = printer_factory->create_printer ("lpr10");         
	    
	    if (CORBA::is_nil(printer.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type Printer" << std::endl;
	        return FAILURE;
	    }


        std::cout << "Printer Client Started" << std::endl;
        
        std::string baseMsg = "process <" + applicationName + "/" + processName + ">" ;
        
      
      
      
        // ===================================================
        // Call print_message method of Printer object
        // ===================================================
	    printer->print_message (">>>>> HELLO <<<<<");
	    
	    
	    
	    // ===================================================
        // Remove Printer
        // ===================================================
	    printer->remove();


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

