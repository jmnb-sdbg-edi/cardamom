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
#include <stdio.h>

#include "Time/clockservice/CosClockService.stub.hpp"


static const int SET_TIME = 1;
static const int SET_RATE = 2;
static const int PAUSE = 3;
static const int RESUME = 4;
static const int EXIT = 5;

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

int
main(int argc, char* argv[])
{
    CORBA::ORB_var orb =
        CORBA::ORB_init(argc, argv, "TimeReader-ORB");


    CORBA::Object_var obj =
        orb->resolve_initial_references("ClockService");

    if (CORBA::is_nil(obj.in())) {
        std::cerr << "Unable to resolve ClockCatalog, Aborting!"
                  << std::endl;
        return -1;
    }
    
    CosClockService::ClockCatalog_var clock_catalog =
        CosClockService::ClockCatalog::_narrow(obj.in());
    
    CosClockService::ClockCatalog::ClockEntry* catalog_entry =
        clock_catalog->get_entry("FederatedControlledClock");
    
    CosClockService::ControlledClock_var ctrl_clock
        = CosClockService::ControlledClock::_narrow(catalog_entry->subject.in());
    
    if (CORBA::is_nil(ctrl_clock.in())) {
        std::cerr << "Unable to resolve FederatedControlledClock, Aborting!"
                  << std::endl;
        return -1;
    }
    int choice = 0;
    choice = get_option();
    
    while (choice != EXIT) {
        try {
        switch (choice) {
        case SET_TIME: {
            TimeBase::TimeT the_time;
            std::cout << "Enter New Time: ";
            std::cin >> the_time;
                ctrl_clock->set(the_time);
            break;
        }
        case SET_RATE: {
            float rate;
            std::cout << "Enter New Rate: ";
            std::cin >> rate;
            ctrl_clock->set_rate(rate);
            break; 
        }
        case PAUSE: {
            ctrl_clock->pause();
            break; 
        }
        case RESUME: {
            ctrl_clock->resume();
            break;
        }

        case EXIT:
        default:
            break;
        }
        }
        catch (const CORBA::SystemException& e) {
                std::cout << e << std::endl;
        }
        system("clear");
        choice = get_option();
    }
    
    return 0;
}
