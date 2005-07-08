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
 * @brief   Demo to show how to use a Clock.
 *
 */

/* 
To start use:
-ORBInitRef ClockService=corbaloc::localhost:4883/ClockService -ORBInitRef LocalClock=corbaloc::localhost:4883/LocalClock 
*/

// Cardamom Foundation include.
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/osthreads/Thread.hpp>

// Cardamom Enhanced View of Time include.
//#include <Time/clockservice/CdmwCosClock.hpp>
#include <Time/clockservice/CosClockService.stub.hpp>
// C++ Standard include.
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

namespace 
{
  class ORBWorker 
    :   public Cdmw::OsSupport::Thread
  {
  public:
    ORBWorker (CORBA::ORB_ptr orb)
      :  m_ORB (orb)
    {
    }

    virtual ~ORBWorker()  throw()
    {
    }

    virtual void
      run() throw()
    {
      try
      {
        this->m_ORB->run ();
      }
      catch (const CORBA::SystemException& e)
      {
        std::cout << "ORBWorker Exception:>> " << e << std::endl;
      }
    }

  private:
    CORBA::ORB_var m_ORB;
  };

} // end of anonymous namespace

///////////////////////////////////////////////////////////////////////////////

int 
main(int argc, 
     char* argv[])
{
  try 
  {
    CORBA::ORB_var  orb;

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
      argv);
#endif


    // ORB in a thread
    ORBWorker orb_worker(orb.in());
    orb_worker.start();        

    // retrieve a clock
    CORBA::Object_var obj = 
      orb->resolve_initial_references("LocalClock");

    CosClockService::Clock_var 
      clock = CosClockService::Clock::_narrow(obj.in());        

    if (CORBA::is_nil(clock.in())) 
    {
      std::cerr << "ClockDemo:>> Unable to resolve Clock"
        << std::endl;
      return -1;
    }

    std::cout<<"ClockDemo >>: current_time is: "<<clock->current_time()<<std::endl;
    orb->shutdown(1);
    orb->destroy();
  } 
  catch (const CORBA::SystemException& e) 
  {
    std::cerr << "Exception:"<< e << std::endl;
    return 1;
  }
  return 0;
}
