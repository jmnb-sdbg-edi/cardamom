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


#include "TraceAndPerf/performancelibrary/Probe.hpp"
#include "performancelibrary/ProbeCollector.hpp"

#include <iostream>

namespace Cdmw
{
namespace Performance
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


Probe::Probe(const char *pApiName, bool autoStart)
            throw(Cdmw::BadParameterException,
	              Cdmw::OutOfMemoryException,
	              Cdmw::InternalErrorException)
	        : m_autoStart(autoStart),
	          m_apiName(),
			  m_probeStatus(PROBE_IDLE)
{
  try
  {
    // API name must be defined
    if (pApiName != 0)
    {
	  m_apiName = pApiName;
    }
	else
    {
      CDMW_THROW2(Cdmw::BadParameterException, "API name","NULL");
    }
  
    // get the Probe Collector singleton
	m_pCollector = ProbeCollector::GetInstance();

    // if auto start is set : start measure
    if (m_autoStart)                
    {
	  this->start();
    }
  }

  catch (const std::bad_alloc &)
  {
	CDMW_THROW(Cdmw::OutOfMemoryException);
  }
}

Probe::~Probe()
    throw()
{
  try
  {
    // if auto start is set and measure is started : stop it
    if (m_autoStart)    	                              
    {
	  this->stop();
    }
  }

  // catch all exceptions
  catch (...)
  {
    // TO DO TRACE exception
  }
}

Probe::Probe(const Probe& rhs)
       throw()
{
  m_autoStart = rhs.m_autoStart;
  m_probeStatus = rhs.m_probeStatus;
  m_apiName = rhs.m_apiName;
}

Probe& Probe::operator=(const Probe& rhs)
                throw()
{
  m_autoStart = rhs.m_autoStart;
  m_probeStatus = rhs.m_probeStatus;
  m_apiName = rhs.m_apiName;

  return *this;
}

void Probe::start()
     throw(Cdmw::InternalErrorException)
{
  // if measurement is activated by Probe Collector
  if (m_pCollector->isActivated())
  {
    // if probe is in idle or stopped get the start time of API treatment
    if (m_probeStatus == PROBE_IDLE || m_probeStatus == PROBE_STOPPED)    	                              
    {
	  m_startTime = OsSupport::OS::get_time();
      m_probeStatus = PROBE_STARTED;
    }
  }
}

void Probe::stop()
            throw(Cdmw::InternalErrorException,
			      Cdmw::OutOfMemoryException)
{
  // if probe is started
  if (m_probeStatus == PROBE_STARTED)    	                              
  {
    // if measurement is activated by Probe Collector
	if (m_pCollector->isActivated())
    {
      // stop the stop time of API treatment and send the probe to collector

	  m_stopTime = OsSupport::OS::get_time();

	  m_pCollector->addProbe(*this);
    }

	m_probeStatus = PROBE_STOPPED;
  }
}

long Probe::getElapsedTime() const
			throw()
{
  long elapsedTime = 0;

  // compute elapsed time in API from start and stop time in micro seconds

  elapsedTime = (m_stopTime.seconds - m_startTime.seconds) * 1000000;
  elapsedTime += (m_stopTime.microseconds - m_startTime.microseconds);

  return elapsedTime;
}



}; // End namespace Performance
}; // End namespace Cdmw

