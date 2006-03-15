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


#include <exception>
#include <iostream>
#include <sstream>


#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "testftrequestlogging2/TestRequestLogging.hpp"
#include "testftrequestlogging2/TestHelloLogger_impl.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"

#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>


namespace Cdmw {
namespace FT {

TestRequestLogging::TestRequestLogging (
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    const std::string& host1,
    const std::string& host2,
    const std::string& host3)
    : Testable( name ),
      m_host1(host1),
      m_host2(host2),
      m_host3(host3)

{
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestRequestLogging::~TestRequestLogging ()
{
}
	
void 
TestRequestLogging::do_tests()
{
    set_nbOfRequestedTestOK(10);

    try
    {
        TEST_INFO( "Resolve the Replication Manager reference" );

        CORBA::Object_var objref = 
           m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = 
           CdmwFT::ReplicationManager::_narrow(objref.in());
        
        if (CORBA::is_nil(rm.in()))
        {
            std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                      << "could not _narrow object to type HelloInterface" 
                      << std::endl;
            abort();
        }
        
        TEST_SUCCEED();

        TEST_INFO("Create objects groups");
        const char * rid1 = 
           "IDL:thalesgroup.com/CdmwRequestLogging/HelloInterface:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;

        CORBA::Object_var obj1;
        
        // set default properties
        TEST_INFO( "set default properties" );
    
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
        
        prop[3].nam.length(1);
        prop[3].nam[0].id="org.omg.ft.Factories";

        ::FT::Locations locs(3);
        locs.length(3);
        locs[0].length(3);
        locs[0][0].id = m_host1.c_str();
        locs[0][0].kind = "hostname";
        locs[0][1].id = "APPL1";
        locs[0][1].kind = "applicationname";
        locs[0][2].id = "P11";
        locs[0][2].kind = "processname";

        locs[1].length(3);
        locs[1][0].id = m_host2.c_str();
        locs[1][0].kind = "hostname";
        locs[1][1].id = "APPL2";
        locs[1][1].kind = "applicationname";
        locs[1][2].id = "P21";
        locs[1][2].kind = "processname";

        locs[2].length(3);
        locs[2][0].id = m_host3.c_str();
        locs[2][0].kind = "hostname";
        locs[2][1].id = "APPL3";
        locs[2][1].kind = "applicationname";
        locs[2][2].id = "P31";
        locs[2][2].kind = "processname";

        const CORBA::ULong factory_infos_len = locs.length();
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(factory_infos_len);
        for (CORBA::ULong i = 0; i < factory_infos_len; ++i) 
        {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            std::cout << '[' << i << "] " << " --- " 
                      << Cdmw::CommonSvcs::Naming::NamingInterface::to_string
                           (locs[i]) << std::endl;

            factoryInfos[i].the_location = locs[i];
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[i].the_criteria = factoryCrit;
        }

        prop[3].val <<= factoryInfos;
        
        ::FT::Criteria crit1;       
        crit1.length(1);
        crit1[0].nam.length(1);
        crit1[0].nam[0].id="org.omg.ft.FTProperties";
        crit1[0].val <<= prop;
        
        try
        {
            obj1 =  rm->create_object(rid1,
                                      crit1,
                                      factory_creation_id1.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        
        
        ::FT::ObjectGroupId ogi1 = 0;
        try
        {
            ogi1 = rm->get_object_group_id(obj1.in());
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("Register the HelloInterface group ref in repository");
        CdmwNamingAndRepository::Repository_var repository =
             CdmwNamingAndRepository::Repository::_nil();

        CORBA::Object_var obj_repository = 
           m_orb->string_to_object("corbaloc::localhost:42562/CdmwRepository");
        repository = 
           CdmwNamingAndRepository::Repository::_narrow(obj_repository.in());

        
        Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                              repository.in());

        CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("dom1/dom2/dom3");

        std::string group_full_name = "HelloInterfaceGroup";

        // ========================================================
        // Register the HelloInterface object in the HELLO name domain
        // ========================================================
        // reserve the name "HelloInterface" in the domain
        try
        {
            CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (group_full_name.c_str());
            // register the object
            helloDomain->register_object (regId.in() , obj1.in ());
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }

        // create object helloLogger

        Cdmw::HelloLogger_impl * helloLogger_impl = new Cdmw::HelloLogger_impl(m_orb.in(), m_rootPOA.in());
        CdmwRequestLogging::HelloLogger_var hello_logger =  helloLogger_impl->_this();
        std::string logger_full_name = "HelloLogger";

        // ========================================================
        // Register the HelloLogger object in the HELLO name domain
        // ========================================================
        // reserve the name "HelloLogger" in the domain
        try
        {
            CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (logger_full_name.c_str());
            // register the object
            helloDomain->register_object (regId.in() , hello_logger.in ());
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e << std::endl;
            TEST_FAILED();
        }


        std::cout << "//" << std::endl;
        std::cout << "// ###########################################################" << std::endl;
        std::cout << "// ##### FIRST STEP" << std::endl;
        std::cout << "//" << std::endl;

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*1000);


        TEST_INFO("Start server processes");
        
        std::ostringstream proc_init11;
        std::ostringstream proc_init21;
        std::ostringstream proc_init31;
        proc_init11<< " --proc-initialise APPL1 P11 " << m_host1;
        proc_init21<< " --proc-initialise APPL2 P21 " << m_host2;
        proc_init31<< " --proc-initialise APPL3 P31 " << m_host3;

        OsSupport::OS::create_process( "platform_admin.sh" , proc_init21.str());   
        OsSupport::OS::sleep(timescale*3000);
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init31.str());   
        OsSupport::OS::sleep(timescale*3000);
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init11.str());   

        OsSupport::OS::sleep(timescale*3000);
        
        std::ostringstream proc_run11;
        std::ostringstream proc_run21;
        std::ostringstream proc_run31;
        proc_run11<< " --proc-run APPL1 P11 " << m_host1;
        proc_run21<< " --proc-run APPL2 P21 " << m_host2;
        proc_run31<< " --proc-run APPL3 P31 " << m_host3;

        OsSupport::OS::create_process( "platform_admin.sh" , proc_run11.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run21.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run31.str());   

        OsSupport::OS::sleep(timescale*5000);

        TEST_INFO("client started")

        Cdmw::CommonSvcs::Naming::NamingInterface ni =
           Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");
    

        std::string full_name = "dom3/P11HelloInterface";
        
        CORBA::Object_var obj;
        
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }

        CdmwRequestLogging::HelloInterface_var hello1P11 =
            CdmwRequestLogging::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P11.in()) );


        full_name = "dom3/P21HelloInterface";
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }

        CdmwRequestLogging::HelloInterface_var hello1P21 = 
           CdmwRequestLogging::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P21.in()) );

        full_name = "dom3/P31HelloInterface";
        
        
        try
        {
            obj = ni.resolve (full_name);
            TEST_SUCCEED();
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout<< e << std::endl;
            TEST_FAILED();
        }
        
        CdmwRequestLogging::HelloInterface_var hello1P31= 
           CdmwRequestLogging::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P31.in()) );


        TEST_INFO("");
        TEST_INFO("START TEST");
        TEST_INFO("");

        // Add new member on the group
        TEST_INFO("get the reference of the object group 1");
        try
        {
            obj1 = rm->get_object_group_ref(obj1.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        // Add new member on the group
        TEST_INFO("add the new member "<<m_host2<<"/APPL2/P21 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  locs[1],
                                  hello1P21.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        // Add new member on the group
        TEST_INFO("add the new member "<<m_host3<<"/APPL3/P31 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  locs[2],
                                  hello1P31.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        // Add new member on the group
        TEST_INFO("add the new member "<<m_host1<<"/APPL1/P11 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  locs[0],
                                  hello1P11.in());
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        m_hello = CdmwRequestLogging::HelloInterface::_narrow(obj1.in());
        
        OsSupport::OS::sleep(timescale*1000);

        TEST_INFO("Invoke an operation with a return value");
        TEST_CHECK(m_hello->do_something(4) == 16);

        CdmwRequestLogging::LogSeq_var traces = 
           hello_logger->get_log_traces();
        for (CORBA::ULong i=0; i < traces->length(); i++)
        {
           TEST_INFO(traces[i]);
        }

        Cdmw::NamingAndRepository::RepositoryInterface::finish();
    }
    catch (const CORBA::UserException& ex)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test TestRequestLogging Exception caught : " << ex
                     << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (const CORBA::SystemException& ex)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test TestRequestLogging Exception caught : " << ex
                     << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
    catch (...)
    {
        std::ostringstream info_stream1;
        info_stream1 << "Test TestRequestLogging Exception caught : " 
                     << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
}

}; // namespace FT
}; // namespace Cdmw
