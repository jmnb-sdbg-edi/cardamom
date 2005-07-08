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

#include <iostream>

#include "Time/ftclocksvcs/ClockServiceWorker.hpp"
#include <iostream>

Cdmw::clock::svcs::ORBWorker::ORBWorker (CORBA::ORB_ptr orb)
    :  orb_ (CORBA::ORB::_duplicate (orb))
{
}

Cdmw::clock::svcs::ORBWorker::~ORBWorker() { }

int
Cdmw::clock::svcs::ORBWorker::svc (void)
{
    try
    {
        this->orb_->run (ACE_ENV_SINGLE_ARG_PARAMETER);
    }
    catch (const CORBA::SystemException& e) { }
    return 0;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


Cdmw::clock::svcs::ReactorWorker::~ReactorWorker()
{
    // No-Op
}

int
Cdmw::clock::svcs::ReactorWorker::svc (void)
{
    // std::cout <<".";
    int result =
        ACE_Reactor::instance ()->run_reactor_event_loop();
    
    if (result == -1)
    {
        std::cerr << "EVoT:>> Error handling events" << std::endl;
        return 1;
    }
    
    return 0;
}
