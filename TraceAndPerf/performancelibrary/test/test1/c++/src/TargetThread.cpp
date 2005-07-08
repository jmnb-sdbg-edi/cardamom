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

#include "test1/TargetThread.hpp"

#include "Foundation/common/System.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "TraceAndPerf/performancelibrary/Probe.hpp"

namespace Cdmw
{
namespace Performance
{

const char* TargetThread::ptargetApiName[APINBR] = 
                  {"TargetApi_1", "TargetApi_2", "TargetApi_3"};

const long TargetThread::targetApiCount[APINBR] = {2,3,4};
const long TargetThread::targetApiSleep[APINBR] = {10,20,30};
				
//----------------------------------------------------------------------------
TargetThread::TargetThread() throw()
{
}

//----------------------------------------------------------------------------
TargetThread::~TargetThread() throw()
{
}

//----------------------------------------------------------------------------
int TargetThread::getApiNbr () throw()
{
  return APINBR;
}

//----------------------------------------------------------------------------
int TargetThread::getApiName (const int api_index,std::string &api_name) throw()
{
  int error = -1;
  
  if (api_index >= 0 && api_index < APINBR)
  {
    api_name = ptargetApiName[api_index];

	error = 0;
  }

  return error;
}

//----------------------------------------------------------------------------
int TargetThread::getApiIndex (const std::string &api_name) throw()
{
  int api_index = -1;
  int i;
  
  for (i=0 ; i < APINBR && api_index == -1 ; i++)
  {
    if (api_name == ptargetApiName[i])
	{
	  api_index = i;
	}
  }

  return api_index;
}

//----------------------------------------------------------------------------
long TargetThread::getApiCount (const int api_index) throw()
{
  long count = -1;

  if (api_index >= 0 && api_index < APINBR)
  {
    count = targetApiCount[api_index];
  }

  return count;
}
      	    
//----------------------------------------------------------------------------
long TargetThread::getApiSleepTime (const int api_index) throw()
{
  long sleep_time = -1;

  if (api_index >= 0 && api_index < APINBR)
  {
	sleep_time = targetApiSleep[api_index];
  }

  return sleep_time;
}

//----------------------------------------------------------------------------
void TargetThread::run()
  throw()
{
  int i;

  for (i=0 ; i < targetApiCount[0] ; i++)
  {
    TargetApi1();
  }
  
  for (i=0 ; i < targetApiCount[1] ; i++)
  {
    TargetApi2();
  }

  for (i=0 ; i < targetApiCount[2] ; i++)
  {
    TargetApi3();
  }
}

//----------------------------------------------------------------------------
void TargetThread::TargetApi1()
{
  Performance::Probe probe(ptargetApiName[0]);
  int timescale = Cdmw::TestUtils::Testable::get_timescale();
  OsSupport::OS::sleep (timescale*targetApiSleep[0]);
}

//----------------------------------------------------------------------------
void TargetThread::TargetApi2()
{
  Performance::Probe probe(ptargetApiName[1]);
  int timescale = Cdmw::TestUtils::Testable::get_timescale();
  OsSupport::OS::sleep (timescale*targetApiSleep[1]);	  
}

//----------------------------------------------------------------------------
void TargetThread::TargetApi3()
{
  Performance::Probe probe(ptargetApiName[2]);
  int timescale = Cdmw::TestUtils::Testable::get_timescale();
  OsSupport::OS::sleep (timescale*targetApiSleep[2]);
}

//----------------------------------------------------------------------------

}; // namespace Performance
}; // namespace Cdmw

