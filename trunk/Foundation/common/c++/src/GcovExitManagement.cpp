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

#ifdef CDMW_USE_GCOV
 
 
#include <iostream>
#include <signal.h>
 
 
namespace Cdmw
{

    namespace Common
    {
        
        void exit_handler(int sig) {
            std::cout << "WARNING: signal ";
            switch (sig) {
                case SIGTERM:
                    std::cout << "SIGTERM";
                    break;
                case SIGINT:
                    std::cout << "SIGINT";
                    break;
                case SIGQUIT:
                    std::cout << "SIGQUIT";
                    break;
                case SIGABRT:
                    std::cout << "SIGABRT";
                    break;
                default:
                    std::cout << sig;
                    break;
            }
            
            std::cout << " was trapped. Exiting..." << std::endl;
            exit(1);
        }

        
        static struct sigaction action;
        
        class GcovExitManagement
        {
            public:

                GcovExitManagement()
                {
                    std::cout << "WARNING: In order to obtain GCOV results, "
                              << "the following signals will be trapped and exit(1) will be called:\n" 
                              << "  SIGTERM, SIGINT, SIGQUIT, SIGABRT " 
                              << std::endl;
                    
                    action.sa_handler=Cdmw::Common::exit_handler;
                    sigemptyset(&action.sa_mask);
                    sigaction(SIGTERM, &action, NULL);
                    sigaction(SIGINT, &action, NULL);
                    sigaction(SIGQUIT, &action, NULL);
                }
        };

        static const GcovExitManagement gcovExitManagement; 

    }
    
}


#endif

