/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "testplatformlibrary/TestServiceDef.hpp"

#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"
#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"

#include <iostream>
#include <string>
#include <memory>


using namespace CdmwPlatformMngtService;
using namespace Cdmw::PlatformMngt;


void displayService (const char* s)
{
    std::cout << "service name :" << s << std::endl << std::endl;
}



void displayService (const CdmwPlatformMngtService::ServiceDef& s)
{
    std::cout << "service name                  :" << (s.service_name).in() << std::endl;
    std::cout << "process ID - application name :" 
              << (s.service_provider.application_name).in() << std::endl;   
    std::cout << "process ID - process name     :" 
              << (s.service_provider.process_name).in() << std::endl;
    std::cout << std::endl;
}



void displayService (const CdmwPlatformMngtService::ServiceProviderID& sp)
{
    std::cout << "service process ID - application name :" 
              << (sp.application_name).in() << std::endl;   
    std::cout << "service process ID - process name     :" 
              << (sp.process_name).in() << std::endl;
    std::cout << std::endl;
}




TestServiceDef::TestServiceDef(const std::string& name, PortableServer::POA_ptr poa)
    : Testable(name)
{
    m_POA = PortableServer::POA::_duplicate(poa);
}


TestServiceDef::~TestServiceDef()
{
}


void TestServiceDef::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (16);
    
    
	ServiceDefContainer_impl::ServiceDefContainer *pSystemContainer = NULL;
	ServiceDefContainer_impl::ServiceDefContainer *pApplicationContainer = NULL;

	
    try
    {
        // create the service containers
        TEST_INFO("");
        TEST_INFO("Creates a System ServiceDefContainer");
        pSystemContainer = 
                ServiceDefContainer_impl::ServiceDefContainer::createSystemContainer ();
 
 
        TEST_INFO("Creates an Application ServiceDefContainer");
        pApplicationContainer = 
                ServiceDefContainer_impl::ServiceDefContainer::createApplicationContainer ("application_1");
                
        TEST_SUCCEED();  
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    } 
    
    
    PortableServer::ServantBase_var servantSys_var;
    PortableServer::ServantBase_var rwServantSys_var;
    PortableServer::ServantBase_var servantApp_var;
    PortableServer::ServantBase_var rwServantApp_var;
    
    ServiceDefContainer_impl* pServiceContainerServantSys;
    RWServiceDefContainer_impl* pRWServiceContainerServantSys;
    ServiceDefContainer_var serviceContainerSys;
    RWServiceDefContainer_var rwServiceContainerSys;
    
    
    ServiceDefContainer_impl* pServiceContainerServantApp;
    RWServiceDefContainer_impl* pRWServiceContainerServantApp;
    ServiceDefContainer_var serviceContainerApp;
    RWServiceDefContainer_var rwServiceContainerApp;
    
    try
    {   
    	TEST_INFO("");
        TEST_INFO("Creates a system ServiceDefContainer Servant");
        pServiceContainerServantSys = 
				new ServiceDefContainer_impl (m_POA.in(), pSystemContainer);
 
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the method end)
	    servantSys_var = pServiceContainerServantSys;
	
	    // activate object and get its reference
	    pServiceContainerServantSys->activate();
        serviceContainerSys = pServiceContainerServantSys->_this();


        TEST_INFO("Creates an system RWServiceDefContainer Servant");
        pRWServiceContainerServantSys = 
				new RWServiceDefContainer_impl (m_POA.in(), pSystemContainer);
 
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the method end)
	    rwServantSys_var = pRWServiceContainerServantSys;
	
	    // activate object and get its reference
	    pRWServiceContainerServantSys->activate();
        rwServiceContainerSys = pRWServiceContainerServantSys->_this();
    
  
  
        TEST_INFO("");
        TEST_INFO("Creates an application ServiceDefContainer Servant");
        pServiceContainerServantApp = 
				new ServiceDefContainer_impl (m_POA.in(),
				                              pApplicationContainer,
				                              serviceContainerSys.in());
 
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the method end)
	    servantApp_var = pServiceContainerServantApp;
	
	    // activate object and get its reference
	    pServiceContainerServantApp->activate();
        serviceContainerApp = pServiceContainerServantApp->_this();


        TEST_INFO("Creates an application RWServiceDefContainer Servant");
        pRWServiceContainerServantApp = 
				new RWServiceDefContainer_impl (m_POA.in(),
				                                pApplicationContainer,
				                                rwServiceContainerSys.in());
 
        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at the method end)
	    rwServantApp_var = pRWServiceContainerServantApp;
	
	    // activate object and get its reference
	    pRWServiceContainerServantApp->activate();
        rwServiceContainerApp = pRWServiceContainerServantApp->_this();  
        TEST_SUCCEED();
    
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
    
    
    
    
    
    
    // Check when no service inserted in container
    try
    {      
        // get service
        TEST_INFO("Check ServiceDefContainer exception if service with undefined name is requested");
        try
        {
            // Get service process Id with bad name : exception must be thrown
            CdmwPlatformMngtService::ServiceProviderID_var serviceId = 
                            rwServiceContainerSys->get_service_def ("s1");
            
            TEST_FAILED();
        }
        catch (ServiceNotFound& ex)
        {
        	TEST_SUCCEED();
        }
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }





    // Check service insertion in container
    try
    {
    	TEST_INFO("");
    	
    	CdmwPlatformMngtService::ServiceDef_var service_def =
    	                    new CdmwPlatformMngtService::ServiceDef();
    	
        TEST_INFO("Check adding services in system ServiceDefContainer");
        
        service_def->service_name = CORBA::string_dup ("s1");
        service_def->service_provider.application_name = CORBA::string_dup ("application 1");
        service_def->service_provider.process_name = CORBA::string_dup ("process 1");
        rwServiceContainerSys->add_service_def (service_def.in());
        
        service_def->service_name = CORBA::string_dup ("s2");
        service_def->service_provider.application_name = CORBA::string_dup ("application 2");
        service_def->service_provider.process_name = CORBA::string_dup ("process 2");
        rwServiceContainerSys->add_service_def (service_def.in());
                
        service_def->service_name = CORBA::string_dup ("s3");
        service_def->service_provider.application_name = CORBA::string_dup ("application 3");
        service_def->service_provider.process_name = CORBA::string_dup ("process 3");
        rwServiceContainerSys->add_service_def (service_def.in());
        
        service_def->service_name = CORBA::string_dup ("s4");
        service_def->service_provider.application_name = CORBA::string_dup ("application 4");
        service_def->service_provider.process_name = CORBA::string_dup ("process 4");
        rwServiceContainerSys->add_service_def (service_def.in());
        
        TEST_SUCCEED();
        
        TEST_INFO("Check adding duplicate service in system ServiceDefContainer");
        try
        {
            // Add service with duplicate name : exception must be thrown
            rwServiceContainerSys->add_service_def (service_def.in());
            
            TEST_FAILED();
        }
        catch (ServiceAlreadyExists& ex)
        {
        	TEST_SUCCEED();
        }
        
        
        TEST_INFO("Check removing undefined service in system ServiceDefContainer");
        try
        {
            // Remove service with undefined name : exception must be thrown
            rwServiceContainerSys->remove_service_def ("s5");
            
            TEST_FAILED();
        }
        catch (ServiceNotFound& ex)
        {
        	TEST_SUCCEED();
        }
        
        TEST_INFO("Check removing service in system ServiceDefContainer");
        rwServiceContainerSys->remove_service_def ("s4");
        TEST_SUCCEED();
        
        
        TEST_INFO("Check adding services in application ServiceDefContainer");
        
        service_def->service_name = CORBA::string_dup ("s4");
        service_def->service_provider.application_name = CORBA::string_dup ("application 4");
        service_def->service_provider.process_name = CORBA::string_dup ("process 4");
        rwServiceContainerApp->add_service_def (service_def.in());
        
        service_def->service_name = CORBA::string_dup ("s5");
        service_def->service_provider.application_name = CORBA::string_dup ("application 5");
        service_def->service_provider.process_name = CORBA::string_dup ("process 5");
        rwServiceContainerApp->add_service_def (service_def.in());
        TEST_SUCCEED();
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }
    
  
  
  
  
  
    
    // Get services from filled container
    try
    { 
        // get service    
        TEST_INFO("Get specific service from system container");
        try
        {
        	std::string service("s1");
            CdmwPlatformMngtService::ServiceProviderID_var serviceId = serviceContainerSys->get_service_def (service.c_str());
               
            std::cout << "service name :" << service.c_str() << std::endl;  
            displayService (serviceId.in());     
             
            TEST_SUCCEED();
        }
        catch (ServiceNotFound& ex)
        {
        	TEST_FAILED();
        }
  
        try
        {
        	std::string service("s3");
            CdmwPlatformMngtService::ServiceProviderID_var serviceId = 
                        serviceContainerSys->get_service_def (service.c_str());
                 
            std::cout << "service name :" << service.c_str() << std::endl;
            displayService (serviceId.in());     
             
            TEST_SUCCEED();
        }
        catch (ServiceNotFound& ex)
        {
        	TEST_FAILED();
        }
  
       
        TEST_INFO("Get undefined service from system container");
        try
        {
        	std::string service("s5");
            CdmwPlatformMngtService::ServiceProviderID_var serviceId = 
                        serviceContainerSys->get_service_def (service.c_str());
               
            TEST_FAILED();
        }
        catch (ServiceNotFound& ex)
        {
        	TEST_SUCCEED();
        }
       
   
   
        TEST_INFO("Get specific service from application container");
        TEST_INFO(" service defined in application container");
        try
        {
        	std::string service("s5");
            CdmwPlatformMngtService::ServiceProviderID_var serviceId = 
                        serviceContainerApp->get_service_def (service.c_str());
               
            std::cout << "service name :" << service.c_str() << std::endl;  
            displayService (serviceId.in());     
             
            TEST_SUCCEED();
        }
        catch (ServiceNotFound& ex)
        {
        	TEST_FAILED();
        }
        
        
        TEST_INFO("Get specific service from application container");
        TEST_INFO(" service defined in parent system container");
        try
        {
        	std::string service("s1");
            CdmwPlatformMngtService::ServiceProviderID_var serviceId = 
                        serviceContainerApp->get_service_def (service.c_str());
               
            std::cout << "service name :" << service.c_str() << std::endl;  
            displayService (serviceId.in());     
             
            TEST_SUCCEED();
        }
        catch (ServiceNotFound& ex)
        {
        	TEST_FAILED();
        }
        
        
        
        TEST_INFO("Get undefined service from application container");
        try
        {
        	std::string service("s6"); 
            serviceContainerApp->get_service_def (service.c_str());
               
            TEST_FAILED();
        }
        catch (ServiceNotFound& ex)
        {
        	TEST_SUCCEED();
        }
        
        
        
        
        // destroy the containers
        TEST_INFO("Destroy containers");
        
        bool ret = ServiceDefContainer_impl::ServiceDefContainer::destroy (pSystemContainer);
        if (ret)
        {
            TEST_SUCCEED();
        }
        else
        {        
            TEST_FAILED();
        }
        
        ret = ServiceDefContainer_impl::ServiceDefContainer::destroy (pApplicationContainer);
        if (ret)
        {
            TEST_SUCCEED();
        }
        else
        {        
            TEST_FAILED();
        }
        
    }
    catch(...)
    {
        std::cout << "FAILURE : Unexpected exception" << std::endl;
        TEST_FAILED();
        return;
    }


	    pServiceContainerServantApp->deactivate();
	    pRWServiceContainerServantApp->deactivate();
}


