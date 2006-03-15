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
#include "testftreplicationmanager/TestPropertyManager.hpp"
#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include <Foundation/ossupport/OS.hpp>
#include <exception>
#include <iostream>

namespace Cdmw {
namespace FT {

// constructor
TestPropertyManager::TestPropertyManager(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name )
: Testable( name )
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

// destructor
TestPropertyManager::~TestPropertyManager()
{
}

// do_tests
void TestPropertyManager::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (64);

    try
    {
        TEST_INFO( "Create the Replication Manager" );

        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());

	    if (CORBA::is_nil(rm.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type HelloInterface" << std::endl;
	        abort();
	    }







        /************************************************************************/
        /*                default properties                                    */
        /************************************************************************/

        TEST_INFO( "define properties" );
        
        ::FT::Properties prop;

        prop.length(1);
        prop[0].nam.length(1);
        
        prop[0].nam[0].id="org.omg.ft.ReplicationStyle";
        prop[0].val <<= (CORBA::UShort)1;

        TEST_INFO( "set_default_properties on an unsupported properties (ReplicationStyle and FT::COOL_PASSIVE)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].val <<= (CORBA::UShort)10;

        TEST_INFO( "set_default_properties on an invalid properties (ReplicationStyle)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }
        catch( ::FT::InvalidProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
  
        prop[0].val <<= (CORBA::UShort)2;

        TEST_INFO( "set_default_properties on a supported properties (ReplicationStyle and FT::WARM_PASSIVE)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_SUCCEED();

        }        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].nam[0].id="org.omg.ft.MembershipStyle";
        prop[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_default_properties on a supported properties (MembershipStyle and FT::MEMB_APP_CTRL)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].val <<= (CORBA::UShort)1;

        TEST_INFO( "set_default_properties on an unsupported properties (MembershipStyle and FT::MEMB_INF_CTRL)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }  
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].nam[0].id="org.omg.ft.ConsistencyStyle";
        prop[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_default_properties on an unsupported properties (ConsistencyStyle)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].nam[0].id="org.omg.ft.FaultMonitoringStyle";
        prop[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_default_properties on a supported properties (FaultMonitoringStyle and FT::PULL)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].val <<= (CORBA::UShort)1;

        TEST_INFO( "set_default_properties on an unsupported properties (FaultMonitoringStyle and FT::PUSH)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }  
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].nam[0].id="org.omg.ft.FaultMonitoringGranularity";
        prop[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_default_properties on an unsupported properties (FaultMonitoringGranularity)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].nam[0].id="org.omg.ft.Factories";
        prop[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_default_properties on an unsupported properties (Factories and default properties)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }
        catch( ::FT::InvalidProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        prop[0].nam[0].id="org.omg.ft.InitialNumberReplicas";
        prop[0].val <<= (CORBA::UShort)2;

        TEST_INFO( "set_default_properties on an unsupported properties (InitialNumberReplicas)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop[0].val <<= (CORBA::UShort)2;

        TEST_INFO( "set_default_properties (MinimumNumberReplicas)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_SUCCEED();
        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_FAILED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].nam[0].id="org.omg.ft.FaultMonitoringInterval";
        prop[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_default_properties on an unsupported properties (FaultMonitoringInterval)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop[0].nam[0].id="org.omg.ft.CheckpointInterval";
        prop[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_default_properties on an unsupported properties (CheckpointInterval)" );
        try
        {
            rm->set_default_properties( prop );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        ::FT::Properties_var prop2;

        TEST_INFO( "get_default_properties " );
        try
        {
            prop2 = rm->get_default_properties();
            TEST_SUCCEED();
            TEST_INFO("length :"<<prop2->length());
            if (prop2->length() == 4)
                TEST_SUCCEED();
            else
                TEST_FAILED();

            bool property_is_found = false;
            
            for (unsigned int i = 0;  i<prop2->length() ; i++)
            {
                property_is_found = false;
                TEST_INFO("Properties : "<<(*prop2)[i].nam[0].id);


                CORBA::UShort val ;
                (*prop2)[i].val >>= val;
                
                std::string s = (*prop2)[i].nam[0].id._retn();
                
                if ( s == "org.omg.ft.ReplicationStyle")
                {
                    if ((*prop2)[i].val >>= val)
                    {
                        if (val == ::FT::WARM_PASSIVE)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MembershipStyle" )
                {
                    if ((*prop2)[i].val >>= val)
                    {
                        if (val == ::FT::MEMB_APP_CTRL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }

                if ( s == "org.omg.ft.FaultMonitoringStyle" )
                {
                    if ((*prop2)[i].val >>= val)
                    {
                        if (val == ::FT::PULL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MinimumNumberReplicas" )
                {
                    if ((*prop2)[i].val >>= val)
                    {
                        if (val == 2)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( property_is_found == false)
                {
                    TEST_INFO("the property is not known by the system");
                    TEST_FAILED();
                }
                
                
            }
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        
        ::FT::Properties prop3;

        prop3.length(1);
        prop3[0].nam.length(1);
        
        prop3[0].nam[0].id="org.omg.ft.ReplicationStyle";
        TEST_INFO( "remove_default_properties (org.omg.ft.ReplicationStyle)" );
        try
        {
            rm->remove_default_properties( prop3 );
            TEST_SUCCEED();
            
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        prop3[0].nam[0].id="org.omg.ft.FaultMonitoringInterval";

        TEST_INFO( "remove_default_properties on an unsupported properties (org.omg.ft.FaultMonitoringInterval)" );
        try
        {
            rm->remove_default_properties( prop3 );
            TEST_SUCCEED();
            
        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
    

        ::FT::Properties_var prop4;

        TEST_INFO( "get_default_properties after remove_default_properties" );
        try
        {
            prop4 = rm->get_default_properties();
            TEST_SUCCEED();
            TEST_INFO("length :"<<prop4->length());
            if (prop4->length() == 3)
                TEST_SUCCEED();
            else
                TEST_FAILED();

            bool property_is_found = false;
            
            for (unsigned int i = 0;  i<prop4->length() ; i++)
            {
                property_is_found = false;
                TEST_INFO("Properties : "<<(*prop4)[i].nam[0].id);


                CORBA::UShort val ;
                (*prop4)[i].val >>= val;
                
                std::string s = (*prop4)[i].nam[0].id._retn();
                
                if ( s == "org.omg.ft.ReplicationStyle")
                {
                    TEST_FAILED();   
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MembershipStyle" )
                {
                    if ((*prop4)[i].val >>= val)
                    {
                        if (val == ::FT::MEMB_APP_CTRL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }

                if ( s == "org.omg.ft.FaultMonitoringStyle" )
                {
                    if ((*prop4)[i].val >>= val)
                    {
                        if (val == ::FT::PULL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MinimumNumberReplicas" )
                {
                    if ((*prop4)[i].val >>= val)
                    {
                        if (val == 2)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( property_is_found == false )
                {
                    TEST_INFO("the property is not known by the system");
                    TEST_FAILED();
                }
            }
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        /************************************************************************/
        /*                type properties                                       */
        /************************************************************************/
       
        const char * rid = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";
        
                TEST_INFO( "define properties" );
        
        ::FT::Properties prop5;

        prop5.length(1);
        prop5[0].nam.length(1);
        
        prop5[0].nam[0].id="org.omg.ft.ReplicationStyle";
        prop5[0].val <<= (CORBA::UShort)1;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.ReplicationStyle and FT::COOL_PASSIVE)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].val <<= (CORBA::UShort)10;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.ReplicationStyle and InvalidProperty)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }
        catch( ::FT::InvalidProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
  
        prop5[0].val <<= (CORBA::UShort)2;

        TEST_INFO( "set_type_properties on a supported properties (org.omg.ft.ReplicationStyle and FT::WARM_PASSIVE)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_SUCCEED();

        }        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].nam[0].id="org.omg.ft.MembershipStyle";
        prop5[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_type_properties on a supported properties (org.omg.ft.MembershipStyle and FT::MEMB_APP_CTRL)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].val <<= (CORBA::UShort)1;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.MembershipStyle and FT::MEMB_INF_CTRL)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }  
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].nam[0].id="org.omg.ft.ConsistencyStyle";
        prop5[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.ConsistencyStyle)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].nam[0].id="org.omg.ft.FaultMonitoringStyle";
        prop5[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_type_properties on a supported properties (org.omg.ft.FaultMonitoringStyle and FT::PULL)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].val <<= (CORBA::UShort)1;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.FaultMonitoringStyle and FT::PUSH)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }  
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].nam[0].id="org.omg.ft.FaultMonitoringGranularity";
        prop5[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.FaultMonitoringGranularity)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].nam[0].id="org.omg.ft.Factories";
        prop5[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_type_properties on a supported properties (org.omg.ft.Factories and default properties)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_SUCCEED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_FAILED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        prop5[0].nam[0].id="org.omg.ft.InitialNumberReplicas";
        prop5[0].val <<= (CORBA::UShort)2;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.InitialNumberReplicas)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop5[0].val <<= (CORBA::UShort)4;

        TEST_INFO( "set_type_properties (org.omg.ft.MinimumNumberReplicas)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_SUCCEED();
        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_FAILED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].nam[0].id="org.omg.ft.FaultMonitoringInterval";
        prop5[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.FaultMonitoringInterval)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        prop5[0].nam[0].id="org.omg.ft.CheckpointInterval";
        prop5[0].val <<= (CORBA::UShort)0;

        TEST_INFO( "set_type_properties on an unsupported properties (org.omg.ft.CheckpointInterval)" );
        try
        {
            rm->set_type_properties( rid,  prop5 );
            TEST_FAILED();

        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        ::FT::Properties_var prop6;

        TEST_INFO( "get_default_properties after set_type_properties" );
        try
        {
            prop6 = rm->get_default_properties();
            TEST_SUCCEED();
            TEST_INFO("length :"<<prop6->length());
            if (prop6->length() == 3)
                TEST_SUCCEED();
            else
                TEST_FAILED();

            bool property_is_found = false;
            
            for (unsigned int i = 0;  i<prop6->length() ; i++)
            {
                property_is_found = false;
                TEST_INFO("Properties : "<<(*prop6)[i].nam[0].id);


                CORBA::UShort val ;
                (*prop6)[i].val >>= val;
                
                std::string s = (*prop6)[i].nam[0].id._retn();
                
                if ( s == "org.omg.ft.ReplicationStyle")
                {
                    TEST_FAILED();   
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MembershipStyle" )
                {
                    if ((*prop6)[i].val >>= val)
                    {
                        if (val == ::FT::MEMB_APP_CTRL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }

                if ( s == "org.omg.ft.FaultMonitoringStyle" )
                {
                    if ((*prop6)[i].val >>= val)
                    {
                        if (val == ::FT::PULL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MinimumNumberReplicas" )
                {
                    if ((*prop6)[i].val >>= val)
                    {
                        if (val == 2)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( property_is_found == false )
                {
                    TEST_INFO("the property is not known by the system");
                    TEST_FAILED();
                }
            }
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }


        ::FT::Properties_var prop7;

        TEST_INFO( "get_type_properties after set_type_properties" );
        try
        {
            prop7 = rm->get_type_properties(rid);
            TEST_SUCCEED();
            TEST_INFO("length :"<<prop7->length());
            if (prop7->length() == 5)
                TEST_SUCCEED();
            else
                TEST_FAILED();

            bool property_is_found = false;
            
            for (unsigned int i = 0;  i<prop7->length() ; i++)
            {
                property_is_found = false;
                TEST_INFO("Properties : "<<(*prop7)[i].nam[0].id);


                CORBA::UShort val ;
                (*prop7)[i].val >>= val;
                
                std::string s = (*prop7)[i].nam[0].id._retn();
                
                if ( s == "org.omg.ft.ReplicationStyle")
                {
                   if ((*prop7)[i].val >>= val)
                    {
                        if (val == ::FT::WARM_PASSIVE)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;

                }
                
                if ( s == "org.omg.ft.MembershipStyle" )
                {
                    if ((*prop7)[i].val >>= val)
                    {
                        if (val == ::FT::MEMB_APP_CTRL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }

                if ( s == "org.omg.ft.FaultMonitoringStyle" )
                {
                    if ((*prop7)[i].val >>= val)
                    {
                        if (val == ::FT::PULL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MinimumNumberReplicas" )
                {
                    if ((*prop7)[i].val >>= val)
                    {
                        if (val == 4)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                

                if (s == "org.omg.ft.Factories")
                {
                    property_is_found = true;
                }
                
                if ( property_is_found == false )
                {
                    TEST_INFO("the property is not known by the system");
                    TEST_FAILED();
                }
            }
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }
        
        ::FT::Properties prop8;

        prop8.length(2);
        prop8[0].nam.length(1);
        prop8[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop8[1].nam.length(1);
        prop8[1].nam[0].id="org.omg.ft.ReplicationStyle";


        TEST_INFO( "remove_type_properties (org.omg.ft.ReplicationStyle and org.omg.ft.MinimumNumberReplicas)" );
        try
        {
            rm->remove_type_properties(rid,  prop8 );
            TEST_SUCCEED();
            
        }
        catch( ::FT::UnsupportedProperty& e )
        {
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        ::FT::Properties_var prop9;

        TEST_INFO( "get_type_properties after remove_type_properties" );
        try
        {
            prop9 = rm->get_type_properties(rid);
            TEST_SUCCEED();
            TEST_INFO("length :"<<prop9->length());
            if (prop9->length() == 4)
                TEST_SUCCEED();
            else
                TEST_FAILED();

            bool property_is_found = false;
            
            for (unsigned int i = 0;  i<prop9->length() ; i++)
            {
                property_is_found = false;
                TEST_INFO("Properties : "<<(*prop9)[i].nam[0].id);


                CORBA::UShort val ;
                (*prop9)[i].val >>= val;
                
                std::string s = (*prop9)[i].nam[0].id._retn();
                
                if ( s == "org.omg.ft.ReplicationStyle")
                {
                    TEST_FAILED();
                    property_is_found = true;

                }
                
                if ( s == "org.omg.ft.MembershipStyle" )
                {
                    if ((*prop9)[i].val >>= val)
                    {
                        if (val == ::FT::MEMB_APP_CTRL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }

                if ( s == "org.omg.ft.FaultMonitoringStyle" )
                {
                    if ((*prop9)[i].val >>= val)
                    {
                        if (val == ::FT::PULL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MinimumNumberReplicas" )
                {
                    if ((*prop9)[i].val >>= val)
                    {                        
                        if (val == 2)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }

                if (s == "org.omg.ft.Factories")
                {
                    property_is_found = true;
                }

                if ( property_is_found == false )
                {
                    TEST_INFO("the property is not known by the system");
                    TEST_FAILED();
                }
            }
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        TEST_INFO("Create a first object");
        ::FT::ObjectGroup_var obj1;
        try
        {    
            // set default properties
            
            ::FT::Location loc;
            loc.length(3);
            loc[0].id = "localhost";
            loc[0].kind = "hostname";
            loc[1].id = "APPL";
            loc[1].kind = "application";
            loc[2].id = "PROC";
            loc[2].kind = "process";    
    
            ::FT::Properties prop;
    
            prop.length(3);
            prop[0].nam.length(1);
            prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
            prop[0].val <<= (CORBA::UShort)2;
            
//             prop[1].nam.length(1);
//             prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
//             prop[1].val <<= (CORBA::UShort)2;
            
            prop[1].nam.length(1);
            prop[1].nam[0].id="org.omg.ft.MembershipStyle";
            prop[1].val <<= (CORBA::UShort)0;
            
            prop[2].nam.length(1);
            prop[2].nam[0].id="org.omg.ft.Factories";
            
            ::FT::FactoryInfos factoryInfos;
            factoryInfos.length(1);
            factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
            factoryInfos[0].the_location = loc;
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[0].the_criteria = factoryCrit;
        
            prop[2].val <<= factoryInfos;
    
    
            ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
            ::FT::Criteria crit;
            crit.length(1);
            crit[0].nam.length(1);
            crit[0].nam[0].id="org.omg.ft.FTProperties";
            crit[0].val <<= prop;

            obj1 = rm->create_object(rid,
                                     crit,
                                     factory_creation_id.out());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
        }

        if (CORBA::is_nil(obj1.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
        


        TEST_INFO("Get_object_group_id");
        ::FT::ObjectGroupId id = rm->get_object_group_id(obj1.in());
        TEST_INFO("group id "<<id);
        


        ::FT::Properties_var prop10;

        TEST_INFO( "get_properties" );
        try
        {
            prop10 = rm->get_properties(obj1.in());
            TEST_SUCCEED();
            TEST_INFO("length :"<<prop10->length());
            if (prop10->length() == 4)
                TEST_SUCCEED();
            else
                TEST_FAILED();

            bool property_is_found = false;
            
            for (unsigned int i = 0;  i<prop10->length() ; i++)
            {
                property_is_found = false;
                TEST_INFO("Properties : "<<(*prop10)[i].nam[0].id);


                CORBA::UShort val ;
                (*prop10)[i].val >>= val;
                
                std::string s = (*prop10)[i].nam[0].id._retn();
                
                if ( s == "org.omg.ft.ReplicationStyle")
                {
                    TEST_FAILED();
                    property_is_found = true;

                }
                
                if ( s == "org.omg.ft.MembershipStyle" )
                {
                    if ((*prop10)[i].val >>= val)
                    {
                        if (val == ::FT::MEMB_APP_CTRL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }

                if ( s == "org.omg.ft.FaultMonitoringStyle" )
                {
                    if ((*prop10)[i].val >>= val)
                    {
                        if (val == ::FT::PULL)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }
                
                if ( s == "org.omg.ft.MinimumNumberReplicas" )
                {
                    if ((*prop10)[i].val >>= val)
                    {                        
                        if (val == 2)
                            TEST_SUCCEED();
                        else
                            TEST_FAILED();
                    } else {
                        TEST_FAILED();
                    }
                    property_is_found = true;
                }

                if (s == "org.omg.ft.Factories")
                {
                    property_is_found = true;
                }

                if ( property_is_found == false )
                {
                    TEST_INFO("the property is not known by the system");
                    TEST_FAILED();
                }
            }
        }
        catch( ::FT::ObjectGroupNotFound& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();   
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            TEST_FAILED();
        }

        

    }
    
    catch( CORBA::Exception& e )
    {
        std::cerr << e << std::endl;;
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



