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

#include <fstream>

#include <Foundation/common/String.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>

#include "testftlocationmanager3/TestClientThread.hpp"
#include "testftlocationmanager3/TestHello_impl.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>


using namespace Cdmw;
using Cdmw::OsSupport::OS;

TestClientThread::TestClientThread( CORBA::ORB_ptr          orb,
                                    PortableServer::POA_ptr rootPOA,
                                    std::string&            proc_name,
                                    int                     step)
    : //m_activation (false),
      m_name(proc_name)
    , m_target_step(step)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
}


void
TestClientThread::start(int& argc, char** argv ) throw() 
{
    try
    {
        std::string ogid = OS::get_option_value( argc, argv, "--object_group_id");
        m_objectGroupId  = atoi(ogid.c_str());
        
        //
        // STEP 1 : 
        //
        std::cout<<m_name<<" : resolve the reference of the Replication Manager"<<std::endl;
        
        // resolve the reference of the Replication Manager
        
        CORBA::Object_var objref;

        objref = m_orb->resolve_initial_references("ReplicationManager");

        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());

        if (CORBA::is_nil(rm.in()))  {
            std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                      << "could not _narrow object to type HelloInterface" << std::endl;
            abort();
        }



        ::FT::Location loc;
        loc.length(3);
        if ( m_name == "PROC1") {
            loc[0].id = "HOST1";
            loc[1].id = "TEST";
            loc[2].id = "PROC1";
        } else if ( m_name == "PROC2") {
            loc[0].id = "HOST2";
            loc[1].id = "TEST";
            loc[2].id = "PROC2";
        } else {
            loc[0].id = "HOST3";
            loc[1].id = "TEST";
            loc[2].id = "PROC3";
        }

        std::cout<<m_name<<" : Set_Location_Name"<<std::endl;
        Cdmw::FT::DataStoreBase::Set_Location_Name(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc));
        

        // create a reference for the PrimaryBackupGroupRepository
        ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primaryBackupGroupRepository_impl
          = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();

        // export GroupRepository in orb
        CdmwFT::Location::PrimaryBackupGroupRepository_var primary_backup_rep_ref
        = primaryBackupGroupRepository_impl;
		
        std::cout<<m_name<<" : register_initial_reference on the FTGroupRepository"<<std::endl;
        m_orb->register_initial_reference("FTGroupRepository", primary_backup_rep_ref.in());
				  

        // create the reference for the StatefullPrimaryBackupAdmin
        ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl* statefullPrimaryBackupAdmin_impl
            = new ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl (
                                                                          m_orb.in(), m_rootPOA.in(), loc, primaryBackupGroupRepository_impl, true);
        
        std::cout<<m_name<<" : create the StatefullPrimaryBackupAdmin"<<std::endl;
        CdmwFT::Location::StatefullPrimaryBackupAdmin_var statefullPrimaryBackupAdmin
            = statefullPrimaryBackupAdmin_impl->_this();


        std::cout<<m_name<<" : create object for object group"<<std::endl;
        // create object hellointerface1
        Cdmw::HelloInterface_impl * helloInterface_impl
            = new ::Cdmw::HelloInterface_impl (
                      m_orb.in(), m_rootPOA.in(), m_name.c_str(), m_target_step);
        CdmwReplicationManager::HelloInterface_var helloInterface
            =  helloInterface_impl->_this();


        
        // register the Member Admin (the StatefullPrimaryBackupAdmin) on the Replication Manager
        // rm->register_location(loc, statefullPrimaryBackupAdmin.in());

        ::CdmwFT::StateTransfer::LocalDataStoreInfos infos;
        infos.length(1);
        infos[0] = *helloInterface->get_local_datastore_info();

        std::cout<<m_name<<" : register_statefull_location"<<std::endl;
        rm->register_statefull_location(loc,
                                        statefullPrimaryBackupAdmin.in(),
                                        infos, false);
        

        // export the object reference to a file
        ::CORBA::String_var ref_string = m_orb->object_to_string(helloInterface.in());
        if ( m_name == "PROC1")
        {
            std::ofstream os("hello1PROC1.ior");
            os << ref_string.in();
            os.close();
        } else if ( m_name == "PROC2") {
            std::ofstream os("hello1PROC2.ior");
            os << ref_string.in();
            os.close();
        } else {
            std::ofstream os("hello1PROC3.ior");
            os << ref_string.in();
            os.close();
        }

        m_orb->run();
    }
    catch( CORBA::Exception& e ) {
        std::cerr << e._name() << std::endl;
    }
    catch( const Cdmw::Exception& e ) {
        std::cerr << e.what() << std::endl;
    }
    
}

 

