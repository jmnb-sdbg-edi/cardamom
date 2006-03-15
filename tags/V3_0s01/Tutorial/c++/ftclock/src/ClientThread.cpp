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


#include <ClientThread.hpp>
#include <TimeoutHandler.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>


static void readKey()
{
    char ch[16];
    fread(ch, 2, 1, stdin);
}


void
ClientThread::run() throw()
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
            std::cout << "[ClientThread]:>> Invoking Requests on the clock"
                      << std::endl;
            for (int i = 0; i < 1000; ++i) {
                std::cout << "[" << i << "]: "   
                          << executor_->current_time() / 10
                          << " usec" << std::endl;
            }
            std::cout << "[ClientThread]:>> Done Invoking Requests on the clock, hit a key to continue...."
                      << std::endl;
            readKey();
            Cdmw::clock::valid::TimeoutHandler handler(executor_.in());
            CosClockService::PeriodicExecution::Periodic_var periodic=
                handler._this();

            // Now we register the newly created object with the Clock Service        
            CosClockService::PeriodicExecution::Controller_var controller = 
                executor_->enable_periodic_execution(periodic.in());
            TimeBase::TimeT period = 10000000; // 100000 * 100 nsec =  1000 msec 
            TimeBase::TimeT offset = 10000000;
            CORBA::Any any;
            
            std::cout << "[ClientThread]:>> Programming timer with:"
                      << "\n\tPeriod: " << period
                      << "\n\toffset: " << offset
                      << std::endl;
            
            controller->start(period, offset, 0, any);
            std::cout << "Controller Scheduled..." << std::endl;
            
            while (true) {
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
    





