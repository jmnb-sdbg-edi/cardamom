/* =========================================================================== *
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
 * =========================================================================== */


#ifndef CDMW_TRACE_CDMWTRACE_HPP
#define CDMW_TRACE_CDMWTRACE_HPP

#include "TraceAndPerf/tracelibrary//Trace.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{

    /**
    * Trace level used in Cdmw to output information.
    */
    enum TraceLevel {

        /**
        * Information purpose
        */
        INF,

        /**
        * Maybe the user has made an error, so we warn it
        */
        WRN,

        /**
        * This level report an error from Cdmw.
        */
        ERR,

        /**
        * The level reports an error from Cdmw, Cdmw is no more
        * able to process safely.
        */
        FTL

    };

    /**
    * Trace domain used inside Cdmw
    *   CDMW_NREP Naming And Repository
    */
    extern const char* CDMW_NREP;

    /**
    *   CDMW_NINT Naming Interface
    */
    extern const char* CDMW_NINT;

    /**
    *   CDMW_LFCL Lifecycle
    */
    extern const char* CDMW_LFCL;

    /**
    *   CDMW_INIT CDMW Initialisation and Cleanup
    */
    extern const char* CDMW_INIT;


#ifndef CDMW_DEACTIVATE_TRACE

/*
* Traces the entry in a given method that doesn't take any parameters
* domain : the trace domain 
* object : the instance on which the method is invoked
* type   : the class of the object
* method : the method invoked
*/
#define CDMW_TRACE_ENTRY(domain, object, type, method) \
do { \
    try \
    { \
        Cdmw::Trace::Stream stream; \
        Cdmw::OsSupport::Thread::ThreadId thread = Cdmw::OsSupport::Thread::self(); \
        CDMW_TRACE_ST(stream, domain, INF, "\n" << object << " : " << type \
            << "\nThread " << thread << " ENTERS " << method << " method\n"); \
    } \
    catch(...) \
    { \
    } \
} while(0)

/*
* Traces the entry in a given method that take one parameter
* domain : the trace domain 
* object : the instance on which the method is invoked
* type   : the class of the object
* method : the method invoked
* param_dir   : the direction of the parameter (in or inout)
* param_name  : the name of the parameter
* param_value : the value of the parameter
*/
#define CDMW_TRACE_ENTRY1(domain, object, type, method, param_dir, param_name, param_value) \
do { \
    try \
    { \
        Cdmw::Trace::Stream stream; \
        Cdmw::OsSupport::Thread::ThreadId thread = Cdmw::OsSupport::Thread::self(); \
        CDMW_TRACE_ST(stream, domain, INF, "\n" << object << " : " << type \
            << "\nThread " << thread << " ENTERS " << method << " method with :\n" \
            << "(" << param_dir << ") " << param_name << " = " << param_value << "\n"); \
    } \
    catch(...) \
    { \
    } \
} while(0)

/*
* Traces the entry in a given method that take two parameters
* domain : the trace domain 
* object : the instance on which the method is invoked
* type   : the class of the object
* method : the method invoked
* param_dir   : the direction of the parameter (in or inout)
* param_name  : the name of the parameter
* param_value : the value of the parameter
*/
#define CDMW_TRACE_ENTRY2(domain, object, type, method, param1_dir, param1_name, param1_value, param2_dir, param2_name, param2_value) \
do { \
    try \
    { \
        Cdmw::Trace::Stream stream; \
        Cdmw::OsSupport::Thread::ThreadId thread = Cdmw::OsSupport::Thread::self(); \
        CDMW_TRACE_ST(stream, domain, INF, "\n" << object << " : " << type \
            << "\nThread " << thread << " ENTERS " << method << " method with :\n" \
            << "(" << param1_dir << ") " << param1_name << " = " << param1_value << "\n" \
            << "(" << param2_dir << ") " << param2_name << " = " << param2_value << "\n"); \
    } \
    catch(...) \
    { \
    } \
} while(0)

/*
* Traces the exit of a given method that returns void and that doesn't have any out parameters
* domain : the trace domain 
* object : the instance on which the method is invoked
* type   : the class of the object
* method : the method invoked
*/
#define CDMW_TRACE_EXIT(domain, object, type, method) \
do { \
    try \
    { \
        Cdmw::Trace::Stream stream; \
        Cdmw::OsSupport::Thread::ThreadId thread = Cdmw::OsSupport::Thread::self(); \
        CDMW_TRACE_ST(stream, domain, INF, "\n" << object << " : " << type \
            << "\nThread " << thread << " EXITS " << method << " method\n"); \
    } \
    catch(...) \
    { \
    } \
} while(0)

/*
* Traces the exit of a given method that has a return value but that doesn't have any out parameters
* domain : the trace domain 
* object : the instance on which the method is invoked
* type   : the class of the object
* method : the method invoked
* ret_value : the returned value
*/
#define CDMW_TRACE_EXIT0(domain, object, type, method, ret_value) \
do { \
    try \
    { \
        Cdmw::Trace::Stream stream; \
        Cdmw::OsSupport::Thread::ThreadId thread = Cdmw::OsSupport::Thread::self(); \
        CDMW_TRACE_ST(stream, domain, INF, "\n" << object << " : " << type \
            << "\nThread " << thread << " EXITS " << method << " method with :\n" \
            << "(return) " << ret_value << "\n"); \
    } \
    catch(...) \
    { \
    } \
} while(0)


/*
* Traces the exit of a given method that has a return value and one out parameter
* domain : the trace domain 
* object : the instance on which the method is invoked
* type   : the class of the object
* method : the method invoked
* param_dir   : the direction of the parameter (inout or out)
* param_name  : the name of the parameter
* param_value : the value of the parameter
* ret_value : the returned value
*/
#define CDMW_TRACE_EXIT1(domain, object, type, method, param_dir, param_name, param_value, ret_value) \
do { \
    try \
    { \
        Cdmw::Trace::Stream stream; \
        Cdmw::OsSupport::Thread::ThreadId thread = Cdmw::OsSupport::Thread::self(); \
        CDMW_TRACE_ST(stream, domain, INF, "\n" << object << " : " << type \
            << "\nThread " << thread << " EXITS " << method << " method with :\n" \
            << "(" << param_dir << ") " << param_name << " = " << param_value << "\n" \
            << "(return) " << ret_value << "\n"); \
    } \
    catch(...) \
    { \
    } \
} while(0)

/*
* Traces the exit of a given method that has a return value and two out parameters
* domain : the trace domain 
* object : the instance on which the method is invoked
* type   : the class of the object
* method : the method invoked
* param_dir   : the direction of the parameter (inout or out)
* param_name  : the name of the parameter
* param_value : the value of the parameter
* ret_value : the returned value
*/
#define CDMW_TRACE_EXIT2(domain, object, type, method, param1_dir, param1_name, param1_value, param2_dir, param2_name, param2_value, ret_value) \
do { \
    try \
    { \
        Cdmw::Trace::Stream stream; \
        Cdmw::OsSupport::Thread::ThreadId thread = Cdmw::OsSupport::Thread::self(); \
        CDMW_TRACE_ST(stream, domain, INF, "\n" << object << " : " << type \
            << "\nThread " << thread << " EXITS " << method << " method with :\n" \
            << "(" << param1_dir << ") " << param1_name << " = " << param1_value << "\n" \
            << "(" << param2_dir << ") " << param2_name << " = " << param2_value << "\n" \
            << "(return) " << ret_value << "\n"); \
    } \
    catch(...) \
    { \
    } \
} while (0)

/*
* Traces an exception raising
* domain : the trace domain 
* object : the instance on which the method is invoked
* type   : the class of the object
* method : the method invoked
* level  : the level of the trace
* exception  : the exception raised
*/
#define CDMW_TRACE_EXCEPTION(domain, object, type, method, level, exception) \
do { \
    try \
    { \
        Cdmw::Trace::Stream stream; \
        Cdmw::OsSupport::Thread::ThreadId thread = Cdmw::OsSupport::Thread::self(); \
        CDMW_TRACE_ST(stream, domain, level, "\n" << object << " : " << type \
            << "\nThread " << thread << " ABORTS " << method << " method with\n" \
            << exception << " exception raised\n"); \
    } \
    catch(...) \
    { \
    } \
} while(0)


#else

// Traces are deactivated at compilation

#define CDMW_TRACE_ENTRY(domain, object, type, method)
#define CDMW_TRACE_ENTRY1(domain, object, type, method, param_dir, param_name, param_value)
#define CDMW_TRACE_ENTRY2(domain, object, type, method, param1_dir, param1_name, param1_value, param2_dir, param2_name, param2_value)
#define CDMW_TRACE_EXIT(domain, object, type, method)
#define CDMW_TRACE_EXIT0(domain, object, type, method, ret_value)
#define CDMW_TRACE_EXIT1(domain, object, type, method, param_dir, param_name, param_value, ret_value)
#define CDMW_TRACE_EXIT2(domain, object, type, method, param1_dir, param1_name, param1_value, param2_dir, param2_name, param2_value, ret_value)
#define CDMW_TRACE_EXCEPTION(domain, object, type, method, level, exception)

#endif //CDMW_DEACTIVATE_TRACE

}

#endif // CDMW_TRACE_CDMW_TRACE_INCL

