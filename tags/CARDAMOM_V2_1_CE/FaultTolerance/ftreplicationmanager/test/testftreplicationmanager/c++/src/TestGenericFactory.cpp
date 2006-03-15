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


#include <Foundation/orbsupport/CORBA.hpp>
#include "testftreplicationmanager/TestGenericFactory.hpp"
#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include <Foundation/ossupport/OS.hpp>
#include <exception>
#include <iostream>
#include <ftreplicationmanager/CdmwFTReplicationManagerPushConsumer.stub.hpp>

namespace Cdmw {
namespace FT {

// constructor
TestGenericFactory::TestGenericFactory(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name )
: Testable( name )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestGenericFactory::~TestGenericFactory()
{
}

// do_tests
void TestGenericFactory::do_tests()
{
    set_nbOfRequestedTestOK (8);

    try
    {
        // Recover the reference of the Replication Manager
        TEST_INFO( "Create the Replication Manager" );

        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());

	    if (CORBA::is_nil(rm.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type HelloInterface" << std::endl;
	        abort();
	    }

        TEST_SUCCEED();

        // set default properties
        TEST_INFO( "set_default_properties" );
    
        ::FT::Properties prop;        
    
        prop.length(4);
        prop[0].nam.length(1);
        prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop[0].val <<= (CORBA::UShort)2;
    
        prop[1].nam.length(1);
        prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
        prop[1].val <<= (CORBA::UShort)2;
    
        prop[2].nam.length(1);
        prop[2].nam[0].id="org.omg.ft.MembershipStyle";
        prop[2].val <<= (CORBA::UShort)0;
            
        ::FT::Location loc;
        loc.length(3);
        loc[0].id = "localhost";
        loc[0].kind = "hostname";
        loc[1].id = "APPL";
        loc[1].kind = "application";
        loc[2].id = "PROC";
        loc[2].kind = "process";    
            
        prop[3].nam.length(1);
        prop[3].nam[0].id="org.omg.ft.Factories";
        
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(1);
        factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
        factoryInfos[0].the_location = loc;
        ::FT::Criteria factoryCrit;        
        factoryCrit.length(0);
        factoryInfos[0].the_criteria = factoryCrit;
        
        prop[3].val <<= factoryInfos;


        TEST_SUCCEED();

        const char * rid = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;
        ::FT::Properties* prop_to_criteria = new ::FT::Properties(prop);        
        ::FT::Criteria crit;        
        crit.length(1);
        crit[0].nam.length(1);
        crit[0].nam[0].id="org.omg.ft.FTProperties";
        crit[0].val <<= prop_to_criteria;

        CORBA::Object_var obj1;
        CORBA::Object_var obj2;
        
        // create a fisrt object with a valid criteria
        TEST_INFO("Create a first object");

        try
        {
            obj1 =  rm->create_object(rid,
                                      crit,
                                      factory_creation_id1.out());
             TEST_SUCCEED();
       }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
     
        if (CORBA::is_nil(obj1.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();

        TEST_INFO("Create a second object");

        prop[2].val <<= (CORBA::UShort)1;
              
        try
        {
            obj2 =  rm->create_object(rid,
                                      crit,
                                      factory_creation_id2.out());
            TEST_SUCCEED();
        }
        

        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
     
        if (CORBA::is_nil(obj2.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();        
     
        ::CdmwFT::ReplicationManagerPushConsumer_var consumer_dumper = 
          ::CdmwFT::ReplicationManagerPushConsumer::_narrow(rm.in());

        consumer_dumper->dump_ogd_datastore();
        consumer_dumper->dump_location_datastore();



        // delete the first object
        TEST_INFO("Delete the first object");
        try
        {
            rm->delete_object(factory_creation_id1.in());    
            TEST_SUCCEED();
        }
        
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        // delete the fisrt object again
        // this must raise an ::FT::ObjectNotFound exception
        TEST_INFO("Delete the first object (second time)");
        try
        {            
            rm->delete_object(factory_creation_id1.in());
        }
        
        catch ( const ::FT::ObjectNotFound& e)
        {
            TEST_SUCCEED(); 
        }
        catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        
    }
    
    catch( const CORBA::Exception& e )
    {
        std::cerr << e << std::endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        std::cerr << e << std::endl;
        TEST_FAILED();
    }


}

}; // namespace FT
}; // namespace Cdmw


