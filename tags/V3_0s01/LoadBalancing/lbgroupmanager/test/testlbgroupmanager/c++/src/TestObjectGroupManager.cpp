/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
 * @brief _implementation for TestObjectGroupManager with cppUnit library.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 *
 */

#include <Foundation/orbsupport/CORBA.hpp>
#include "testlbgroupmanager/TestObjectGroupManager.hpp"
#include "testlbgroupmanager/CORBAManager.hpp"
#include "idllib/CdmwLBGroupManager.stub.hpp"
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <exception>
#include <iostream>


#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif


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

CdmwLB::LBGroupManager_var TestObjectGroupManager::M_gm;
CORBA::ORB_var TestObjectGroupManager::M_orb;
CdmwGroupManager::HelloInterface1_var TestObjectGroupManager::M_obj1;
CdmwGroupManager::HelloInterface2_var TestObjectGroupManager::M_obj2;
CdmwGroupManager::HelloInterface1_var TestObjectGroupManager::M_obj3;
/*****************************************************************************/

    CPPUNIT_TEST_SUITE_REGISTRATION( TestObjectGroupManager );

/*****************************************************************************/
    
 
void
TestObjectGroupManager::setUp()
{
    M_orb = CORBAManager::instance()->get_orb();
    CORBA::Object_var obj = M_orb->resolve_initial_references("LBGroupManager");    
    //    CORBA::Object_var obj = M_orb->string_to_object
    this->M_gm = CdmwLB::LBGroupManager::_narrow(obj.in());
    CPPUNIT_ASSERT(!CORBA::is_nil(this->M_gm.in() ) );

    // create two objects hello1
    obj1_impl = new Cdmw::HelloInterface1_impl();
    obj3_impl = new Cdmw::HelloInterface1_impl();

    // create an object hello2
    obj2_impl = new Cdmw::HelloInterface2_impl();

    M_obj1 = obj1_impl->_this();
    M_obj2 = obj2_impl->_this();
    M_obj3 = obj3_impl->_this();
}


/*****************************************************************************/


void
TestObjectGroupManager::tearDown()
{
}

/*****************************************************************************/

void
TestObjectGroupManager::add_member()
{
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
        
    CORBA::Object_var obj1;
    DEBUG_ECHO("\n");
    DEBUG_DUMP( "   Create a first Object Group   ");
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

    ::PortableGroup::Location loc;
    loc.length(3);
    loc[0].id="HOST1";
    loc[0].kind="hostname1";
    loc[1].id="APPL1";
    loc[1].kind="applicationname1";
    loc[2].id="PROC1";
    loc[2].kind="processname1";


    DEBUG_DUMP( "   Create a new Object Group   ");
    // create a new object group
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

    DEBUG_DUMP( "   Add a new member on the Object Group   ");
    // add a new member on the group 
    try
        {
            obj1 = M_gm->add_member(obj1.in(),
                                    loc,
                                    M_obj1.in());
            CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
        } catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            CPPUNIT_FAIL("Error in adding object group");            
        }
 
    DEBUG_DUMP( "   Add again the same member on the group (MemberAlreadyPresent exception must be raised)   ");
    // add again the same member on the group
    // the MemberAlreadyPresent exception must be raised
    CPPUNIT_ASSERT_THROW(obj1 = M_gm->add_member(obj1.in(),
                                                 loc,
                                                 M_obj1.in()),
                         ::PortableGroup::MemberAlreadyPresent);
      
    loc[0].id="HOST2";
    
    DEBUG_DUMP( "   Add a new member on the group with a bad type Id (ObjectNotAdded exception must be raised)   ");
    // add a new member on the group with a bad type Id 
    // the  ObjectNotAdded exception must be raised
    CPPUNIT_ASSERT_THROW(obj1 = M_gm->add_member(obj1.in(),
                                                 loc,
                                                 M_obj2.in()),
                         ::PortableGroup::ObjectNotAdded);
    
    DEBUG_DUMP( "   Delete the Object Group   ");
    // delete the object group
    try
        {
            M_gm->delete_object(factory_creation_id1.in());
        }
    
    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in deleting object group");
        }
      

    DEBUG_DUMP("   Add a new menber on the group previously deleted (ObjectGroupNotFound exception must be raised)   ");
    // add a new menber on the group previously deleted
    // the ::PortableGroup::ObjectGroupNotFound exception must be raised
    CPPUNIT_ASSERT_THROW(M_gm->add_member(obj1.in(),
                                          loc,
                                          M_obj1.in()),
                         ::PortableGroup::ObjectGroupNotFound);
      
}

void
TestObjectGroupManager::remove_member()
{
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
        
    CORBA::Object_var obj1;

    DEBUG_ECHO("\n"); 
    DEBUG_DUMP("   Create a first object group   ");
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

    ::PortableGroup::Location loc;
    loc.length(3);
    loc[0].id="HOST1";
    loc[0].kind="hostname1";
    loc[1].id="APPL1";
    loc[1].kind="applicationname1";
    loc[2].id="PROC1";
    loc[2].kind="processname1";


    DEBUG_DUMP( "   Add a new member on the Object Group   ");
    // add a new member on the group 
    try
        {
            obj1 = M_gm->add_member(obj1.in(),
                                    loc,
                                    M_obj1.in());
            CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
        } catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            CPPUNIT_FAIL("Error in adding object group");            
        }

    DEBUG_DUMP("   Remove the new member on the group (No exception must be raised)  ");
    // remove a new member on the group with a bad location
    // the exception MemberNotFound must be raised
    CPPUNIT_ASSERT_NO_THROW(M_gm->remove_member(obj1.in(),
                                                loc));
                      
    
    
    DEBUG_DUMP("   Remove again the same member on the group (MemberNotFound exception must be raised)  ");
    // remove a new member on the group with a bad location
    // the exception MemberNotFound must be raised
    CPPUNIT_ASSERT_THROW(M_gm->remove_member(obj1.in(),
                                             loc),
                         ::PortableGroup::MemberNotFound);
    
    ::PortableGroup::Location empty_loc;
    empty_loc.length(0);
    DEBUG_DUMP("   Remove a member on the group with an empty location(MemberNotFound exception must be raised)  ");
    // remove a new member on the group with a bad location
    // the exception MemberNotFound must be raised
    CPPUNIT_ASSERT_THROW(M_gm->remove_member(obj1.in(),
                                             loc),
                         ::PortableGroup::MemberNotFound);
    

    DEBUG_DUMP( "   Delete the Object Group   ");
    // delete the object group
    try
        {
            M_gm->delete_object(factory_creation_id1.in());
        }
    
    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in deleting object group");
        }


    DEBUG_DUMP("   Remove a new member on the group previously deleted (ObjectGroupNotFound exception must be raised)   ");
    // remove a new menber on the group previously deleted
    // the ::PortableGroup::ObjectGroupNotFound exception must be raised
    CPPUNIT_ASSERT_THROW(M_gm->remove_member(obj1.in(),
                                             loc),
                         ::PortableGroup::ObjectGroupNotFound);
}

void
TestObjectGroupManager::get_object_group_id()
{
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
        
    CORBA::Object_var obj1;
    
    DEBUG_ECHO("\n"); 
    DEBUG_DUMP("   Create a first object group   ");
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

    // get_object_group_id on the group created
    ::PortableGroup::ObjectGroupId objID = 0;
    DEBUG_DUMP("   Get_object_group_id on the group...   ");
    CPPUNIT_ASSERT_NO_THROW(objID = M_gm->get_object_group_id(obj1.in()));
    DEBUG_DUMP("   ...ObjectGroupId --> " << objID);

    DEBUG_DUMP("   Delete the first object group   ");
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

    DEBUG_DUMP("   Get_object_group_id on the group previously deleted (ObjectGroupNotFound exception must be raised)   ");
    // get_object_group_id on the group previously deleted
    // the ::PortableGroup::ObjectGroupNotFound exception must be raised
    CPPUNIT_ASSERT_THROW(M_gm->get_object_group_id(obj1.in()),
                         ::PortableGroup::ObjectGroupNotFound);

     DEBUG_DUMP("   Get_object_group_id on the Hello object (CORBA::INV_OBJREF exception must be raised)   ");
    // get_object_group_id on the group previously deleted
    // the ::PortableGroup::ObjectGroupNotFound exception must be raised
    CPPUNIT_ASSERT_THROW(M_gm->get_object_group_id(M_obj1.in()),
                         CORBA::INV_OBJREF);

}

void
TestObjectGroupManager::get_object_group_ref()
{
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
        
    CORBA::Object_var obj1;
    
    DEBUG_ECHO("\n"); 
    DEBUG_DUMP("   Create a first object group   ");
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

    // get_object_group_ref on the group created
    PortableGroup::ObjectGroup_var objRef;
    DEBUG_DUMP("   Get_object_group_ref on the group...   ");
    CPPUNIT_ASSERT_NO_THROW(objRef = M_gm->get_object_group_ref(obj1.in()));
    DEBUG_DUMP("   ...ObjectGroupReference --> " << M_orb->object_to_string(objRef.in()));
    
    // get_object_group_ref from the group id
    ::PortableGroup::ObjectGroupId objID=0;
    CPPUNIT_ASSERT_NO_THROW(objID = M_gm->get_object_group_id(obj1.in()));
    
    DEBUG_DUMP("   Get_object_group_ref_from_id on the group...   ");
    CPPUNIT_ASSERT_NO_THROW(objRef = M_gm->get_object_group_ref_from_id(objID));
    DEBUG_DUMP("   ...ObjectGroupReference --> " << M_orb->object_to_string(objRef.in()));
    
    DEBUG_DUMP("   Delete the first object group   ");
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

    DEBUG_DUMP("   Get_object_group_ref on the group previously deleted (ObjectGroupNotFound exception must be raised)   ");
    // get_object_group_ref on the group previously deleted
    // the ::PortableGroup::ObjectGroupNotFound exception must be raised
    CPPUNIT_ASSERT_THROW(M_gm->get_object_group_ref(obj1.in()),
                         ::PortableGroup::ObjectGroupNotFound);

    DEBUG_DUMP("   Get_object_group_ref_from_id on the group previously deleted (ObjectGroupNotFound exception must be raised)   ");
    // get_object_group_ref on the group previously deleted
    // the ::PortableGroup::ObjectGroupNotFound exception must be raised
    CPPUNIT_ASSERT_THROW(M_gm->get_object_group_ref_from_id(objID),
                         ::PortableGroup::ObjectGroupNotFound);
}

void
TestObjectGroupManager::get_object_group_version_from_ref()
{
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
        
    CORBA::Object_var obj1;
    
    DEBUG_ECHO("\n"); 
    DEBUG_DUMP("   Create a first object group   ");
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
    ::PortableGroup::Location loc;
    loc.length(3);
    loc[0].id="HOST1";
    loc[0].kind="hostname1";
    loc[1].id="APPL1";
    loc[1].kind="applicationname1";
    loc[2].id="PROC1";
    loc[2].kind="processname1";

    ::PortableGroup::ObjectGroupRefVersion og_version = 0;

    DEBUG_DUMP( "   Get object_group_version_from_ref of the object group created...it should be 1   ");
    CPPUNIT_ASSERT_NO_THROW(og_version = M_gm->get_object_group_version_from_ref(obj1.in()));
    DEBUG_DUMP( "   Object Group Version =  " << og_version );
    
    DEBUG_DUMP( "   Add a new member on the Object Group   ");
    // add a new member on the group 
    try
      {
	obj1 = M_gm->add_member(obj1.in(),
				loc,
				M_obj1.in());
	CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
      } catch( const CORBA::Exception& e )
        {
	  std::cerr << e << std::endl;;
	  CPPUNIT_FAIL("Error in adding object group");            
        }
    
    DEBUG_DUMP( "   Get object_group_version_from_ref of the object group modified...it should be 2   ");
    CPPUNIT_ASSERT_NO_THROW(og_version = M_gm->get_object_group_version_from_ref(obj1.in()));
    DEBUG_DUMP( "   NEW Object Group Version =  " << og_version );
    DEBUG_DUMP("   Delete the first object group   ");
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
    DEBUG_DUMP( "   Get object_group_version_from_ref of the object group previously deleted (ObjectGroupNotFound exception must be raised)   ");
    CPPUNIT_ASSERT_THROW(M_gm->get_object_group_version_from_ref(obj1.in()),
                         ::PortableGroup::ObjectGroupNotFound);
}

void
TestObjectGroupManager::get_object_group_version_from_gid()
{
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
        
    CORBA::Object_var obj1;
    
    DEBUG_ECHO("\n"); 
    DEBUG_DUMP("   Create a first object group   ");
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
    ::PortableGroup::Location loc;
    loc.length(3);
    loc[0].id="HOST1";
    loc[0].kind="hostname1";
    loc[1].id="APPL1";
    loc[1].kind="applicationname1";
    loc[2].id="PROC1";
    loc[2].kind="processname1";

    ::PortableGroup::ObjectGroupRefVersion og_version = 0;
    ::PortableGroup::ObjectGroupId objID = 0;
    CPPUNIT_ASSERT_NO_THROW(objID = M_gm->get_object_group_id(obj1.in()));
    
    DEBUG_DUMP( "   Get object_group_version_from_gid of the object group created...it should be 1   ");
    CPPUNIT_ASSERT_NO_THROW(og_version = M_gm->get_object_group_version_from_gid(objID));
    DEBUG_DUMP( "   Object Group Version =  " << og_version );
    
    DEBUG_DUMP( "   Add a new member on the Object Group   ");
    // add a new member on the group 
    try
      {
	obj1 = M_gm->add_member(obj1.in(),
				loc,
				M_obj1.in());
	CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
      } catch( const CORBA::Exception& e )
        {
	  std::cerr << e << std::endl;;
	  CPPUNIT_FAIL("Error in adding object group");            
        }
    
    DEBUG_DUMP( "   Get object_group_version_from_ref of the object group modified...it should be 2   ");
    CPPUNIT_ASSERT_NO_THROW(og_version = M_gm->get_object_group_version_from_gid(objID));
    DEBUG_DUMP( "   NEW Object Group Version =  " << og_version );
    DEBUG_DUMP("   Delete the first object group   ");
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
    DEBUG_DUMP( "   Get object_group_version_from_ref of the object group previously deleted (ObjectGroupNotFound exception must be raised)   ");
    CPPUNIT_ASSERT_THROW(M_gm->get_object_group_version_from_gid(objID),
                         ::PortableGroup::ObjectGroupNotFound);
}


void
TestObjectGroupManager::get_member_ref()
{
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
        
    CORBA::Object_var obj1;
    
    DEBUG_ECHO("\n"); 
    DEBUG_DUMP("   Create a first object group   ");
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
    
    ::PortableGroup::Location loc;
    loc.length(3);
    loc[0].id="HOST1";
    loc[0].kind="hostname1";
    loc[1].id="APPL1";
    loc[1].kind="applicationname1";
    loc[2].id="PROC1";
    loc[2].kind="processname1";

    DEBUG_DUMP( "   Add a new member on the Object Group   ");
    // add a new member on the group 
    try
        {
            obj1 = M_gm->add_member(obj1.in(),
                                    loc,
                                    M_obj1.in());
            CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
        } catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            CPPUNIT_FAIL("Error in adding object group");            
        }

    // get_member_ref on the group for the member just added
    CORBA::Object_var objRef;
    DEBUG_DUMP("   Get_member_ref on the group...   ");
    CPPUNIT_ASSERT_NO_THROW(objRef = M_gm->get_member_ref(obj1.in(), loc));
    DEBUG_DUMP("   ...MemberReference --> " << M_orb->object_to_string(objRef.in()));


    // get_member_ref on the group with a bad location
    // the exception ObjectNotAdded must be raised
    loc[0].id = "HOST2";
    DEBUG_DUMP( "Get_member_ref on the group with a bad location (MemberNotFound exception must be raised)");
    CPPUNIT_ASSERT_THROW(M_gm->get_member_ref(obj1.in(), loc),
                         ::PortableGroup::MemberNotFound);
    
    DEBUG_DUMP( "   Delete the Object Group   ");
    // delete the object group
    try
        {
            M_gm->delete_object(factory_creation_id1.in());
        }
    
    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in deleting object group");
        }
   
    DEBUG_DUMP("   Get_member_ref on the group previously deleted (ObjectGroupNotFound exception must be raised)   ");
    // get_member_ref on the group previously deleted
    // the ::PortableGroup::ObjectGroupNotFound exception must be raised
    CPPUNIT_ASSERT_THROW(M_gm->get_member_ref(obj1.in(),
                                              loc),
                         ::PortableGroup::ObjectGroupNotFound);
}



void
TestObjectGroupManager::locations_of_members()
{
    ::PortableGroup::Locations_var locations;
    // set lb_policy property
    
    ::PortableGroup::Properties prop;
    
    prop.length(1);
    prop[0].nam.length(1);
    prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
    std::string lb_policy = "default";
    prop[0].val <<=  lb_policy.c_str();
    
    const char * rid = "IDL:CdmwGroupManager/HelloInterface1:1.0";
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
    ::PortableGroup::Criteria crit;        
    crit.length(1);
    crit[0].nam.length(1);
    crit[0].nam[0].id="cdmw.lb.LBProperties";
    crit[0].val <<= prop;
        
    CORBA::Object_var obj1;
    
    DEBUG_ECHO("\n"); 
    DEBUG_DUMP("   Create a first object group   ");
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
    
    ::PortableGroup::Location loc;
    loc.length(3);
    loc[0].id="HOST1";
    loc[0].kind="hostname1";
    loc[1].id="APPL1";
    loc[1].kind="applicationname1";
    loc[2].id="PROC1";
    loc[2].kind="processname1";

    DEBUG_DUMP( "   Add the first member on the Object Group    ");
    // add a new member on the group 
    try
        {
            obj1 = M_gm->add_member(obj1.in(),
                                    loc,
                                    M_obj1.in());
            CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
        } catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            CPPUNIT_FAIL("Error in adding object group");            
        }

    loc[0].id = "HOST2";
    loc[0].kind="hostname2";
    loc[1].id="APPL2";
    loc[1].kind="applicationname2";
    loc[2].id="PROC2";
    loc[2].kind="processname2";

    DEBUG_DUMP( "   Add the second member on the Object Group    ");
    // add a new member on the group 
    try
        {
            obj1 = M_gm->add_member(obj1.in(),
                                    loc,
                                    M_obj3.in());
            CPPUNIT_ASSERT(!CORBA::is_nil(obj1.in() ) );
        } catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;;
            CPPUNIT_FAIL("Error in adding object group");            
        }

    // locations_of_members on the group created,
    // no exception must be raised
    DEBUG_DUMP("   Locations_of_members on the group (No exception must be raised)   ");
    CPPUNIT_ASSERT_NO_THROW(locations = M_gm->locations_of_members(obj1.in()));
    for (CORBA::ULong i = 0; i<locations->length(); i++)
        {
            const std::string loc = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(locations[i]);
            DEBUG_DUMP("       Location Member " << i << " --> " << loc);
        }
    
    
    DEBUG_DUMP( "   Delete the Object Group   ");
    // delete the object group
    try
        {
            M_gm->delete_object(factory_creation_id1.in());
        }
    
    catch( const CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            CPPUNIT_FAIL("Error in deleting object group");
        }
    
    DEBUG_DUMP("   Locations_of_members on the group previously deleted (ObjectGroupNotFound exception must be raised)    ");
    // get_member_ref on the group previously deleted
    // the ::PortableGroup::ObjectGroupNotFound exception must be raised
    CPPUNIT_ASSERT_THROW(locations = M_gm->locations_of_members(obj1.in()),
                         ::PortableGroup::ObjectGroupNotFound);
   
}


