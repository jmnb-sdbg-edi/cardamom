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

#include "testflattransaction/TestHello_impl.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"

#include "testflattransaction/TestFTFlatTransaction.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>

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
    const std::string& host3 )
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

// determine if the process is into the list
bool TestFTFlatTransaction::contain(const std::string& process_name, const ::CdmwFlatTransaction::LocationList* list)
{
    bool result = false;
    int size = list->length();

    for (int i = 0;
         i < size;
         i++)
    {
        if (strcmp((*list)[i], process_name.c_str()) == 0)
        {
            result = true;
            break;
        } else {
            // do nothing
        }
        
    }
    return result;
}



// do_tests
void TestFTFlatTransaction::do_tests()
{
    set_nbOfRequestedTestOK (35);

    try
    {
 
        
        TEST_INFO( "Resolve the Replication Manager reference" );

        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());
        
        if (CORBA::is_nil(rm.in()))
        {
            std::cout<< "could not _narrow object to type HelloInterface" << std::endl;
            abort();
        }
        
        TEST_SUCCEED();
        
        // create a first object with an invalid criteria
        // the invalid criteria is the FaultMonitoringStyle 
        TEST_INFO("Create objects groups");

        const char * rid1 = "IDL:thalesgroup.com/CdmwFlatTransaction/HelloInterface1:1.0";

        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id1;
        ::FT::GenericFactory::FactoryCreationId_var factory_creation_id2;

        CORBA::Object_var obj1;
        CORBA::Object_var obj2;
        
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



        try
        {
            obj1 =  rm->create_object(rid1,
                                      crit1,
                                      factory_creation_id1.out());

            TEST_SUCCEED(); 
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        


        



        CdmwFlatTransaction::HelloInterface1_var hello1ObjectGroup = CdmwFlatTransaction::HelloInterface1::_narrow(obj1.in());

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

        OsSupport::OS::sleep(timescale*20000);
        
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

        CORBA::Object_var obj_repository = m_orb->string_to_object("corbaloc::localhost:42562/CdmwRepository");
        repository = CdmwNamingAndRepository::Repository::_narrow(obj_repository.in());

        
        Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                              repository.in());
        
        Cdmw::CommonSvcs::Naming::NamingInterface ni =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");
    

        std::string full_name = "dom3/P11HelloInterface1";
        
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
        
        CdmwFlatTransaction::HelloInterface1_var hello1P11 = CdmwFlatTransaction::HelloInterface1::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P11.in()) );
 

        full_name = "dom3/P21HelloInterface1";
        
        
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
        
        CdmwFlatTransaction::HelloInterface1_var hello1P21 = CdmwFlatTransaction::HelloInterface1::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P21.in()) );

        full_name = "dom3/P31HelloInterface1";
        
        
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
        
        CdmwFlatTransaction::HelloInterface1_var hello1P31 = CdmwFlatTransaction::HelloInterface1::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(hello1P11.in()) );






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
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        
        OsSupport::OS::sleep(timescale*1000);
        
        // Add new member on the group
        TEST_INFO("add the new member"<<m_host3<<"/APPL3/P31 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  locs[2],
                                  hello1P31.in());
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        try
        {        
            rm->set_first_backup (locs[2]);
            TEST_SUCCEED();
        }
        catch( CORBA::Exception& e)
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }
        
        OsSupport::OS::sleep(timescale*1000);

        // Add new member on the group
        TEST_INFO("add the new member"<<m_host1<<"/APPL1/P11 in group 1");
        try
        {
            obj1 = rm->add_member(obj1.in(),
                                  locs[0],
                                  hello1P11.in());
            TEST_SUCCEED();
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            TEST_FAILED();
        }

        
        OsSupport::OS::sleep(timescale*1000);
        
        TEST_INFO("Try to modified datastore without flat transaction");
        
        try
        {
            hello1P21->trigger_on_insert(1, 1, "1-1-one");
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P11->get_info_from_datastore(1, 1)<<" -> expected \"1-1-one\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 1), "1-1-one"));

            TEST_INFO("data for oid 1 (datastore2):"<<hello1P21->get_info_from_datastore(1, 1)<<" -> expected \"1-1-one\"");
            TEST_CHECK(!strcmp(hello1P21->get_info_from_datastore(1, 1), "1-1-one"));

            TEST_INFO("data for oid 1 (datastore3):"<<hello1P31->get_info_from_datastore(1, 1)<<" -> expected \"1-1-one\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 1), "1-1-one"));
        }
        
        catch(...)
        {
            TEST_FAILED();
        }


        TEST_INFO("Modified datastore using flat transaction");
        TEST_INFO("start to buffer transactions");
        
        try
        {

            //type insert = 0, update = 1; remove = 2

            CdmwFlatTransaction::datastructs datas;
            datas.length(6);
            datas[0].type = 0;
            datas[0].dsid = 1;
            datas[0].oid = 2;
            datas[0].data = "1-2-one";
            
            datas[1].type = 0;
            datas[1].dsid = 1;
            datas[1].oid = 3;
            datas[1].data = "1-3-one";

            datas[2].type = 1;
            datas[2].dsid = 1;
            datas[2].oid = 1;
            datas[2].data = "1-1-two";

            datas[3].type = 2;
            datas[3].dsid = 1;
            datas[3].oid = 1;
            datas[3].data = "";

            datas[4].type = 0;
            datas[4].dsid = 1;
            datas[4].oid = 1;
            datas[4].data = "1-1-three";

            datas[5].type = 1;
            datas[5].dsid = 1;
            datas[5].oid = 2;
            datas[5].data = "1-2-two";

            TEST_CHECK(hello1P21->begin_without_commit(datas));
        }
        catch(...)
        {
            TEST_FAILED();
        }

        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("Check if the modification is just realised in the scope of the transaction");
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P11->get_info_from_datastore(1, 1)<<" -> expected \"1-1-one\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 1), "1-1-one"));

            TEST_INFO("data for oid 1 (datastore2):"<<hello1P21->get_info_from_datastore(1, 1)<<" -> expected \"1-1-one\"");
            TEST_CHECK(!strcmp(hello1P21->get_info_from_datastore(1, 1), "1-1-one"));

            TEST_INFO("data for oid 1 (datastore3):"<<hello1P31->get_info_from_datastore(1, 1)<<" -> expected \"1-1-one\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 1), "1-1-one"));

            // test new values
            TEST_INFO("data for oid 2 (datastore1):"<<hello1P11->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 2), "NotFoundException"));

            TEST_INFO("data for oid 2 (datastore2):"<<hello1P21->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P21->get_info_from_datastore(1, 2), "NotFoundException"));

            TEST_INFO("data for oid 2 (datastore3):"<<hello1P31->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 2), "NotFoundException"));
            
        }
        
        catch(...)
        {
            TEST_FAILED();
        }

        try
        {
            // modif on the datastore 1
            //type insert = 0, update = 1; remove = 2

            CdmwFlatTransaction::datastructs datas;
            datas.length(9);
            datas[0].type = 0;
            datas[0].dsid = 1;
            datas[0].oid = 2;
            datas[0].data = "1-2-one";
            
            datas[1].type = 0;
            datas[1].dsid = 1;
            datas[1].oid = 3;
            datas[1].data = "1-3-one";

            datas[2].type = 1;
            datas[2].dsid = 1;
            datas[2].oid = 1;
            datas[2].data = "1-1-two";

            datas[3].type = 2;
            datas[3].dsid = 1;
            datas[3].oid = 1;
            datas[3].data = "";

            datas[4].type = 0;
            datas[4].dsid = 1;
            datas[4].oid = 1;
            datas[4].data = "1-1-three";

            datas[5].type = 1;
            datas[5].dsid = 1;
            datas[5].oid = 2;
            datas[5].data = "1-2-two";

            datas[6].type = 2;
            datas[6].dsid = 1;
            datas[6].oid = 2;
            datas[6].data = "";

            datas[7].type = 1;
            datas[7].dsid = 1;
            datas[7].oid = 3;
            datas[7].data = "1-3-two";

            datas[8].type = 1;
            datas[8].dsid = 1;
            datas[8].oid = 1;
            datas[8].data = "1-1-four";




            TEST_CHECK(hello1P21->begin_with_commit(datas));
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        
        OsSupport::OS::sleep(timescale*1000);
        TEST_INFO("Get info from data store");
        try
        {
            TEST_INFO("Check if the modification is realise on all the datastore (primary and backups)");
            TEST_INFO("data for oid 1 (datastore1):"<<hello1P11->get_info_from_datastore(1, 1)<<" -> expected \"1-1-four\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 1), "1-1-four"));

            TEST_INFO("data for oid 1 (datastore2):"<<hello1P21->get_info_from_datastore(1, 1)<<" -> expected \"1-1-four\"");
            TEST_CHECK(!strcmp(hello1P21->get_info_from_datastore(1, 1), "1-1-four"));

            TEST_INFO("data for oid 1 (datastore3):"<<hello1P31->get_info_from_datastore(1, 1)<<" -> expected \"1-1-four\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 1), "1-1-four"));

            // test new values
            TEST_INFO("data for oid 2 (datastore1):"<<hello1P11->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 2), "NotFoundException"));

            TEST_INFO("data for oid 2 (datastore2):"<<hello1P21->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P21->get_info_from_datastore(1, 2), "NotFoundException"));

            TEST_INFO("data for oid 2 (datastore3):"<<hello1P31->get_info_from_datastore(1, 2)<<" -> expected \"NotFoundException\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 2), "NotFoundException"));
            
            TEST_INFO("data for oid 3 (datastore1):"<<hello1P11->get_info_from_datastore(1, 3)<<" -> expected \"1-3-two\"");
            TEST_CHECK(!strcmp(hello1P11->get_info_from_datastore(1, 3), "1-3-two"));

            TEST_INFO("data for oid 3 (datastore2):"<<hello1P21->get_info_from_datastore(1, 3)<<" -> expected \"1-3-two\"");
            TEST_CHECK(!strcmp(hello1P21->get_info_from_datastore(1, 3), "1-3-two"));

            TEST_INFO("data for oid 3 (datastore3):"<<hello1P31->get_info_from_datastore(1, 3)<<" -> expected \"1-3-two\"");
            TEST_CHECK(!strcmp(hello1P31->get_info_from_datastore(1, 3), "1-3-two"));
        }        
        catch(...)
        {
            TEST_FAILED();
        }

        TEST_INFO("Try to initiate transaction from a backup");
        try
        {
            hello1P11->begin();
            TEST_FAILED();
        }
        catch(CORBA::COMM_FAILURE)
        {
            TEST_SUCCEED();
        }
        catch(...)
        {
            TEST_FAILED();
        }

        Cdmw::NamingAndRepository::RepositoryInterface::finish();
        
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

}; // namespace FT
}; // namespace Cdmw

