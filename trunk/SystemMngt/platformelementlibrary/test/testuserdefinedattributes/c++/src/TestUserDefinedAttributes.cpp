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
#include "testuserdefinedattributes/TestUserDefinedAttributes.hpp"

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
 //new for test
// //  set_nbOfRequestedTestOK (7);


 CdmwPlatformMngt::System_ptr mysystem;
 CdmwPlatformMngt::Application_ptr myapp;
 CdmwPlatformMngt::Process_ptr myproc;
 CdmwPlatformMngt::Host_ptr myhost;



 try
    {

        /*get the orb*/
        m_orb = CORBA::ORB::_duplicate( Cdmw::TestUtils::CORBATestManager::instance()->get_ORB() );

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

	myapp = mysystem -> get_application("HELLO_WORLD");
	
	myhost = mysystem -> get_host(hostname);

	myproc = myhost->get_process("HelloProcess1", "HELLO_WORLD");

	if (CORBA::is_nil(mysystem) )
	  {
	    TEST_INFO("Error in narrowing CdmwPlatformMngt::System_ptr");
	    exit(-1);
	  }


	try
	  {
	    
	    CORBA::Any any_S;
	    
	    CORBA::Short L = 3;
	    
	    any_S <<= L;
	    
  	    TEST_INFO ("Testing the MODE on SYSTEM : Defining a User defined Attribute of with READ_ONLY Mode: ");
	    mysystem -> define_property_with_mode ("short_property_readonly",
						   any_S,
						   CosPropertyService::read_only
						   );
	    
	    mysystem -> define_property ("short_property_readonly",
					 any_S);
	    
	    TEST_INFO ("Re - defining a user defined attribute with READ_ONLY Mode: ");
	    
	    CPPUNIT_ASSERT(0);
	  }
	catch (const CosPropertyService::ReadOnlyProperty& read_only_ex)
	  {
	    TEST_INFO("TEST OK THROWN THE EXCEPTION: "<< read_only_ex);
	    CPPUNIT_ASSERT(1);
	  }
	try
	  {
	    
	    
	    CORBA::Any any_S;
	    
	    CORBA::Short L = 3;
	    
	    any_S <<= L;
	    
	    TEST_INFO ("Testing the MODE on APPLICATION: Defining a User defined Attribute of type Short whith READ_ONLY Mode: ");
	    myapp -> define_property_with_mode ("short_property_readonly",
						any_S,
						CosPropertyService::read_only
						);
	    
	    myapp -> define_property ("short_property_readonly",
				      any_S);
	    
	    TEST_INFO ("Re-defining a User defined Attribute with READ_ONLY Mode: ");
	    
	    CPPUNIT_ASSERT(0);
	  }
	catch (const CosPropertyService::ReadOnlyProperty& read_only_ex)
	  {
	    TEST_INFO("TEST OK THROWN THE EXCEPTION: " << read_only_ex);
//	    TEST_INFO(read_only_ex);
	    CPPUNIT_ASSERT(1);
	  }
	try
	  {
	    
	    
	    CORBA::Any any_S;
	    
	    CORBA::Short L = 3;
	    
	    any_S <<= L;
	    
	    TEST_INFO ("Testing the MODE on HOST: Defining a User defined Attribute with READ_ONLY Mode: ");
	    myhost -> define_property_with_mode ("short_property_readonly",
						   any_S,
						   CosPropertyService::read_only
						   );
	    
	    myhost -> define_property ("short_property_readonly",
					 any_S);
	    
	    TEST_INFO ("Re-defining the user defined attribute  with READ_ONLY Mode: ");
	    
	    CPPUNIT_ASSERT(0);
	  }
	catch (const CosPropertyService::ReadOnlyProperty& read_only_ex)
	  {
	    TEST_INFO("TEST OK THROWN THE EXCEPTION: "<< read_only_ex);
//	    TEST_INFO(read_only_ex);
	    CPPUNIT_ASSERT(1);
	  }

	
	try
	  {
	    
	    
	    CORBA::Any any_S;
	    
	    CORBA::Short L = 3;
	    
	    any_S <<= L;
	    
	    TEST_INFO ("Testing the MODE on PROCESS: Defining a User defined Attribute with READ_ONLY Mode: ");
	    myproc -> define_property_with_mode ("short_property_readonly",
						   any_S,
						   CosPropertyService::read_only
						   );
	    
	    myproc -> define_property ("short_property_readonly",
					 any_S);
	    
	    TEST_INFO ("Re-defining a user defined attribute of type Short with READ_ONLY Mode: ");
	    
	    CPPUNIT_ASSERT(0);
	  }
	catch (const CosPropertyService::ReadOnlyProperty& read_only_ex)
	  {
	    TEST_INFO("TEST OK THROWN THE EXCEPTION: "<< read_only_ex);
//	    TEST_INFO(read_only_ex);
	    CPPUNIT_ASSERT(1);
	  }
	

	try
	  {
	    
	    
	    CORBA::Any* return_any_val; 
	    CORBA::Any any_S;
	    CORBA::Short L = 3;
	    any_S <<= L;
	    TEST_INFO ("Testing on Supervised Object SYSTEM / APPLICATION / HOST /PROCESS the override and the recover of User defined Attribute:");
	    TEST_INFO("Defining a SHORT User defined Attribute with value : " << L);
	    
	    mysystem -> define_property_with_mode ("short_property",
						   any_S,
						   CosPropertyService::normal
						   );
	    myapp ->  define_property_with_mode ("short_property", 
						   any_S,
						   CosPropertyService::normal
						   );	
	    myhost ->  define_property_with_mode ("short_property", 
						 any_S,
						 CosPropertyService::normal
						 );	
	    
	    myproc ->  define_property_with_mode ("short_property", 
						 any_S,
						 CosPropertyService::normal
						 );	

	    
	    
	    L = 4;
	    any_S <<= L;
	    
	    TEST_INFO ("Re-Defining a Short User defined Attribute with NORMAL Mode and new value: "<< L);
	    
	    mysystem -> define_property ("short_property",
					 any_S);
 	 
	    CORBA::Short l_out_sys; 
	    return_any_val = mysystem-> get_property_value("short_property");
            (*return_any_val) >>= l_out_sys;
	    
	    myapp -> define_property ("short_property",
					 any_S);
	    CORBA::Short l_out_app; 
	    return_any_val = myapp-> get_property_value("short_property");
            (*return_any_val) >>=  l_out_app;
 	 
	    myhost -> define_property ("short_property",
					 any_S);
	    CORBA::Short l_out_host;
	    return_any_val = myhost-> get_property_value("short_property");
            (*return_any_val) >>=  l_out_host;
         
	    myproc -> define_property ("short_property",
					 any_S);
	    CORBA::Short l_out_proc;  
	    return_any_val = myproc-> get_property_value("short_property");
            (*return_any_val) >>=  l_out_proc;
	    
	    if ((l_out_sys==L)&&(l_out_proc==L)&&(l_out_host==L)&&(l_out_app==L)) 
	      {
		TEST_INFO ("Test OK: Re-defined the Short User Defined attribute and got the value:"<< L << " and No Exception Thrown");
		
		CPPUNIT_ASSERT(1);
	      }
	    else CPPUNIT_ASSERT(0);
	
	    
	    std::string str = " string property value ";
	    
	    
	    TEST_INFO ("Defining a user defined attribute of type String whith NORMAL Mode with value ': "<< str.c_str() << "'");
	    
	    CORBA::Any any_str;
	    any_str <<= str.c_str();
	    
	    mysystem -> define_property_with_mode ("str_property",
						   any_str,
						   CosPropertyService::normal
						   );
	    
	    myapp -> define_property_with_mode ("str_property",
						any_str,
						CosPropertyService::normal
						    );
	    myhost -> define_property_with_mode ("str_property",
						 any_str,
						 CosPropertyService::normal
						 );
	    myproc -> define_property_with_mode ("str_property",
						 any_str,
						 CosPropertyService::normal
						 );
	    
	    CORBA::Any any_s_redef;
	    
	    str = "new string property value ";
	    
	    any_s_redef <<= str.c_str();
	    
	    
	    TEST_INFO ("Re-defining a user defined attribute of type String whith NORMAL Mode with value: '"<< str.c_str() << "'");
	    
	    mysystem -> define_property ("str_property",
					 any_s_redef);
	    
	    return_any_val = mysystem-> get_property_value("str_property");
	    const char* str_out_sys ;
	    (*return_any_val) >>=  str_out_sys;
	    
	    
	    myapp -> define_property ("str_property",
					  any_s_redef);
	    return_any_val = myapp-> get_property_value("str_property");
	    const char* str_out_app ;
	    (*return_any_val) >>=  str_out_app;
	    
	    
	    myhost -> define_property ("str_property",
				       any_s_redef);
	    return_any_val = myhost-> get_property_value("str_property");
	    const char* str_out_host ;
	    (*return_any_val) >>=  str_out_host;
	    
	    
	    myproc -> define_property ("str_property",
				       any_s_redef);
	    return_any_val = myproc-> get_property_value("str_property");
	    const char* str_out_proc ;
	    (*return_any_val) >>=  str_out_proc;
	    
	    
	    //TEST_INFO ("Test on Application: Re-defining a user defined attribute of type Short whith NORMAL Mode: ");
	    if ((str_out_sys==str)&&(str_out_app==str)&&(str_out_host==str)&&(str_out_proc==str))
	      {
		CPPUNIT_ASSERT(1);
		
		TEST_INFO ("Test OK: Re-defined the String User Defined attribute and got the value:'"<< str << "' and No Exception Thrown");
	      }
	    
		
	    CORBA::Any any_F;
	    float F = 2.4 ;
	    TEST_INFO ("Defining a Float User defined Attribute of type Short whith NORMAL Mode with value: "<< F);
	    
	    any_F <<= F;
	    
	    mysystem -> define_property_with_mode ("float_property",
						   any_F,
						   CosPropertyService::normal
						   );
	    
	    myapp -> define_property_with_mode ("float_property",
						    any_F,
						    CosPropertyService::normal
						       );

	    myhost -> define_property_with_mode ("float_property",
						 any_F,
						 CosPropertyService::normal
						 );
	    
	    myproc -> define_property_with_mode ("float_property",
						 any_F,
						 CosPropertyService::normal
						 );
	    
	    float FNew = 4.8 ;
	    any_F <<= FNew;
	    TEST_INFO ("Re-Defining a Float User defined Attribute of type Short whith NORMAL Mode with value: "<< FNew);
	    
		
	    mysystem -> define_property ("float_property",
					 any_F
					 );
	    CORBA::Float l_out_sys_float; 
	    return_any_val = mysystem-> get_property_value("float_property");
	    (*return_any_val) >>= l_out_sys_float;
	    
		
	    myapp -> define_property ("float_property",
				      any_F
				      );
	    CORBA::Float l_out_app_float; 
	    return_any_val = myapp-> get_property_value("float_property");
	    (*return_any_val) >>= l_out_app_float;
	    
	    myhost -> define_property ("float_property",
				       any_F
					   );
	    CORBA::Float l_out_host_float; 
	    return_any_val = myhost-> get_property_value("float_property");
	    (*return_any_val) >>= l_out_host_float;
	    
	    myproc -> define_property ("float_property",
				       any_F
				       );
	    CORBA::Float l_out_proc_float; 
	    return_any_val = myproc-> get_property_value("float_property");
	    (*return_any_val) >>= l_out_proc_float;
	    if ((l_out_host_float==FNew)&&(l_out_sys_float==FNew)&&(l_out_proc_float==FNew)&&(l_out_app_float==FNew))
	      {
		TEST_INFO (" Re-defined the Float User defined Attribute and got the value: "<<FNew);
		CPPUNIT_ASSERT(1); 
	      }
	    else CPPUNIT_ASSERT(0);
	    
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
     std::cerr << "unexpected exception" << std::endl;
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


