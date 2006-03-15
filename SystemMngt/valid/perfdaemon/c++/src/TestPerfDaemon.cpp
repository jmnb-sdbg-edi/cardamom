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

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <perfdaemon/TestPerfDaemon.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "perfdaemon/TestManage_impl.hpp"
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION( Cdmw::TestPerfDaemon) ;

namespace Cdmw 
{



// constructor
/*
TestPerfDaemon::TestPerfDaemon(CORBA::ORB_ptr orb,
                               PortableServer::POA_ptr poa,
                               const std::string& name,
                               const std::string& nbtest,
                               const std::string& supervision,
                               const std::string& limittime,
                               const std::string& percentcase)
    : Testable(name),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_nbtest(nbtest),
      m_supervision(supervision),
      m_limittime(limittime),
      m_percentcase(percentcase)
{

}
*/

TestPerfDaemon::TestPerfDaemon()
{
}

// destructor
TestPerfDaemon::~TestPerfDaemon()
{
}

// do_tests
void TestPerfDaemon::do_tests()
{

    using namespace Cdmw;

// //     set_nbOfRequestedTestOK (5);
    
    CdmwNamingAndRepository::Repository_var repository
    = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
    
        
    Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                          repository.in());
        
    Cdmw::CommonSvcs::Naming::NamingInterface ni =
    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");
    

    std::string full_name = "dom3/killer";


    CORBA::Object_var obj;
        
    try
    {
        obj = ni.resolve (full_name);
        CPPUNIT_ASSERT(true);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
        CPPUNIT_ASSERT(false);
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
        CPPUNIT_ASSERT(false);
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
        CPPUNIT_ASSERT(false);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
        CPPUNIT_ASSERT(false);
    }
    Cdmw::Test::killer_var killer = Cdmw::Test::killer::_narrow(obj.in());
    CPPUNIT_ASSERT( ! CORBA::is_nil(killer.in()) );


            
    Cdmw::Test::observer_impl* observer_obj = new Cdmw::Test::observer_impl(m_orb.in());
    Cdmw::Test::observer_var observer = observer_obj->_this();

    // Get the CORBA reference of the system of the CDMW Supervision
    CORBA::Object_var systemObj = m_orb->string_to_object(m_supervision.c_str());
        
    CdmwPlatformMngt::System_var system = CdmwPlatformMngt::System::_narrow(systemObj.in());
    if (CORBA::is_nil(system.in())) {
        CPPUNIT_ASSERT(false);
    }
    system->register_observer( "test_observer", observer.in() );
        

    TEST_INFO("initialise the killer");
    try
    {
        killer->init(observer.in());
        CPPUNIT_ASSERT(true);
    }
    catch(...)
    {
        CPPUNIT_ASSERT(false);
    }
        

    int timescale = Cdmw::TestUtils::get_timescale();
    int nb_test = std::atoi(m_nbtest.c_str());
    TEST_INFO("start "<<nb_test<<" test(s)");
    for (int incr = 0; incr != nb_test; incr++)
    {   
        TEST_INFO("Start test n°"<< incr);
        Cdmw::OsSupport::OS::sleep(timescale*1500);
        killer->kill_daemon();
//        while(observer_obj->host_is_available())
        {
            Cdmw::OsSupport::OS::sleep(timescale*2000);
        }
            
        Cdmw::OsSupport::OS::sleep(timescale*1000);
        killer->start_daemon();
//        while(!observer_obj->host_is_available())
        {
            Cdmw::OsSupport::OS::sleep(timescale*3000);
        }

            
        CdmwPlatformMngtBase::TimeStamp time1 = observer_obj->get_local_time_before();
        CdmwPlatformMngtBase::TimeStamp time2 = observer_obj->get_local_time_after();

        //std::cout<<"local_time_before   :"<<time1.seconds<<"."<<time1.microseconds<<std::endl;
        std::cout<<"local_time_before :"<<time1.second<<"."<<time1.millisecond<<"."<<time1.microsecond<<std::endl;
        std::cout<<"local_time_after    :"<<time2.second<<"."<<time2.millisecond<<"."<<time2.microsecond<<std::endl;

        m_remotetime[incr] = (double)(((time2.minute * 1000000000) + (time2.second * 1000000) + (time2.millisecond * 1000) + time2.microsecond) -
                                      ((time1.minute * 1000000000) + (time1.second * 1000000) + (time1.millisecond * 1000) + time1.microsecond))/1000;
        std::cout<<"remote time "<<m_remotetime[incr]<<"ms"<<std::endl;

        observer_obj->incrementation();
    }
        
    try
    {
        killer->auto_kill();
        CPPUNIT_ASSERT(false);
    }
    catch(const CORBA::COMM_FAILURE&)
    {
        CPPUNIT_ASSERT(true);
    }
        
    TEST_INFO("unregister_observer");
    system->unregister_observer( "test_observer" );
    TEST_INFO("END");
        

    TEST_INFO("average");
    int size = m_remotetime.size();
    double result = 0;
    int nb_inf_limit = 0;
    
    int limit = std::atoi(m_limittime.c_str());
    int percent = std::atoi(m_percentcase.c_str());
    
    for (int incr = 0; incr < size; incr++)
    {
        result = result + m_remotetime[incr];
        if (m_remotetime[incr]<limit)
            nb_inf_limit++;
    }
    result = result / size;
    
    std::cout<<"average :"<<result<<"ms"<<std::endl;
    std::cout<<"with "<<size<<" test(s), "<<nb_inf_limit<<" test(s) are under the limit ("<<limit<<"ms)"<<std::endl;
    float resultpercent = (nb_inf_limit * 100) / size;
    
    std::cout<<"so in "<<resultpercent<<"%, the detection is under the limit"<<std::endl; 
    
    if (resultpercent> percent)
        CPPUNIT_ASSERT(true);
    else
        CPPUNIT_ASSERT(false);
    
    std::ofstream ofs("PTest_V_SMG12.txt");
    ofs <<"average :"<<result<<"ms"<<std::endl;
    ofs <<"with "<<size<<" test(s), "<<nb_inf_limit<<" test(s) are under the limit ("<<limit<<"ms)"<<std::endl;
    ofs <<"so in "<<resultpercent<<"%, the detection is under the limit"<<std::endl; 
    ofs.close();


    
}


} // end namespace Cdmw

