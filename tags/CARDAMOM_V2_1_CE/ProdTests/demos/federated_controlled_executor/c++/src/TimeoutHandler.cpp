#include <iostream>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <Time/clockservice/CdmwCosClock.hpp>
#include <Time/clockservice/CdmwTime.stub.hpp>
#include <ace/Task.h>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "TimeoutHandler.hpp"

static const size_t PERIOD = 50000000; // 500 mec;
static const std::string CLOCK_NAME_OPTION = "--CdmwClockName";
static const std::string HELP_OPTION = "--help";
static const std::string EXEC_LIMIT_OPTION = "--ExecLimit";
static const std::string PERIOD_OPTION = "--period";

TimeoutHandler::TimeoutHandler(CosClockService::ControlledClock_ptr clock)
    : m_count(0),
      m_clock(CosClockService::ControlledClock::_duplicate(clock))
{
    // No-Op
}

TimeoutHandler::~TimeoutHandler()
{
    // No-Op
}

CORBA::Boolean
TimeoutHandler::do_work(const CORBA::Any&)
    throw (CORBA::SystemException)
{
    std::cerr << "[Reading " << m_count++ << "]:>> " << m_clock->current_time() << std::endl;
    return true;
}

namespace
{    
    class ORBWorker : public ACE_Task_Base
    {
        public:
            ORBWorker(CORBA::ORB_ptr orb);
            virtual int svc (void);
        private:
            CORBA::ORB_var m_ORB;
    };
    
    ORBWorker::ORBWorker(CORBA::ORB_ptr orb)
        : m_ORB(orb)
    {
    }
    
    int
    ORBWorker::svc(void)
    {
        try
        {       
            this->m_ORB->run();
        }
        catch (const CORBA::Exception& e)
        {
            std::cerr << "ORBWorker:>> CORBA exception caught: " << e << std::endl;
        }    
        catch (...)
        {
            std::cerr << "ORBWorker:>> Generic exception caught" << std::endl;
        }    
        return 0;
    }
}

void print_help() 
{
    std::cerr << "usage: TimeoutHandler <start_at_time> <options>" << std::endl;
    std::cerr << "where: <start_at_time> is the absolute time at which controller starts" << std::endl;
    std::cerr << "       <options>       indicates the following list of options:" << std::endl;
    std::cerr << "                       --CdmwFederatedCtrlClock=on|off" << std::endl;
    std::cerr << "                       --CdmwLocalisationService=<port>" << std::endl;
    std::cerr << "                       --CdmwClockName=FederatedControlledClock|FederatedControlledExecutor" << std::endl;
    std::cerr << "                       --TimerPolicy=ENFORCE_INTERVAL|ENFORCE_DEADLINE|CANCEL_ALL [default]" << std::endl;
    std::cerr << "                       --CdmwMessageResend=<send_repetitions> [default is 2]" << std::endl;
    std::cerr << "                       --ExecLimit=<number_of_executions> [default is zero]" << std::endl;
    std::cerr << "                       --period=<the_period> [default is 50000000 (5sec)]" << std::endl;
}

int
main(int argc, char* argv[])
{
    try
    {
        std::string help_option = Cdmw::OsSupport::OS::get_option_value(argc, argv, HELP_OPTION);
        if (help_option =="yes")
        {
            print_help();
            std::exit(-1);
        }

        TimeBase::TimeT start_time = 0;
        std::istringstream iss(argv[1]);
        iss >> start_time; 

        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        strategyList.add_multicast();
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

        CORBA::Object_var CORBA_obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var        rootPOA     = PortableServer::POA::_narrow(CORBA_obj.in());
        PortableServer::POAManager_var poa_manager = rootPOA->the_POAManager();
        poa_manager->activate();

        ORBWorker orb_worker(orb.in());
        int retval = orb_worker.activate(THR_NEW_LWP | THR_JOINABLE, 5);
        if (retval < 0)
        {
            std::cerr << "TimeoutHandler:>> Error on starting orb thread!" << std::endl;
            std::exit(-1);
        }
        Cdmw::OsSupport::OS::sleep(1, 0);

        if (!Cdmw::clock::CosClock::init(orb.in(), argc, argv))
        {
            std::cerr << "TimeoutHandler:>> CosClock Initialization Failed" << std::endl;
            std::exit(-1);
        }
        std::string clock_name = Cdmw::OsSupport::OS::get_option_value(argc, argv, CLOCK_NAME_OPTION);
        if (clock_name == "no")
        {
            std::cerr << "TimeoutHandler:>> The Clock Name must be defined" << std::endl;
            print_help();
            std::exit(-1);
        }
        if (clock_name != "FederatedControlledClock" && clock_name != "FederatedControlledExecutor")
        {
            std::cerr << "TimeoutHandler:>> Invalid clock type specified -- " << clock_name << std::endl;
            print_help();
            std::exit(-1);
        }
        if (clock_name == "FederatedControlledClock")
        {
            orb_worker.wait();
        }
        if (clock_name == "FederatedControlledExecutor")
        {
            CORBA::Object_var obj = orb->resolve_initial_references("ClockService");
            if (CORBA::is_nil(obj.in()))
            {
                std::cerr << "TimeoutHandler:>> Unable to resolve ClockService" << std::endl;
                std::exit(-1);
            }
            CosClockService::ClockCatalog_var clock_catalog = CosClockService::ClockCatalog::_narrow(obj.in());
            CosClockService::ClockCatalog::ClockEntry* catalog_entry = clock_catalog->get_entry(clock_name.c_str());
            Cdmw::clock::ControlledExecutor_var fed_ctrl_executor = Cdmw::clock::ControlledExecutor::_narrow(catalog_entry->subject.in());
            if (CORBA::is_nil(fed_ctrl_executor.in()))
            {
                std::cerr << "TimeoutHandler:>> Unable to resolve FederatedControlledExecutor" << std::endl;
                std::exit(-1);
            }
            TimeoutHandler handler(fed_ctrl_executor.in());
            CosClockService::PeriodicExecution::Periodic_var periodic = handler._this();
            CosClockService::PeriodicExecution::Controller_var controller; 

            std::string timer_policy = Cdmw::OsSupport::OS::get_option_value(argc, argv, "TimerPolicy");
            if (timer_policy == "ENFORCE_INTERVAL") 
                controller = fed_ctrl_executor->enable_periodic_execution_with_policy(periodic.in(), Cdmw::clock::policy::ENFORCE_INTERVAL);
            else if (timer_policy == "ENFORCE_DEADLINE")
                controller = fed_ctrl_executor->enable_periodic_execution_with_policy(periodic.in(), Cdmw::clock::policy::ENFORCE_DEADLINE);
            else 
                controller = fed_ctrl_executor->enable_periodic_execution(periodic.in());
    
            long exec_limit = 0;
            std::string exec_value = Cdmw::OsSupport::OS::get_option_value(argc, argv, EXEC_LIMIT_OPTION);
            if (exec_value != "no") 
                exec_limit = atol(exec_value.c_str());
            long the_period = PERIOD;
            std::string period_value = Cdmw::OsSupport::OS::get_option_value(argc, argv, PERIOD_OPTION);
            if (period_value != "no") 
                the_period = atol(period_value.c_str());
            CORBA::Any param;
            controller->start_at(the_period, start_time, exec_limit, param);
            orb_worker.wait();
        }
        std::exit(0);
    }
    catch (std::exception& e)
    {
        std::cerr << "TimeoutHandler:>> std exception caught: " << e.what() << std::endl;
        std::exit(-1);
    }
    catch (CORBA::Exception& e)
    {
        std::cerr << "TimeoutHandler:>> CORBA exception caught: " << e << std::endl;
        std::exit(-1);
    }
    catch (...)
    {
        std::cerr << "TimeoutHandler:>> Generic exception caught" << std::endl;
        std::exit(-1);
    }
}
