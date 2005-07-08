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
#include "ctools/time/HighResTimer.h"
#include "ctools/time/HighResTime.h"
#include "ctools/time/HighResClock.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "USAGE:\n\t" << argv[0] << " <iterations>"
                  << std::endl;
        exit(-1);
    }

    size_t iterations = atoi(argv[1]);
    
    ctools::time::HighResTimer timer;
    {
        timer.start();
        for  (size_t i = 0; i < iterations; ++i) { 
	  //            std::cout << ".";
	  std::cout << ctools::time::HighResClock::getTime() << "." << std::endl;
        } 
        std::cout << std::endl;
        timer.stop();
    }

    ctools::time::HighResTime time;
    timer.getElapsedTime(time);
    std::cout << "Time: " << time << std::endl;

    std::cout << "Time (ms): " << time.toMillisec()
              << std::endl;
    std::cout << "Time (ns): " << time.toNanosec()
              << std::endl;
    return 0;
}
