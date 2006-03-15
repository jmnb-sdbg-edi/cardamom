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
#include <limits>
#include "testpersistenceuserdefinedattributes/TestUserDefinedAttributes.hpp"

#include "Foundation/common/Options.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "idllib/CdmwPlatformMngtSupervisionObserver.skel.hpp"
#include "idllib/CdmwPlatformMngtSystem.stub.hpp"
#include "idllib/CdmwPlatformMngtApplication.stub.hpp"

 
using namespace std;

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

CPPUNIT_TEST_SUITE_REGISTRATION(TestUserDefinedAttributes);

namespace Cdmw
{
namespace PlatformMngt
{



TestUserDefinedAttributes::TestUserDefinedAttributes() throw() 
{

}


TestUserDefinedAttributes::~TestUserDefinedAttributes()
    throw()
{

}

//@brief   do_tests method for new navigability methods of IBB2
//@brief   developments  .

void
TestUserDefinedAttributes::do_tests()                
{
 /*get the orb*/
 m_orb = CORBA::ORB::_duplicate( Cdmw::TestUtils::CORBATestManager::instance()->get_ORB() );

 //new for test
// //  set_nbOfRequestedTestOK (1);


 CdmwPlatformMngt::System_ptr 		mysystem;
 CdmwPlatformMngt::Application_ptr 	myapp;
 CdmwPlatformMngt::Process_ptr 		myproc;
 CdmwPlatformMngt::Process_ptr		my_managed_proc; 	
 CdmwPlatformMngt::Host_ptr 		myhost;
 CdmwPlatformMngt::Entity_ptr 		myent;


 try
    {

        // ===================================================
        // Get the HelloInterface object
        // ===================================================

        // import the object reference from the ior file
        CORBA::Object_var objref;

        //define the corbaloc
        const char *corbaloc_str = "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";
        objref = m_orb->string_to_object(corbaloc_str);
        char * hostname  = getenv("HOSTNAME");


        //resolve the System_ptr
        mysystem = CdmwPlatformMngt::System::_narrow(objref.in() );

	TEST_INFO("RESOLVING APPLICATION HELLO WORLD  ");
	myapp = mysystem -> get_application("HELLO_WORLD");

	TEST_INFO("RESOLVING HOST  "<<hostname<<std::endl);
	myhost = mysystem -> get_host(hostname);

	TEST_INFO("RESOLVING Un-Managed PROCESS: HelloProcess1");
	myproc = myhost->get_process("HelloProcess1", "HELLO_WORLD");
		
	TEST_INFO("RESOLVING Managed PROCESS: HelloProcess2");
	my_managed_proc = myhost->get_process("HelloProcess2","HELLO_WORLD");

	TEST_INFO("RESOLVING ENTITY of "<< hostname<<std::endl);
	myent = mysystem->get_entity("HelloEntity");		


	if (CORBA::is_nil(mysystem) )
	  {
	    TEST_INFO("Error in narrowing CdmwPlatformMngt::System_ptr");
	    exit(-1);
	  }
	
	try
	  {
	    
	   
	    //TEST_INFO ("Test on System::Defining a user defined attribute of type Short: ");

	   
   	    CORBA::Short s = 3;
	    CORBA::Short s2 = 4;
	    CORBA::Short e = 5; 
	    float f = 2.4 ;	
	    std::string str = " string property value into application ";
	
	    //CORBA::Any* return_any_val; 
	    
	    TEST_INFO("Set the Property: system_short_propert on System with value "<< s << std::endl);
    	    TEST_INFO("Set the String Property : app_str_property on Application with value "<<str<< std::endl);
	    TEST_INFO("Set the Float Property: value host_float_property on Host with value "<<f << std::endl);
    	    TEST_INFO("Set the Short Property: proc_short_property on Un-Managed Process with value "<<s<<std::endl);
	    TEST_INFO("Set the Short Property: managed_proc_short_property on Managed Process with value "<<s2<<std::endl);
	    TEST_INFO("Set the Short Property: entity_short_property on Entity with value "<<e<<std::endl);

	   
	    TEST_INFO(">>>>>>>>>>>>>>>>>>> wait 30s : Stop the daemon  >>>>>>>>>>>>>>>>>>>>>>>>");	 
	    sleep(30);
	    TEST_INFO(">>>>>>>>>>>>>>>>>>>  30 s ended >>>>>>>>>>>>>>>>>>>>>>>>");
	    
	    CORBA::Any* return_any_val_sys;
	    return_any_val_sys = mysystem-> get_property_value("system_short_property");
	    short l_system_out;
	    (*return_any_val_sys) >>=  l_system_out;
  	    TEST_INFO("Any value recovered from SYSTEM with value: "<<l_system_out);


	    CORBA::Any* return_any_val_app;
	    return_any_val_app = myapp-> get_property_value("app_str_property");
	    const char * l_app_out;	                                                                                                                (*return_any_val_app) >>=  l_app_out;
	    TEST_INFO("Any value recovered from APPLICATION with value: "<<l_app_out);


	    CORBA::Any* return_any_val_host;
	    return_any_val_host = myhost-> get_property_value("host_float_property");
	    float l_host_out;	                                                                                                          
	    (*return_any_val_host) >>=  l_host_out;
	    TEST_INFO("Any value recovered from HOST with value: "<<l_host_out);
	

	    CORBA::Any* return_any_val_proc;
	    return_any_val_proc = myproc-> get_property_value("proc_short_property");
	    short l_proc_out;	                                                                                                          
	    (*return_any_val_proc) >>=  l_proc_out;
	    TEST_INFO("Any value recovered from PROCESS with value:  "<<l_proc_out);


	    CORBA::Any* return_any_val_managed_proc;
	    return_any_val_managed_proc = my_managed_proc-> get_property_value("managed_proc_short_property");
	    short l_managed_proc_out;	                                                                                                 (*return_any_val_managed_proc) >>=  l_managed_proc_out;
	    TEST_INFO("Any value recovered from MANAGED-PROCESS with value:  "<<l_managed_proc_out);

	    
	    CORBA::Any* return_any_val_system_entity;
	    return_any_val_system_entity = myent-> get_property_value("entity_short_property");
	    short l_system_entity_out;	                                                                                                 (*return_any_val_system_entity) >>= l_system_entity_out;
	    TEST_INFO("Any value recovered from System ENTITY with value:  "<<l_system_entity_out);


            if ((l_system_out==s)&&(str==l_app_out)&&(l_host_out==f)&&(l_proc_out==s)&&(l_managed_proc_out=s2)&&(l_system_entity_out=e))
            CPPUNIT_ASSERT(1);
            else
            CPPUNIT_ASSERT(0);
	  }
	catch( const CORBA::Exception  & ex )
	  {
	    CPPUNIT_ASSERT(0); //test nok
	    std::cerr << ex._name() << std::endl;
	    TEST_INFO ("Test on System::getting the userdefined attributes ");
	  }
	catch (const Cdmw::Exception & ex)
	  {
	    CPPUNIT_ASSERT(0); //test nok
	    std::cerr << ex.what() << std::endl;
	    exit(-1);
	  }
    }
 catch(...)
   {
     std::cerr << "unexpected exception in recovering supervised object" << std::endl;
     exit(-1);
   }
 
 
 // ========================================================
 // program stopping
 // ========================================================
 
 // ===================================================
 // Call ORB cleanup
 // ===================================================
 Cdmw::OrbSupport::OrbSupport::ORB_cleanup(m_orb.in());
 
 // =====================================================
 // Destroy orb
 // =====================================================
 
 if (!CORBA::is_nil(m_orb.in()))
   {
     try
       {     
	 m_orb->destroy();
       }
     catch(const CORBA::Exception& ex)
       {
	 std::cerr << ex << std::endl;
	 exit(-1);
       }
   }
}    

} // End namespace PlatformMngt
} // End namespace Cdmw


