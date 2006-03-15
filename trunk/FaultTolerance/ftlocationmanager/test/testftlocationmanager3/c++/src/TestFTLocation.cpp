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

#include "testftlocationmanager3/TestHello_impl.hpp"
//#include "testftlocationmanager2/TestFTActivationHandler_impl.hpp"

#include "testftlocationmanager3/TestFTLocation.hpp"
#include "testftlocationmanager3/TestClientThread.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>

using namespace std;

namespace Cdmw {
namespace FT {

// constructor
TestFTLocation::TestFTLocation(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    const std::string& process_name )
    : Testable( name ),
      m_process_name(process_name)
      
    
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}

//destructor
TestFTLocation::~TestFTLocation()
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
            cout << "[**** TestFTLocation::DumpActualDSContent] " << ex_str << endl;
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
void TestFTLocation::do_tests()
{
    set_nbOfRequestedTestOK (36);

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

        CORBA::Object_var obj1;
        
        // set default properties
        TEST_INFO( "set_default_properties" );
    

        ::FT::Properties prop;
    
        prop.length(2);
        prop[0].nam.length(1);
        prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
        prop[0].val <<= (CORBA::UShort)2;

        std::vector<std::string> slocvect(3);
        slocvect[0] = "HOST1/TEST/PROC1";
        slocvect[1] = "HOST2/TEST/PROC2";
        slocvect[2] = "HOST3/TEST/PROC3";


        const CORBA::ULong MAX_LOCS=slocvect.size();
        ::FT::FactoryInfos factoryInfos;
        factoryInfos.length(MAX_LOCS);
        for (CORBA::ULong i = 0; i < MAX_LOCS; ++i) {
            factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
            ::FT::Location_var loc = 
                  Cdmw::CommonSvcs::Naming::NamingInterface::to_name(slocvect[i]);
            std::cerr << '[' << i << "] " << slocvect[i] << " --- " 
                      << Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc.in()) << std::endl;

            factoryInfos[i].the_location = loc.in();
            ::FT::Criteria factoryCrit;        
            factoryCrit.length(0);
            factoryInfos[i].the_criteria = factoryCrit;
        }
    
        prop[1].nam.length(1);
        prop[1].nam[0].id="org.omg.ft.Factories";
        prop[1].val <<= factoryInfos;

        ::CdmwFT::StateTransfer::DataStoreIDs dsids;
        dsids.length(2);
        dsids[0] = 1;
        dsids[1] = 2;

        ::FT::Criteria crit;        
        crit.length(2);
        crit[0].nam.length(1);
        crit[0].nam[0].id="org.omg.ft.FTProperties";
        crit[0].val <<= prop;

        crit[1].nam.length(1);
        crit[1].nam[0].id="com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
        crit[1].val <<= dsids;




//         ::FT::Properties prop;
    
//         prop.length(5);
//         prop[0].nam.length(1);
//         prop[0].nam[0].id="MinimumNumberReplicas";
//         prop[0].val <<= (CORBA::UShort)2;
    
//         prop[1].nam.length(1);
//         prop[1].nam[0].id="ReplicationStyle";
//         prop[1].val <<= (CORBA::UShort)2;
    
//         prop[2].nam.length(1);
//         prop[2].nam[0].id="MemberShipStyle";
//         prop[2].val <<= (CORBA::UShort)0;
    
//         prop[3].nam.length(1);
//         prop[3].nam[0].id="FaultMonitoringStyle";
//         prop[3].val <<= (CORBA::UShort)0;


//         ::CdmwFT::StateTransfer::DataStoreIDs dsids;
//         dsids.length(2);
//         dsids[0] = 1;
//         dsids[1] = 2;

//         prop[4].nam.length( 1 );
//         prop[4].nam[0].id = "com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
//         prop[4].val <<= dsids;
//         const ::FT::Criteria crit1 = prop;


        try {
            obj1 =  rm->create_object(rid1,
                                      crit,
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
        //             one prepare_insert should have been completed at the
        //             member level.
        //

        std::cout << "//" << std::endl;
        std::cout << "// ###########################################################" << std::endl;
        std::cout << "// ##### FIRST STEP" << std::endl;
        std::cout << "//" << std::endl;
        
        // FIRST STEP: the primary installs a client interceptor and intercepts

        std::string iorhello1PROC1 = "hello1PROC1.ior";
        std::string iorhello1PROC2 = "hello1PROC2.ior";
        std::string iorhello1PROC3 = "hello1PROC3.ior";
        
        
        // first delete the ior files for synchronisation
        try {
            OsSupport::OS::unlink (iorhello1PROC1);	
        }
        catch (...) {
        }

        try {
            OsSupport::OS::unlink (iorhello1PROC2);	
        }
        catch (...) {
        }

        try {
            OsSupport::OS::unlink (iorhello1PROC3);	
        }
        catch (...) {
        }
        
        std::ostringstream proc11;
        std::ostringstream proc12;
        std::ostringstream proc13;
        proc11<< " --process=PROC1 --object_group_id=" << ogid
             << " --client_interceptor=prepare" << " --where=reply" << " --count=" << 2;
        proc12<< " --process=PROC2 --object_group_id="<< ogid
             << " --step=" << 1;
        proc13<< " --process=PROC3 --object_group_id="<< ogid
             << " --step=" << 1;

        std::cout << "[**** TestFTLocation::do_tests] creating PROC1 with args " << proc11 << std::endl;
        OsSupport::OS::ProcessId pid1  = 
        OsSupport::OS::create_process( m_process_name, proc11.str());
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*2000);
        std::cout << "[**** TestFTLocation::do_tests] creating PROC2 with args " << proc12 << std::endl;
        OsSupport::OS::ProcessId pid2  = 
        OsSupport::OS::create_process( m_process_name, proc12.str());   
        OsSupport::OS::sleep(timescale*2000);
        std::cout << "[**** TestFTLocation::do_tests] creating PROC3 with args " << proc13 << std::endl;
        OsSupport::OS::ProcessId pid3  = 
        OsSupport::OS::create_process( m_process_name, proc13.str());   
        OsSupport::OS::sleep(timescale*4000);
        TEST_SUCCEED();

        OsSupport::OS::sleep(timescale*1000);
        

        // wait creation of ior files
        while ( !(  OsSupport::OS::file_exists(iorhello1PROC1)
                 && OsSupport::OS::file_exists(iorhello1PROC2)
                 && OsSupport::OS::file_exists(iorhello1PROC3)
                 )
              )
        {
            OsSupport::OS::sleep(timescale*1000);
        }

        TEST_INFO("get the reference for the synchronisation object");
        std::string file_url_prefix = "file://";        
        std::string file_url;
        

        TEST_INFO("get the reference for object to insert into the object group");
        file_url = file_url_prefix;
        file_url += iorhello1PROC1;
        
        CORBA::Object_var object_hello1PROC1 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface_var hello1PROC1 = CdmwReplicationManager::HelloInterface::_narrow(object_hello1PROC1.in());

        file_url = file_url_prefix;
        file_url += iorhello1PROC2;
        
        CORBA::Object_var object_hello1PROC2 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface_var hello1PROC2 = CdmwReplicationManager::HelloInterface::_narrow(object_hello1PROC2.in());

        file_url = file_url_prefix;
        file_url += iorhello1PROC3;
        
        CORBA::Object_var object_hello1PROC3 = m_orb->string_to_object(file_url.c_str());
        CdmwReplicationManager::HelloInterface_var hello1PROC3 = CdmwReplicationManager::HelloInterface::_narrow(object_hello1PROC3.in());


        TEST_SUCCEED();

        TEST_INFO("client started")
        


        TEST_INFO("");
        TEST_INFO("[**** TestFTLocation::do_tests] START TEST");
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
        TEST_INFO("[**** TestFTLocation::do_tests] add the member HOST1/TEST/PROC1 in group 1");
        ::FT::Location loc;
        loc.length(3);
        loc[0].id = "HOST1";
        loc[1].id = "TEST";
        loc[2].id = "PROC1";
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1PROC1.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO("add the member HOST2/TEST/PROC2 in group 1");
        loc[0].id = "HOST2";
        loc[1].id = "TEST";
        loc[2].id = "PROC2";
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1PROC2.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO("[**** TestFTLocation::do_tests] add the member HOST3/TEST/PROC3 in group 1");
        loc[0].id = "HOST3";
        loc[1].id = "TEST";
        loc[2].id = "PROC3";
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1PROC3.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO("[**** TestFTLocation::do_tests] narrowing the group reference");
        CdmwReplicationManager::HelloInterface_var grp = CdmwReplicationManager::HelloInterface::_narrow(obj1.in());
        if (::CORBA::is_nil(grp.in())) {
            TEST_FAILED();
        }
        else {
            TEST_SUCCEED();
        }
        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("[**** TestFTLocation::do_tests] store 11/\"one/one\": should not failed");
        try {
            bool expected[] = { true, false };

            hello1PROC1->insert(1, 11, "one/one");

            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC1/1");
            //DumpExpectedDSContent   (1, 1, 11, 2, 1, expected);
            if (DumpActualDSContent (hello1PROC1.in(), 1, 1, 11, 2, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
            
            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC2/1");
            //DumpExpectedDSContent   (2, 1, 11, 2, 1, expected);
            if (DumpActualDSContent (hello1PROC2.in(), 2, 1, 11, 2, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();

            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC3/1");
            //DumpExpectedDSContent   (3, 1, 11, 2, 1, expected);
            if (DumpActualDSContent (hello1PROC3.in(), 3, 1, 11, 2, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }
        catch(...) {
            TEST_FAILED();
        }


        TEST_INFO("[**** TestFTLocation::do_tests] store 12/\"one/two\": should failed ");
        TEST_INFO("      because of a primary failure during the 3rd 'prepare'");
        TEST_INFO("      request (i.e. the first request of this test)");
        try {
            hello1PROC1->insert(1, 12, "one/two"); // <--- Should failed and return an exception
            TEST_FAILED();
        }
        catch(...) {
            // TODO: check the exception type (COMM_FAILURE ???)
            TEST_SUCCEED();

            TEST_INFO("[**** TestFTLocation::do_tests] checking datastore");
            bool expected[] = { true, false };

            try {
                hello1PROC1->ping();  // -> should return an exception
                TEST_FAILED();
            }
            catch(...) {
                // TODO: check the exception type (COMM_FAILURE ???)
                TEST_SUCCEED();
            }

            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC2/1");
            //DumpExpectedDSContent   (2, 1, 11, 2, 1, expected);
            if (DumpActualDSContent (hello1PROC2.in(), 2, 1, 11, 2, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();

            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC3/1");
            //DumpExpectedDSContent   (3, 1, 11, 2, 1, expected);
            if (DumpActualDSContent (hello1PROC3.in(), 3, 1, 11, 2, 1, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }


        // Notify the FTManager that PROC1 has died
        TEST_INFO("Send a fault report for the location HOST1/TEST/PROC1");
        CosNotification::StructuredEvent fault_event;
        fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup( "FT_CORBA" );
        fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup( "ObjectCrashFault" );
        fault_event.filterable_data.length( 2 );
        fault_event.filterable_data[0].name = CORBA::string_dup( "FTDomainId" );
        fault_event.filterable_data[0].value <<= "thalesgroup.com";
        fault_event.filterable_data[1].name = CORBA::string_dup( "Location" );
        loc[0].id = "HOST1";
        loc[1].id = "TEST";
        loc[2].id = "PROC1";
        fault_event.filterable_data[1].value <<= loc;
        fault_notifier->push_structured_fault( fault_event );
 


        sleep(10);
        

        TEST_INFO("removing member HOST2/TEST/PROC2");
        loc[0].id = "HOST2";
        loc[1].id = "TEST";
        loc[2].id = "PROC2";
        try  {
            obj1 = rm->remove_member(obj1.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO("removing member HOST3/TEST/PROC3");
        loc[0].id = "HOST3";
        loc[1].id = "TEST";
        loc[2].id = "PROC3";
        try  {
            obj1 = rm->remove_member(obj1.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        TEST_INFO( "Killing process 2" );
        OsSupport::OS::kill_process( pid2);

        TEST_INFO( "Killing process 3" );
        OsSupport::OS::kill_process( pid3);
        //
        // END FIRST STEP
        // ######################################################################


        // ######################################################################
        // SECOND STEP: the primary installs a client interceptor and intercepts
        //             the request to a Cohort commit method. It calls exit the
        //             2nd time commit is called. This way, at least one commit
        //             has **not** been completed at the member level.
        //

        std::cout << "//" << std::endl;
        std::cout << "// ###########################################################" << std::endl;
        std::cout << "// ##### SECOND STEP" << std::endl;
        std::cout << "//" << std::endl;
        
        // first delete the ior files for synchronisation
        try {
            OsSupport::OS::unlink (iorhello1PROC1);	
        }
        catch (...) {
        }

        try {
            OsSupport::OS::unlink (iorhello1PROC2);	
        }
        catch (...) {
        }

        try {
            OsSupport::OS::unlink (iorhello1PROC3);	
        }
        catch (...) {
        }
        
        std::ostringstream proc21;
        std::ostringstream proc22;
        std::ostringstream proc23;
        proc21<<" --process=PROC1 --object_group_id=" << ogid
              <<" --client_interceptor=commit" << " --where=reply" << " --count=" << 2;
        proc22<<" --process=PROC2 --object_group_id=" << ogid
              << " --step=" << 2;
        proc23<<" --process=PROC3 --object_group_id=" << ogid
              << " --step=" << 2;

        std::cout << "[**** TestFTLocation::do_tests] creating PROC1 with args " << proc21 << std::endl;
        pid1 = OsSupport::OS::create_process( m_process_name, proc21.str());
        OsSupport::OS::sleep(timescale*2000);
        std::cout << "[**** TestFTLocation::do_tests] creating PROC2 with args " << proc22 << std::endl;
        pid2 = OsSupport::OS::create_process( m_process_name, proc22.str());   
        OsSupport::OS::sleep(timescale*2000);
        std::cout << "[**** TestFTLocation::do_tests] creating PROC3 with args " << proc23 << std::endl;
        pid3 = OsSupport::OS::create_process( m_process_name, proc23.str());   
        OsSupport::OS::sleep(timescale*4000);
        TEST_SUCCEED();

        timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep(timescale*1000);
        

        
        // wait creation of ior files
        while ( !(  OsSupport::OS::file_exists(iorhello1PROC1)
                 && OsSupport::OS::file_exists(iorhello1PROC2)
                 && OsSupport::OS::file_exists(iorhello1PROC3)
                 )
              )
        {
            OsSupport::OS::sleep(timescale*1000);
        }

        TEST_INFO("get the reference for the synchronisation object");
        file_url_prefix = "file://";        
        

        TEST_INFO("get the reference for object to insert into the object group");
        file_url = file_url_prefix;
        file_url += iorhello1PROC1;
        
        object_hello1PROC1 = m_orb->string_to_object(file_url.c_str());
        hello1PROC1 = CdmwReplicationManager::HelloInterface::_narrow(object_hello1PROC1.in());

        file_url = file_url_prefix;
        file_url += iorhello1PROC2;
        
        object_hello1PROC2 = m_orb->string_to_object(file_url.c_str());
        hello1PROC2 = CdmwReplicationManager::HelloInterface::_narrow(object_hello1PROC2.in());

        file_url = file_url_prefix;
        file_url += iorhello1PROC3;
        
        object_hello1PROC3 = m_orb->string_to_object(file_url.c_str());
        hello1PROC3 = CdmwReplicationManager::HelloInterface::_narrow(object_hello1PROC3.in());

        TEST_SUCCEED();

        TEST_INFO("");
        TEST_INFO("START TEST");
        TEST_INFO("");

        // Add group members: PROC1 first, then PROC2 and 3. PROC1 become the
        // primary process.
        TEST_INFO("add the member HOST1/TEST/PROC1 in group 1");
        loc[0].id = "HOST1";
        loc[1].id = "TEST";
        loc[2].id = "PROC1";
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1PROC1.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO("add the member HOST2/TEST/PROC2 in group 1");
        loc[0].id = "HOST2";
        loc[1].id = "TEST";
        loc[2].id = "PROC2";
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1PROC2.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO("add the member HOST3/TEST/PROC3 in group 1");
        loc[0].id = "HOST3";
        loc[1].id = "TEST";
        loc[2].id = "PROC3";
        try  {
            obj1 = rm->add_member(obj1.in(),
                                  loc,
                                  hello1PROC3.in());
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO("narrowing the group reference");
        grp = CdmwReplicationManager::HelloInterface::_narrow(obj1.in());
        if (::CORBA::is_nil(grp.in())) {
            TEST_FAILED();
        }
        else {
            TEST_SUCCEED();
        }
        
        OsSupport::OS::sleep(timescale*1000);
        

        TEST_INFO("[**** TestFTLocation::do_tests] store 13/\"one/three\": should not failed");
        try {
            bool expected[] = { true, false };

            hello1PROC1->insert(1, 13, "one/three");

            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC1/1");
            //DumpExpectedDSContent   (1, 1, 13, 2, 3, expected);
            if (DumpActualDSContent (hello1PROC1.in(), 1, 1, 13, 2, 3, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
            
            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC2/1");
            //DumpExpectedDSContent   (2, 1, 13, 2, 3, expected);
            if (DumpActualDSContent (hello1PROC2.in(), 2, 1, 13, 2, 3, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();

            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC3/1");
            //DumpExpectedDSContent   (3, 1, 13, 2, 3, expected);
            if (DumpActualDSContent (hello1PROC3.in(), 3, 1, 13, 2, 3, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }
        catch(...) {
            TEST_FAILED();
        }


        TEST_INFO("[**** TestFTLocation::do_tests] store 14/\"one/four\": should failed ");
        TEST_INFO("      because of a primary failure during the reply to the third");
        TEST_INFO("      'commit' (i.e. the first commit reply of this test)");
        try {
            hello1PROC1->insert(1, 14, "one/four"); // <--- Should failed and return an exception
            TEST_FAILED();
        }
        catch(...) {
            // TODO: check the exception type (COMM_FAILURE ???)
            TEST_SUCCEED();

            TEST_INFO("[**** TestFTLocation::do_tests] checking datastore");

            try {
                hello1PROC1->ping();  // -> should return an exception
                TEST_FAILED();
            }
            catch(...) {
                // TODO: check the exception type (COMM_FAILURE ???)
                TEST_SUCCEED();
            }

            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC2/1 **BEFORE** Time-Out");
            bool expected[] = { true, true };
            //DumpExpectedDSContent   (2, 1, 13, 2, 3, expected);
            if (DumpActualDSContent (hello1PROC2.in(), 2, 1, 13, 2, 3, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();

            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC3/1 **BEFORE** Time-Out");
            expected[0] = true, expected[1] = false;
            //DumpExpectedDSContent   (3, 1, 13, 2, 3, expected);
            if (DumpActualDSContent (hello1PROC3.in(), 3, 1, 13, 2, 3, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
            

            // Notify the FTManager that HOST1/TEST/PROC1 has died
            TEST_INFO("Send a fault report for the location HOST1/TEST/PROC1");
            fault_event.header.fixed_header.event_type.domain_name = CORBA::string_dup( "FT_CORBA" );
            fault_event.header.fixed_header.event_type.type_name = CORBA::string_dup( "ObjectCrashFault" );
            fault_event.filterable_data.length( 2 );
            fault_event.filterable_data[0].name = CORBA::string_dup( "FTDomainId" );
            fault_event.filterable_data[0].value <<= "thalesgroup.com";
            fault_event.filterable_data[1].name = CORBA::string_dup( "Location" );
            loc[0].id = "HOST1";
            loc[1].id = "TEST";
            loc[2].id = "PROC1";
            fault_event.filterable_data[1].value <<= loc;
            fault_notifier->push_structured_fault( fault_event );
            OsSupport::OS::sleep(timescale*4000); // Wait for PROC3/DSID1 time out to fire

            
            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC2/1 **AFTER** DS Time-Out");
            expected[0] = true, expected[1] = true;
            //DumpExpectedDSContent   (3, 1, 13, 2, 3, expected);
            if (DumpActualDSContent (hello1PROC2.in(), 3, 1, 13, 2, 3, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
            
            TEST_INFO("[**** TestFTLocation::do_tests] Checking DataStore PROC3/1 **AFTER** DS Time-Out");
            expected[0] = true, expected[1] = true;
            //DumpExpectedDSContent   (3, 1, 13, 2, 3, expected);
            if (DumpActualDSContent (hello1PROC3.in(), 3, 1, 13, 2, 3, expected))
                TEST_SUCCEED();
            else
                TEST_FAILED();
        }



        TEST_INFO("removing member HOST2/TEST/PROC2");
        loc[0].id = "HOST2";
        loc[1].id = "TEST";
        loc[2].id = "PROC2";
        try  {
            obj1 = rm->remove_member(obj1.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO("removing member HOST3/TEST/PROC3");
        loc[0].id = "HOST3";
        loc[1].id = "TEST";
        loc[2].id = "PROC3";
        try  {
            obj1 = rm->remove_member(obj1.in(), loc);
            TEST_SUCCEED();
        } catch( CORBA::Exception& e ) {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            TEST_FAILED();
        }

        TEST_INFO( "Killing process 2" );
        OsSupport::OS::kill_process( pid2);

        TEST_INFO( "Killing process 3" );
        OsSupport::OS::kill_process( pid3);
        //
        // END SECOND STEP
        // ######################################################################

        OsSupport::OS::sleep(timescale*2000);
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

