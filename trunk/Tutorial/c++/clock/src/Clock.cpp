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
 * @brief   Demo to show how to use a Clock.
 */

// Cardamom Foundation include.
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Thread.hpp>

// Cardamom Enhanced View of Time include.
#include <Time/clockservice/CdmwCosClock.hpp>
#include <Time/clockservice/CosClockService.stub.hpp>
#include <Time/clockservice/Time.hpp>
#include <Time/clockservice/Util.hpp>

// C++ Standard include.
#include <iostream>

using namespace std;

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
} // end of anonymous namespace

///////////////////////////////////////////////////////////////////////////////

int
main (int argc, char *argv[])
{
    bool ORB_initialized             = false;
    bool CosClockService_initialized = false;
    bool ORBWorker_thread_created    = false;

    //CORBA::ORB_var orb;
    CORBA::ORB_var orb;
    ORBWorker*     orb_worker = 0;

    try
    {
        // initialize ORB.
#ifndef  NOT_LOCALISATION
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded ();
        strategyList.add_PoaThreadPerConnection ();
        strategyList.add_multicast ();
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init (argc, argv, strategyList);
#else
        orb = CORBA::ORB_init (argc, argv);
#endif
        if (CORBA::is_nil(orb.in()))
        {
            std::cerr << "ClockDemo:>> could not initialize ORB!" << std::endl;

            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);
                              //orb, orb_worker);

            // end program.
            return -1;
        }
        ORB_initialized = true;

        // initializes the CosClockServices.
        if (Cdmw::clock::CosClock::init (orb.in (), argc, argv) == false)
        {
            std::cerr << "ClockDemo:>> could not initialize clock service!" << std::endl;

            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);
                              //orb, orb_worker);

            // end program.
            return -1;
        }
        CosClockService_initialized = true;

        // create an ORBWorker object.
        orb_worker = new ORBWorker(orb.in());
        if (orb_worker == 0)
        {
            std::cerr << "ClockDemo:>> could not create ORBWorker object!" << std::endl;

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
            std::cerr << "ClockDemo:>> could not create ORBWorkwer thread" << std::endl;

            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);

            // end program.
            return -1;
        }

        // wait for the ORBWorker thread to run the ORB.
        Cdmw::OsSupport::OS::sleep(1000);

        // retrieve a clock.
        CORBA::Object_ptr obj = orb->resolve_initial_references ("LocalClock");

        CosClockService::Clock_ptr clock = CosClockService::Clock::_narrow (obj);

        if (CORBA::is_nil (clock))
        {
            std::cerr << "ClockDemo:>> could not resolve Clock" << std::endl;

            release_resources(ORB_initialized,
                              CosClockService_initialized,
                              ORBWorker_thread_created,
                              orb.in(), orb_worker);

            // end program.
            return -1;
        }

        cdmw_tm         cdmwtm;
        char*           cdmwdate = 0;
        TimeBase::TimeT curr_time = clock->current_time();
        if (Cdmw::clock::util::TimeT2cdmw_tm(curr_time, cdmwtm) == 0)
            cdmwdate = Cdmw::clock::util::asctime(cdmwtm); 
        std::cout << "ClockDemo >>: current time is: " << curr_time << std::endl;
        if (cdmwdate) 
           std::cout << "ClockDemo >>: current date (GMT 0 not daylight time) is: " << cdmwdate;

        release_resources(ORB_initialized,
                          CosClockService_initialized,
                          ORBWorker_thread_created,
                          orb.in(), orb_worker);
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << "ClockDemo:>> CORBA::SystemException: " << e << std::endl;

        release_resources(ORB_initialized,
                          CosClockService_initialized,
                          ORBWorker_thread_created,
                          orb.in(), orb_worker);

        // end program
        return 1;
    }
    catch (...)
    {
        std::cerr << "ClockDemo:>> generic exception caught" << std::endl;

        release_resources(ORB_initialized,
                          CosClockService_initialized,
                          ORBWorker_thread_created,
                          orb.in(), orb_worker);

        // end program
        return 1;
    }

    // end program.
    return 0;
}
