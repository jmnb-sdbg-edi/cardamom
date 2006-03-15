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
 
#include "ccm_ft_timers/TimeoutHandler.hpp"
#include <iostream>
#include <Foundation/orbsupport/OrbSupport.hpp>

using namespace CosClockService;
using namespace CosClockService::PeriodicExecution;

Cdmw::clock::qualif::TimeoutHandler::TimeoutHandler(Executor_ptr executor)
    : executor_(Executor::_duplicate(executor)),
      count_(0)
{ 
    // No-Op    
}

Cdmw::clock::qualif::TimeoutHandler::~TimeoutHandler() { }

CORBA::Boolean
Cdmw::clock::qualif::TimeoutHandler::do_work(const CORBA::Any&) 
    throw (CORBA::SystemException)
{
    try {
        std::cout << "[******** PERIODIC EXECUTION ******* ]: "    << std::endl;
        std::cout << "[" << count_++ << "]: "   
                  << executor_->current_time() / 10
                  << " usec" << std::endl;
        
    }
    catch (const CORBA::Exception& e) {
        std::cerr << "UNEXPECTED CORBA EXCEPTION !! :\n" 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                  << std::endl;
        return 0;
    }
    
    return 1;
}
