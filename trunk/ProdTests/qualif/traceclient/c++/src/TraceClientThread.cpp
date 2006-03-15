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


#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>


#include "Foundation/common/Locations.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"

#include "TraceAndPerf/tracelibrary/Trace.hpp"
#include "TraceAndPerf/performancelibrary/Probe.hpp"

#include "traceclient/TraceClientThread.hpp"




TraceClientThread::TraceClientThread(CORBA::ORB_ptr orb, 
                           std::string& application_name,
                           std::string& process_name,
                           bool multi_domain_level,
                           std::string& trace_message,
                           long trace_count,
                           long perf_count)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_application_name (application_name),
      m_process_name (process_name),
      m_multi_domain_level (multi_domain_level),
      m_trace_message (trace_message),
      m_trace_count (trace_count),
      m_perf_count (perf_count),
      m_exit_required(false)
{
    
}



TraceClientThread::~TraceClientThread() throw()
{
}


void TraceClientThread::stop() 
{ 
     m_exit_required = true; 
}
 

void TraceClientThread::run () throw()
{
    try
    {        
        std::cout << "Trace Client Thread Started" << std::endl;
        
        std::string baseMsg = "process <" + m_application_name + "/" + m_process_name + ">" ;
        std::string msg;
        long count_for_trace = 0;
        long count_for_perf = 0;
        
        // ===================================================
        // trace a message (use CDMW_TRACE in Cardamom thread)
        // ===================================================
        msg = baseMsg + "Client Thread started";  
                                       
        CDMW_TRACE("TEST", 1, msg.c_str());



        // ====================================================
        // thread loop
        // ====================================================
        
        msg = baseMsg + m_trace_message + " counter=";
        
        while (!m_exit_required) 
        {
            //
            // Trace messages
            //
            
            // if requested trace count = -1 process infinite loop
            
            if (m_trace_count==-1 || count_for_trace < m_trace_count)
            {
                count_for_trace++;
                
                if (m_multi_domain_level)
                {
                    /* ECR-0123
                    CDMW_TRACE("DOMAIN-1", 1, msg.c_str() << count_for_trace);
                    CDMW_TRACE("DOMAIN-1", 5, msg.c_str() << count_for_trace);
                    CDMW_TRACE("DOMAIN-2", 2, msg.c_str() << count_for_trace);
                    CDMW_TRACE("DOMAIN-2", 6, msg.c_str() << count_for_trace);
                    */
                    CDMW_TRACE_CCM("COMP_A", "DOMAIN-1", 1, msg.c_str() << count_for_trace);
                    CDMW_TRACE_CCM("COMP_B", "DOMAIN-1", 5, msg.c_str() << count_for_trace);
                    CDMW_TRACE_CCM("COMP_A", "DOMAIN-2", 2, msg.c_str() << count_for_trace);
                    CDMW_TRACE_CCM("COMP_B", "DOMAIN-2", 6, msg.c_str() << count_for_trace);
                }
                
                else
                {            
                    /* ECR-0123
                    CDMW_TRACE("TEST", 1, msg.c_str() << count_for_trace);
                    */
                    CDMW_TRACE_CCM("COMP_A", "TEST", 1, msg.c_str() << count_for_trace);
                    CDMW_TRACE_CCM("COMP_B", "TEST", 1, msg.c_str() << count_for_trace);
                }
            }
            
            else if (m_trace_count!=0 && count_for_trace == m_trace_count)
            {
                count_for_trace++;
                
                std::string msg2 = baseMsg + "!!!! Trace terminated !!!!";  
                                       
                CDMW_TRACE("TEST", 1, msg2.c_str());
                
                std::cout << "Trace terminated : System can be stopped !!!!!!" << std::endl;
            }
            
            
            //
            // Performance 
            //
                        
            // if requested performance cycle = -1 process infinite loop
            
            if (m_perf_count==-1 || count_for_perf < m_perf_count)
            {
                count_for_perf++;
                
                target_api (count_for_perf);                
            }
            
            else if (m_perf_count!=0 && count_for_perf == m_perf_count)
            {
                count_for_perf++;
                
                std::string msg2 = baseMsg + "!!!! Performance Cycle terminated !!!!";  
                                       
                std::cout << msg2.c_str() << std::endl;
                                
                std::cout << "Performance terminated : System can be stopped !!!!!!" << std::endl;
            }
            
            // ===================================================
            // set thread sleeping
            // ===================================================
            Cdmw::OsSupport::OS::sleep (2000);
        } 
    }
    catch (...)
    {
        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                  << "Unexpected exception in thread loop" << std::endl;
    }
}


void TraceClientThread::target_api(long count_for_perf)
{
    Cdmw::Performance::Probe probe("target_api");
    
    std::cout << "Performance cycle number = ***" << count_for_perf << "***" << std::endl;
}

