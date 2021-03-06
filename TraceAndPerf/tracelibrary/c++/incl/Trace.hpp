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


#include "TraceAndPerf/tracelibrary/FlushAreaMngr.hpp"
#include "TraceAndPerf/tracelibrary/StreamMngr.hpp"
#include "TraceAndPerf/tracelibrary/Stream.hpp"

#ifndef CDMW_TRACE_TRACE_INCL
#define CDMW_TRACE_TRACE_INCL

#if defined  (CDMW_TRACE_LEVEL) && (CDMW_TRACE_LEVEL >= 1 )

#define CDMW_TRACE_REGISTER() Cdmw::Trace::StreamMngr::instance().register_thread()

#define CDMW_TRACE_UNREGISTER() Cdmw::Trace::StreamMngr::instance().unregister_thread()

#define CDMW_TRACE_ACTIVE_FLUSHING() Cdmw::Trace::FlushAreaMngr::instance().activate_flushing()

#define CDMW_TRACE(domain, level, str) \
    do { \
        if (Cdmw::Trace::FlushAreaMngr::is_to_be_traced(domain, level)) {  \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_line(__LINE__); \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_file(__FILE__); \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_component_name(""); \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_userDomain(domain); \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_userLevel(level); \
            Cdmw::Trace::StreamMngr::instance().get_stream() << str << std::ends; \
            Cdmw::Trace::StreamMngr::instance().get_stream().do_flush(); \
        } \
    } while(0)


// ECR-0123
#define CDMW_TRACE_CCM(componentName, domain, level, str)                                       \
    do {                                                                                        \
        if (Cdmw::Trace::FlushAreaMngr::is_to_be_traced(componentName, domain, level)) {        \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_line(__LINE__);                \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_file(__FILE__);                \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_component_name(componentName); \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_userDomain(domain);            \
            Cdmw::Trace::StreamMngr::instance().get_stream().set_userLevel(level);              \
            Cdmw::Trace::StreamMngr::instance().get_stream() << str << std::ends;               \
            Cdmw::Trace::StreamMngr::instance().get_stream().do_flush();                        \
        }                                                                                       \
    } while(0)


#define CDMW_TRACE_ST(stream, domain, level, str) \
    do {  \
        if (Cdmw::Trace::FlushAreaMngr::is_to_be_traced(domain, level)) { \
            stream.set_line(__LINE__); \
            stream.set_file(__FILE__); \
            stream.set_userDomain(domain); \
            stream.set_userLevel(level); \
            stream << str << std::ends; \
            stream.do_flush(); \
        } \
    } while(0)


// ECR-0123
#define CDMW_TRACE_ST_CCM(stream, componentName, domain, level, str)                     \
    do {                                                                                 \
        if (Cdmw::Trace::FlushAreaMngr::is_to_be_traced(componentName, domain, level)) { \
            stream.set_line(__LINE__);                                                   \
            stream.set_file(__FILE__);                                                   \
            stream.set_component_name(componentName);                                    \
            stream.set_userDomain(domain);                                               \
            stream.set_userLevel(level);                                                 \
            stream << str << std::ends;                                                  \
            stream.do_flush();                                                           \
        }                                                                                \
    } while(0)


#else

// Traces are deactivated at compilation
#define CDMW_TRACE_REGISTER()
#define CDMW_TRACE_UNREGISTER()
#define CDMW_TRACE_ACTIVE_FLUSHING()
#define CDMW_TRACE(domain, level, str)
#define CDMW_TRACE_ST(stream, domain, level, str)
// ECR-0123
#define CDMW_TRACE_CCM(componentName, domain, level, str)
#define CDMW_TRACE_ST_CCM(stream, componentName, domain, level, str)

#endif //CDMW_DEACTIVATE_TRACE

#endif //CDMW_TRACE_TRACE_INCL

