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




#include "testftinitsequencing/TestHello_impl.hpp"
#include "testftinitsequencing/TestFTInitSequencing.hpp"

namespace Cdmw {
namespace FT {

// constructor
TestFTInitSequencing::TestFTInitSequencing(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA,
    const std::string& name,
    CdmwFT::ReplicationManager_ptr rm,
    const std::string& host1,
    const std::string& host2,
    CdmwReplicationManager::time_collector_ptr obj)
    : Testable( name ),
      m_host1(host1),
      m_host2(host2)
      
    
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_rootPOA =  PortableServer::POA::_duplicate ( rootPOA );
    m_replicationManager = CdmwFT::ReplicationManager::_duplicate(rm);
    m_time_collector = CdmwReplicationManager::time_collector::_duplicate(obj);
}

//destructor
TestFTInitSequencing::~TestFTInitSequencing()
{
}




// do_tests
void TestFTInitSequencing::do_tests()
{
    set_nbOfRequestedTestOK (1);

    try
    {
        
        std::ostringstream proc_init11;
        proc_init11<< " --proc-initialise APPL1 P11 " << m_host1;
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        TEST_INFO("Create process");
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init11.str());  

        std::ostringstream proc_init21;
        proc_init21<< " --proc-initialise APPL2 P21 " << m_host2;
        OsSupport::OS::create_process( "platform_admin.sh" , proc_init21.str());   

        OsSupport::OS::sleep(timescale*3000);

        std::ostringstream proc_run11;
        proc_run11<< " --proc-run APPL1 P11 " << m_host1;
        TEST_INFO("Create process");
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run11.str());  
        OsSupport::OS::sleep(timescale*1000);
        std::ostringstream proc_run21;
        proc_run21<< " --proc-run APPL2 P21 " << m_host2;
        OsSupport::OS::create_process( "platform_admin.sh" , proc_run21.str());   


        OsSupport::OS::sleep(timescale*20000);
        
        
        CdmwReplicationManager::timestruct time_end_primary = m_time_collector->get_init_primary_data_stores_time();
        OsSupport::OS::Timeval time1;
        time1.seconds = time_end_primary.second;
        time1.microseconds = time_end_primary.microsecond;
        
        CdmwReplicationManager::timestruct time_start_backup = m_time_collector->get_get_record_iterator_time();
        OsSupport::OS::Timeval time2;
        time2.seconds = time_end_primary.second;
        time2.microseconds = time_start_backup.microsecond;
        
        std::cout<<"time after the init of the primary "<<time1.seconds<<":"<<time1.microseconds<<std::endl;
        std::cout<<"time before the init of the backup "<<time2.seconds<<":"<<time2.microseconds<<std::endl;

        if (time1 < time2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

        
        
        
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

