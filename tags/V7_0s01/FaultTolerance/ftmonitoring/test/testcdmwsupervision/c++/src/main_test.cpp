/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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

// Std include files
#include <iostream>
#include <sstream>
#include <string.h>

#include <Foundation/ossupport/OS.hpp>

#include "testcdmwsupervision/TestMonitoringLibrary.hpp"

using namespace std;

int main (int argc, char* argv[])
{
    
    int ret_code = 0;
    int timeOn;
    std::string timeOnOpt =
      Cdmw::OsSupport::OS::get_option_value(argc, argv, "--timeOn");

    if (timeOnOpt == "no")
    {
      cout << "Mutual/Simple Monitoring will run during 5s" << std::endl;
      timeOn=5000;
    }
    else if (timeOnOpt == "keypressed")
    {
      cout << "Mutual/Simple Monitoring will run until key pressed + enter" << std::endl;
      cout << "When you have finish to read the message, press key + enter" << std::endl;
      char dummy;
      cin >> dummy;
      timeOn=0;
    }
    else
    {
      timeOn=atoi(timeOnOpt.c_str());
      cout << "Mutual/Simple Monitoring will run during "
           << timeOn << " milliseconds" << std::endl;
    }

    TestMonitoringLibrary myTestMonitoringLibrary(argc, argv, timeOn);
    myTestMonitoringLibrary.start();
       
    return ret_code;
}

