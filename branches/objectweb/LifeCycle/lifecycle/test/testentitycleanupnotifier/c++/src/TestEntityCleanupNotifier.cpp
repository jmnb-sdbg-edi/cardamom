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


#include "testentitycleanupnotifier/TestEntityCleanupNotifier.hpp"
// NOPNAR: #include "CdmwNamingAndRepository.stub.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "LifeCycle/lifecycle/EntityCleanupNotifier.hpp"

namespace
{
    const char* PROPERTY_NAME = "EntityData";

    
    const char* OBJECT_1_NAME = "OBJECT  1";
    const char* OBJECT_2_NAME = "OBJECT  2";
    const char* OBJECT_3_NAME = "OBJECT  3";
};
namespace Cdmw
{
namespace LifeCycle
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


TestEntityCleanupNotifier::TestEntityCleanupNotifier(CosPropertyService::PropertySetDefFactory_ptr fac)
    throw()
    :Testable("Cdmw::LifeCycle::EntityCleanupNotifier"),
     m_factory(CosPropertyService::PropertySetDefFactory::_duplicate(fac))

{
    // does nothing
}


TestEntityCleanupNotifier::~TestEntityCleanupNotifier()
    throw()
{
    // does nothing
}


TestEntityCleanupNotifier::TestEntityCleanupNotifier(const TestEntityCleanupNotifier& rhs)
    throw()
    :Testable("Cdmw::LifeCycle::EntityCleanupNotifier"),
    m_factory(CosPropertyService::PropertySetDefFactory::_duplicate(rhs.m_factory.in()))
{
    // does nothing
}
		
TestEntityCleanupNotifier&
TestEntityCleanupNotifier::operator=(const TestEntityCleanupNotifier& rhs)
    throw()
{
    if (this != &rhs) {
        m_factory = rhs.m_factory;
    }
    return *this;
}

void TestEntityCleanupNotifier::do_tests()		
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (14);
    
    // This tests EntityCleanupNotifier class
    //-------------------------------------------------
    TEST_INFO("Testing EntityCleanupNotifier Persistence");

    using namespace Cdmw::LifeCycle;

    try {

        // Create a PropertySet
        CosPropertyService::PropertySetDef_var setdef = m_factory-> create_propertysetdef();
        CosPropertyService::PropertySet_var 
            set = CosPropertyService::PropertySet::_narrow(setdef.in());
        
        {
            TEST_INFO("Creating an EntityCleanupNotifier object");
            EntityCleanupNotifier ecn(CdmwLifeCycle::ObjectRegistration::_nil());
            
            TEST_INFO("First init()");
            TEST_CHECK(ecn.init(set.in(),PROPERTY_NAME));
            
            TEST_INFO("Testing that Property is not defined");
            TEST_CHECK(!set->is_property_defined(PROPERTY_NAME));
            
            TEST_INFO("Testing that Property is not defined");
            TEST_CHECK(ecn.entries() == 0);
            
            // Create 2 Entries
            TEST_INFO("Inserting two entries");
            {
                PortableServer::ObjectId_var oid
                    = PortableServer::string_to_ObjectId(OBJECT_1_NAME);
                ecn.insert_entry(oid.in(),OBJECT_1_NAME);
            }
            {
                PortableServer::ObjectId_var oid
                    = PortableServer::string_to_ObjectId(OBJECT_2_NAME);
                ecn.insert_entry(oid.in(),OBJECT_2_NAME);
            }
            
            TEST_INFO("Checking that entries are inserted.");
            TEST_CHECK(ecn.entries() == 2);            
            
            // Dump entries
            ecn.dump(std::cout);
            
            TEST_CHECK(ecn.finish());
            TEST_INFO("State persisted.");
            

            TEST_INFO("Checking that Property is defined.");
            TEST_CHECK(set->is_property_defined(PROPERTY_NAME));
        } 
        // State is persisted. Retreive it!
        
        {
            TEST_INFO("Creating an EntityCleanupNotifier object");
            EntityCleanupNotifier ecn(CdmwLifeCycle::ObjectRegistration::_nil());
            
            TEST_CHECK(ecn.entries() == 0);
            
            TEST_INFO("Retrieving previous state");
            TEST_CHECK(ecn.init(set.in(),PROPERTY_NAME));
            
            TEST_INFO("Checking number of entries");
            TEST_CHECK(ecn.entries() == 2);
            
            // Dump entries
            ecn.dump(std::cout);
            
            // Add 1 entry
            TEST_INFO("Adding another entry");
            {
                PortableServer::ObjectId_var oid
                    = PortableServer::string_to_ObjectId(OBJECT_3_NAME);
                ecn.insert_entry(oid.in(),OBJECT_3_NAME);
            }
            
            TEST_INFO("Checking number of entries.");
            TEST_CHECK(ecn.entries() == 3);
            
            // Dump entries
            ecn.dump(std::cout);
            
            TEST_CHECK(ecn.finish());
            TEST_INFO("New state persisted.");
        } 
        // State is persisted. Retreive it again!
        
        {
            TEST_INFO("Creating an EntityCleanupNotifier object");
            EntityCleanupNotifier ecn(CdmwLifeCycle::ObjectRegistration::_nil());
            
            TEST_CHECK(ecn.entries() == 0);
            
            TEST_INFO("Retrieving previous state");
            TEST_CHECK(ecn.init(set.in(),PROPERTY_NAME));
            
            TEST_INFO("Checking number of entries");
            TEST_CHECK(ecn.entries() == 3);
            
            // Dump entries
            ecn.dump(std::cout);
        } 

    } catch (const CORBA::Exception & e) {
        std::cerr << "ERROR: " << e << std::endl;
    }
}
		
}; // End namespace LifeCyle
}; // End namespace Cdmw

