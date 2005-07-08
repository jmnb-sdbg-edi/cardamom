/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
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
*/
/* ===================================================================== */


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Thread.hpp"

#include "testosthreads/TestThreadDriver.hpp"
#include "testosthreads/TestMutexThreadDriver.hpp"
#include "testosthreads/TestReaderWriterLockDriver.hpp"
#include "testosthreads/TestConditionDriver.hpp"
#include "testosthreads/TestBarrierDriver.hpp"

using namespace Cdmw::OsSupport;

void print_help()
{
    std::cout << "Cdmw::OsThreads Unit testing" << std::endl;
    std::cout << " options are:" << std::endl;
    std::cout << "   --help:           print this message" << std::endl;
    std::cout << "   --test-all:       test all the CSC" << std::endl;
    std::cout << "   --test-Thread:    test the Thread class" << std::endl;
    std::cout << "   --test-Mutex:     test the Mutex class" << std::endl;
    std::cout << "   --test-Lock:      test the Lock class" << std::endl;
    std::cout << "   --test-Condition: test the Condition class" << std::endl;
    std::cout << "   --test-Barrier:   test the Barrier class" << std::endl;

}

//
// Return the of the option specified in the option parameter
//    if the option is not on the command line "no" is returned
//
std::string
get_option_value(int argc, char** argv, const std::string& option)
throw ( Cdmw::OutOfMemoryException,
        Cdmw::InternalErrorException )
{
    std::string option_value = "no";

    try
    {
        int iArg(1);


        // We scan all arguments to find the matching with option

        while ( iArg < argc)
        {

            std::string arg( argv[iArg] );

            // We check if in the current argument we have, the option

            if ( arg.find(option) != std::string::npos )
            {

                std::string::size_type pos_sep = arg.find('=');

                // We have no separator the option is only here so
                //    we return yes, or if the equal is not followed
                //    by something

                if ( ( pos_sep == std::string::npos ) || (pos_sep + 1 == (arg.size()) ))
                {

                    option_value = "yes";

                    // Else we return the value
                }
                else
                {

                    option_value = arg.substr( pos_sep + 1 );
                }

            }

            iArg++;
        }

    }
    catch ( const std::bad_alloc& )
    {

        CDMW_THROW( Cdmw::OutOfMemoryException );

    }
    catch ( ... )
    {

        CDMW_THROW( Cdmw::InternalErrorException );

    }

    return option_value;
}

int main(int argc, char** argv)
{
    bool all(false);


    // Help message
    std::string sleepTime = get_option_value(argc, argv, "--sleep");

    if ( sleepTime != "no" )
    {
        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        Thread::sleep(timescale*atoi(sleepTime.c_str())*1000);
        return EXIT_SUCCESS;
    }


    // No arguments we do all the test
    if ( argc == 1 )
    {
        all = true;
    }

    // Help message
    if ( get_option_value(argc, argv, "--help") == "yes" )
    {
        print_help();
        return EXIT_SUCCESS;
    }

    // --test-all supersedes all other options
    if ( get_option_value(argc, argv, "--test-all") == "yes" )
    {
        all = true;
    }

    if ( all || get_option_value(argc, argv, "--test-Thread") == "yes" )
    {

        // Testing the Thread object
        TestThreadDriver testThreadDriver;
        testThreadDriver.start();
    }


    if ( all || get_option_value(argc, argv, "--test-Mutex") == "yes" )
    {

        // Testing the Mutex, MutexGuard objects
        TestMutexThreadDriver testMutexThreadDriver;
        testMutexThreadDriver.start();
    }

    if ( all || get_option_value(argc, argv, "--test-Condition") == "yes" )
    {

        // Testing the Condition object
        TestConditionDriver testConditionDriver;
        testConditionDriver.start();
    }

    if ( all || get_option_value(argc, argv, "--test-Lock") == "yes" )
    {

        // Testing the ReaderWriterLock, ReaderWriterLockGuard objects
        TestReaderWriterLockDriver testReaderWriterLockDriver;
        testReaderWriterLockDriver.start();
    }

    if ( all || get_option_value(argc, argv, "--test-Barrier") == "yes" )
    {

        // Testing the Barrier object
        TestBarrierDriver testBarrierDriver;
        testBarrierDriver.start();
    }

}


