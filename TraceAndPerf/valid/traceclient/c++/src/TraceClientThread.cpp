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

#include "traceclient/TraceClientThread.hpp"





TraceClientThread::TraceClientThread(CORBA::ORB_ptr orb, 
                           std::string& applicationName,
                           std::string& processName,
                           std::string& traceMessage,
                           unsigned long traceCount)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_applicationName (applicationName),
      m_processName (processName),
      m_traceMessage (traceMessage),
      m_traceCount (traceCount),
      m_exit_required(false)
{
    
}



TraceClientThread::~TraceClientThread() throw()
{
}

 

void TraceClientThread::run () throw()
{
    try
    {        
        std::cout << "Trace Client Thread Started" << std::endl;
        
        std::string baseMsg = "process <" + m_applicationName + "/" + m_processName + ">" ;
        std::string msg;
        unsigned long count = 0;
        
        // ===================================================
        // trace a message (use CDMW_TRACE in Cardamom thread)
        // ===================================================
        msg = baseMsg + "Client Thread started";  
                                       
        /* ECR-0123
        CDMW_TRACE("TEST", 1, msg.c_str());
        */
        CDMW_TRACE_CCM("TEST", "TEST", 1, msg.c_str());



        // ====================================================
        // thread loop
        // ====================================================
        
        msg = baseMsg + m_traceMessage + " counter=";
        
        while (!m_exit_required) 
        {
            if (count < m_traceCount)
            {
                count++;
            
		/* ECR-0123
                CDMW_TRACE("TEST", 1, msg.c_str() << count);
		*/
                CDMW_TRACE_CCM("TEST", "TEST", 1, msg.c_str() << count);
            }
            
            else if (count == m_traceCount)
            {
                count++;
                
                std::string msg2 = baseMsg + "!!!! Trace terminated !!!!";  
                                       
		/* ECR-0123
                CDMW_TRACE("TEST", 1, msg2.c_str());
		*/
                CDMW_TRACE_CCM("TEST", "TEST", 1, msg2.c_str());
                
                std::cout << "Trace terminated : System can be stopped !!!!!!" << std::endl;
            }
            
            // ===================================================
            // set thread sleeping
            // ===================================================
            Cdmw::OsSupport::OS::sleep (1000);
        } 
    }
    catch (...)
    {
        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
	              << "Unexpected exception in thread loop" << std::endl;
    }
}


