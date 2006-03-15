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


#include <iostream>
#include <exception>
#include <sstream>


#include "idllib/CdmwFTReplicationManager.stub.hpp"

#include "testflattransaction3/TestHello_impl.hpp"

#include "testflattransaction3/TestFTFlatTransaction.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

using namespace std;

namespace Cdmw {
namespace FT {

// constructor
TestFTFlatTransaction::TestFTFlatTransaction(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    const std::string& process_name,
    const std::string& host1,
    const std::string& host2,
    const std::string& host3)
    : Testable( name ),
      m_process_name(process_name),
      m_host1(host1),
      m_host2(host2),
      m_host3(host3)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestFTFlatTransaction::~TestFTFlatTransaction()
{
}

// Use only DSIDs 1 and 2
const char* data_strings[][10] = {
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
  { "one/zero", "one/one", "one/two", "one/three", "one/four", "one/five", "one/six", "one/seven", "one/eight", "one/nine" },
  { "two/zero", "two/one", "two/two", "two/three", "two/four", "two/five", "two/six", "two/seven", "two/eight", "two/nine" },
};


void
DumpExpectedDSContent ( int proc, CORBA::Long dsid, CORBA::Long first_oid, int oid_number
                      , int first_data, bool expected[])
{
    // Dump PROC1 datastore 1
    cout << " -> DS(PROC " << proc << "/DSID " << dsid << ") expected content:" << std::endl;
    CORBA::Long oid  = first_oid;
    CORBA::Long data = first_data;
    for (int i=0; i<oid_number; ++i) {
        if (expected[i]) {
            // Build the expected data
            const char* exp_data = data_strings[dsid][data];
            std::cout << "    oid " << oid << ", data \"" << exp_data << "\"" << std::endl;
        }
        oid++;
        data++;
    }
}


bool
DumpActualDSContent ( CdmwReplicationManager::HelloInterface_ptr hello, int proc
                    , CORBA::Long dsid, CORBA::Long first_oid, int oid_number
                    , int first_data, bool expected[])
{
    // Dump PROC1 datastore 1
    bool* received = new bool[oid_number];
    cout << " -> DS(PROC " << proc << "/DSID " << dsid << ") actual content:" << std::endl;
    CORBA::Long oid  = first_oid;
    CORBA::Long data = first_data;

    for (int i=0; i<oid_number; ++i) {
        try {
            ::CORBA::String_var actual = hello->get_data(dsid, oid);
            const char* exp_data = data_strings[dsid][data];
            cout << "    oid " << oid << ", data \"" << actual.in() << "\", (exp. \"" << exp_data << "\")" << std::endl;
            if (expected[i])
                if (strcmp(actual.in(), exp_data) == 0) received[i] = true;
            else 
                received[i] = false;
        }
        catch (const CORBA::SystemException& e) {
            std::string ex_str = Cdmw::OrbSupport::OrbSupport::exception_to_string(e);
            cout << "[---- TestFTFlatTransaction::DumpActualDSContent] " << ex_str << endl;
            if (expected[i]) return false;
            else             received[i] = false;

        }
        oid++;
        data++;
    }

    bool result = true;
    for (int i=0; i<oid_number; ++i) {
        cout << " -----------> exp " << expected[i] << ", rec" << received[i] << std::endl;
        if (expected[i] != received[i]) {
            result = false;
            break;
        }
    }
    delete [] received;
    return result;
}


// do_tests
void TestFTFlatTransaction::do_tests()
{
    set_nbOfRequestedTestOK (29);


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
        TEST_INFO("Create objects groups");

        const char * rid1 = "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id3;

        CORBA::Object_var obj1;
        
        // set default properties
        TEST_INFO( "set_default_properties" );
    
        ::FT::Properties prop;
    
        prop.length(2);
        prop[0].nam.length(1);
        prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop[0].val <<= (CORBA::UShort)2;

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
        
        prop[1].nam.length(1);
        prop[1].nam[0].id="org.omg.ft.Factories";
        prop[1].val <<= factoryInfos;

        ::CdmwFT::StateTransfer::DataStoreIDs dsids;
        dsids.length(1);
        dsids[0] = 1;

        ::FT::Criteria crit1;        
        crit1.length(2);
        crit1[0].nam.length(1);
        crit1[0].nam[0].id="org.omg.ft.FTProperties";
        crit1[0].val <<= prop;

        crit1[1].nam.length(1);
        crit1[1].nam[0].id="com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
        crit1[1].val <<= dsids;


        try {
            obj1 =  rm->create_object(rid1,
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

        //
        // This validation test uses interceptors to control where the primary
        // failure occurs. The interceptor implementation is done in such a way
        // that the reply to prepare or the request to commit may be
        // conditionally intercepted, the condition being the number of occurences
        // of these calls. The primary exits when the condition is met.
        //
        // The test consists of two steps executed one after the other:
        //
        // - step1: the primary will exit when the reply to prepare occurs
        //          for the 2nd time. This simulates a primary failure during the
        //          1st phase of a state transfer.
        // - step2: the primary will exit when the request to commit occurs for the
        //          2nd time. This simulates a primary failure during the 2nd phase
        //          of a state transfer.
        // - step3: a backup will exit when the request to prepare occurs for the
        //          2nd time. This simulates a backup failure during a state transfer.
        //
        // In both steps, PROC1 is the primary. Processes are started on the same
        // machine, but Locations' HOST fields are different. The primary process 
        // is launched with the "client_interceptor" option and installs a client
        // interceptor.
        // 
        // When PROC1 exits, the FTManager reissues the method to the new primary
        // (which can be either PROC2 or PROC3). The implementation of HelloInterface
        // checks wether the host process has been started with a "client_interceptor"
        // option. If yes, it executes the method just as a normal replica would do.
        // If not, it first checks the previous content of the datastores and compares
        // them to the expected values.
        //


        // ######################################################################
        // FIRST STEP: the primary installs a client interceptor and intercepts
        //             the reply to a Cohort prepare method. It calls 
        //             exit the 2nd time this reply occurs. This way, at least
        //             one prepare should have been completed at the
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
        proc_init11<< " --proc-initialise APPL1 P11 "<< m_host1;
        proc_init21<< " --proc-initialise APPL2 P21 "<< m_host2;
        proc_init31<< " --proc-initialise APPL3 P31 "<< m_host3;

        OsSupport::OS::create_process( "platform_admin.sh" , proc_init11.str());   
        OsSupport::OS::sleep(timescale*3000);
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init21.str());   
        OsSupport::OS::sleep(timescale*3000);
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init31.str());   

        OsSupport::OS::sleep(timescale*10000);
        
        std::ostringstream proc_run11;
        std::ostringstream proc_run21;
        std::ostringstream proc_run31;
        proc_run11<< " --proc-run APPL1 P11 "<< m_host1;
        proc_run21<< " --proc-run APPL2 P21 "<< m_host2;
        proc_run31<< " --proc-run APPL3 P31 "<< m_host3;

        OsSupport::OS::create_process( "platform_admin.sh" , proc_run11.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run21.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run31.str());   

        OsSupport::OS::sleep(timescale*5000);

        TEST_INFO("client started")
        
        CdmwNamingAndRepository::Repository_var repository =
                                  CdmwNamingAndRepository::Repository::_nil();

        CORBA::Object_var obj2 = m_orb->string_to_object("corbaloc::localhost:42562/CdmwRepository");
        repository = CdmwNamingAndRepository::Repository::_narrow(obj2.in());

        
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
        TEST_INFO("[---- TestFTFlatTransaction::do_tests] START TEST");
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
        TEST_INFO("[---- TestFTFlatTransaction::do_tests] add the member "
                  <<m_host1.c_str()<<"/APPL1/P11 in group 1");

        try  {
            obj1 = rm->add_member(obj1.in(),
                                  locs[0],
                                  helloPROC11.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }


        TEST_INFO("[---- TestFTFlatTransaction::do_tests] add the member "
                  <<m_host2.c_str()<<"/APPL2/P21 in group 1");
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  locs[1],
                                  helloPROC21.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        try
        {        
            rm->set_first_backup (locs[1]);
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e)
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        

        TEST_INFO("[---- TestFTFlatTransaction::do_tests] add the member "
                  <<m_host3.c_str()<<"/APPL3/P31 in group 1");
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  locs[2],
                                  helloPROC31.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*3000);


        TEST_INFO("[---- TestFTFlatTransaction::do_tests] narrowing the group reference");
        CdmwReplicationManager::HelloInterface_var grp = CdmwReplicationManager::HelloInterface::_narrow(obj1.in());
        if (::CORBA::is_nil(grp.in())) {
            TEST_FAILED();
        }
        else {
            TEST_SUCCEED();
        }
        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("[---- TestFTFlatTransaction::do_tests] store 11 to 14 /\"one/xxx\": should not failed");
        try {
            bool expected[] = { true, true, true, true };

            helloPROC11->insert_and_commit();
            
            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC11/1");
            //DumpExpectedDSContent   (1, 1, 11, 2, 1, expected);
            if (DumpActualDSContent (helloPROC11.in(), 1, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
            
            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC21/1");
            //DumpExpectedDSContent   (2, 1, 11, 2, 1, expected);
            if (DumpActualDSContent (helloPROC21.in(), 2, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();

            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC31/1");
            //DumpExpectedDSContent   (3, 1, 11, 2, 1, expected);
            if (DumpActualDSContent (helloPROC31.in(), 3, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }
        catch(...) {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);


        TEST_INFO("[---- TestFTFlatTransaction::do_tests] store 11 to 14 /\"two/xxx\": should not failed");
        try {
            bool expected[] = { true, true, true, true };

            helloPROC11->update_and_rollback();
            
            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC11/1");
            //DumpExpectedDSContent   (1, 1, 11, 2, 2, expected);
            if (DumpActualDSContent (helloPROC11.in(), 1, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
            
            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC21/1");
            //DumpExpectedDSContent   (2, 1, 11, 2, 2, expected);
            if (DumpActualDSContent (helloPROC21.in(), 2, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();

            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC31/1");
            //DumpExpectedDSContent   (3, 1, 11, 2, 2, expected);
            if (DumpActualDSContent (helloPROC31.in(), 3, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }
        catch(...) {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);


        TEST_INFO("[---- TestFTFlatTransaction::do_tests] store 11 ot 14 /\"three/xxx\": should failed ");
        TEST_INFO("      because of a primary failure during the first 'prepare'");
        TEST_INFO("      request (i.e. the first request of this test)");
        try {
            helloPROC11->setup_interception(0, "commit","request", "kill");
            helloPROC11->update_kill_and_commit();
            TEST_FAILED();
        }
        catch(...) {
            // TODO: check the exception type (COMM_FAILURE ???)
            TEST_SUCCEED();

            TEST_INFO("[---- TestFTFlatTransaction::do_tests] checking datastore");
            bool expected[] = { true, true, true, true};

            try {
                helloPROC11->ping();  // -> should return an exception
                TEST_FAILED();
            }
            catch(...) {
                // TODO: check the exception type (COMM_FAILURE ???)
                TEST_SUCCEED();
            }

            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC21/1");
            //DumpExpectedDSContent   (2, 1, 11, 2, 2, expected);
            if (DumpActualDSContent (helloPROC21.in(), 2, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();

            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC31/1");
            //DumpExpectedDSContent   (3, 1, 11, 2, 2, expected);
            if (DumpActualDSContent (helloPROC31.in(), 3, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);

        TEST_INFO("[---- TestFTFlatTransaction::do_tests] store 11 to 14 /\"two/xxx\": should failed because the request was forward to a bad cohort ");
        try {
            bool expected[] = { true, true, true, true };

            helloPROC31->setup_interception(0, "prepare", "request", "forward");
            helloPROC21->update_forward_and_commit();
            
            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC21/1");
            //DumpExpectedDSContent   (2, 1, 11, 2, 2, expected);
            if (DumpActualDSContent (helloPROC21.in(), 2, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();

            TEST_INFO("[---- TestFTFlatTransaction::do_tests] Checking DataStore PROC31/1");
            //DumpExpectedDSContent   (3, 1, 11, 2, 2, expected);
            if (DumpActualDSContent (helloPROC31.in(), 3, 1, 11, 4, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }
        catch(const CORBA::SystemException& ex) {
            std::cerr<<ex<<std::endl;
            TEST_FAILED();
        }
        catch(...) {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);

 
        TEST_INFO("removing member "<<m_host2.c_str()<<"/APPL2/P21");
        try  {
            obj1 = rm->remove_member(obj1.in(), locs[1]);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("removing member "<<m_host3.c_str()<<"APPL3/P31");
        try  {
            obj1 = rm->remove_member(obj1.in(), locs[2]);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        std::ostringstream proc_stop11;
        std::ostringstream proc_stop21;
        std::ostringstream proc_stop31;
        proc_stop11<< " --proc-stop APPL1 P11 "<< m_host1;
        proc_stop21<< " --proc-stop APPL2 P21 "<< m_host2;
        proc_stop31<< " --proc-stop APPL3 P31 "<< m_host3;

        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop11.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop21.str());   
        OsSupport::OS::create_process( "platform_admin.sh" , proc_stop31.str());   

        OsSupport::OS::sleep(timescale*5000);


        //
        // END FIRST STEP
        // ######################################################################
        
        Cdmw::NamingAndRepository::RepositoryInterface::finish();
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

