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

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <testcrashtimedetection/TestCrashProcessMeasure.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include <fstream>
#include <sstream>
namespace Cdmw 
{



// constructor
TestCrashProcessMeasure::TestCrashProcessMeasure(CORBA::ORB_ptr orb,
                                                 PortableServer::POA_ptr poa,
                                                 const std::string& name,
                                                 const std::string& nbobserver,
                                                 const std::string& nbtest,
                                                 const std::string& supervision,
                                                 const std::string& limittime,
                                                 const std::string& percentcase)
    : Testable(name),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_nbobserver(nbobserver),
      m_nbtest(nbtest),
      m_supervision(supervision),
      m_limittime(limittime),
      m_percentcase(percentcase)
{

}

// destructor
TestCrashProcessMeasure::~TestCrashProcessMeasure()
{
}

// do_tests
void TestCrashProcessMeasure::do_tests()
{

    using namespace Cdmw;

    set_nbOfRequestedTestOK (1);
    
    CdmwNamingAndRepository::Repository_var repository
    = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
    
        
    Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                          repository.in());
        
    Cdmw::CommonSvcs::Naming::NamingInterface ni =
    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");

    int nb_observer = ::atoi(m_nbobserver.c_str());

    add_nbOfRequestedTestOK (2*nb_observer);

    for (int i = 0; i< nb_observer; i++)
    {
        std::ostringstream  full_name;
        full_name<< "dom3/P2"<<i+1;
        std::cout<<"get observer from "<< full_name.str()<<std::endl;
        
        CORBA::Object_var obj;
        
        try
        {
            obj = ni.resolve (full_name.str());
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
        Cdmw::Test::observer_var observer = Cdmw::Test::observer::_narrow(obj.in());
        TEST_CHECK( ! CORBA::is_nil(observer.in()) );

        m_observer_list[i] = Cdmw::Test::observer::_duplicate(observer.in());
    }
        

     int timescale = Cdmw::TestUtils::Testable::get_timescale();
     int nb_test = std::atoi(m_nbtest.c_str());


     TEST_INFO("start "<<nb_test<<" test(s)");
     for (int incr = 0; incr != nb_test; incr++)
     {   
         TEST_INFO("Start test n°"<< incr);
         TEST_INFO("kill the process")
         //kill the process
         std::string file("/tmp/CdmwDaemon_21869/APPL1/killed.pid");
         std::ifstream ifs(file.c_str());
         if (ifs.good())
         {
             std::string pid_daemon;
             ifs >> pid_daemon;
             ifs.close();
             Cdmw::OsSupport::OS::kill_process(::atoi(pid_daemon.c_str()));
         }
         Cdmw::OsSupport::OS::sleep(timescale*2000);
         bool first = true;
         // find the time in all the observer
         for (int j = 0; j < nb_observer; j++)
         {
             CdmwPlatformMngtBase::TimeStamp time1 = m_observer_list[j]->get_time_detection();
             CdmwPlatformMngtBase::TimeStamp time2 = m_observer_list[j]->get_time_notification();
             
             std::cout<<"local_time_before :"<<time1.minute<<"."<<time1.second<<"."<<time1.millisecond<<"."<<time1.microsecond<<std::endl;
             std::cout<<"local_time_after  :"<<time2.minute<<"."<<time2.second<<"."<<time2.millisecond<<"."<<time2.microsecond<<std::endl;
             
             double diff = (double)(((time2.minute * 60000000) + (time2.second * 1000000) + (time2.millisecond * 1000) + time2.microsecond) -
                                    ((time1.minute * 60000000) + (time1.second * 1000000) + (time1.millisecond * 1000) + time1.microsecond))/1000;
            if (first)
            {
                m_time[incr] = diff;
            }
            else
            {
                if (m_time[incr] < diff)
                    m_time[incr] = diff;
            }
        }
        std::cout<<"max time "<<m_time[incr]<<"ms"<<std::endl;

        TEST_INFO("restart the process");
        
        std::ostringstream proc_initkilled;
        proc_initkilled<< " --proc-initialise APPL1 killed " << Cdmw::OsSupport::OS::get_hostname();

        Cdmw::OsSupport::OS::create_process( "platform_admin.sh" , proc_initkilled.str());   
        Cdmw::OsSupport::OS::sleep(timescale*3000);
        
        std::ostringstream proc_runkilled;
        proc_runkilled<< " --proc-run APPL1 killed " << Cdmw::OsSupport::OS::get_hostname();

        Cdmw::OsSupport::OS::create_process( "platform_admin.sh" , proc_runkilled.str());
        Cdmw::OsSupport::OS::sleep(timescale*1000);
     }
        

    TEST_INFO("average");
    int size = m_time.size();
    double result = 0;
    int nb_inf_limit = 0;
    
    int limit = std::atoi(m_limittime.c_str());
    int percent = std::atoi(m_percentcase.c_str());
    
    for (int incr = 0; incr < size; incr++)
    {
        result = result + m_time[incr];
        if (m_time[incr]<limit)
            nb_inf_limit++;
    }
    result = result / size;
    
    std::cout<<"average :"<<result<<"ms"<<std::endl;
    std::cout<<"with "<<size<<" test(s), "<<nb_inf_limit<<" test(s) are under the limit ("<<limit<<"ms)"<<std::endl;
    float resultpercent = (nb_inf_limit * 100) / size;
    
    std::cout<<"so in "<<resultpercent<<"%, the detection is under the limit"<<std::endl; 
    
    if (resultpercent> percent)
        TEST_SUCCEED();
    else
        TEST_FAILED();
    
    std::ofstream ofs("PTest_Q-068.txt");
    ofs <<"average :"<<result<<"ms"<<std::endl;
    ofs <<"with "<<size<<" test(s), "<<nb_inf_limit<<" test(s) are under the limit ("<<limit<<"ms)"<<std::endl;
    ofs <<"so in "<<resultpercent<<"%, the detection is under the limit"<<std::endl; 
    ofs.close();


    
}


} // end namespace Cdmw

