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
 * @author Fabrizio Morciano <html>fmorciano@amsjv.it<\html>.
 * 
 * @brief   Demo to show how to use a ControlledClock.
 *
 */
/* 
  To start use:
  -ORBInitRef ClockService=corbaloc::localhost:4883/ClockService -ORBInitRef LocalClock=corbaloc::localhost:4883/LocalClock 
 */
 
// Cardamom Foundation include.
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Thread.hpp>

// Cardamom Enhanced View of Time include.
#include <Time/clockservice/CdmwTime.skel.hpp>
#include <Time/clockservice/Util.hpp>
#include <Time/clockservice/CdmwCosClock.hpp>

// C++ Standard include.
#include <iostream>

using namespace std;
using namespace Cdmw::clock;

///////////////////////////////////////////////////////////////////////////////

namespace 
{
    class ORBWorker : public Cdmw::OsSupport::Thread
    {
    public:
        ORBWorker(CORBA::ORB_ptr orb);
        virtual ~ORBWorker() throw ();
        virtual void run()   throw ();
    private:
        CORBA::ORB_var m_ORB;
    };
    
    ORBWorker::ORBWorker(CORBA::ORB_ptr orb)
        : m_ORB(orb)
    {
    }
    
    ORBWorker::~ORBWorker() throw ()
    {
    }

    void ORBWorker::run() throw ()
    {
        try
        {
            try
            {
                this->m_ORB->run();
            }
            catch (const CORBA::SystemException& e)
            {
                std::cout << "ClockDemo:>> CORBA::SystemException caught while running ORB: " << e << std::endl;
            }
            catch (...)
            {
                std::cout << "ClockDemo:>> unexpected exception caught while running ORB" << std::endl;
            }
        }
        catch (...)
        {
        }
    }

    void release_resources(bool           ORB_initialized,
                           bool           CosClockService_initialized,
                           bool           ORBWorker_thread_created,
                           CORBA::ORB_ptr orb,
                           ORBWorker*     orb_worker) throw ()
    {
        try
        {
            try
            {
                // close CosClockService.
                if (CosClockService_initialized)
                    Cdmw::clock::CosClock::close();

                // shutdown ORB.
                if (ORB_initialized)
                {
                    orb->shutdown(1);
                    orb->destroy();
                }

                // synchronize with the ORBWorker thread.
                if (ORBWorker_thread_created)
                {
                    try
                    {
                        orb_worker->join();
                    }
                    catch (const Cdmw::InternalErrorException& e)
                    {
                       // also in the case this exception is raised, the
                       // synchronization with the possibly started and/or
                       // possibly ended thread is achieved.
                    }
                }

                // destroy ORBWorker object
                delete orb_worker;
            }
            catch (...)
            {
                std::cerr << "ClockDemo:>> generic exception while releasing resources" << std::endl;
            }
        }
        catch (...)
        {
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    TimeBase::TimeT 
    abs_delta(const TimeBase::TimeT num1,
              const TimeBase::TimeT num2)
    {
        if (num1 > num2)
            return num1 -num2;
        return num2-num1;
    }

    ///////////////////////////////////////////////////////////////////////////////

    int
      sign_delta(const TimeBase::TimeT num1,
                 const TimeBase::TimeT num2)
    {
      if (num1 > num2)
        return 1;
      if( num1 == num2 )
        return 0;
      return -1;
    }

    void
    run_clock(CosClockService::ControlledClock_ptr cc,
              int times,
              float rate)
    {
        // change the rate
        std::cout << "ControlledClockDemo:>> Rate = " << rate << std::endl;
        cc->set_rate(rate);

        TimeBase::TimeT 
            currentTime = cc->current_time();
        TimeBase::TimeT 
            lastTime = currentTime;
      
        for (int i = 0; i < times; ++i) 
        {
            std::cout << "[" << i << "] - Current Time: ";
            std::cout << currentTime;
            Cdmw::OsSupport::OS::sleep(1000);
            currentTime = cc->current_time();
            std::cout<< " | Delta (TimeT): ";
            int retrieve_sign = sign_delta(currentTime, lastTime);
            if(retrieve_sign!=0)
              std::cout<< (retrieve_sign  > 0 ? '+' : '-'); 
            TimeBase::TimeT delta = abs_delta(currentTime, lastTime);
            std::cout << delta << std::endl;
            lastTime = currentTime;
        }
    }

} // end of anonymous namespace

///////////////////////////////////////////////////////////////////////////////

int 
main(int argc, 
     char* argv[])
{
    bool ORB_initialized             = false;
    bool CosClockService_initialized = false;
    bool ORBWorker_thread_created    = false;

    //CORBA::ORB_var orb;
    CORBA::ORB_var orb;
    ORBWorker*     orb_worker = 0;

    try 
    {
        // init the ORB
#ifndef  NOT_LOCALISATION
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();
        strategyList.add_multicast();
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc,
                                                     argv,
                                                     strategyList);
#else
        orb = CORBA::ORB_init (argc, 
                               argv, 
                               "" );
#endif
        if (CORBA::is_nil(orb.in()))
        {
            std::cerr << "ControlledClockDemo:>> could not initialize ORB!" << std::endl;

            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);

            // end program.
            return -1;
        }
        ORB_initialized = true;

        // initializes the CosClockServices.
        if (Cdmw::clock::CosClock::init (orb.in (), argc, argv) == false)
        {
            std::cerr << "ControlledClockDemo:>> could not initialize clock service!" << std::endl;

            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);

            // end program.
            return -1;
        }
        CosClockService_initialized = true;

        // create an ORBWorker object.
        orb_worker = new ORBWorker(orb.in());
        if (orb_worker == 0)
        {
            std::cerr << "ControlledClockDemo:>> could not create ORBWorker object!" << std::endl;
                                                                                                                             
            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);
                                                                                                                             
            // end program
            return -1;
        }
                                                                                                                             
        // start the ORBWorker thread.
        try
        {
            orb_worker->start();
            ORBWorker_thread_created = true;
        }
        catch (const Cdmw::OutOfMemoryException&)
        {
        }
        catch (const Cdmw::OsSupport::ThreadSchedulingException&)
        {
        }
        catch (const Cdmw::InternalErrorException&)
        {
        }
        if (ORBWorker_thread_created == false)
        {
            std::cerr << "ControlledClockDemo:>> could not create ORBWorkwer thread" << std::endl;
                                                                                                                             
            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);
                                                                                                                             
            // end program.
            return -1;
        }
                                                                                                                             
        // wait for the ORBWorker thread to run the ORB.
        Cdmw::OsSupport::OS::sleep(1000);
        
        // retrieve a catalog
        CORBA::Object_var obj = 
            orb->resolve_initial_references("ClockService");

        // recover a catalogs
        CosClockService::ClockCatalog_var 
          clockCatalog = CosClockService::ClockCatalog::_narrow(obj.in());
        
        if (CORBA::is_nil(clockCatalog.in())) 
        {
            std::cerr << "ControlledClockDemo:>> Unable to resolve ClockCatalog" << std::endl;

            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);

            return -1;
        }

        // search an entry for a ControlledClock
        CosClockService::ClockCatalog::ClockEntry_var 
          clockEntry = clockCatalog->get_entry("ControlledClock");
        
        // cast ControlledClock
        CosClockService::ControlledClock_var 
          controlledClock = CosClockService::ControlledClock::_narrow(clockEntry->subject.in());
        
        if (CORBA::is_nil(controlledClock.in())) 
        {
            std::cerr << "ControlledClockDemo:>> Unable to resolve ControlledClock" << std::endl;

            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);

            return -1;
        }
      
        // show clock usage
        run_clock(controlledClock.in(), 
                  4, 
                  1.0 );
        int i;

        std::cout << "\nControlledClockDemo:>> Stop..." << std::endl;
        run_clock(controlledClock.in(), 
                  4, 
                  0.0 );

        std::cout << "\nControlledClockDemo:>> Back to the future..." << std::endl;
        for(i=1;i<3;++i)
          run_clock(controlledClock.in(), 
                    4, 
                    (float)i);

        // reset clock
        controlledClock->set_rate(1.0);
               
        Cdmw::clock::ControlledClockEx_var
          controlledClock_ex = Cdmw::clock::ControlledClockEx::_narrow(clockEntry->subject.in());

        float rate = controlledClock_ex->get_rate();
        std::cout<<"ControlledClockEx -> Rate is:"<<rate<<std::endl;

        release_resources(ORB_initialized,
                          CosClockService_initialized,
                          ORBWorker_thread_created,
                          orb.in(), orb_worker);
    } 
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "ControlledClockDemo:>> CORBA::SystemException: " << e << std::endl;
                                                                                                                             
        release_resources(ORB_initialized,
                          CosClockService_initialized,
                          ORBWorker_thread_created,
                          orb.in(), orb_worker);
                                                                                                                             
        // end program
        return 1;
    }
    catch (...)
    {
        std::cerr << "ControlledClockDemo:>> generic exception caught" << std::endl;
                                                                                                                             
        release_resources(ORB_initialized,
                          CosClockService_initialized,
                          ORBWorker_thread_created,
                          orb.in(), orb_worker);
                                                                                                                             
        // end program
        return 1;
    }

    // end program
    return 0;
}

