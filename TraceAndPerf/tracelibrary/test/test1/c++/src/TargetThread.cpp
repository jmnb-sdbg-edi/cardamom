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

#include "test1/TargetThread.hpp"

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "TraceAndPerf/tracelibrary/Trace.hpp"


namespace Cdmw
{
namespace Trace
{

//----------------------------------------------------------------------------
TargetThread::TargetThread()
    throw()
        : m_logger(Cdmw::Logging::Logger::Get_Logger("test")) // ECR-0169
{
}

//----------------------------------------------------------------------------
TargetThread::~TargetThread() throw()
{
    delete m_logger; // ECR-0169
}

//----------------------------------------------------------------------------
void TargetThread::run()
  throw()
{
    int i;

    CDMW_TRACE_ACTIVE_FLUSHING();

    // ECR-0169
    // CDMW_TRACE("test", 1, "start target thread");
    CDMW_LOG(m_logger, 1, "start target thread");

    for (i=0 ; i < 10 ; i++)
    {
        std::ostringstream name_str;
        name_str << "target loop cycle " << i;

        // ECR-0169
        // CDMW_TRACE("test", 1, name_str.str());
        CDMW_LOG(m_logger, 1, name_str.str());

        int timescale = Cdmw::TestUtils::Testable::get_timescale();
        OsSupport::OS::sleep (timescale*30);
    }

    // ECR-0169
    // CDMW_TRACE("test", 1, "end target thread");
    CDMW_LOG(m_logger, 1, "end target thread");
}

}; // namespace Trace
}; // namespace Cdmw

