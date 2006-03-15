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

#include "testftlogging/TestHello_impl.hpp"
#include <Foundation/common/String.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp>
#include "testftlogging/TestClientThread.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>


using namespace Cdmw;
using Cdmw::OsSupport::OS;

TestClientThread::TestClientThread( CORBA::ORB_ptr orb,
                                    PortableServer::POA_ptr rootPOA,
                                    std::string& application_name)
    : m_name(application_name)
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
//         std::string ogd2 = OS::get_option_value( argc, argv, "--object_group_id2");
//         m_objectGroupId2 = atoi(ogd2.c_str());
        

        //
        // STEP 1 : 
        //
        std::cout<<m_name<<" : resolve the reference of the Replication Manager"<<std::endl;
        
        // resolve the reference of the Replication Manager
        
        CORBA::Object_var objref;

        objref = m_orb->resolve_initial_references("ReplicationManager");

        CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(objref.in());

	    if (CORBA::is_nil(rm.in()))
	    {
	        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	                  << "could not _narrow object to type HelloInterface" << std::endl;
	        abort();
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

        std::cout<<m_name<<" : Set_Location_Name"<<std::endl;
        Cdmw::FT::DataStoreBase::Set_Location_Name(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc));
        
        // create a reference for the PrimaryBackupGroupRepository

        ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primaryBackupGroupRepository_impl
          = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();

#if 0
        // export GroupRepository in orb
        CdmwFT::Location::PrimaryBackupGroupRepository_var primary_backup_rep_ref
        = primaryBackupGroupRepository_impl;
		
        std::cout<<m_name<<" : register_initial_reference on the FTGroupRepository"<<std::endl;
        m_orb->register_initial_reference("FTGroupRepository", primary_backup_rep_ref.in());
        std::cout<<m_name<<" : after register_initial_reference on the FTGroupRepository"<<std::endl;
#endif			  

        // create the reference for the PrimaryBackupAdmin
        std::cout<<m_name<<" : create the PrimaryBackupAdmin impl"<<std::endl;
        ::Cdmw::FT::Location::PrimaryBackupAdmin_impl* primaryBackupAdmin_impl
          = new ::Cdmw::FT::Location::PrimaryBackupAdmin_impl(m_orb.in(), m_rootPOA.in(), loc, primaryBackupGroupRepository_impl, true);
        
        std::cout<<m_name<<" : create the StatefullPrimaryBackupAdmin"<<std::endl;
        CdmwFT::Location::PrimaryBackupAdmin_var primaryBackupAdmin = primaryBackupAdmin_impl->_this();


        // register the Member Admin (the StatefullPrimaryBackupAdmin) on the Replication Manager
        rm->register_location(loc, primaryBackupAdmin.in());

        std::cout<<m_name<<" : create object for object group"<<std::endl;
        // create object hellointerface
        Cdmw::HelloInterface_impl * helloInterface_impl =
            new Cdmw::HelloInterface_impl(m_orb.in(), m_rootPOA.in());
        CdmwRequestLogging::HelloInterface_var helloInterface1 =
            helloInterface_impl->_this();

        // export the object reference to a file
        CORBA::String_var ref_string =
            m_orb->object_to_string(helloInterface1.in());
        std::ofstream os("hello1APPL1.ior");
        os << ref_string.in();
        os.close();

        m_orb->run();
        
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

 

