/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * This file and the information it contains are confidential and proprietary. 
 * They shall not be reproduced nor disclosed to any person except to those 
 * having a need to know them without prior written consent of the owner.
*/
/* ===================================================================== */


#include <exception>
#include <iostream>
#include <sstream>


#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "testftrequestlogging/TestRequestLogging.hpp"
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
    const size_t duration_time,
    const std::string& host1,
    const std::string& host2,
    const std::string& host3)
    : Testable( name ),
      m_duration_time(duration_time),
      m_host1(host1),
      m_host2(host2),
      m_host3(host3)

{
    m_duration_time = duration_time/10;
    m_orb = CORBA::ORB::_duplicate(orb);
    m_rootPOA = PortableServer::POA::_duplicate(rootPOA);
}

TestRequestLogging::~TestRequestLogging ()
{
}
	
void 
TestRequestLogging::do_tests()
{
    set_nbOfRequestedTestOK(29);

    try
    {
        TEST_INFO( "Resolve the Replication Manager reference" );

        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());
        
        if (CORBA::is_nil(rm.in()))
        {
            std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                      << "could not _narrow object to type HelloInterface" << std::endl;
            abort();
        }
        
        TEST_SUCCEED();

        TEST_INFO("Create objects groups");
        const char * rid1 = "IDL:thalesgroup.com/CdmwRequestLogging/HelloInterface:1.0";

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

        CdmwNamingAndRepository::Repository_var repository =
             CdmwNamingAndRepository::Repository::_nil();

        CORBA::Object_var obj_repository = 
           m_orb->string_to_object("corbaloc::localhost:42562/CdmwRepository");
        repository = 
           CdmwNamingAndRepository::Repository::_narrow(obj_repository.in());

        
        Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                              repository.in());
        
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
        
        // Set_first_backup
        TEST_INFO("set the first backup "<<m_host3);
        try
        {
            rm->set_first_backup(locs[2]);
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

        TEST_INFO("Invoke a simple operation");
        m_hello->hello();
        TEST_SUCCEED();

        TEST_INFO("Check log size");
        CORBA::ULong size = m_hello->get_log_size();
        TEST_INFO("log size is " << size);
        TEST_CHECK(size == 1);

        TEST_INFO("Invoke an operation with a return value");
        TEST_CHECK(m_hello->do_something(4) == 16);

        TEST_INFO("Check log size");
        TEST_CHECK(m_hello->get_log_size() == 3);

        TEST_INFO("Invoke same operation with a different param to check the logged value is not returned.");
        TEST_CHECK(m_hello->do_something(5) == 25);
        
        TEST_INFO("Wait for timeouts to fire");
        OsSupport::OS::sleep(14000);
        TEST_INFO("Check log size");
        size = m_hello->get_log_size();
        TEST_INFO("log size is " << size);
        TEST_CHECK(size == 0);
        TEST_INFO("Check the retention of reply last at most request_duration_time");
        CdmwRequestLogging::TimeSeq_var durations = 
           m_hello->get_recorded_durations();
        for (CORBA::ULong i=0; i < durations->length(); i++)
        {
           TEST_INFO(durations[i]);
           TEST_CHECK(m_duration_time <= durations[i]);
        }

        TEST_INFO("Invoke an operation with an inout and an out value");
        char* val = CORBA::string_dup("hello");
        ::CdmwRequestLogging::MyStruct_var sval;
        CORBA::String_var ret = m_hello->do_something_more(val, sval.out());
        TEST_CHECK(strcmp(ret.in(), "hello") == 0);
        TEST_CHECK(strcmp(val, "olleh") == 0);
        TEST_CHECK(strcmp(sval->val.in(), "olleh|hello") == 0);
        TEST_INFO(sval->hostname.in());
        TEST_INFO(m_host2.c_str());
        TEST_CHECK(strcmp(sval->hostname.in(), m_host2.c_str()) == 0);

        TEST_INFO("Check log size has been increased only once last time");
        size = m_hello->get_log_size();
        TEST_INFO("log size is " << size);
        TEST_CHECK(size == 3);

        TEST_INFO("Repeat same operation with different inout and an out value");
        val = CORBA::string_dup("world");
        ::CdmwRequestLogging::MyStruct_var sval2;
        TEST_CHECK(strcmp(m_hello->do_something_more(val, sval2.out()), "world") == 0);
        TEST_CHECK(strcmp(val, "dlrow") == 0);
        TEST_CHECK(strcmp(sval2->val.in(), "dlrow|world") == 0);
        TEST_CHECK(strcmp(sval2->hostname.in(), m_host3.c_str()) == 0);

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
        info_stream1 << "Test TestRequestLogging Exception caught : " << std::endl;
        TEST_INFO(info_stream1.str());	
        TEST_FAILED();
    }
}

}; // namespace FT
}; // namespace Cdmw
