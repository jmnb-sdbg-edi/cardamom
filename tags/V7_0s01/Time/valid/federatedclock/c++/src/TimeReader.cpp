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

#include <iostream>
#include <unistd.h>
#include "Time/clockservice/CdmwCosClock.hpp"
#include "Time/clockservice/CosClockService.stub.hpp"

#include "Time/clockservice/Util.hpp"

#include <Foundation/osthreads/Thread.hpp>

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

int
main(int argc, char* argv[])
{
    
    CORBA::ORB_var orb ;

        //CORBA::ORB_init(argc, argv, "TimeReader-ORB");
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
                               "TimeReader-ORB" );
#endif



        // init Cardamom Clock Service.
        if(!Cdmw::clock::CosClock::init(orb.in(),
                                         argc,
                                         argv))
        {
            std::cerr<<"Error on init clock service!"<<std::endl;
            return -1;
        }

        // ORB in a thread
        ORBWorker orb_worker(orb.in());
        orb_worker.start();

    Cdmw::OsSupport::OS::sleep(2,0);
    CORBA::Object_var obj =
        orb->resolve_initial_references("ClockService");

    if (CORBA::is_nil(obj.in())) {
        std::cerr << "Unable to resolve ClockService, Aborting!"
                  << std::endl;
        return -1;
    }
    
    CosClockService::ClockCatalog_var clock_catalog =
        CosClockService::ClockCatalog::_narrow(obj.in());
    
    CosClockService::ClockCatalog::ClockEntry* catalog_entry =
        clock_catalog->get_entry("FederatedControlledClock");
    
    CosClockService::ControlledClock_var fed_ctrl_clock
        = CosClockService::ControlledClock::_narrow(catalog_entry->subject.in());
    
    if (CORBA::is_nil(fed_ctrl_clock.in())) {
        std::cerr << "Unable to resolve FederatedControlledClock!"
                  << std::endl;
    }

    catalog_entry =
        clock_catalog->get_entry("ControlledExecutor");

    CosClockService::ControlledClock_var ctrl_clock
        = CosClockService::ControlledClock::_narrow(catalog_entry->subject.in());
    
    if (CORBA::is_nil(ctrl_clock.in())) {
        std::cerr << "Unable to resolve FederatedControlledClock, Aborting!"
                  << std::endl;
    }

    
    while (true) {
        std::cout << ">> Current Time: "
                  << fed_ctrl_clock->current_time()
                  << std::endl;
    //    sleep(0,100000000); // -- sleep for 100ms
        Cdmw::OsSupport::OS::sleep(2,0);
    }
    
    return 0;
}
