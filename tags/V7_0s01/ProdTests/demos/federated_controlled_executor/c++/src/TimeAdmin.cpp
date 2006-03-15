#include <iostream>
#include <string>
#include <stdio.h>

#include "Time/clockservice/CosClockService.stub.hpp"
#include <Time/clockservice/CdmwCosClock.hpp>

#include <Foundation/ossupport/OS.hpp>

static const int SET_TIME = 1;
static const int SET_RATE = 2;
static const int PAUSE = 3;
static const int RESUME = 4;
static const int EXIT = 5;

static const std::string CLOCK_NAME_OPTION = "--CdmwClockName";

int
get_option()
{
    std::cout << "\t[1] Set Time\n"
              << "\t[2] Set Rate\n"
              << "\t[3] Pause\n"
              << "\t[4] Resume\n"
              << "\t[5] Exit\n"
              << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;
    return choice;        
}

void print_help()
{
    std::cerr << "usage: TimeAdmin -ORBInitRef ClockService=IOR --CdmwClockName=<clock-name>" << std::endl;
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
    
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "TimeReader-ORB");
        if (CORBA::is_nil(orb.in())) {
            std::cerr << "TimeAdmin:>> Unable to resolve ORB" << std::endl;
            std::exit(-1);
        }
        CORBA::Object_var obj = orb->resolve_initial_references("ClockService");
        if (CORBA::is_nil(obj.in()))
        {
            std::cerr << "TimeAdmin:>> Unable to resolve ClockService" << std::endl;
            std::exit(-1);
        }

        std::string clock_name = Cdmw::OsSupport::OS::get_option_value(argc, argv, CLOCK_NAME_OPTION);
        CosClockService::ClockCatalog_var clock_catalog = CosClockService::ClockCatalog::_narrow(obj.in());
        if (CORBA::is_nil(clock_catalog.in()))
        {
            std::cerr << "TimeAdmin:>> Unable to resolve ClockCatalog" << std::endl;
            std::exit(-1);
        }
        CosClockService::ClockCatalog::ClockEntry* catalog_entry = clock_catalog->get_entry(clock_name.c_str());
        CosClockService::ControlledClock_var ctrl_clock = CosClockService::ControlledClock::_narrow(catalog_entry->subject.in());
        if (CORBA::is_nil(ctrl_clock.in()))
        {
            std::cerr << "TimeAdmin:>> Unable to resolve " << clock_name << std::endl;
            std::exit(-1);
        }

        int choice = get_option();
        while (choice != EXIT)
        {
            switch (choice)
            {
                case SET_TIME:
                               {
                                   TimeBase::TimeT the_time;
                                   std::cerr << "Enter New Time: ";
                                   std::cin >> the_time;
                                   try
                                   {
                                       ctrl_clock->set(the_time);
                                   }
                                   catch (const CORBA::Exception& e)
                                   {
                                       std::cerr << "TimeAdmin:>> CORBA exception caught: " << e << std::endl;
                                   }
                                   catch (...)
                                   {
                                       std::cerr << "TimeAdmin:>> Generic exception caught" << std::endl;
                                   }
                                   break;
                               }
                case SET_RATE:
                               {
                                   float rate;
                                   std::cerr << "Enter New Rate: ";
                                   std::cin >> rate;
                                   try
                                   {
                                       ctrl_clock->set_rate(rate);
                                   }
                                   catch (const CORBA::Exception& e)
                                   {
                                       std::cerr << "TimeAdmin:>> CORBA exception caught: " << e << std::endl;
                                   }
                                   catch (...)
                                   {
                                       std::cerr << "TimeAdmin:>> Generic exception caught" << std::endl;
                                   }
                                   break; 
                               }
                case PAUSE:
                               {
                                   try
                                   {
                                       ctrl_clock->pause();
                                   }
                                   catch (const CORBA::Exception& e)
                                   {
                                       std::cerr << "TimeAdmin:>> CORBA exception caught: " << e << std::endl;
                                   }
                                   catch (...)
                                   {
                                       std::cerr << "TimeAdmin:>> Generic exception caught" << std::endl;
                                   }
                                   break; 
                               }
                case RESUME:
                               {
                                   try
                                   {
                                       ctrl_clock->resume();
                                   }
                                   catch (const CORBA::Exception& e)
                                   {
                                       std::cerr << "TimeAdmin:>> CORBA exception caught: " << e << std::endl;
                                   }
                                   catch (...)
                                   {
                                       std::cerr << "TimeAdmin:>> Generic exception caught" << std::endl;
                                   }
                                   break;
                               }
                case EXIT:
                default:
                                   break;
            }
            ::system("clear");
            choice = get_option();
        }
        std::exit(0);
    }
    catch (std::exception& e)
    {
        std::cerr << "TimeAdmin:>> std exception caught: " << e.what() << std::endl;
        std::exit(-1);
    }
    catch (const CORBA::Exception& e)
    {
        std::cerr << "TimeAdmin:>> CORBA exception caught: " << e << std::endl;
        std::exit(-1);
    }
    catch (...)
    {
        std::cerr << "TimeAdmin:>> Generic exception caught" << std::endl;
        std::exit(-1);
    }
}
