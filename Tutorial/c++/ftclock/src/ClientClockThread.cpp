/* =========================================================================== *
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
 * =========================================================================== */


#include <ClientClockThread.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>

static void readKey()
{
    char ch[16];
    fread(ch, 2, 1, stdin);
}

void
ClientClockThread::run() throw()
{

    std::cout << "------------------------------" << std::endl; 
    std::cout << "Hit a key to start client..." << std::endl;
    readKey();
    std::cout << "Client is running..." << std::endl;

    try
    {
        std::cout << "\nThe client is ready to start periodic Execution!"
                  << std::endl;

        try {
            std::cout << "[ClientClockThread]:>> Invoking Requests on the clock"
                      << std::endl;
            readKey();
            while (true) {
            for (int i = 0; i < 1000; ++i) {
                std::cout << "[" << i << "]: "   
                          << clock_->current_time() / 10
                          << " usec" << std::endl;
            }
            std::cout << "[ClientClockThread]:>> Done Invoking Requests on the clock, hit a key to continue...."
                      << std::endl;
            readKey();

                        
            Cdmw::OsSupport::OS::sleep(1000);
            } 
            
        }
        catch (const CORBA::Exception& e)
        {
            std::cerr << "UNEXPECTED CORBA EXCEPTION !! :\n" 
                      << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                      << std::endl;
        }
        
    }
    catch (const CORBA::Exception& e) {
        std::cerr << "UNEXPECTED CORBA EXCEPTION !! :\n" 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                  << std::endl;
    }
    
}
    





