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


#include <exception>
#include <iostream>
#include <sstream>


#include "testperfbinder/TestHello_impl.hpp"
#include "testperfbinder/TestRepositoryBinder.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"

namespace Cdmw {

// constructor
TestRepositoryBinder::TestRepositoryBinder(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& binder_number)
    : Testable( "TestRepositoryBinder" ),
      m_binder_number(binder_number)    
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestRepositoryBinder::~TestRepositoryBinder()
{
}




// do_tests
void TestRepositoryBinder::do_tests()
{
    set_nbOfRequestedTestOK (41);

    try
    {
        
        std::cout << "Init Repository" << std::endl;
        
        CdmwNamingAndRepository::Repository_var repository =
        CdmwNamingAndRepository::Repository::_nil();
        
        
        CORBA::Object_var obj =
        PlatformMngt::PlatformInterface::Get_service(PlatformMngt::ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
        repository = CdmwNamingAndRepository::Repository::_narrow(obj.in());
        
        
        CosNaming::NamingContextExt_var defaultRootCtx;
        
        // ---------------
        TEST_INFO("Get the default root context");
        
        try
        {
            
            CosNaming::NamingContext_var temp_defaultRootCtx =
            repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
            
            defaultRootCtx = CosNaming::NamingContextExt::_narrow(temp_defaultRootCtx.in());
            TEST_SUCCEED();
            
        }
        catch(CORBA::Exception &e)
        {
            std::cout << e._name();
            TEST_FAILED();
        }
        catch(...)
        {
            std::cout << "Cannot get Root Context" << std::endl;
            TEST_FAILED();
            return;
        }
    
        
        int bind_number = ::atoi(m_binder_number.c_str());
        TEST_INFO("bind " << bind_number << " object(s) ");
        

        Cdmw::HelloInterface1s list;
        list.length(bind_number);
        
        for (int inc = 0; inc< bind_number; inc++)
        {
            std::stringstream stream;
            
            stream << "object"<<inc<<std::ends;
            Cdmw::HelloInterface1_impl* impl = new Cdmw::HelloInterface1_impl(m_orb.in(),
                                                                              m_rootPOA.in(),
                                                                              stream.str());
            
            list[inc] = impl->_this();
            CosNaming::Name_var name = new CosNaming::Name;
            name->length(1);
            CosNaming::NameComponent_var nc = new CosNaming::NameComponent;
            nc->id = CORBA::string_dup(stream.str().c_str());
            nc->kind = CORBA::string_dup("object");
            name->operator[](0) = nc.in();
        
            try
            {
                defaultRootCtx->bind(name.in(), list[inc].in());
                TEST_SUCCEED();
            }
            catch(CORBA::Exception &e)
            {
                std::cout << e._name();
                TEST_FAILED();
            }
        }


        for (int inc = 0; inc< bind_number; inc++)
        {
            std::stringstream stream;
            
            stream << "object"<<inc<<std::ends;

            CosNaming::Name_var name = new CosNaming::Name;
            name->length(1);
            CosNaming::NameComponent_var nc = new CosNaming::NameComponent;
            nc->id = CORBA::string_dup(stream.str().c_str());
            nc->kind = CORBA::string_dup("object");
            name->operator[](0) = nc.in();
            
            try
            {
                obj = defaultRootCtx->resolve(name.in());
                
                Cdmw::HelloInterface1_var res_obj1 =Cdmw::HelloInterface1::_narrow(obj.in());
                
                std::string res = res_obj1->hello();
                if (strcmp(res.c_str(), stream.str().c_str()) == 0)
                    TEST_SUCCEED();
                else
                    TEST_FAILED();
                
            }
            catch(CORBA::Exception &e)
            {
                std::cout << e._name();
                TEST_FAILED();
            }
        }

    }
    
    catch(const CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e )
    {
        std::cerr << e.what() << std::endl;
        TEST_FAILED();
    }


}


}; // namespace Cdmw

