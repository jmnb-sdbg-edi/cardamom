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

#include "repositoryinterface/RepositoryInterfaceTest.hpp"

#include <Foundation/common/Assert.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

 
namespace {
    const int POA_THREAD_POOL_SIZE = 2;
    
    const std::string DEFAULT_NAME_DOMAIN_INVALID = "%$.*\\'/é|..,\n;¤µ€";
    const std::string DEFAULT_NAME_DOMAIN_UNKNOWN = "UndefinedNameDomain";
    const std::string DEFAULT_NAME_DOMAIN         = "DefaultNameDomain";
    const std::string SUB_NAME_DOMAIN             = "SubNameDomain";
    const std::string OTHER_NAME_DOMAIN           = "AnotherNameDomain";
}



namespace Cdmw
{
namespace NamingAndRepository
{

    using namespace Cdmw::CommonSvcs::Naming;
    
    RepositoryInterfaceTest::RepositoryInterfaceTest(int argc, char* argv[])
        throw ()
        : Testable("Test RepositoryInterface"),
          m_orb(CORBA::ORB::_nil()),
          m_repository(CdmwNamingAndRepository::Repository::_nil())
    {
        // ===================================================
        // Initialize the ORB
        // ===================================================        
        try { 
            OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
            m_orb =  OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Unexpected exception during ORB init: " << e << std::endl;
            CDMW_ASSERT(false);
        }
        
        // ===================================================
        // Get the root POA 
        // ===================================================
        PortableServer::POA_var rootPOA;
        try {
            CORBA::Object_var obj = m_orb->resolve_initial_references("RootPOA");
            rootPOA = PortableServer::POA::_narrow(obj.in());
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Unexpected exception during get root POA: " << e << std::endl;
            CDMW_ASSERT(false);
        }
        
        // ===================================================
        // Activate the root POA manager
        // ===================================================
        try {
            PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
            poaMgr->activate();
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Unexpected exception during POA manager activate: " << e << std::endl;
            CDMW_ASSERT(false);
        }
    }
    
    
    void RepositoryInterfaceTest::init(const char* namingReposURL)
        throw (CORBA::SystemException)
    {
        // ===================================================
        // Get the naming and repository
        //     from the input URL
        // ===================================================
        CORBA::Object_var obj = m_orb->string_to_object(namingReposURL);
        m_repository = CdmwNamingAndRepository::Repository::_narrow(obj.in());            
    }
    
    
    void RepositoryInterfaceTest::destroyORB()
        throw ()
    {
        m_repository = CdmwNamingAndRepository::Repository::_nil();
        if (! CORBA::is_nil(m_orb.in())) {
            try {
                m_orb->destroy();
                m_orb = CORBA::ORB::_nil();
            } catch (const CORBA::SystemException& e) {
                std::cerr << "Unexpected exception during ORB destroy: " << e << std::endl;
            }
        }
    }
    
    
    void RepositoryInterfaceTest::do_tests()
    {
        // set number of requested successfull tests
        set_nbOfRequestedTestOK(9);
        
        TEST_INFO("Try RepositoryInterface.init with invalid domain name");
        try {
            RepositoryInterface::init(DEFAULT_NAME_DOMAIN_INVALID, m_repository.in());
            TEST_INFO("expected InvalidName exception bas not been thrown");
            TEST_FAILED();
        } catch (const CdmwNamingAndRepository::InvalidName& e) {
            TEST_INFO("expected InvalidName exception catched");
            TEST_SUCCEED();
        } catch (const CORBA::Exception& e) {
            TEST_INFO("Unexpected exception: " << e);
            TEST_FAILED();
        }
        
        TEST_INFO("Try RepositoryInterface.init with unwkown domain name");
        try {
            RepositoryInterface::init(DEFAULT_NAME_DOMAIN_UNKNOWN, m_repository.in());
            TEST_INFO("expected NoNameDomain exception bas not been thrown");
            TEST_FAILED();
        } catch (const CdmwNamingAndRepository::NoNameDomain& e) {
            TEST_INFO("expected NoNameDomain exception catched");
            TEST_SUCCEED();
        } catch (const CORBA::Exception& e) {
            TEST_INFO("Unexpected exception: " << e);
            TEST_FAILED();
        }
        
        TEST_INFO("RepositoryInterface.init with valid domain name");
        try {
            RepositoryInterface::init(DEFAULT_NAME_DOMAIN, m_repository.in());
            TEST_INFO("RepositoryInterface successfully initialised");
            TEST_SUCCEED();
        } catch (const CORBA::Exception& e) {
            TEST_INFO("Unexpected exception: " << e);
            TEST_FAILED();
        }
        
        TEST_INFO("Test getRepository()");
        try {
            CdmwNamingAndRepository::Repository_var rep = 
                RepositoryInterface::get_repository();
            if (m_repository->_is_equivalent(rep.in()))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        } catch (const CORBA::SystemException& e) {
            TEST_INFO("Unexpected exception: " << e);
            TEST_FAILED();
        }
        

        TEST_INFO("Get NamingInterface for default name domain");
        try {
            NamingInterface ni = RepositoryInterface::get_domain_naming_interface();
            try {
                // check that ni correspond to default name domain
                CORBA::Object_var obj = ni.resolve(SUB_NAME_DOMAIN);
                TEST_SUCCEED();
            } catch (const CosNaming::NamingContext::NotFound& e) {
                TEST_INFO("NamingInterface doesn't correspond to default name domain");
                TEST_FAILED();
            }
        } catch (const CORBA::Exception& e) {
            TEST_INFO("Cannot retrieve default name domain: " << e);
            TEST_FAILED();
        }


        TEST_INFO("Get NamingContext for default name domain");
        try {
            CosNaming::NamingContext_var nc = RepositoryInterface::get_domain_context();
            try {
                // check that nc correspond to default name domain
                CosNaming::Name_var name = 
                    NamingInterface::to_name(SUB_NAME_DOMAIN);
                CORBA::Object_var obj = nc->resolve(name);
                TEST_SUCCEED();
            } catch (const CosNaming::NamingContext::NotFound& e) {
                TEST_INFO("NamingContext doesn't correspond to default name domain");
                TEST_FAILED();
            }
        } catch (const CORBA::Exception& e) {
            TEST_INFO("Cannot retrieve default name domain: " << e);
            TEST_FAILED();
        }


        TEST_INFO("Get default name domain");
        try {
            CdmwNamingAndRepository::NameDomain_var nd = 
                RepositoryInterface::get_domain();
            try {
                // check that nd correspond to default name domain
                CdmwNamingAndRepository::NameDomain_var subnd = 
                    nd->resolve_sub_domain(SUB_NAME_DOMAIN.c_str());
                TEST_SUCCEED();
            } catch (const CdmwNamingAndRepository::NoNameDomain& e) {
                TEST_INFO("NameDomain doesn't correspond to default name domain");
                TEST_FAILED();
            }
        } catch (const CORBA::Exception& e) {
            TEST_INFO("Cannot retrieve default name domain: " << e);
            TEST_FAILED();
        }

        TEST_INFO("Try to get an unkown name domain");
        try {
            CdmwNamingAndRepository::NameDomain_var nd = 
                RepositoryInterface::get_domain(DEFAULT_NAME_DOMAIN_UNKNOWN);
            TEST_INFO("expected NoNameDomain exception has not been thrown");
            TEST_FAILED();
        } catch (const CdmwNamingAndRepository::NoNameDomain& e) {
            TEST_INFO("expected NoNameDomain exception catched");
            TEST_SUCCEED();
        } catch (const CdmwNamingAndRepository::InvalidName& e) {
            TEST_INFO("Name " << DEFAULT_NAME_DOMAIN_UNKNOWN << " is an invalid name");
            TEST_FAILED();
        }
        
        TEST_INFO("Try to get another name domain");
        try {
            CdmwNamingAndRepository::NameDomain_var nd = 
                RepositoryInterface::get_domain(OTHER_NAME_DOMAIN);
            try {
                // check that nd doesn't correspond to default name domain
                CdmwNamingAndRepository::NameDomain_var subnd = 
                    nd->resolve_sub_domain(SUB_NAME_DOMAIN.c_str());
                TEST_INFO("returned NameDomain correspond to default name domain");
                TEST_FAILED();
            } catch (const CdmwNamingAndRepository::NoNameDomain& e) {
                TEST_SUCCEED();
            }
        } catch (const CORBA::SystemException& e) {
            TEST_INFO("Cannot retrieve other name domain: " << e);
            TEST_FAILED();
        }
        
        
    
        // ================================================================
        // Release all static object references held by RepositoryInterface
        // ================================================================
        RepositoryInterface::finish();
    }
    
};  // namespace NamingAndRepository

};  // namespace Cdmw
    

