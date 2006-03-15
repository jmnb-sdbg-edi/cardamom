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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"
#include "testgroupcreator/TestHello.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "testgroupcreator/FTGroupCreatorTester.hpp"
#include <fstream>
#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif
#include <sstream>

namespace Cdmw {
namespace FT {
        

TestGroupCreator::TestGroupCreator(CORBA::ORB_ptr                  orb,
                                   PortableServer::POA_ptr         rootPOA,
                                   const std::string& name)
    : Testable( name ),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(rootPOA))
{
}


TestGroupCreator::~TestGroupCreator()
{
}

void TestGroupCreator::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK( 8 );
    int timescale = Cdmw::TestUtils::Testable::get_timescale();

    try{

        TEST_INFO("Start the fault manager");
        std::string managerArgs(" --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml --groupConf=CdmwFTGroupCreator1_conf.xml  --validateXML");
        OsSupport::OS::ProcessId ft_manager_id = OsSupport::OS::create_process("cdmw_ft_manager", managerArgs);
        OsSupport::OS::sleep(timescale*15000);
        TEST_SUCCEED();

        TEST_INFO("Get the fault manager reference");
        std::string fault_manager_corbaloc("corbaloc::localhost:4555/fault_manager");
        CORBA::Object_var fm_obj
        = m_orb->string_to_object(fault_manager_corbaloc.c_str());
        
        if ( CORBA::is_nil( fm_obj.in() ) )
            CDMW_THROW2(Exception,
                        Exception::FATAL,
                        "Invalid corbaloc" );
        
        std::string daemonArgs(" --CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml"); 
        TEST_INFO("Starting the PlatformDaemon");
        OsSupport::OS::ProcessId  daemon_id = OsSupport::OS::create_process("cdmw_platform_daemon.sh", daemonArgs);   
        OsSupport::OS::sleep(timescale*8000);
        TEST_SUCCEED();
        
        std::string groupCreatorArgs(" --CdmwXMLFile=CdmwFaultToleranceGroupCreatorInit_conf.xml --conf=CdmwFTGroupCreator2_conf.xml --validateXML"); 
        TEST_INFO("Starting the group creator");
        OsSupport::OS::create_process("cdmw_ft_group_creator", groupCreatorArgs);   
        OsSupport::OS::sleep(timescale*8000);
        TEST_SUCCEED();

        CORBA::Object_var obj = m_orb->string_to_object("corbaname::localhost:21869#ReplicationManager");

        CdmwFT::ReplicationManager_var replication_manager = CdmwFT::ReplicationManager::_narrow(obj.in());
        std::cout<<m_orb->object_to_string(replication_manager.in())<<std::endl;
        if (CORBA::is_nil(replication_manager.in()))
            TEST_FAILED();
        else
            TEST_SUCCEED();
        
        try
        {

            ::FT::Name group_name;
            group_name.length(1);
            group_name[0].id="GROUP1";
            
            ::FT::ObjectGroup_var object = replication_manager->get_object_group_ref_from_name(group_name);
            if (CORBA::is_nil(object.in()))
                TEST_FAILED();
            else
                TEST_SUCCEED();     
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        try
        {
            ::FT::Name group_name;
            group_name.length(1);
            group_name[0].id="GROUP2";
            
            ::FT::ObjectGroup_var object = replication_manager->get_object_group_ref_from_name(group_name);
            if (CORBA::is_nil(object.in()))
                TEST_FAILED();
            else
                TEST_SUCCEED();     
        }
        catch(...)
        {
            TEST_FAILED();
        }
        
        

        TEST_INFO("Kill the processes");
        
        OsSupport::OS::kill_process(daemon_id);
        OsSupport::OS::kill_process(ft_manager_id);
        TEST_SUCCEED();


        TEST_INFO("Start the fault manager with a bad groupconf file");
        std::string managerArgs2(" --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml --groupConf=CdmwFTGroupCreator3_conf.xml  --validateXML");
        OsSupport::OS::create_process("cdmw_ft_manager", managerArgs2);
        OsSupport::OS::sleep(timescale*5000);
        TEST_SUCCEED();

    }
    catch(const CORBA::SystemException& ex)
    {
        std::cout<<"<<<TEST_FAILED>>> : "<<ex<<std::endl;            
    }    
    catch(...)
    {
        std::cout<<"<<<TEST_FAILED>>>"<<std::endl;            
    }
}

}; // namespace FT
}; // namespace Cdmw

