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


#include "testlogging/TestLogging.hpp"
#include <Foundation/logging/FunctionLogger.hpp>



using Cdmw::Common::TestLogging;
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
    int op_return(int input)
    {
        CDMW_LOG_FUNCTION(m_logger,"");
        CDMW_LOG_FUNCTION_RETURN(input);
        return input;
    }
    void op_exception(int input) throw(int)
    {
        CDMW_LOG_FUNCTION(m_logger,"");
        CDMW_LOG_FUNCTION_EXCEPTION(input);
        throw input;
    }
    
 private:
    Cdmw::Logging::Logger * m_logger;
};

/**
 * Default constructor.
 */
TestLogging::TestLogging()
        : Testable("[TestLogging]")
{
}


/**
 * Destructor.
 */
TestLogging::~TestLogging()
    throw()
{
}


/**
 * Run the tests.
 */
void
TestLogging::do_tests()
{
    set_nbOfRequestedTestOK(19);

    TEST_INFO("Obtaining a default logger for the \"TEST_LOGGING\" domain.\n"
              "Only messages with a level <= INFO (ie. lvl <= 8) are displayed.");
    Logger* logger = Cdmw::Logging::Logger::Get_Logger("TEST_LOGGING");
    TEST_CHECK(logger != 0);

    TEST_INFO("Logging an info message.");
    CDMW_INFO(logger, "my info message 1");
    if (logger->is_info_enabled()) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    TEST_INFO("Logging a warning message.");
    CDMW_WARN(logger, "my warning message 1");
    if (logger->is_warn_enabled()) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    TEST_INFO("Logging an error message.");
    CDMW_ERROR(logger, "my error message 1");
    if (logger->is_error_enabled()) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    TEST_INFO("Logging a fatal error message.");
    CDMW_FATAL(logger, "my fatal error message 1");
    if (logger->is_fatal_enabled()) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    TEST_INFO("Logging a debug message (lvl > INFO).");
    CDMW_DEBUG(logger, "my debug message 1");
    if (logger->is_debug_enabled()) {
        TEST_FAILED();
    }
    else {
        TEST_SUCCEED();
    }

    const short level_7 = 7;
    TEST_INFO("Logging a generic message w/ lvl " << level_7 << ".");
    CDMW_LOG(logger, level_7, "my generic message 1");
    if (logger->is_enabled_for(level_7)) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    const short level_20 = 20;
    TEST_INFO("Logging a generic message w/ lvl " << level_20 << ".");
    CDMW_LOG(logger, level_20, "my generic message 2");
    if (logger->is_enabled_for(level_20)) {
        TEST_FAILED();
    }
    else {
        TEST_SUCCEED();
    }

    delete logger;

    TEST_INFO("Obtaining a default logger for the \"COMPONENT1\" of "
              "the \"TEST_LOGGING\" domain.\n"
              "Only messages with a level <= INFO (ie. lvl <= 8) are displayed.");
    logger = Cdmw::Logging::Logger::Get_Logger("COMPONENT1", "TEST_LOGGING");
    TEST_CHECK(logger != 0);

    TEST_INFO("Logging an info message.");
    CDMW_INFO(logger, "my info message 2");
    if (logger->is_info_enabled()) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    TEST_INFO("Logging a warning message.");
    CDMW_WARN(logger, "my warning message 2");
    if (logger->is_warn_enabled()) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    TEST_INFO("Logging an error message.");
    CDMW_ERROR(logger, "my error message 2");
    if (logger->is_error_enabled()) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    TEST_INFO("Logging a fatal error message.");
    CDMW_FATAL(logger, "my fatal error message 2");
    if (logger->is_fatal_enabled()) {
        TEST_SUCCEED();
    }
    else {
        TEST_FAILED();
    }

    TEST_INFO("Logging a debug message (lvl > 8).");
    CDMW_DEBUG(logger, "my debug message 2");
    if (logger->is_debug_enabled()) {
        TEST_FAILED();
    }
    else {
        TEST_SUCCEED();
    }

    TEST_INFO("Logging a generic message w/ lvl " << level_7 << ".");
    CDMW_LOG(logger, level_7, "my generic message 3");
    if (logger->is_debug_enabled()) {
        TEST_FAILED();
    }
    else {
        TEST_SUCCEED();
    }

    TEST_INFO("Logging a generic message w/ lvl " << level_20 << ".");
    CDMW_LOG(logger, level_20, "my generic message 4");
    if (logger->is_enabled_for(level_20)) {
        TEST_FAILED();
    }
    else {
        TEST_SUCCEED();
    }

    TEST_INFO("Calling the Function logger. VISUAL CHECK!.");
    FunctionLoggerUser flu(logger);

    TEST_INFO("Calling op1 (with no return and no exception).");
    flu.op1();
    TEST_SUCCEED();

    int retval = 30;
    TEST_INFO("Calling op_return that return value " << retval);
    int res = flu.op_return(retval);
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
