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


#include <Foundation/common/String.hpp>

#include <fstream>

#include "testftlocationmanager/TestFTActivationHandler_impl.hpp"
#include "testftlocationmanager/TestSync_impl.hpp"

#include "testftlocationmanager/TestClientThread.hpp"
#include "testftlocationmanager/TestHello_impl.hpp"
#include "testftlocationmanager/TestPrimaryBackupAdmin.hpp"


using namespace Cdmw;
using Cdmw::OsSupport::OS;

TestClientThread::TestClientThread( CORBA::ORB_ptr orb,
                                    PortableServer::POA_ptr rootPOA,
                                    std::string& application_name)
    : //m_activation (false),
      m_name(application_name)
{

    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    
}


void
TestClientThread::start(int& argc, char** argv ) throw() 
{
    try
    {
        std::string ogd1 = OS::get_option_value( argc, argv, "--object_group_id1");
        m_objectGroupId1 = atoi(ogd1.c_str());
        std::string ogd2 = OS::get_option_value( argc, argv, "--object_group_id2");
        m_objectGroupId2 = atoi(ogd2.c_str());
        

        //
        // STEP 1 : 
        //
        std::cout<<m_name<<" : resolve the reference of the Replication Manager"<<std::endl;
        
        // resolve the reference of the Replication Manager
        
        CORBA::Object_var objref;
        
        
        std::cout<<"resolve the reference of the Replication Manager"<<std::endl;
        objref = m_orb->resolve_initial_references("ReplicationManager");

//        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());

	    if (CORBA::is_nil(rm.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type HelloInterface" << std::endl;
	        abort();
	    }



        Cdmw::SynchInterface_impl * Synch_impl = new Cdmw::SynchInterface_impl();
        CdmwLocationManager::Synch_var synch = Synch_impl->_this();

        CORBA::String_var ref_string = m_orb->object_to_string(synch.in());
        if ( m_name == "APPL1")
        {
        std::ofstream os("SynchAPPL1.ior");
        os << ref_string.in();
        os.close();
        } else {
        std::ofstream os("SynchAPPL2.ior");
        os << ref_string.in();
        os.close();
        }



        ::FT::Location loc;
        loc.length(3);
        if ( m_name == "APPL1")
        {
            loc[0].id = "HOST1";
            loc[1].id = "APPL1";
            loc[2].id = "PROC1";
        } else {
            loc[0].id = "HOST2";
            loc[1].id = "APPL2";
            loc[2].id = "PROC2";
        }
        loc[0].kind = "hostname";
        loc[1].kind = "application";
        loc[2].kind = "process";    
        

        // create a reference for the PrimaryBackupGroupRepository

        ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primaryBackupGroupRepository_impl
          = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();

		  // export GroupRepository in orb
		  CdmwFT::Location::PrimaryBackupGroupRepository_var primary_backup_rep_ref
			  = primaryBackupGroupRepository_impl;
		  
		  m_orb->register_initial_reference("FTGroupRepository", 
				                              primary_backup_rep_ref.in());
				  
        // create a reference for the PrimaryBackupAdmin
        ::Cdmw::FT::Location::PrimaryBackupAdmin_impl * primaryBackupAdmin_impl
          = new ::Cdmw::FT::Location::PrimaryBackupAdmin_impl(m_orb.in(), m_rootPOA.in(), loc, primaryBackupGroupRepository_impl);
        
        CdmwFT::Location::PrimaryBackupAdmin_var primaryBackupAdmin = primaryBackupAdmin_impl->_this();

        // export the object reference to a file
        ref_string = m_orb->object_to_string(primaryBackupAdmin.in());
        if ( m_name == "APPL1")
        {
            std::ofstream os("pbaAPPL1.ior");
            os << ref_string.in();
            os.close();
        } else {
            std::ofstream os("pbaAPPL2.ior");
            os << ref_string.in();
            os.close();
        }

        
        
        // register the Member Admin (the PrimaryBackupAdmin) on the Replication Manager
        rm->register_location(loc, primaryBackupAdmin.in());

        Cdmw::HelloInterface1_impl * helloInterface1_impl = new Cdmw::HelloInterface1_impl();
        CdmwReplicationManager::HelloInterface1_var helloInterface1 =  helloInterface1_impl->_this();



        ref_string = m_orb->object_to_string(helloInterface1.in());
        if ( m_name == "APPL1")
        {
        std::ofstream os("HelloInterface1_1.ior");
        os << ref_string.in();
        os.close();
        } else {
        std::ofstream os("HelloInterface1_2.ior");
        os << ref_string.in();
        os.close();
        }




        // Add new member on the group1
        std::cout<<m_name<<" : get the reference of the object group"<<std::endl;
        try
        {
            m_objectGroup1 = rm->get_object_group_ref_from_gid(m_objectGroupId1);
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        // Add new member on the group1
        std::cout<<m_name<<" : add a new member"<<std::endl;
        try
        {
            m_objectGroup1 = rm->add_member(m_objectGroup1.in(),
                                  loc,
                                  helloInterface1.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        
        // resolve the Activation Manager
        
        objref = m_orb->resolve_initial_references("FTActivationManager");

        CdmwFT::Location::ActivationManager_var activationManager = CdmwFT::Location::ActivationManager::_narrow(objref.in());
        
        // create an Activation Handle
        ::Cdmw::FT::TestFTActivationHandler_impl * testFTActivationHandler_impl1
          = new ::Cdmw::FT::TestFTActivationHandler_impl();
        
         // register the Activation Handle into the Activation Manager
        ::CdmwFT::Location::HandlerId activationManagerHandler1;
        
        try
        {
            activationManagerHandler1 = activationManager->register_activation_handler(m_objectGroup1.in(), testFTActivationHandler_impl1);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }


        

        Cdmw::HelloInterface2_impl * helloInterface2_impl = new Cdmw::HelloInterface2_impl();
        CdmwReplicationManager::HelloInterface2_var helloInterface2 =  helloInterface2_impl->_this();

        ref_string = m_orb->object_to_string(helloInterface2.in());
        if ( m_name == "APPL1")
        {
        std::ofstream os("HelloInterface2_1.ior");
        os << ref_string.in();
        os.close();
        } else {
        std::ofstream os("HelloInterface2_2.ior");
        os << ref_string.in();
        os.close();
        }

        std::cout<<m_name<<" : get the reference of the object group"<<std::endl;
        try
        {
            m_objectGroup2 = rm->get_object_group_ref_from_gid(m_objectGroupId2);
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        // Add new member on the group2
        std::cout<<m_name<<" : add a new member"<<std::endl;
        try
        {
            m_objectGroup2 = rm->add_member(m_objectGroup2.in(),
                                            loc,
                                            helloInterface2.in());
        }
        
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        // create an Activation Handle
        ::Cdmw::FT::TestFTActivationHandler_impl * testFTActivationHandler_impl2
          = new ::Cdmw::FT::TestFTActivationHandler_impl();
        
         // register the Activation Handle into the Activation Manager
        ::CdmwFT::Location::HandlerId activationManagerHandler2;
        
        try
        {
            activationManagerHandler2 = activationManager->register_activation_handler(m_objectGroup2.in(), testFTActivationHandler_impl2);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }




        int timescale = Cdmw::TestUtils::Testable::get_timescale();
 
        Synch_impl->m_flags = true;
        OsSupport::OS::sleep(timescale*3000);
        do
        {
            OsSupport::OS::sleep(timescale*1000);
        }
        while ( Synch_impl->m_flags );
        
        std::cout << m_name << " continue 1 received - start step 1" << std::endl;


        // get information about group 1
        
        try
        {
            Synch_impl->m_result_get_object_group = primaryBackupGroupRepository_impl->get_object_group (helloInterface1.in());
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_other_group_members = primaryBackupGroupRepository_impl->get_other_group_members (helloInterface1.in());
            
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_object_group_version = primaryBackupGroupRepository_impl->get_object_group_version (helloInterface1.in());
            
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        try
        {
            Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface1.in());
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        ::FT::ObjectGroupId object_group_id = rm->get_object_group_id(Synch_impl->m_result_get_object_group.in());

        try
        {
            Synch_impl->m_result_get_object_group_from_gid = primaryBackupGroupRepository_impl->get_object_group_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        try
        {
            Synch_impl->m_result_get_other_group_members_from_gid = primaryBackupGroupRepository_impl->get_other_group_members_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_object_group_version_from_gid = primaryBackupGroupRepository_impl->get_object_group_version_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }
        
        Synch_impl->m_flags = true;
        OsSupport::OS::sleep(timescale*3000);
        do
        {
            OsSupport::OS::sleep(timescale*1000);
        }
        while ( Synch_impl->m_flags );
        
        std::cout << m_name << " continue 2 received - start step 2" << std::endl;


        //
        // STEP 2 : check if the group info is changed after the update_group_view
        //

        // get information about group 1
        
        try
        {
            Synch_impl->m_result_get_object_group = primaryBackupGroupRepository_impl->get_object_group (helloInterface1.in());
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_other_group_members = primaryBackupGroupRepository_impl->get_other_group_members (helloInterface1.in());
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_object_group_version = primaryBackupGroupRepository_impl->get_object_group_version (helloInterface1.in());
            
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        try
        {
            Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface1.in());
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        object_group_id = rm->get_object_group_id(Synch_impl->m_result_get_object_group.in());

        try
        {
            Synch_impl->m_result_get_object_group_from_gid = primaryBackupGroupRepository_impl->get_object_group_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        try
        {
            Synch_impl->m_result_get_other_group_members_from_gid = primaryBackupGroupRepository_impl->get_other_group_members_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_object_group_version_from_gid = primaryBackupGroupRepository_impl->get_object_group_version_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }
        
        Synch_impl->m_flags = true;

        OsSupport::OS::sleep(timescale*3000);
        do
        {
            OsSupport::OS::sleep(timescale*1000);
        }
        while ( Synch_impl->m_flags );
        
        std::cout << m_name << " continue 2-1 received" << std::endl;


        Synch_impl->m_flags = true;

        OsSupport::OS::sleep(timescale*3000);
        do
        {
            OsSupport::OS::sleep(timescale*1000);
        }
        while ( Synch_impl->m_flags );
        
        std::cout << m_name << " continue 3 received - start step 3" << std::endl;


        //
        // STEP 3 : check if the primary is been set and if the activation handler is activated or not
        //

        // get information about group 1

        try
        {
            Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface1.in());
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        try
        {
            Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }

        OsSupport::OS::sleep(timescale*500);
        try
        {
            Synch_impl->m_activation =  testFTActivationHandler_impl1->m_activation;
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }


        Synch_impl->m_flags = true;
        OsSupport::OS::sleep(timescale*3000);
        do
        {
            OsSupport::OS::sleep(timescale*1000);
        }
        while ( Synch_impl->m_flags );
        
        std::cout << m_name << " continue 4 received - start step 4" << std::endl;


        //
        //STEP 4 : check if the primary is been set and if the activation handler is activated or not (the primary is changed)
        //

        // get information about group 1

        try
        {
            Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface1.in());
        }
        catch (const ::FT::MemberNotFound& ex)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ 
                      << ex <<std::endl; 
        }
        catch (const CORBA::SystemException& ex)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ 
                      << ex <<std::endl; 
        }
    
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        try
        {
            Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }

        OsSupport::OS::sleep(timescale*500);
        try
        {
            Synch_impl->m_activation =  testFTActivationHandler_impl1->m_activation;
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }


        Synch_impl->m_flags = true;

        OsSupport::OS::sleep(timescale*3000);
        do
        {
            OsSupport::OS::sleep(timescale*1000);
        }
        while ( Synch_impl->m_flags );
        
        std::cout << m_name << " continue 5 received - start step 5" << std::endl;

        //
        // STEP 5 : check if the group info of the second object is not changed
        //

        // get information about group 2
        
        try
        {
            Synch_impl->m_result_get_object_group = primaryBackupGroupRepository_impl->get_object_group (helloInterface2.in());
        }
        catch (const ::FT::MemberNotFound& )
        {
            std::cerr << m_name <<  "MemberNotFound exception raised" << std::endl;
            std::cerr <<m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << m_name <<  "CORBA::SystemException raised: " << e << std::endl;
            std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        catch (...)
        {
            std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        try
        {
            Synch_impl->m_result_get_other_group_members = primaryBackupGroupRepository_impl->get_other_group_members (helloInterface2.in());
        }
        catch (const ::FT::MemberNotFound& )
        {
            std::cerr << m_name <<  "MemberNotFound exception raised" << std::endl;
            std::cerr <<m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << m_name <<  "CORBA::SystemException raised: " << e << std::endl;
            std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        catch (...)
        {
            std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_object_group_version = primaryBackupGroupRepository_impl->get_object_group_version (helloInterface2.in());
            
        }
        catch (const ::FT::MemberNotFound& )
        {
            std::cerr << m_name <<  "MemberNotFound exception raised" << std::endl;
            std::cerr <<m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << m_name <<  "CORBA::SystemException raised: " << e << std::endl;
            std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        catch (...)
        {
            std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }

        try
        {
            Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface2.in());
        }
        catch (const ::FT::MemberNotFound& )
        {
            std::cerr << m_name <<  "MemberNotFound exception raised" << std::endl;
            std::cerr <<m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << m_name <<  "CORBA::SystemException raised: " << e << std::endl;
            std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        catch (...)
        {
            std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        ::FT::ObjectGroupId object_group_id2 = rm->get_object_group_id(Synch_impl->m_result_get_object_group.in());

        try
        {
            Synch_impl->m_result_get_object_group_from_gid = primaryBackupGroupRepository_impl->get_object_group_from_gid (object_group_id2);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        try
        {
            Synch_impl->m_result_get_other_group_members_from_gid = primaryBackupGroupRepository_impl->get_other_group_members_from_gid (object_group_id2);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_object_group_version_from_gid = primaryBackupGroupRepository_impl->get_object_group_version_from_gid (object_group_id2);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id2);
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }
        
        Synch_impl->m_flags = true;


        OsSupport::OS::sleep(timescale*3000);
        do
        {
            OsSupport::OS::sleep(timescale*1000);
        }
        while ( Synch_impl->m_flags );
        
        std::cout << m_name << " continue 6 received - start step 6" << std::endl;

        //
        // STEP 6 : check if the second object doesn't exist after the remove on the fisrt object
        //

        // get information about group 2
        if (m_name == "APPL1")
        {
            try
            {
                Synch_impl->m_result_get_object_group = primaryBackupGroupRepository_impl->get_object_group (helloInterface2.in());
                object_group_id2 = rm->get_object_group_id(Synch_impl->m_result_get_object_group.in());
                std::cout << m_name << " get_object_group on group id = " 
                          << object_group_id2 << std::endl;
            }
            catch (const ::FT::MemberNotFound& )
            {
                std::cerr << m_name <<  "MemberNotFound exception raised" << std::endl;
                std::cerr <<m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << m_name <<  "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_get_other_group_members = primaryBackupGroupRepository_impl->get_other_group_members (helloInterface2.in());
            }
            catch (const ::FT::MemberNotFound& )
            {
                std::cerr << m_name << "MemberNotFound exception raised" << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_get_object_group_version = primaryBackupGroupRepository_impl->get_object_group_version (helloInterface2.in());
                
            }
            catch (const ::FT::MemberNotFound& )
            {
                std::cerr << m_name << "MemberNotFound exception raised" << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            
            try
            {
                Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface2.in());
            }
            catch (const ::FT::MemberNotFound& )
            {
                std::cerr << m_name << "MemberNotFound exception raised" << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_get_object_group_from_gid = primaryBackupGroupRepository_impl->get_object_group_from_gid (object_group_id2);
            }
            catch (const ::FT::ObjectGroupNotFound& )
            {
                std::cerr << m_name << "ObjectGroupNotFound exception raised" << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            
            try
            {
                Synch_impl->m_result_get_other_group_members_from_gid = primaryBackupGroupRepository_impl->get_other_group_members_from_gid (object_group_id2);
            }
            catch (const ::FT::ObjectGroupNotFound& )
            {
                std::cerr << m_name << "ObjectGroupNotFound exception raised" << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_get_object_group_version_from_gid = primaryBackupGroupRepository_impl->get_object_group_version_from_gid (object_group_id2);
            }
            catch (const ::FT::ObjectGroupNotFound& )
            {
                std::cerr << m_name << "ObjectGroupNotFound exception raised" << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id2);
            }
            catch (const ::FT::ObjectGroupNotFound& )
            {
                std::cerr << m_name << "ObjectGroupNotFound exception raised" << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const CORBA::SystemException& e)
            {
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl;
            }
        }
        else  // APPL2
        {
            try
            {
                Synch_impl->m_result_get_object_group = primaryBackupGroupRepository_impl->get_object_group (helloInterface2.in());
                object_group_id2 = rm->get_object_group_id(Synch_impl->m_result_get_object_group.in());
                std::cerr <<m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const ::FT::MemberNotFound& )
            {
                Synch_impl->m_result_get_object_group = ::FT::ObjectGroup::_nil();
                std::cerr << m_name <<  "MemberNotFound exception raised" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                Synch_impl->m_result_get_object_group = ::FT::ObjectGroup::_nil();
                std::cerr << m_name <<  "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                Synch_impl->m_result_get_object_group = ::FT::ObjectGroup::_nil();
                std::cerr << m_name <<  " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_get_other_group_members = primaryBackupGroupRepository_impl->get_other_group_members (helloInterface2.in());
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
                
            }
            catch (const ::FT::MemberNotFound& )
            {
                Synch_impl->m_result_get_other_group_members = new ::CdmwFT::MemberInfos(1);
                Synch_impl->m_result_get_other_group_members->length(0);
                std::cerr << m_name << "MemberNotFound exception raised" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                Synch_impl->m_result_get_other_group_members = new ::CdmwFT::MemberInfos(1);
                Synch_impl->m_result_get_other_group_members->length(0);
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                Synch_impl->m_result_get_other_group_members = new ::CdmwFT::MemberInfos(1);
                Synch_impl->m_result_get_other_group_members->length(0);
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_get_object_group_version = primaryBackupGroupRepository_impl->get_object_group_version (helloInterface2.in());
                
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const ::FT::MemberNotFound& )
            {
                Synch_impl->m_result_get_object_group_version = 0;
                std::cerr << m_name << "MemberNotFound exception raised" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                Synch_impl->m_result_get_object_group_version = 0;
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                Synch_impl->m_result_get_object_group_version = 0;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            
            try
            {
                Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface2.in());
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const ::FT::MemberNotFound& )
            {
                Synch_impl->m_result_is_primary = false;
                std::cerr << m_name << "MemberNotFound exception raised" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                Synch_impl->m_result_is_primary = false;
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                Synch_impl->m_result_is_primary = false;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_get_object_group_from_gid = primaryBackupGroupRepository_impl->get_object_group_from_gid (object_group_id2);
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const ::FT::ObjectGroupNotFound& )
            {
                Synch_impl->m_result_get_object_group_from_gid = ::FT::ObjectGroup::_nil();
                std::cerr << m_name << "ObjectGroupNotFound exception raised" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                Synch_impl->m_result_get_object_group_from_gid = ::FT::ObjectGroup::_nil();
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                Synch_impl->m_result_get_object_group_from_gid = ::FT::ObjectGroup::_nil();
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            
            try
            {
                Synch_impl->m_result_get_other_group_members_from_gid = primaryBackupGroupRepository_impl->get_other_group_members_from_gid (object_group_id2);
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const ::FT::ObjectGroupNotFound& )
            {
                Synch_impl->m_result_get_other_group_members_from_gid = new ::CdmwFT::MemberInfos(1);
                Synch_impl->m_result_get_other_group_members_from_gid->length(0);
                std::cerr << m_name << "ObjectGroupNotFound exception raised" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                Synch_impl->m_result_get_other_group_members_from_gid = new ::CdmwFT::MemberInfos(1);
                Synch_impl->m_result_get_other_group_members_from_gid->length(0);
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                Synch_impl->m_result_get_other_group_members_from_gid = new ::CdmwFT::MemberInfos(1);
                Synch_impl->m_result_get_other_group_members_from_gid->length(0);
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_get_object_group_version_from_gid = primaryBackupGroupRepository_impl->get_object_group_version_from_gid (object_group_id2);
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const ::FT::ObjectGroupNotFound& )
            {
                Synch_impl->m_result_get_object_group_version_from_gid = 0;
                std::cerr << m_name << "ObjectGroupNotFound exception raised" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                Synch_impl->m_result_get_object_group_version_from_gid = 0;
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                Synch_impl->m_result_get_object_group_version_from_gid = 0;
                std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
             
            try
            {
                Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id2);
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (const ::FT::ObjectGroupNotFound& )
            {
                Synch_impl->m_result_is_primary_from_gid = false;
                std::cerr << m_name << "ObjectGroupNotFound exception raised" << std::endl;
            }
            catch (const CORBA::SystemException& e)
            {
                Synch_impl->m_result_is_primary_from_gid = false;
                std::cerr << m_name << "CORBA::SystemException raised: " << e << std::endl;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl; 
            }
            catch (...)
            {
                Synch_impl->m_result_is_primary_from_gid = false;
                std::cerr << m_name << " **** TEST FAILED AT LINE " << __LINE__ 
                          << " IN FILE: " << __FILE__ << std::endl;
            }
        }
        
        Synch_impl->m_flags = true;

        OsSupport::OS::sleep(timescale*3000);
        do
        {
            OsSupport::OS::sleep(timescale*1000);
        }
        while ( Synch_impl->m_flags );

        std::cout << m_name << " continue 7 received - start step 7" << std::endl;

        //
        // STEP 7 : Check primary for the first object
        //

        // get information about group 1
        
        try
        {
            Synch_impl->m_result_get_object_group = primaryBackupGroupRepository_impl->get_object_group (helloInterface1.in());
            
        }
        catch (const ::FT::MemberNotFound& e )
        {
            std::cerr<<"const ::FT::MemberNotFound& e"<<std::endl;
            Synch_impl->m_result_get_object_group = ::FT::ObjectGroup::_nil();
        } 
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_other_group_members = primaryBackupGroupRepository_impl->get_other_group_members (helloInterface1.in());
        }
        catch (const ::FT::MemberNotFound& e )
        {
            std::cerr<<"const ::FT::MemberNotFound& e"<<std::endl;
            Synch_impl->m_result_get_other_group_members = new ::CdmwFT::MemberInfos(1);
            Synch_impl->m_result_get_other_group_members->length(0);
        } 
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_object_group_version = primaryBackupGroupRepository_impl->get_object_group_version (helloInterface1.in());
            
        }
        catch (const ::FT::MemberNotFound& e )
        {
            std::cerr<<"const ::FT::MemberNotFound& e"<<std::endl;
            Synch_impl->m_result_get_object_group_version = 0;
        } 
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        try
        {
            Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface1.in());
        }
        catch (const ::FT::MemberNotFound& e )
        {
            std::cerr<<"const ::FT::MemberNotFound& e"<<std::endl;
            Synch_impl->m_result_is_primary = false;
        } 
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        object_group_id = m_objectGroupId1;
        std::cout<<"object_group_id"<<object_group_id<<std::endl;
        
        try
        {
            Synch_impl->m_result_get_object_group_from_gid = primaryBackupGroupRepository_impl->get_object_group_from_gid (object_group_id);
        }
        catch (const ::FT::ObjectGroupNotFound& e )
        {
            std::cerr<<"const ::FT::ObjectGroupNotFound& e"<<std::endl;
            Synch_impl->m_result_get_object_group_from_gid = ::FT::ObjectGroup::_nil();
        } 
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
        
        try
        {
            Synch_impl->m_result_get_other_group_members_from_gid = primaryBackupGroupRepository_impl->get_other_group_members_from_gid (object_group_id);
        }
        catch (const ::FT::ObjectGroupNotFound& e )
        {
            std::cerr<<"const ::FT::ObjectGroupNotFound& e"<<std::endl;
            Synch_impl->m_result_get_other_group_members_from_gid = new ::CdmwFT::MemberInfos(1);
            Synch_impl->m_result_get_other_group_members_from_gid->length(0);
        } 
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;

        }
       catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_get_object_group_version_from_gid = primaryBackupGroupRepository_impl->get_object_group_version_from_gid (object_group_id);
        }
        catch (const ::FT::ObjectGroupNotFound& e )
        {
            std::cerr<<"const ::FT::ObjectGroupNotFound& e"<<std::endl;
            Synch_impl->m_result_get_object_group_version_from_gid = 0;
        } 
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
        }
         
        try
        {
            Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id);
        }
        catch (const ::FT::ObjectGroupNotFound& e )
        {
            std::cerr<<"const ::FT::ObjectGroupNotFound& e"<<std::endl;
            Synch_impl->m_result_is_primary_from_gid = false;
        } 
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            
        }
        catch (...)
        {
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl;
        }
        
        Synch_impl->m_flags = true;

//         OsSupport::OS::sleep(timescale*3000);
//         do
//         {
//             OsSupport::OS::sleep(timescale*1000);
//         }
//         while ( Synch_impl->m_flags );

//         std::cout << m_name << " continue 8 received - start step 8" << std::endl;


//         //
//         // STEP 8 : 
//         //


//         try
//         {
//             Synch_impl->m_result_get_object_group = primaryBackupGroupRepository_impl->get_object_group (helloInterface1.in());
            
//         }
//         catch (const ::FT::MemberNotFound& e )
//         {
//             std::cerr<<"const ::FT::MemberNotFound& e"<<std::endl;
//             Synch_impl->m_result_get_object_group = ::FT::ObjectGroup::_nil();
//         } 
//         catch (...)
//         {
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl; 
//         }
         
//         try
//         {
//             Synch_impl->m_result_get_other_group_members = primaryBackupGroupRepository_impl->get_other_group_members (helloInterface1.in());
            
            
//         }
//         catch (const ::FT::MemberNotFound& e )
//         {
//             std::cerr<<"const ::FT::MemberNotFound& e"<<std::endl;
//             Synch_impl->m_result_get_other_group_members = new ::CdmwFT::MemberInfos();
//         } 
//         catch (...)
//         {
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl; 
//         }
         
//         try
//         {
//             Synch_impl->m_result_get_object_group_version = primaryBackupGroupRepository_impl->get_object_group_version (helloInterface1.in());
            
//         }
//         catch (const ::FT::MemberNotFound& e )
//         {
//             std::cerr<<"const ::FT::MemberNotFound& e"<<std::endl;
//             Synch_impl->m_result_get_object_group_version = 0;
//         } 
//         catch (...)
//         {
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl; 
//         }
        
//         try
//         {
//             Synch_impl->m_result_is_primary = primaryBackupGroupRepository_impl->is_primary (helloInterface1.in());
//         }
//         catch (const ::FT::MemberNotFound& e )
//         {
//             std::cerr<<"const ::FT::MemberNotFound& e"<<std::endl;
//             Synch_impl->m_result_is_primary = false;
//         } 
//         catch (...)
//         {
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl; 
//         }
         
//         object_group_id = m_objectGroupId1;
//         std::cout<<"object_group_id"<<object_group_id<<std::endl;
        
//         try
//         {
//             Synch_impl->m_result_get_object_group_from_gid = primaryBackupGroupRepository_impl->get_object_group_from_gid (object_group_id);
//         }
//         catch (const ::FT::ObjectGroupNotFound& e )
//         {
//             std::cerr<<"const ::FT::ObjectGroupNotFound& e"<<std::endl;
//             Synch_impl->m_result_get_object_group_from_gid = ::FT::ObjectGroup::_nil();
//         } 
//         catch( CORBA::Exception& e )
//         {
//             std::cerr << e._name() << std::endl;
            
//         }
//         catch (...)
//         {
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl; 
//         }
        
//         try
//         {
//             Synch_impl->m_result_get_other_group_members_from_gid = primaryBackupGroupRepository_impl->get_other_group_members_from_gid (object_group_id);
//         }
//         catch (const ::FT::ObjectGroupNotFound& e )
//         {
//             std::cerr<<"const ::FT::ObjectGroupNotFound& e"<<std::endl;
//             Synch_impl->m_result_get_other_group_members_from_gid = new ::CdmwFT::MemberInfos();
//         } 
//         catch( CORBA::Exception& e )
//         {
//             std::cerr << e._name() << std::endl;

//         }
//         catch (...)
//         {
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl; 
//         }
         
//         try
//         {
//             Synch_impl->m_result_get_object_group_version_from_gid = primaryBackupGroupRepository_impl->get_object_group_version_from_gid (object_group_id);
//         }
//         catch (const ::FT::ObjectGroupNotFound& e )
//         {
//             std::cerr<<"const ::FT::ObjectGroupNotFound& e"<<std::endl;
//             Synch_impl->m_result_get_object_group_version_from_gid = 0;
//         } 
//         catch( CORBA::Exception& e )
//         {
//             std::cerr << e._name() << std::endl;
            
//         }
//         catch (...)
//         {
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl; 
//         }
         
//         try
//         {
//             Synch_impl->m_result_is_primary_from_gid = primaryBackupGroupRepository_impl->is_primary_from_gid (object_group_id);
//         }
//         catch (const ::FT::ObjectGroupNotFound& e )
//         {
//             std::cerr<<"const ::FT::ObjectGroupNotFound& e"<<std::endl;
//             Synch_impl->m_result_is_primary_from_gid = false;
//         } 
//         catch( CORBA::Exception& e )
//         {
//             std::cerr << e._name() << std::endl;
            
//         }
//         catch (...)
//         {
//             std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
//                       << " IN FILE: " << __FILE__ << std::endl;
//         }
        
//         Synch_impl->m_flags = true;



        sleep(100);
        
    }
    
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;

    }
    catch( const Cdmw::Exception& e )
    {
        std::cerr << e.what() << std::endl;

    }
    
}

 

