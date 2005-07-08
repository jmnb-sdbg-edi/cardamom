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


#include "idllib/CdmwFTReplicationManager.stub.hpp"
#include "testftmanagerreplication2/TestHello_impl.hpp"
#include "testftmanagerreplication2/TestFTManagerReplication2.hpp"

#include <Repository/naminginterface/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

namespace Cdmw {
namespace FT {

// constructor
TestFTManagerReplication2::TestFTManagerReplication2(
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
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestFTManagerReplication2::~TestFTManagerReplication2()
{
}


// do_tests
void TestFTManagerReplication2::do_tests()
{
    set_nbOfRequestedTestOK (31);


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


        
        TEST_INFO( "Resolve the Fault Notifier reference" );
        ::FT::FaultNotifier_var fault_notifier
            = rm->get_fault_notifier(); 
        
        TEST_SUCCEED();


        
        // create the object groups
        TEST_INFO("Create the object group (stateless)");

        const char * rid = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;

        CORBA::Object_var obj1;
        CORBA::Object_var obj2;
        
        // set default properties
        TEST_INFO( "set_default_properties" );
    
        ::FT::Properties prop1;
    
        prop1.length(2);
        prop1[0].nam.length(1);
        prop1[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop1[0].val <<= (CORBA::UShort)2;



        std::vector<std::string> slocvect(3);
        slocvect[0] =  m_host1 +".hostname/APPL1.applicationname/P11.processname";
        slocvect[1] =  m_host2 +".hostname/APPL2.applicationname/P21.processname";
        slocvect[2] =  m_host3 +".hostname/APPL3.applicationname/P31.processname";


        const CORBA::ULong MAX_LOCS=slocvect.size();
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(MAX_LOCS);
        for (CORBA::ULong i = 0; i < MAX_LOCS; ++i) {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            ::FT::Location_var loc = 
              Cdmw::NamingAndRepository::NamingInterface::to_name(slocvect[i]);
            std::cerr << '[' << i << "] " << slocvect[i] << " --- " 
                      << Cdmw::NamingAndRepository::NamingInterface::to_string(loc.in()) << std::endl;
            
            factoryInfos[i].the_location = loc.in();
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[i].the_criteria = factoryCrit;
        }
        
        prop1[1].nam.length(1);
        prop1[1].nam[0].id="org.omg.ft.Factories";
        prop1[1].val <<= factoryInfos;


        ::FT::Criteria crit1;        
        crit1.length(1);
        crit1[0].nam.length(1);
        crit1[0].nam[0].id="org.omg.ft.FTProperties";
        crit1[0].val <<= prop1;


        try {
            obj1 =  rm->create_object(rid,
                                      crit1,
                                      factory_creation_id1.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        



        ::FT::ObjectGroupId ogid = 0;
        try {
            ogid = rm->get_object_group_id(obj1.in());
            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        

        TEST_INFO("start processes");




        // ######################################################################
        // FIRST STEP: the primary installs a client interceptor and intercepts
        //             the reply to a Cohort prepare_insert method. It calls 
        //             exit the 2nd time this reply occurs. This way, at least
        //             one prepare_insert should have been completed at the
        //             member level.
        //

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
        proc_init11<< " --proc-initialise APPL1 P11";
        proc_init21<< " --proc-initialise APPL2 P21";
        proc_init31<< " --proc-initialise APPL3 P31";

        OsSupport::OS::create_process( "platform_admin.sh" , proc_init11.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init21.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init31.str());   

        OsSupport::OS::sleep(timescale*20000);
        
        std::ostringstream proc_run11;
        std::ostringstream proc_run21;
        std::ostringstream proc_run31;
        proc_run11<< " --proc-run APPL1 P11";
        proc_run21<< " --proc-run APPL2 P21";
        proc_run31<< " --proc-run APPL3 P31";

        OsSupport::OS::create_process( "platform_admin.sh" , proc_run11.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run21.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run31.str());   

        OsSupport::OS::sleep(timescale*5000);

        TEST_INFO("client started")
        
        CdmwNamingAndRepository::Repository_var repository =
                                  CdmwNamingAndRepository::Repository::_nil();

        CORBA::Object_var obj_repo = m_orb->string_to_object("corbaloc::localhost:42562/CdmwRepository");
        repository = CdmwNamingAndRepository::Repository::_narrow(obj_repo.in());

        
        Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                              repository.in());
        
        Cdmw::NamingAndRepository::NamingInterface ni =
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
        
        
        CdmwReplicationManager::HelloInterface_var helloPROC11 = CdmwReplicationManager::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(helloPROC11.in()) );

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
        
        
        CdmwReplicationManager::HelloInterface_var helloPROC21 = CdmwReplicationManager::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(helloPROC21.in()) );

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
        
        
        CdmwReplicationManager::HelloInterface_var helloPROC31 = CdmwReplicationManager::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(helloPROC31.in()) );



        TEST_INFO("");
        TEST_INFO("[---- TestFTManagerReplication2::do_tests] START TEST");
        TEST_INFO("");

        // Add new member on group1: PROC1 first, then PROC2 and 3
        TEST_INFO("get the reference of the object group 1");
        try {
            obj1 = rm->get_object_group_ref(obj1.in());
        }
        catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        // Add group members: PROC1 first, then PROC2 and 3. PROC1 become the primary process.
        TEST_INFO("[---- TestFTManagerReplication2::do_tests] add the member "<<m_host1.c_str()<<"/APPL1/P11 in group 1");

        ::FT::Location loc;
        loc.length(3);
        loc[0].id = m_host1.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL1";
        loc[1].kind = "applicationname";        
        loc[2].id = "P11";
        loc[2].kind = "processname";

        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloPROC11.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }




        TEST_INFO("[---- TestFTManagerReplication2::do_tests] add the member "<<m_host2.c_str()<<"/APPL2/P21 in group 1");
        loc[0].id = m_host2.c_str();
        loc[1].id = "APPL2";
        loc[2].id = "P21";
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloPROC21.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }


//         OsSupport::OS::sleep(timescale*1000);
//         TEST_INFO("Kill the FaultManager on the host 1");
//         TEST_INFO("Press any key and Enter");
//         std::string dummy;
//         std::cin >> dummy;







        ::FT::Properties prop2;
    
        prop2.length(2);
        prop2[0].nam.length(1);
        prop2[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop2[0].val <<= (CORBA::UShort)2;



        slocvect[0] =  m_host1 +".hostname/APPL1.applicationname/P12.processname";
        slocvect[1] =  m_host2 +".hostname/APPL2.applicationname/P22.processname";
        slocvect[2] =  m_host3 +".hostname/APPL3.applicationname/P32.processname";


        factoryInfos.length(slocvect.size());
        for (CORBA::ULong i = 0; i < MAX_LOCS; ++i) {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            ::FT::Location_var loc = 
              Cdmw::NamingAndRepository::NamingInterface::to_name(slocvect[i]);
            std::cerr << '[' << i << "] " << slocvect[i] << " --- " 
                      << Cdmw::NamingAndRepository::NamingInterface::to_string(loc.in()) << std::endl;
            
            factoryInfos[i].the_location = loc.in();
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[i].the_criteria = factoryCrit;
        }
        
        prop2[1].nam.length(1);
        prop2[1].nam[0].id="org.omg.ft.Factories";
        prop2[1].val <<= factoryInfos;



        ::FT::Criteria crit2;        
        crit2.length(2);
        crit2[0].nam.length(1);
        crit2[0].nam[0].id="org.omg.ft.FTProperties";
        crit2[0].val <<= prop2;

        
        ::CdmwFT::StateTransfer::DataStoreIDs dsids;
        dsids.length(1);
        dsids[0] = 1;

        crit2[1].nam.length(1);
        crit2[1].nam[0].id="com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
        crit2[1].val <<= dsids;

        try {
            obj2 =  rm->create_object(rid,
                                      crit2,
                                      factory_creation_id2.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }




        TEST_INFO("Start server processes");
        
        std::ostringstream proc_init12;
        std::ostringstream proc_init22;
        std::ostringstream proc_init32;
        proc_init12<< " --proc-initialise APPL1 P12";
        proc_init22<< " --proc-initialise APPL2 P22";
        proc_init32<< " --proc-initialise APPL3 P32";

        OsSupport::OS::create_process( "platform_admin.sh" , proc_init12.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init22.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init32.str());   

        OsSupport::OS::sleep(timescale*20000);
        
        std::ostringstream proc_run12;
        std::ostringstream proc_run22;
        std::ostringstream proc_run32;
        proc_run12<< " --proc-run APPL1 P12";
        proc_run22<< " --proc-run APPL2 P22";
        proc_run32<< " --proc-run APPL3 P32";

        OsSupport::OS::create_process( "platform_admin.sh" , proc_run12.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run22.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run32.str());   

        OsSupport::OS::sleep(timescale*5000);


        full_name = "dom3/P12HelloInterface";
        
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
        
        
        CdmwReplicationManager::HelloInterface_var helloPROC12 = CdmwReplicationManager::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(helloPROC12.in()) );

        full_name = "dom3/P22HelloInterface";
        
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
        
        
        CdmwReplicationManager::HelloInterface_var helloPROC22 = CdmwReplicationManager::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(helloPROC22.in()) );

        full_name = "dom3/P32HelloInterface";
        
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
        
        
        CdmwReplicationManager::HelloInterface_var helloPROC32 = CdmwReplicationManager::HelloInterface::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(helloPROC32.in()) );


        // Add group members: PROC1 first, then PROC2 and 3. PROC1 become the primary process.
        TEST_INFO("[---- TestFTManagerReplication2::do_tests] add the member "<<m_host1.c_str()<<"/APPL1/P12 in group 1");


        loc[0].id = m_host1.c_str();
        loc[0].kind = "hostname";
        loc[1].id = "APPL1";
        loc[1].kind = "applicationname";        
        loc[2].id = "P12";
        loc[2].kind = "processname";

        try  {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  helloPROC12.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO("[---- TestFTManagerReplication2::do_tests] add the member "<<m_host2.c_str()<<"/APPL2/P22 in group 1");
        loc[0].id = m_host2.c_str();
        loc[1].id = "APPL2";
        loc[2].id = "P22";
        try  {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  helloPROC22.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }


        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Kill the primary FaultManager");
        TEST_INFO("Press any key and Enter");
        std::string dummy;
        std::cin >> dummy;


        TEST_INFO("[---- TestFTManagerReplication2::do_tests] add the member "<<m_host3.c_str()<<"/APPL3/P31 in group 1");
        loc[0].id = m_host3.c_str();
        loc[1].id = "APPL3";
        loc[2].id = "P31";
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  helloPROC31.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }


        TEST_INFO("[---- TestFTManagerReplication2::do_tests] add the member "<<m_host3.c_str()<<"/APPL3/P32 in group 1");
        loc[0].id = m_host3.c_str();
        loc[1].id = "APPL3";
        loc[2].id = "P32";
        try  {
            obj2 = rm->add_member(obj2.in(),
                                  loc,
                                  helloPROC32.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }






 
        TEST_INFO("removing member "<<m_host1.c_str()<<"/APPL1/P11");
        loc[0].id = m_host1.c_str();
        loc[1].id = "APPL1";
        loc[2].id = "P11";
        try  {
            obj1 = rm->remove_member(obj1.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("removing member "<<m_host2.c_str()<<"/APPL2/P21");
        loc[0].id = m_host2.c_str();
        loc[1].id = "APPL2";
        loc[2].id = "P21";
        try  {
            obj1 = rm->remove_member(obj1.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("removing member "<<m_host3.c_str()<<"APPL3/P31");
        loc[0].id = m_host3.c_str();
        loc[1].id = "APPL3";
        loc[2].id = "P31";
        try  {
            obj1 = rm->remove_member(obj1.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("delete object");
        try {
            rm->delete_object(factory_creation_id1.in());
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();      
        }
        
        TEST_INFO("removing member "<<m_host1.c_str()<<"/APPL1/P12");
        loc[0].id = m_host1.c_str();
        loc[1].id = "APPL1";
        loc[2].id = "P12";
        try  {
            obj2 = rm->remove_member(obj2.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("removing member "<<m_host2.c_str()<<"/APPL2/P22");
        loc[0].id = m_host2.c_str();
        loc[1].id = "APPL2";
        loc[2].id = "P22";
        try  {
            obj2 = rm->remove_member(obj2.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("removing member "<<m_host3.c_str()<<"APPL3/P32");
        loc[0].id = m_host3.c_str();
        loc[1].id = "APPL3";
        loc[2].id = "P32";
        try  {
            obj2 = rm->remove_member(obj2.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("delete object");
        try {
            rm->delete_object(factory_creation_id2.in());
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();      
        }
        

        std::ostringstream proc_stop11;
        std::ostringstream proc_stop21;
        std::ostringstream proc_stop31;
        proc_stop11<< " --proc-stop APPL1 P11";
        proc_stop21<< " --proc-stop APPL2 P21";
        proc_stop31<< " --proc-stop APPL3 P31";

        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop11.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop21.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop31.str());   

        std::ostringstream proc_stop12;
        std::ostringstream proc_stop22;
        std::ostringstream proc_stop32;
        proc_stop12<< " --proc-stop APPL1 P12";
        proc_stop22<< " --proc-stop APPL2 P22";
        proc_stop32<< " --proc-stop APPL3 P32";

        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop12.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop22.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop32.str());   

        OsSupport::OS::sleep(timescale*5000);


        //
        // END FIRST STEP
        // ######################################################################

    }
    catch(const CORBA::Exception& e ) {
        std::cerr << e._name() << std::endl;
        TEST_FAILED();
    }
    catch( const Cdmw::Exception& e ) {
        std::cerr << e.what() << std::endl;
        TEST_FAILED();
    }

}

}; // namespace FT
}; // namespace Cdmw

