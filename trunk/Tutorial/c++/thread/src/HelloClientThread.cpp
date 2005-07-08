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
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


#include "Hello.stub.hpp"
#include "HelloEntityFactory.stub.hpp"

#include "HelloClientProcessControl.hpp"
#include "HelloClientThread.hpp"



namespace Cdmw
{
namespace Tutorial
{




HelloClientThread::HelloClientThread(CORBA::ORB_ptr orb, 
                           CdmwNamingAndRepository::Repository_ptr rep,
                           std::string& applicationName,
                           std::string& processName)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_repository(CdmwNamingAndRepository::Repository::_duplicate(rep)),
      m_applicationName (applicationName),
      m_processName (processName),
      m_exit_required(false)
{
    // ========================================================
    // Get the Default Root context from Repository
    // ========================================================
    CORBA::Object_var obj = m_repository->resolve_root_context (
                               CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
    CosNaming::NamingContextExt_var rootCtx = 
                               CosNaming::NamingContextExt::_narrow(obj.in());
                                   
                                   
    // ========================================================
    // Get the Factory Finder from Default Root Context
    // ========================================================
    CORBA::Object_var obj2 = rootCtx->resolve_str (
                               CdmwNamingAndRepository::FACTORY_FINDER);
    CdmwLifeCycle::FactoryFinder_var factoryFinder = 
                               CdmwLifeCycle::FactoryFinder::_narrow(obj2.in());
                                   
         
    // ======================================================
    // find the HelloInterface factory object from repository
    // ======================================================
    CdmwLifeCycle::FactoryBase_var factory = factoryFinder->find_factory (
                    "Hello.Domain/Hello.Factories/HelloEntity.factory");
                        
                        
	HelloEntityFactoryModule::HelloEntityFactory_var helloInterface_factory = 
	         HelloEntityFactoryModule::HelloEntityFactory::_narrow(factory.in());

    // ======================================================
	// get the HelloInterface proxy from the factory
	// ======================================================
	m_helloInterface = Hello::HelloInterface::_nil();
	             	             	    
	m_helloInterface = helloInterface_factory->create_HelloInterface ("HelloInterface_001",
	                                                                  "Hello_001"); 
	    
	if (CORBA::is_nil(m_helloInterface.in()))
	{
	    std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	              << "could not _narrow object to type HelloInterface" << std::endl;
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
            // ===================================================
            // Call displayHello method of HelloInterface object
            // ===================================================
            if (!CORBA::is_nil(m_helloInterface.in()))
	        {
	            m_helloInterface->display_hello();
	        }


            // ===================================================
            // set thread sleeping
            // ===================================================
            Cdmw::OsSupport::OS::sleep (10000);
        } 
    }
    catch (...)
    {
        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	              << "Unexpected exception in thread loop" << std::endl;
    }
}




}; // namespace Tutorial
}; // namespace Cdmw
