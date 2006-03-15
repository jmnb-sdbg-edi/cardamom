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
#include <iostream>
#include <sstream>

#undef CDMW_TRACE_LEVEL
#define CDMW_TRACE_LEVEL 6

#include "test2/TargetThread.hpp"

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "TraceAndPerf/tracelibrary/Trace.hpp"

namespace
{
    const char* DOMAIN1 = "DomainTest1";
    const char* DOMAIN2 = "DomainTest2";
    const char* COMPONENT1 = "ComponentTest1";
    const char* COMPONENT2 = "ComponentTest2";
}


namespace Cdmw
{
namespace Trace
{

//----------------------------------------------------------------------------
TargetThread::TargetThread(CdmwTrace::TraceProducer_ptr traceProducer)
    throw()
        : m_logger1(Cdmw::Logging::Logger::Get_Logger(COMPONENT1, DOMAIN1)),
          m_logger2(Cdmw::Logging::Logger::Get_Logger(COMPONENT2, DOMAIN1)),
          m_logger3(Cdmw::Logging::Logger::Get_Logger(COMPONENT1, DOMAIN2))
{
    // set the TraceProducer proxy
    m_traceProducer = CdmwTrace::TraceProducer::_duplicate(traceProducer);
}

//----------------------------------------------------------------------------
TargetThread::~TargetThread() throw()
{
    delete m_logger1;
    delete m_logger2;
}

//----------------------------------------------------------------------------
void TargetThread::run()
  throw()
{
    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    
    CDMW_TRACE_ACTIVE_FLUSHING();
    
    // first deactivate all components, all domains and levels
    m_traceProducer->deactivate_level(CdmwTrace::ALL_COMPONENT_NAMES, 
                                      CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);
                                      
    CDMW_LOG(m_logger1, 1, "trace_0_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_0_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_0_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_0_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_0_5");  // is not traced
    
    // activate the filter ComponentTest1, DomainTest1, 1
    m_traceProducer->activate_level(COMPONENT1, DOMAIN1, 1);
    
    CDMW_LOG(m_logger1, 1, "trace_1_A_1");  // is traced    
    CDMW_LOG(m_logger1, 2, "trace_1_A_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_1_A_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_1_A_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_1_A_5");  // is not traced
    
    // deactivate the filter ComponentTest1, DomainTest1, 1
    m_traceProducer->deactivate_level(COMPONENT1, DOMAIN1, 1);
    
    CDMW_LOG(m_logger1, 1, "trace_1_B_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_1_B_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_1_B_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_1_B_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_1_B_5");  // is not traced
    
    // activate the filter ComponentTest1, DomainTest1, 2
    m_traceProducer->activate_level(COMPONENT1, DOMAIN1, 2);
    
    CDMW_LOG(m_logger1, 1, "trace_2_A_1");  // is not traced   
    CDMW_LOG(m_logger1, 2, "trace_2_A_2");  // is traced    
    CDMW_LOG(m_logger2, 1, "trace_2_A_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_2_A_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_2_A_5");  // is not traced
    
    // deactivate the filter ComponentTest1, DomainTest1, 2
    m_traceProducer->deactivate_level(COMPONENT1, DOMAIN1, 2);
    
    CDMW_LOG(m_logger1, 1, "trace_2_B_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_2_B_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_2_B_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_2_B_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_2_B_5");  // is not traced
    
    OsSupport::OS::sleep (timescale*30);
    
    // activate the filter ComponentTest2, DomainTest1, 1
    m_traceProducer->activate_level(COMPONENT2, DOMAIN1, 1);
    
    CDMW_LOG(m_logger1, 1, "trace_3_A_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_3_A_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_3_A_3");  // is traced    
    CDMW_LOG(m_logger2, 2, "trace_3_A_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_3_A_5");  // is not traced
    
    // deactivate the filter ComponentTest2, DomainTest1, 1
    m_traceProducer->deactivate_level(COMPONENT2, DOMAIN1, 1);
    
    CDMW_LOG(m_logger1, 1, "trace_3_B_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_3_B_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_3_B_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_3_B_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_3_B_5");  // is not traced
    
    OsSupport::OS::sleep (timescale*30);
    
    // activate the filter ComponentTest2, DomainTest1, 2
    m_traceProducer->activate_level(COMPONENT2, DOMAIN1, 2);
    
    CDMW_LOG(m_logger1, 1, "trace_4_A_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_4_A_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_4_A_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_4_A_4");  // is traced
    CDMW_LOG(m_logger3, 1, "trace_4_A_5");  // is not traced
    
    // deactivate the filter ComponentTest2, DomainTest1, 2
    m_traceProducer->deactivate_level(COMPONENT2, DOMAIN1, 2);
    
    CDMW_LOG(m_logger1, 1, "trace_4_B_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_4_B_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_4_B_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_4_B_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_4_B_5");  // is not traced
    
    OsSupport::OS::sleep (timescale*30);
    
    // activate the filter ComponentTest1, DomainTest2, 1
    m_traceProducer->activate_level(COMPONENT1, DOMAIN2, 1);
    
    CDMW_LOG(m_logger1, 1, "trace_5_A_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_5_A_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_5_A_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_5_A_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_5_A_5");  // is traced
    
    // deactivate the filter ComponentTest1, DomainTest2, 1
    m_traceProducer->deactivate_level(COMPONENT1, DOMAIN2, 1);
    
    CDMW_LOG(m_logger1, 1, "trace_5_B_1");  // is not traced    
    CDMW_LOG(m_logger1, 2, "trace_5_B_2");  // is not traced    
    CDMW_LOG(m_logger2, 1, "trace_5_B_3");  // is not traced    
    CDMW_LOG(m_logger2, 2, "trace_5_B_4");  // is not traced
    CDMW_LOG(m_logger3, 1, "trace_5_B_5");  // is not traced

}

}; // namespace Trace
}; // namespace Cdmw

