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


#include <Foundation/common/Locations.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Assert.hpp>

#include <TraceAndPerf/tracelibrary/Trace.hpp>

#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include "HelloEntityFactory.stub.hpp"
#include "HelloClientThread.hpp"



namespace 
{

    const char* COMPLETE_CHANNEL_NAME = "EVENT_CHANNELS/EventChannel1";
 
}; // End anonymous namespace




namespace Cdmw
{
namespace Tutorial
{




HelloClientThread::HelloClientThread(CORBA::ORB_ptr orb, 
                           std::string& applicationName,
                           std::string& processName)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_applicationName (applicationName),
      m_processName (processName),
      m_exit_required(false),
      m_remove_request(0)
{
    
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
    // find the HelloInterface factory object from repository
    // ======================================================
    CdmwLifeCycle::FactoryBase_var factory = factoryFinder->find_factory (
                    "Hello.Domain/Hello.Factories/HelloEntity.factory");
                        
                        
	HelloEntityFactoryModule::HelloEntityFactory_var hello_factory = 
	         HelloEntityFactoryModule::HelloEntityFactory::_narrow(factory.in());

    // ======================================================
	// create the HelloInterface proxy from the factory
	// ======================================================
	       	    
	m_helloInterface = hello_factory->create_HelloInterface ("HelloInterface_001");        
	    
	if (CORBA::is_nil(m_helloInterface.in()))
	{
	    std::cout << "could not _narrow object to type HelloInterface" << std::endl;
	}
	
	// ======================================================
	// create the HelloInterface proxy 2 from the factory
	// ======================================================
	       	    
	m_helloInterface2 = hello_factory->create_HelloInterface ("HelloInterface_002");        
	    
	if (CORBA::is_nil(m_helloInterface2.in()))
	{
	    std::cout << "could not _narrow object to type HelloInterface" << std::endl;
	}
}



HelloClientThread::~HelloClientThread() throw()
{
}

 

void HelloClientThread::run () throw()
{
    try
    {        
        std::cout << "Hello Client Thread Started" << std::endl;
        
        std::string baseMsg = "process <" + m_applicationName + "/" + m_processName + ">" ;
        
        // ===================================================
        // trace a message (use CDMW_TRACE in Cardamom thread)
        // ===================================================
        std::string msg = baseMsg + "Client Thread started";  
                                       
        CDMW_TRACE("TUTOR", 1, msg.c_str());



        // ====================================================
        // thread loop
        // ====================================================
        
        while (!m_exit_required) 
        {  
            try
            {          
                // ===================================================
                // Call displayHello method of HelloInterface object
                // ===================================================
	            m_helloInterface->display_hello();
	            
	            
	            // if server is restarted, try to remove HelloInterface2
                if (m_remove_request == 1)
                {        
                    m_remove_request = 2;
                    
                    // ===================================================
                    // Remove HelloInterface2
                    // ===================================================
                    try
                    {
                        std::cout << ">> HelloServer has been restarted" << std::endl;
                        
	                    m_helloInterface2->remove();
	                    
	                    std::cout << ">> HelloInterface2 removing successfull" << std::endl;
	                }
	                catch (CosLifeCycle::NotRemovable)
	                {
	                    std::cout << "====> Exception CosLifeCycle::NotRemovable" << std::endl;
	                }
	            
	                catch (CORBA::SystemException& e)
	                {
	                    std::ostringstream exceptionInfo;
                        exceptionInfo << e;
        
                        std::cout << "====> remove() call error"
                                  <<  exceptionInfo.str().c_str() << std::endl;
	                }
	                
                }
	        }	        
	        catch (const CORBA::SystemException& e)
            {
               std::ostringstream exceptionInfo;
               exceptionInfo << e;
        
               std::cout << "====> display_hello() call error"
                         <<  exceptionInfo.str().c_str() << std::endl;
                
               // server is stopped, set to remove HelloInterface2 when server restart
               if (m_remove_request == 0)
               {    
                   std::cout << ">> HelloServer has been stopped" << std::endl;    
                   m_remove_request = 1;
               }
            }
	    	    
            // ===================================================
            // set thread sleeping
            // ===================================================
            Cdmw::OsSupport::OS::sleep (10000);
        }
        
        // ===================================================
        // Remove HelloInterface
        // ===================================================
	    m_helloInterface->remove();

    }
    catch (...)
    {
        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	              << "Unexpected exception in thread loop" << std::endl;
    }
}




}; // namespace Tutorial
}; // namespace Cdmw
