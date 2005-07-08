/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

/**
 * @file ClockService.cpp
 *
 * @author Angelo Corsaro <acorsaro@amsjv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 * @author Fabrizio Morciano    <fmorciano@amsjv.it>
 */


#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>
#include <string>

#include <Time/clockservice/ControlledClock_impl.hpp>
#include <Time/clockservice/FederatedControlledClock_impl.hpp>
#include <Time/clockservice/FederatedControlledClockSvc.hpp>
#include <Time/clockservice/ClockCatalog_impl.hpp>
#include <Time/clockservice/Clock_impl.hpp>
#include <Time/clockservice/UTCTimeService_impl.hpp>
#include <Time/clockservice/UTC_impl.hpp>
#include <Time/clockservice/CosClockService.stub.hpp>
#include <Time/clockservice/Executor_impl.hpp>
#include <Time/clockservice/ControlledExecutor_impl.hpp>
#include <Time/clockservice/Macro.hpp>

#include <Foundation/ossupport/OS.hpp>

#ifndef NOT_LOCALISATION
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#endif

#include <Foundation/common/Assert.hpp>

#include "ace/Get_Opt.h"
#include "ace/Thread.h"
#include "ace/Thread_Manager.h"
#include "ace/TP_Reactor.h"
#include "ace/Select_Reactor.h"
#include "ace/Get_Opt.h"
#include "ace/Task.h"
#include "ace/Timer_Queue.h"
#include "ace/Timer_Heap.h"
#include "ace/Timer_Wheel.h"
#include "ace/Timer_Hash.h"
#include "ace/Timer_List.h"

#include "tao/ORB_Core.h"
#include "tao/PortableServer/ORB_Manager.h"

using namespace std;
using namespace CosClockService;
using namespace Cdmw::clock;
using namespace Cdmw;

namespace 
{
    std::string HELP_ME_OPTION                ("--help");
    std::string TIMER_WHEEL_SPOKES_OPTION     ("--CdmwTimerWheelSpokes");
    std::string TIMER_WHEEL_RESOLUTION_OPTION ("--CdmwTimerWheelResolution");
    std::string REACTOR_THREADS_OPTION        ("--CdmwReactorThreads");
    std::string ORB_THREADS_OPTION            ("--CdmwOrbThreads");
    std::string VIRTUAL_CLOCK_NUM_OPTION      ("--CdmwVirtualClocks");
    std::string MESSAGE_RESEND_OPTION         ("--CdmwMessageResend");
    std::string CLOCK_CATALOG_IOR_OPTION      ("--CdmwClockCatalogIORFile=<filename>");
    std::string LOCAL_CLOCK_IOR_OPTION        ("--CdmwLocalClockIORFile=<filename>");
    std::string FEDERATED_CTRL_CLOCK_OPTION   ("--CdmwFederatedCtrlClock");
    
    std::string clock_catalog_file_name       ("ClockCatalog.ior");
    std::string local_clock_file_name         ("LocalClock.ior"  );
    std::string CLOCK_CATALOG_NAME            ("ClockService"    );
    std::string LOCAL_CLOCK_NAME              ("LocalClock"      );
    std::string CLOCK_SERVICE_PORT            ("4883"            );
    
    std::string TOPIC_UPDATE_MANAGER_CORBALOC
    ("corbaloc:miop:1.0@1.0-TopicUpdateManager-1/225.1.1.225:15555");

    int    federation_id          = 0;
    int    send_repetitions       = 2;
    bool   federation_on          = false;
    
    size_t orb_threads            = 10;
    size_t reactor_threads        = 10;
    size_t timer_wheel_spokes     = 8192;
    size_t timer_wheel_resolution = 1000; // 1ms
    int max_virtual_clock=10;
        
    //--------------------------------------------------------------------------
        
    template <class T>
    bool  write_obj_to_ior(CORBA::ORB_var&     orb,
                           T&                  ob, 
                           const std::string&  file_name )
    {
        std::ofstream  out_file( file_name.c_str() );
        if( !out_file )
            return true;
        
        CORBA::String_var 
            str = orb->object_to_string( ob.in() );
        out_file<<str.in();
        out_file.close();
        
        return false;
    }
        
    //-----------------------------------------------------------------------------

    void print_help(char* appname) {
        std::cerr << "USAGE:  \n   "
                  << appname << "\n"
                  << "\t [--CdmwReactorThreads=<threads num> (default 10)]\n"
                  << "\t [--CdmwOrbThreads==<threads num> (default 10)]\n"
                  << "\t [--CdmwTimerWheelSpokes==<spokes> (default 8192)]\n"
                  << "\t [--CdmwTimerWheelResolution=<resolution in usec> (default 1ms)]\n"
                  << "\t [--CdmwVirtualClocks=<number of virtual clocks> (default=10)]\n"
                  << "\t [--CdmwMessageResend=<number of resend> (default 2)]\n"
                  << "\t [--CdmwClockCatalogIORFile=<filename> (default ClockCatalog.ior)]\n"
                  << "\t [--CdmwLocalClockIORFile=<filename> (default LocalClock.ior)]\n"
                  << "\t [--help]\n"
                  << std::endl;
        
    }
    
    //-----------------------------------------------------------------------------
    void parse_args( int argc, char* argv[] )
    {
        // -- HELP_ME_OPTION
        std::string arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  HELP_ME_OPTION); 
        if (arg_value == "yes") {
            print_help(argv[0]);
            exit(-1);
        }

        // -- TIMER_WHEEL_SPOKES_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  TIMER_WHEEL_SPOKES_OPTION);
        if (arg_value != "no") 
            timer_wheel_spokes =
                atoi(arg_value.c_str());
        
        // -- TIMER_WHEEL_RESOLUTION_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                       argv,
                                                       TIMER_WHEEL_RESOLUTION_OPTION);
        if (arg_value != "no") 
            timer_wheel_resolution =
                atoi(arg_value.c_str());

        // -- REACTOR_THREADS_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                       argv,
                                                       REACTOR_THREADS_OPTION);
        if (arg_value != "no") 
            reactor_threads =
                atoi(arg_value.c_str());

        // -- ORB_THREADS_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                       argv,
                                                       ORB_THREADS_OPTION);
        if (arg_value != "no") 
            orb_threads =
                atoi(arg_value.c_str());

        // -- VIRTUAL_CLOCK_NUM_OPTION 
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                       argv, 
                                                       VIRTUAL_CLOCK_NUM_OPTION);
        if (arg_value != "no") 
            max_virtual_clock =
                atoi(arg_value.c_str());

        // -- MESSAGE_RESEND_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                       argv,
                                                       MESSAGE_RESEND_OPTION);
        if (arg_value != "no") 
            send_repetitions =
                atoi(arg_value.c_str());
        

        // -- CLOCK_CATALOG_IOR_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                       argv,
                                                       MESSAGE_RESEND_OPTION);
        if (arg_value != "no") 
            clock_catalog_file_name = arg_value;
        
        // -- LOCAL_CLOCK_IOR_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                       argv,
                                                       MESSAGE_RESEND_OPTION);
        if (arg_value != "no") 
            local_clock_file_name = arg_value;

        // -- FEDERATED_CTRL_CLOCK_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  FEDERATED_CTRL_CLOCK_OPTION);
        if (arg_value == "yes") 
            federation_on = true;

        // -- LOCALISATION_PORT_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION);
        if (arg_value != "no") 
            CLOCK_SERVICE_PORT = arg_value;


    }
        
    ///////////////////////////////////////////////////////////////////////////

    class ORB_Worker : public ACE_Task_Base
    {
    public:
        ORB_Worker (CORBA::ORB_ptr orb);
        virtual ~ORB_Worker();
            
    public:
        virtual int svc (void);
        
    private:
        CORBA::ORB_var orb_;
    };
        
    ORB_Worker::ORB_Worker (CORBA::ORB_ptr orb)
        :  orb_ (CORBA::ORB::_duplicate (orb))
    {
    }
        
    ORB_Worker::~ORB_Worker() { }
        
    int
    ORB_Worker::svc (void)
    {
        try
        {
            this->orb_->run (ACE_ENV_SINGLE_ARG_PARAMETER);
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << "ORB_Worker:>> " << e << std::endl;
        }
        return 0;
    }
        
    ///////////////////////////////////////////////////////////////////////////
        
    class Reactor_Worker : public ACE_Task_Base
    {
    public:
        virtual ~Reactor_Worker();
            
    public:
        virtual int svc (void);
    };
        
    Reactor_Worker::~Reactor_Worker()
    {
        // No-Op
    }
        
    int
    Reactor_Worker::svc (void)
    {
        // std::cout <<".";
        int result =
            ACE_Reactor::instance ()->run_reactor_event_loop();
            
        if (result == -1)
        {
            std::cerr << "ClockService:>> Error handling events" << std::endl;
            return 1;
        }
            
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////
        
    /**
     * Purpose:
     * <p> Create an C++ string from array of C string (i.e. argv[] vector of
     * command-line parameter)
     * 
     *@param argv the array of C string to be transformed
     *
     *@param argc the number of element in argv[] vector
     *
     *@return a string containing all arguments and single space before EOL.
     *
     *@exception
     */
        
    std::string
    argv_to_string (int argc, char* argv[])
    {
        int n;
        std::string str("");
        
        for (n=0; n < argc; ++n) {
            str += argv[n];
            str += " ";
        }
        return str;
    }       
    
} // end of unnamed namespace

///////////////////////////////////////////////////////////////////////////////
        
int main (int argc, char* argv[])
{
    try
    {
        parse_args( argc, argv ); 

#ifndef  NOT_LOCALISATION
        std::cerr << "** ClockService ** "
                  << "- Localisation Service port:"
                  << CLOCK_SERVICE_PORT
                  << std::endl;
        // set localisation_port
        char** ORB_init_argv = 0;
        int ORB_init_argc(0);
        std::string arguments( Common::Options::LOCALISATION_SERVICE_OPTION );
        arguments += "=" + CLOCK_SERVICE_PORT;
        
        std::string strArgv = argv_to_string(argc,argv) + arguments;
        ORB_init_argv = Common::String::to_char_array (Common::String::to_strings(strArgv));
        ORB_init_argc = argc + 1;

        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        strategyList.add_multicast();
        
        CORBA::ORB_var 
            orb = Cdmw::OrbSupport::OrbSupport::ORB_init(ORB_init_argc, 
                                                         ORB_init_argv, 
                                                         strategyList);
#else
        CORBA::ORB_var 
            orb = CORBA::ORB_init (argc, 
                                   argv, 
                                   "" );
#endif
        // reference to POA
        CORBA::Object_var obj =
            orb->resolve_initial_references("RootPOA" );    
        
        PortableServer::POA_var poa =
            PortableServer::POA::_narrow (obj.in () );    
        
        if (CORBA::is_nil (poa.in ()))
        {
            std::cerr<<"ClockService:>> Panic -- nil RootPOA"<<std::endl;
            return 1;
        }
        
        PortableServer::POAManager_var mgr =
            poa->the_POAManager ();
        
        mgr->activate ();    

        ///////////////////////////////////////////////////////////////////////////////
        // -- Create Thread Pool Reactor and configure it to be the
        // -- default reactor.
        
        ACE_TP_Reactor tp_reactor;
        ACE_Reactor reactor (&tp_reactor);
        ACE_Timer_Wheel timer_queue(timer_wheel_spokes,
                                    timer_wheel_resolution);
        reactor.timer_queue(&timer_queue);
        ACE_Reactor::instance (&reactor); 

        int retval;
        Reactor_Worker reactor_worker;
        retval =
            reactor_worker.activate(THR_NEW_LWP | THR_JOINABLE, reactor_threads);
        if (retval != 0)
            std::cerr << "ClockService:>> Cannot activate the Thread Pool\n" << std::endl;
        
        ORB_Worker orb_worker (orb.in ());
        
        retval =
            orb_worker.activate (THR_NEW_LWP | THR_JOINABLE, orb_threads);
       
        ///////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////
        // Registration for ClockCatalog
    
        // ClockCatalog registration
        ClockCatalog_impl  clockcatalog_impl;
        ClockCatalog_var clockcatalog_var = clockcatalog_impl._this();

        // UTC Registration
        UtcTimeService_impl utc_service_impl;
        UtcTimeService_var utctimeservice_var = utc_service_impl._this();
    
        // register a valid clock
        ClockCatalog::ClockEntry utcEntry;
    
        utcEntry.subject =  UtcTimeService::_duplicate(utctimeservice_var.in());
        utcEntry.name    =  CORBA::string_dup("UTCTimeService");
    
        clockcatalog_impl._cxx_register(utcEntry);

        ////////////////////////////////////////////////////////////////////////
        // -- Add Controlled Clocks to Clock Catalog -- 
        
        string controlledclock("ControlledClock");
        for(int i=0;
            i<max_virtual_clock;
            ++i)
        {            
            ControlledClock_impl *controlledClockServant = new ControlledClock_impl;
            ControlledClockEx_var controlledClock = controlledClockServant->_this();           
            
            string name(controlledclock);
            ostringstream oss;
            
            if(i>0)
            {
                oss<<i;
                name+=oss.str();
            }                          
            
            if(i==0)
            {
                CDMW_ASSERT( name == controlledclock);
            }
            else
            {
                CDMW_ASSERT( name != controlledclock);               
            }
            
            ClockCatalog::ClockEntry controlledClockEntry;
            controlledClockEntry.subject = ControlledClockEx::_duplicate(controlledClock.in());            
            controlledClockEntry.name =CORBA::string_dup(name.c_str());
        
            clockcatalog_impl._cxx_register(controlledClockEntry);
        }

        ////////////////////////////////////////////////////////////////////////
        // -- Add Federated Controlled Clocks to Clock Catalog --
        ControlledClockEx_var federatedControlledClock;
        if (federation_on) {
            string federatedCtrlClockName("FederatedControlledClock"); 
            try {
                FederatedControlledClockSvc::init(orb.in(),
                                                  poa.in(),
                                                  TOPIC_UPDATE_MANAGER_CORBALOC,
                                                  send_repetitions);

                FederatedControlledClock_impl* federatedCtrlclockServant =
                    new FederatedControlledClock_impl(federation_id);
                
                federatedControlledClock =
                    federatedCtrlclockServant->_this();
                
                ClockCatalog::ClockEntry federatedControlledClockEntry;
                
                federatedControlledClockEntry.subject =
                    ControlledClockEx::_duplicate(federatedControlledClock.in());
                
                federatedControlledClockEntry.name =
                    CORBA::string_dup(federatedCtrlClockName.c_str());
                
                clockcatalog_impl._cxx_register(federatedControlledClockEntry);
                std::cout << "Running Clock Service with Federated Controlled Clock"
                          << std::endl;
                
            } catch (const CORBA::SystemException& e) {
                std::cerr << ">> WARNING: Unable to init Controlled Clock Federation!"
                          << std::endl; 
            }
            
        }
        ///////////////////////////////////////////////////////////////////////
        // -- Add "ClockService" Executor Clocks to Clock Catalog -- 

        string  executorName("LocalExecutor");
        Executor_impl executorClockServant(&reactor);
        PeriodicExecution::Executor_var executorClock = 
            executorClockServant._this();
            
        ClockCatalog::ClockEntry executorClockEntry;        
        executorClockEntry.subject = 
            PeriodicExecution::Executor::_duplicate(executorClock.in());
        executorClockEntry.name    = CORBA::string_dup(executorName.c_str());

        clockcatalog_impl._cxx_register(executorClockEntry);


        ///////////////////////////////////////////////////////////////////////
        // -- Add ControlledExecutor Clocks to Clock Catalog -- 

        string  controlledExecutorName("ControlledExecutor");
        ControlledExecutor_impl controlledExecutorClockServant(&reactor);
        ControlledExecutor_var 
            controlledExecutorClock = controlledExecutorClockServant._this();
            
        ClockCatalog::ClockEntry controlledExecutorClockEntry;      
        controlledExecutorClockEntry.subject = 
            ControlledExecutor::_duplicate(controlledExecutorClock.in());
        controlledExecutorClockEntry.name    = 
            CORBA::string_dup(controlledExecutorName.c_str());

        clockcatalog_impl._cxx_register(controlledExecutorClockEntry);

        ///////////////////////////////////////////////////////////////////////
        // write clockcatalog ior
#ifndef NOT_LOCALISATION
        try
        {
            OrbSupport::OrbSupport::bind_object_to_corbaloc(
                orb.in(), CLOCK_CATALOG_NAME, clockcatalog_var.in() );
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            std::cout << "ORB Exception : Already bound >> " << e << std::endl;
            exit(-1);
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << "ORB Exception:>> " << e << std::endl;
            exit(-1);
        }        
        orb->register_initial_reference(CLOCK_CATALOG_NAME.c_str(),
                                        clockcatalog_var.in());


#else
        orb->register_initial_reference(CLOCK_CATALOG_NAME.c_str(), 
                                        clockcatalog_var.in());

        // write clockcatalog ior
        write_obj_to_ior( orb,
                          clockcatalog_var,
                          clock_catalog_file_name );
#endif

        ///////////////////////////////////////////////////////////////////////
        // -- Local Clock --
        Clock_impl clock_impl;
        //CosClockService::PeriodicExecution::Executor_var clock_var =
        CosClockService::Clock_var clock_var = clock_impl._this();

#ifndef NOT_LOCALISATION
        try
        {
            OrbSupport::OrbSupport::bind_object_to_corbaloc(
                orb.in(), LOCAL_CLOCK_NAME, clock_var.in() );
            
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            std::cout << "ORB Exception : Already bound >> " << e << std::endl;
            exit(-1);
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << "ORB Exception:>> " << e << std::endl;
            exit(-1);
        }
        orb->register_initial_reference(LOCAL_CLOCK_NAME.c_str(),
                                        clock_var.in());
#else
        orb->register_initial_reference(LOCAL_CLOCK_NAME.c_str(), 
                                        clock_var.in());
        // write localclock ior
        write_obj_to_ior( orb,
                          clock_var,
                          local_clock_file_name );
#endif
        
        ///////////////////////////////////////////////////////////////////////
        // Create PropertySetDef object and register.
        TAO_PropertySetDef propset_impl ;

        CosPropertyService::PropertySetDef_var 
            propset_var = propset_impl._this ();

        // Create PropertySet factory and then register.
        TAO_PropertySetFactory propset_factory_impl ;
    
        CosPropertyService::PropertySetFactory_var 
            propset_factory = propset_factory_impl._this ();

        ///////////////////////////////////////////////////////////////////////
        // TAO registratio for valuetype differs from standard
        TAO_OBV_REGISTER_FACTORY (UTC_impl_init, UTC );

        if (retval != 0)
            std::cerr << "ClockService:>> Cannot activate ORB Thread Pool\n" << std::endl;
        
        
        std::cout<<"ClockService ready!!\n";
        orb_worker.thr_mgr ()->wait ();
    }
    catch(const CORBA::SystemException& e)
    {
        std::cerr << "ClockService:>> " << e << std::endl;
        return 1;
    }
  
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
//  End Of File
///////////////////////////////////////////////////////////////////////////////
