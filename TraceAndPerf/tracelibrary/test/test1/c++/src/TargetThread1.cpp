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
#include <iostream>
#include <sstream>

#include "test1/TargetThread1.hpp"

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
TargetThread1::TargetThread1() throw()
{
}

//----------------------------------------------------------------------------
TargetThread1::~TargetThread1() throw()
{
}

//----------------------------------------------------------------------------
void TargetThread1::run()
  throw()
{
  int i;

  CDMW_TRACE_ACTIVE_FLUSHING();

  CDMW_TRACE("test", 1, "start target thread1");

  for (i=0 ; i < 1 ; i++)
  {
  	std::ostringstream name_str;
  	name_str << "target loop cycle " << i;

    CDMW_TRACE("domain1", 1, name_str.str());
    CDMW_TRACE("domain1", 2, name_str.str());
    CDMW_TRACE("domain1", 3, name_str.str());
    CDMW_TRACE("domain2", 1, name_str.str());
    CDMW_TRACE("domain2", 2, name_str.str());
    CDMW_TRACE("domain2", 3, name_str.str());
    CDMW_TRACE("domain3", 1, name_str.str());
    CDMW_TRACE("domain3", 2, name_str.str());
    CDMW_TRACE("domain3", 3, name_str.str());

	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep (timescale*30);
  }

  CDMW_TRACE("test", 1, "end target thread1");
}

}; // namespace Trace
}; // namespace Cdmw

