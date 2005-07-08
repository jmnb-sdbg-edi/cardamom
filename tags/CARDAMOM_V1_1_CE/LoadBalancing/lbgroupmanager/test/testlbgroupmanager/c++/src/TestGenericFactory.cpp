/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

/**
 * @brief _implementation for TestGenericFactory with cppUnit library.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */

#include <Foundation/orbsupport/CORBA.hpp>
#include "testlbgroupmanager/TestGenericFactory.hpp"
#include <Foundation/ossupport/OS.hpp>
#include <exception>
#include <iostream>
#include "testlbgroupmanager/CORBAManager.hpp"


#define ECHO_HEADER() \
    "[Debug] "

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)


#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment ;\
} while(0)

#endif

CdmwLB::LBGroupManager_var TestGenericFactory::M_gm;
CORBA::ORB_var TestGenericFactory::M_orb;
/*****************************************************************************/

    CPPUNIT_TEST_SUITE_REGISTRATION( TestGenericFactory );

/*****************************************************************************/
    
 
void
TestGenericFactory::setUp()
{
    M_orb = CORBAManager::instance()->get_orb();
    CORBA::Object_var obj = M_orb->resolve_initial_references("LBGroupManager");
    
    this->M_gm = CdmwLB::LBGroupManager::_narrow(obj.in());
    CPPUNIT_ASSERT(!CORBA::is_nil(this->M_gm.in() ) );
}


/*****************************************************************************/


void
TestGenericFactory::tearDown()
{
}

/*****************************************************************************/


void
TestGenericFactory::create_object()
{
    // set lb_policy property
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";

    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(0);
        
    CORBA::Object_var obj1;
    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create a first Object Group without specifying any LB Policy (default policy must be set)   ");
    //create the first object group     
    try
        {
            obj1 =  M_gm->create_object(rid,
                                        crit,
                                        factory_creation_id1.out());
            CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
        }

    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in creating object group");
        }

        // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id2;
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;

    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create a second Object Group specifying Random LB Policy   ");
    try
        {
            obj1 =  M_gm->create_object(rid,
                                        crit,
                                        factory_creation_id2.out());
            CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
        }

    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in creating object group");
        }

    DEBUG_DUMP( "   Delete the first Object Group   ");
    // delete the first object group
    try
        {
            M_gm->delete_object(factory_creation_id1.in());
        }
    
    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in deleting object group");
        }

    DEBUG_DUMP( "   Delete the second Object Group   ");
    // delete the second object group
    try
        {
            M_gm->delete_object(factory_creation_id2.in());
        }
    
    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in deleting object group");
        }
}

void
TestGenericFactory::delete_object()
{
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;

    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";

    CORBA::Object_var obj1;
    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create a  first Object Group   ");
    try
        {
            obj1 =  M_gm->create_object(rid,
                                        crit,
                                        factory_creation_id1.out());
            CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
        }

    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in creating object group");
        }

    DEBUG_DUMP( "   Delete the first Object Group   ");
    // delete the first object group
    try
        {
            M_gm->delete_object(factory_creation_id1.in());
        }
    
    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in deleting object group");
        }
    

    DEBUG_DUMP( "   Delete again the same Object Group (ObjectGroupNotFound exception must be raised)   ");
    CPPUNIT_ASSERT_THROW( M_gm->delete_object(factory_creation_id1.in()),
                          ::PortableGroup::ObjectNotFound);
}


