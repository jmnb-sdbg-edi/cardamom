/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/*
 * Authors: 
 *  Angelo Corsaro      <acorsaro@selex-si.com>
 *  Raffaele Mele       <rmele@progesi.it>
 *  Fabrizio Morciano   <fmorciano@selex-si.com>
 */

#ifdef  WIN32
#pragma warning( disable : 4786 )
#endif

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <Time/clockservice/CdmwCosClock.hpp>
#include <Time/clockservice/CosClockService.stub.hpp>
#include <Time/clockservice/Macro.hpp>

#include <clockservice/ControlledClock_impl.hpp>

#ifndef NOT_FEDERATION
#  include <clockservice/FederatedControlledClock.hpp>
#  include <clockservice/FederatedControlledClockSvc.hpp>
#endif

#include "clockservice/ClockCatalog_impl.hpp"
#include "clockservice/Clock_impl.hpp"
#include "clockservice/UTCTimeService_impl.hpp"
#include "clockservice/UTC_impl.hpp"
#include "clockservice/Executor_impl.hpp"
#include "clockservice/ControlledExecutor_impl.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"
#include "clockservice/ControllerCancelAllPolicy.hpp"
#include "clockservice/ControllerEnforceIntervalPolicy.hpp" 
#include "clockservice/ControllerEnforceDeadlinePolicy.hpp" 

#include <Foundation/common/Assert.hpp>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Guard_T.h>
#include <Foundation/osthreads/Mutex.hpp>

#ifndef NOT_LOCALISATION
#   include <Foundation/common/Options.hpp>
#   include <Foundation/common/String.hpp>
#   include <Foundation/orbsupport/OrbSupport.hpp>
#   include <Foundation/ossupport/OS.hpp>
#endif

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
    std::string VIRTUAL_CLOCK_NUM_OPTION      ("--CdmwVirtualClocks");
    std::string MESSAGE_RESEND_OPTION         ("--CdmwMessageResend");
    std::string CLOCK_CATALOG_IOR_OPTION      ("--CdmwClockCatalogIORFile=<filename>");
    std::string LOCAL_CLOCK_IOR_OPTION        ("--CdmwLocalClockIORFile=<filename>");
    std::string FEDERATED_CTRL_CLOCK_OPTION   ("--CdmwFederatedCtrlClock");
    
    std::string clock_catalog_file_name       ("ClockCatalog.ior");
    std::string policycontrollerupdateadmin_file_name
                                              ("PolicyControllerUpdateAdmin.ior");
    std::string local_clock_file_name         ("LocalClock.ior"  );
    std::string CLOCK_CATALOG_NAME            ("ClockService"    );
    std::string LOCAL_CLOCK_NAME              ("LocalClock"      );
    std::string POLICY_CONTROLLER_UPDATE_ADMIN              
                                              ("PolicyControllerUpdateAdmin");

    std::string CLOCK_SERVICE_PORT            ("4883"            );
    
    std::string TOPIC_UPDATE_MANAGER_CORBALOC
    ("corbaloc:miop:1.0@1.0-TopicUpdateManager-1/225.1.1.225:15555");

    int    federation_id          = 0;
    int    send_repetitions       = 2;
    bool   federation_off         = false;
    size_t reactor_threads        = 10;
    size_t timer_wheel_spokes     = 8192;
    size_t timer_wheel_resolution = 1000; // 1ms
    int max_virtual_clock=10;
        
    //--------------------------------------------------------------------------
        
    template <class T>
    bool  write_obj_to_ior(CORBA::ORB_ptr&     orb,
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


    void print_help(char* appname) 
    {
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
                  << "\t [--CdmwFederatedCtrlClock=on|off (default is on)\n"
                  << "\t [--help]\n"
                  << std::endl;        
    }
    
    //-----------------------------------------------------------------------------
    void parse_args( int argc, char* argv[] )
    {
#ifndef WIN32
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

#ifndef NOT_FEDERATION
        // -- FEDERATED_CTRL_CLOCK_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  FEDERATED_CTRL_CLOCK_OPTION);
        if (arg_value == "off") 
            federation_off = true;
#endif
#ifndef NOT_LOCALISATION
        // -- LOCALISATION_PORT_OPTION
        arg_value =
            Cdmw::OsSupport::OS::get_option_value(argc,
                                                  argv,
                                                  Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION);
        if (arg_value != "no") 
            CLOCK_SERVICE_PORT = arg_value;
#endif
#endif
    }
        
    ///////////////////////////////////////////////////////////////////////////

    class Reactor_Worker : public ACE_Task_Base
    {
        
        ACE_Reactor *m_reactor;
    public:
        
        Reactor_Worker(ACE_Reactor *ref)
            :m_reactor(ref)
        {
        }
        
        Reactor_Worker::~Reactor_Worker()
        {
        }
        
        int
        Reactor_Worker::svc (void)
        {
            int result = m_reactor->run_reactor_event_loop();
            
            if (result == -1)
            {
                std::cerr << "ClockService:>> Error handling events" << std::endl;
                return 1;
            }
            return 0;
        }
    };
   
} // end of unnamed namespace

///////////////////////////////////////////////////////////////////////////////

struct  CosClock_impl
{
    PortableServer::POA_var m_poa;
    CORBA::ORB_ptr          m_orb;

    std::auto_ptr<Reactor_Worker> m_reactor_worker;
    std::auto_ptr<ACE_Reactor>    m_reactor;
    std::auto_ptr<ACE_TP_Reactor> m_tp_reactor;

    CosClock_impl()
    {
    }

    ~CosClock_impl()
    {
    }
};

///////////////////////////////////////////////////////////////////////////////

static  
std::auto_ptr<CosClock_impl> s_PIMPL(0);

///////////////////////////////////////////////////////////////////////////////

bool 
CosClock::init(CORBA::ORB_ptr orb,
               int argc, 
               char *argv[])
{
    try
    {        
        if(s_PIMPL.get())
            return true;
            
        s_PIMPL = std::auto_ptr<CosClock_impl>( new CosClock_impl ) ;

        // just a ref 
        CosClock_impl &pimpl_ref = *(s_PIMPL.get());

        parse_args( argc, argv ); 
        // reference to orb        
        pimpl_ref.m_orb = orb;
        
        // reference to POA
        CORBA::Object_var obj =
            pimpl_ref.m_orb->resolve_initial_references("RootPOA" );            
        pimpl_ref.m_poa = PortableServer::POA::_narrow (obj.in () );    
        
        if (CORBA::is_nil (pimpl_ref.m_poa.in ()))
        {
            std::cerr<<"ClockService:>> Panic -- nil RootPOA"<<std::endl;
            exit(-1);
        }
        
        PortableServer::POAManager_var mgr =
            pimpl_ref.m_poa->the_POAManager ();
        
        mgr->activate ();    
        
        ///////////////////////////////////////////////////////////////////////////////
        // -- Create Thread Pool Reactor 
        
        pimpl_ref.m_tp_reactor = std::auto_ptr<ACE_TP_Reactor>(new ACE_TP_Reactor);
        
        pimpl_ref.m_reactor = 
            std::auto_ptr<ACE_Reactor>(new  ACE_Reactor(pimpl_ref.m_tp_reactor.get()));
                
        ACE_Timer_Wheel* m_timer_queue = new ACE_Timer_Wheel(timer_wheel_spokes, timer_wheel_resolution);

        pimpl_ref.m_reactor->timer_queue(m_timer_queue);

        // replace reactor
        //ACE_Reactor::instance (pimpl_ref.m_reactor); 
        
        int retval;
        pimpl_ref.m_reactor_worker = 
            std::auto_ptr<Reactor_Worker>(new Reactor_Worker(pimpl_ref.m_reactor.get()));
        retval =
            pimpl_ref.m_reactor_worker->activate(THR_NEW_LWP | THR_JOINABLE, reactor_threads);
        
        if (retval != 0)
        {
            std::cerr << "ClockService:>> Cannot activate the Thread Pool\n" << std::endl;
            return false;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Registration for ControllerUpdateAdmin
        
        // registration
        Cdmw::clock::policy::ControllerUpdateAdmin_impl* ref2controllerupdatepolicyadmin 
          = Cdmw::clock::policy::ControllerUpdateAdmin_impl::instance();
        Cdmw::clock::policy::ControllerUpdateAdmin_var admin_var 
          = ref2controllerupdatepolicyadmin->_this();
        // now memory is under POA control          

        Cdmw::clock::ControllerCancelAllPolicy* 
           cancelAllPolicy = new Cdmw::clock::ControllerCancelAllPolicy();
        Cdmw::clock::policy::ControllerUpdate_ptr 
           ca_policy = cancelAllPolicy->_this();            
        admin_var->_cxx_register(Cdmw::clock::policy::CANCEL_ALL,
                                 ca_policy);
        
        Cdmw::clock::ControllerEnforceIntervalPolicy* 
               enforceIntervalPolicy = new Cdmw::clock::ControllerEnforceIntervalPolicy();
        Cdmw::clock::policy::ControllerUpdate_ptr 
                ei_policy = enforceIntervalPolicy->_this();            
        admin_var->_cxx_register(Cdmw::clock::policy::ENFORCE_INTERVAL,
                                     ei_policy);
        
        Cdmw::clock::ControllerEnforceDeadlinePolicy* 
               enforceDeadlinePolicy = new Cdmw::clock::ControllerEnforceDeadlinePolicy();
        Cdmw::clock::policy::ControllerUpdate_ptr 
                ed_policy = enforceDeadlinePolicy->_this();            
        admin_var->_cxx_register(Cdmw::clock::policy::ENFORCE_DEADLINE,
                                     ed_policy);
                
        ///////////////////////////////////////////////////////////////////////
        // write ControllerUpdateAdmin ior
#ifndef NOT_LOCALISATION
        try
        {
            OrbSupport::OrbSupport::bind_object_to_corbaloc(
                pimpl_ref.m_orb, 
                POLICY_CONTROLLER_UPDATE_ADMIN, 
                admin_var.in() );
            pimpl_ref.m_orb->register_initial_reference(
            POLICY_CONTROLLER_UPDATE_ADMIN.c_str(), 
            admin_var.in());
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            std::cerr << "ORB Exception : Already bound >> " << e << std::endl;
            exit(-1);
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << "ORB Exception:>> " << e << std::endl;
            exit(-1);
        }        
#else
        pimpl_ref.m_orb->register_initial_reference(POLICY_CONTROLLER_UPDATE_ADMIN.c_str(), 
                                                    admin_var.in());
        // write clockcatalog ior
        write_obj_to_ior(pimpl_ref.m_orb,
                         admin_var,
                         policycontrollerupdateadmin_file_name);
#endif           
        ///////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////
        // Registration for ClockCatalog
        
        // ClockCatalog registration
        ClockCatalog_impl* ref2clockcatalog = new ClockCatalog_impl;
        ClockCatalog_var clockcatalog_var = ref2clockcatalog->_this();
        // now memory is under POA control
     
        // UTC Registration
        UtcTimeService_impl* ref2utctimeservice = new UtcTimeService_impl;
        UtcTimeService_var utctimeservice_var = ref2utctimeservice->_this();
        
        // register a valid clock
        ClockCatalog::ClockEntry utcEntry;
        
        utcEntry.subject =  UtcTimeService::_duplicate(utctimeservice_var.in());
        utcEntry.name    =  CORBA::string_dup("UTCTimeService");
        
        ref2clockcatalog->_cxx_register(utcEntry);
        
        ////////////////////////////////////////////////////////////////////////
        // -- Add Controlled Clocks to Clock Catalog -- 
        
        string controlledclock("ControlledClock");
        for(int i=0;
        i<max_virtual_clock;
        ++i)
        {            
            //pimpl_ref.m_controlled_clocks.push_back(new ControlledClock_impl);
            ControlledClock_impl< ::Cdmw::OsSupport::Mutex >* ref2controlledclock =
                new ControlledClock_impl< ::Cdmw::OsSupport::Mutex >;
            ControlledClockEx_var controlledClock = ref2controlledclock->_this();           
            
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
            
            ref2clockcatalog->_cxx_register(controlledClockEntry);
        }
        
#ifndef NOT_FEDERATION
        ////////////////////////////////////////////////////////////////////////
        // -- Add Federated Controlled Clocks to Clock Catalog --
        ControlledClockEx_var  federatedControlledClock;
        ControlledExecutor_var federatedControlledExecutor;
        
        if (!federation_off) 
        {
            string federatedCtrlClockName("FederatedControlledClock");
            string federatedCtrlExecutorName("FederatedControlledExecutor"); 
            try 
            {
                FederatedControlledClockSvc::init(pimpl_ref.m_orb,
                                                  pimpl_ref.m_poa.in(),
                                                  TOPIC_UPDATE_MANAGER_CORBALOC,
                                                  send_repetitions);
                
                FederatedControlledClock_t* ref2federatedControlledClock =
                    new FederatedControlledClock_t(federation_id);

                FederatedControlledExecutor_t* ref2federatedControlledExecutor =
                    new FederatedControlledExecutor_t(federation_id + 1, 
                                                      pimpl_ref.m_reactor.get());

                federatedControlledClock =
                    ref2federatedControlledClock->_this();
                federatedControlledExecutor =
                    ref2federatedControlledExecutor->_this();
                
                ClockCatalog::ClockEntry federatedControlledClockEntry;
                ClockCatalog::ClockEntry federatedControlledExecutorEntry;
                
                federatedControlledClockEntry.subject =
                    ControlledClockEx::_duplicate(federatedControlledClock.in());
                
                federatedControlledExecutorEntry.subject =
                    ControlledExecutor::_duplicate(federatedControlledExecutor.in());
                
                federatedControlledClockEntry.name =
                    CORBA::string_dup(federatedCtrlClockName.c_str());
                
                federatedControlledExecutorEntry.name =
                    CORBA::string_dup(federatedCtrlExecutorName.c_str());
                
                ref2clockcatalog->_cxx_register(federatedControlledClockEntry);
                ref2clockcatalog->_cxx_register(federatedControlledExecutorEntry);                
            } 
            catch (const CORBA::SystemException& e) 
            {
                std::cerr << ">> WARNING: Unable to init Controlled Clock Federation!"
                    << std::endl;
                std::cerr << ">> WARNING: Raised Exception -- " << e << std::endl;
            }            
        }
#endif
        ///////////////////////////////////////////////////////////////////////
        // -- Add "ClockService" Executor Clocks to Clock Catalog -- 

        string  executorName("LocalExecutor");
        Executor_impl* ref2executor = new Executor_impl(pimpl_ref.m_reactor.get());

        PeriodicExecution::Executor_var 
            executorClock = ref2executor->_this();
       
        ClockCatalog::ClockEntry executorClockEntry;        
        executorClockEntry.subject = 
            PeriodicExecution::Executor::_duplicate(executorClock.in());
        executorClockEntry.name    = CORBA::string_dup(executorName.c_str());
        
        ref2clockcatalog->_cxx_register(executorClockEntry);
        
        ///////////////////////////////////////////////////////////////////////
        // -- Add ControlledExecutor Clocks to Clock Catalog -- 
        
        string  controlledExecutorName("ControlledExecutor");
        
        ControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>* ref2controlledexecutor =
            new ControlledExecutor_impl<ACE_Recursive_Thread_Mutex, ACE_Guard>(pimpl_ref.m_reactor.get());
        ControlledExecutor_var 
            controlledExecutorClock = ref2controlledexecutor->_this();
        
        ClockCatalog::ClockEntry controlledExecutorClockEntry;      
        controlledExecutorClockEntry.subject = 
            ControlledExecutor::_duplicate(controlledExecutorClock.in());
        controlledExecutorClockEntry.name    = 
            CORBA::string_dup(controlledExecutorName.c_str());
        
        ref2clockcatalog->_cxx_register(controlledExecutorClockEntry);
        
        ///////////////////////////////////////////////////////////////////////
        // write clockcatalog ior
#ifndef NOT_LOCALISATION
        try
        {
            OrbSupport::OrbSupport::bind_object_to_corbaloc(
                pimpl_ref.m_orb, CLOCK_CATALOG_NAME, clockcatalog_var.in() );
            pimpl_ref.m_orb->register_initial_reference(CLOCK_CATALOG_NAME.c_str(), 
                                                        clockcatalog_var.in());
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            std::cerr << "ORB Exception : Already bound >> " << e << std::endl;
            exit(-1);
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << "ORB Exception:>> " << e << std::endl;
            exit(-1);
        }        
#else
        pimpl_ref.m_orb->register_initial_reference(CLOCK_CATALOG_NAME.c_str(), 
                                                    clockcatalog_var.in());
        // write clockcatalog ior
        write_obj_to_ior(pimpl_ref.m_orb,
                         clockcatalog_var,
                         clock_catalog_file_name);
#endif
        
        ///////////////////////////////////////////////////////////////////////
        // -- Local Clock --
        Clock_impl *ref2clock = new Clock_impl;
        CosClockService::Clock_var clock_var = ref2clock->_this();
        
#ifndef NOT_LOCALISATION
        try
        {
            OrbSupport::OrbSupport::bind_object_to_corbaloc(
                pimpl_ref.m_orb, LOCAL_CLOCK_NAME, clock_var.in() );

        pimpl_ref.m_orb->register_initial_reference(LOCAL_CLOCK_NAME.c_str(),
            clock_var.in());
            
        }
        catch (const CORBA::BAD_PARAM& e)
        {
            std::cerr << "ORB Exception : Already bound >> " << e << std::endl;
            exit(-1);
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << "ORB Exception:>> " << e << std::endl;
            exit(-1);
        }
#else
        pimpl_ref.m_orb->register_initial_reference(LOCAL_CLOCK_NAME.c_str(), 
            clock_var.in());
        // write localclock ior
        write_obj_to_ior( pimpl_ref.m_orb,
            clock_var,
            local_clock_file_name );
#endif
                    
        ///////////////////////////////////////////////////////////////////////
        // Create PropertySetDef object and register.

        TAO_PropertySetDef* m_propset_servant = new TAO_PropertySetDef();
        CosPropertyService::PropertySetDef_var propset_var = m_propset_servant->_this();
        
        // Create PropertySet factory and then register.
        TAO_PropertySetFactory* m_propset_factory_servant = new TAO_PropertySetFactory();
        CosPropertyService::PropertySetFactory_var propset_factory = m_propset_factory_servant->_this();
        
        ///////////////////////////////////////////////////////////////////////
        // TAO registration for valuetype differs from standard
        TAO_OBV_REGISTER_FACTORY (UTC_impl_init, UTC );
        
        if (retval != 0)
        {
            std::cerr << "ClockService:>> Cannot activate ORB Thread Pool\n" << std::endl;
            return false;
        }
    }
    catch(const CORBA::SystemException& e)
    {
        std::cerr << "ClockService:>> " << e << std::endl;
        exit(-1);
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////////////////

void
CosClock::close()
{
    if(!s_PIMPL.get())
        return;
    
    // just a ref 
    CosClock_impl &pimpl_ref = *(s_PIMPL.get());
    
    pimpl_ref.m_reactor->end_reactor_event_loop();  
    pimpl_ref.m_reactor_worker->wait();

    // release resources
    pimpl_ref.m_tp_reactor     = std::auto_ptr<ACE_TP_Reactor>(0);
    pimpl_ref.m_reactor        = std::auto_ptr<ACE_Reactor>(0);
    pimpl_ref.m_reactor_worker = std::auto_ptr<Reactor_Worker>(0);
}

///////////////////////////////////////////////////////////////////////////////

PortableServer::POA_ptr 
CosClock::get_POA()
{
    if(s_PIMPL.get())
    return s_PIMPL->m_poa.in();

    return 0;
}
        


///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
