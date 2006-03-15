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
 * @author Antonello Cedrone <acedrone@vitrociset.it>
 * @author Raffaele Mele <rmele@progesi.it>
 */

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <sys/time.h>
#include <fstream>
#include <unistd.h>
#include <Foundation/ossupport/OS.hpp>

using namespace Cdmw::OsSupport;


int main(int argc, char* argv[])
{

  try
    {

      struct timeval tv;
      struct timezone tz;

      // wait for actviation of other process
      sleep(2);

      std::cerr	<< "-------------------------" << std::endl
      << "      Start Killer" << std::endl
      << "-------------------------" << std::endl;
      for (int n=0; n < argc; n++)
        std::cerr << "argv[" << n <<"]: " << argv[n] << std::endl;
      std::cerr 	<< "-------------------------" << std::endl;
      // Read the HelloProcess PID
      std::string file_pid_str = "/tmp/CdmwDaemon_";
      file_pid_str = file_pid_str + argv[1] +  "/SmgPerfApplication/HelloProcess.pid";
      std::ifstream file_pid(file_pid_str.c_str());
      if (!file_pid)
        {
          std::cerr << "*** ERROR: Cannot open PID input file "
          << file_pid_str
          << " ***"
          << std::endl;
          std::exit(1);
        }
      std::string pid_str;
      file_pid >> pid_str;
      int pid = atoi(pid_str.c_str());
      if (pid)
        {
          std::cerr	<< "HelloProcess PID: "
          << pid
          << std::endl;
          OS::kill_process(pid);
          // read killing time from the OS
          std::ofstream of("time2process.dat",std::ios::out|std::ios::app);
          gettimeofday(&tv, &tz);
          if (of) 
          {
              std::cerr << "*** WRITING FILE time2process.dat ***" << std::endl;
              of << "Sec "
              << tv.tv_sec
              << " Usec "
              << tv.tv_usec
              << std::endl;
			  of.close();
          }
          else 
          {
              std::cerr << "*** ERROR IN WRITING FILE time2process.dat ***" << std::endl;
          }
        }
      else
        {
          std::cerr << "*** ERROR: Invalid PID from "
          << file_pid_str
          << " ***"
          << std::endl;
        }

      file_pid.close();
      return 0;

    }
  catch (const std::exception & ex)
    {
      std::cerr << "Caught generic exception :" << ex.what() << std::endl;
    }

}


