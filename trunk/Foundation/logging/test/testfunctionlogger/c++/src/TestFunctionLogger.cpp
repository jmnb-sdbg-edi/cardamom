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


#include <Foundation/logging/Logger.hpp>


#include "testfunctionlogger/TestFunctionLogger.hpp"
#include <Foundation/logging/FunctionLogger.hpp>



using Cdmw::Common::TestFunctionLogger;
using Cdmw::Logging::Logger;

class FunctionLoggerUser
{
 public:
    FunctionLoggerUser(Cdmw::Logging::Logger * logger)
        : m_logger(logger) {}
    
    void op1()
    {
        CDMW_LOG_FUNCTION(m_logger,"");
    }
    int op_return(int input1, int input2)
    {
        CDMW_LOG_FUNCTION(m_logger,input1 << "+" << input2);
        CDMW_LOG_FUNCTION_RETURN(input1+input2);
        return input1+input2;
    }
    void op_exception(int input) throw(int)
    {
        CDMW_LOG_FUNCTION(m_logger,"Input=" << input);
        CDMW_LOG_FUNCTION_EXCEPTION(input);
        throw input;
    }
    
 private:
    Cdmw::Logging::Logger * m_logger;
};

/**
 * Default constructor.
 */
TestFunctionLogger::TestFunctionLogger()
        : Testable("[TestFunctionLogger]")
{
}


/**
 * Destructor.
 */
TestFunctionLogger::~TestFunctionLogger()
    throw()
{
}


/**
 * Run the tests.
 */
void
TestFunctionLogger::do_tests()
{
    set_nbOfRequestedTestOK(4);

    TEST_INFO("Obtaining a default logger for the \"TEST_FUNCTION_LOGGER\" domain.\n");
    Logger* logger = Cdmw::Logging::Logger::Get_Logger("TEST_FUNCTION_LOGGER");
    TEST_CHECK(logger != 0);


    TEST_INFO("Calling the Function logger. VISUAL CHECK!.");
    FunctionLoggerUser flu(logger);

    TEST_INFO("Calling op1 (with no return and no exception).");
    flu.op1();
    TEST_SUCCEED();

    int retval = 30;
    TEST_INFO("Calling op_return that return value " << retval);
    int res = flu.op_return(20,10);
    TEST_CHECK(res == retval);

    retval = 50;
    TEST_INFO("Calling op_exception that should raise exception with value " << retval);
    try {
        flu.op_exception(retval);
        TEST_FAILED();
    } catch (int ex) {
        TEST_CHECK(ex == retval);
    }
    
    delete logger;
}
