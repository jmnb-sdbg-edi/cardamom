#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <Time/clockservice/CdmwCosClock.hpp>
#include <ace/Task.h>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>

static const std::string CLOCK_NAME_OPTION = "--CdmwClockName";

namespace
{    
    class ORBWorker : public ACE_Task_Base
    {
    public:
        ORBWorker (CORBA::ORB_ptr orb);
        virtual int svc (void);
    private:
        CORBA::ORB_var m_ORB;
    };
    
    ORBWorker::ORBWorker (CORBA::ORB_ptr orb)
        :  m_ORB (orb)
    {
    }
    
    int
    ORBWorker::svc (void)
    {
        try
        {       
          this->m_ORB->run ();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << "ORBWorker:>> " << e << std::endl;
        }    
        return 0;
    }
}

void print_help()
{
    std::cerr << "usage: TimeReader -ORBInitRef ClockService=IOR --CdmwClockName=<clock-name>" << std::endl;
    std::cerr << "where: <clock-name> can be FederatedControlledClock|FederatedControlledExecutor" << std::endl;
}
    
int
main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            print_help();
            std::exit(-1);
        }

        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();

        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        if (CORBA::is_nil(orb.in()))
        {
            std::cerr << "TimeReader:>> Unable to resolve ORB" << std::endl;
            std::exit(-1);
        }
        CORBA::Object_var obj = orb->resolve_initial_references("ClockService");
        if (CORBA::is_nil(obj.in()))
        {
            std::cerr << "TimeReader:>> Unable to resolve ClockService" << std::endl;
            std::exit(-1);
        }
        CosClockService::ClockCatalog_var clock_catalog = CosClockService::ClockCatalog::_narrow(obj.in());
        if (CORBA::is_nil(clock_catalog.in()))
        {
            std::cerr << "TimeReader:>> Unable to resolve ClockCatalog" << std::endl;
            std::exit(-1);
        }
        std::string clock_name = Cdmw::OsSupport::OS::get_option_value(argc, argv, CLOCK_NAME_OPTION);
        CosClockService::ClockCatalog::ClockEntry* catalog_entry = clock_catalog->get_entry(clock_name.c_str());
        CosClockService::ControlledClock_var ctrl_clock = CosClockService::ControlledClock::_narrow(catalog_entry->subject.in());
        if (CORBA::is_nil(ctrl_clock.in()))
        {
            std::cerr << "TimeReader:>> Unable to resolve " << clock_name << std::endl;
            std::exit(-1);
        }
    
        for (int m_count = 0; ; m_count++)
        {
            TimeBase::TimeT cur_time = ctrl_clock->current_time();
            std::cerr << "[Reading " << m_count << "]: Current Time: " << cur_time << std::endl;
            usleep(5000000); // -- sleep for 100ms
        }

        std::exit(0);
    }
    catch (std::exception& e)
    {
        std::cerr << "TimeReader:>> std exception caught: " << e.what() << std::endl;
        std::exit(-1);
    }
    catch (CORBA::Exception& e)
    {
        std::cerr << "TimeReader:>> CORBA exception caught: " << e << std::endl;
        std::exit(-1);
    }
    catch (...)
    {
        std::cerr << "TimeReader:>> Generic exception caught" << std::endl;
        std::exit(-1);
    }
}
