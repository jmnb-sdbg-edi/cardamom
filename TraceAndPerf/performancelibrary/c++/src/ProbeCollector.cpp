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


#include <iostream>
#include <fstream>

#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "performancelibrary/ProbeCollector.hpp"
#include "TraceAndPerf/performancelibrary/Probe.hpp"


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

ProbeCollector *ProbeCollector::M_pInstance = NULL;
OsSupport::Mutex ProbeCollector::M_mutex;

ProbeCollector::ProbeCollector()
    throw()
	: m_active(false)
{
}


ProbeCollector::~ProbeCollector()
    throw()
{
  try
  {
    long size = m_functionStat.size();

    FuncStatMap::iterator iter = m_functionStat.begin();

    int i;

    for (i=0 ; i < size ; i++)
    {
	  FunctionStat *pfunctionStat = iter->second;
	  if (pfunctionStat != NULL)
      {
	    delete pfunctionStat;
      }

	  iter++;
    }
  }

  // catch all exceptions
  catch (...)
  {
    // TO DO TRACE exception
  }
}

ProbeCollector* ProbeCollector::GetInstance()
			 throw(Cdmw::OutOfMemoryException)
{
  try
  {
    // if singleton instance is not created try it
    if (M_pInstance == NULL)
    {
	  // lock mutex to block concurent threads
	  // (mutex is unlocked at end of block)
	  CDMW_MUTEX_GUARD (M_mutex);

	  // recheck the instance to avoid reallocation for concurent threads
	  if (M_pInstance == NULL)
      {
        M_pInstance = new ProbeCollector();
      }
    }
  }

  catch (const std::bad_alloc &)
  {
	CDMW_THROW(Cdmw::OutOfMemoryException);
  }

  // catch any exception from Mutex and assert it
  catch (...)
  {
    CDMW_ASSERT(false == true);
  }

  return M_pInstance;
}

void ProbeCollector::addProbe (const Probe &probe)
			 throw(Cdmw::OutOfMemoryException)
{
  try
  {
    FunctionStat *pfunctionStat = NULL;

	std::string papiName = probe.getApiName();

    CDMW_WRITER_LOCK_GUARD (m_readWriteMapLock);

    // in map search Function Stat corresponding to the API name from Probe

    FuncStatMap::iterator iter = m_functionStat.find(papiName);

    if (iter == m_functionStat.end())      // if not found
    {
      // create a new Function Stat and insert it in the map
	  pfunctionStat = new FunctionStat(papiName);
      m_functionStat.insert(FuncStatMap::value_type(papiName,pfunctionStat));
    }
    else
    {
      // get the Function Stat from iterator
      pfunctionStat = iter->second;
    }

    // update the Function Stat with elapsed time from Probe
    pfunctionStat->update(probe.getElapsedTime());
  }

  catch (const std::bad_alloc &)
  {
	CDMW_THROW(Cdmw::OutOfMemoryException);
  }

  // catch any exception from Mutex Guard and assert it
  catch (...)
  {
    CDMW_ASSERT(false == true);
  }
}

void ProbeCollector::writeStatistic (const std::string &logFileName)
             throw(Cdmw::BadParameterException)
{
  try
  {
    // open the log file name and write each Function Stat values of the map

    std::ofstream perfoLog (logFileName.c_str(),std::ios::out);
    if (perfoLog)
    {
      CDMW_READER_LOCK_GUARD (m_readWriteMapLock);

      long size = m_functionStat.size();

      FuncStatMap::iterator iter = m_functionStat.begin();

      int i;

      // for each Function Stat from the map
      for (i=0 ; i < size ; i++)
      {
        FunctionStat *pfunctionStat = iter->second;
	
        perfoLog << pfunctionStat->getFunctionName().c_str() << ";";
        perfoLog << pfunctionStat->getMinElapsedTime() << ";";
	    perfoLog << pfunctionStat->getMaxElapsedTime() << ";";
	    perfoLog << pfunctionStat->getSumElapsedTime() << ";";
	    perfoLog << pfunctionStat->getHitCount() << std::endl;

	    iter++;
      }

	  perfoLog.close();
    }
    else
    {
      CDMW_THROW2(BadParameterException,"Perfomance Log File cannot be opened",
                   logFileName.c_str());
    }
  }

  catch (BadParameterException&)
  {
    throw;
  }

  // catch any exception from Mutex Guard and assert it
  catch (...)
  {
    CDMW_ASSERT(false == true);
  }
}

const Performance::FunctionStat* ProbeCollector::getFunctionStat (const Probe &probe)
			 throw()
{
  const FunctionStat *pfunctionStat = NULL;
   
  try
  {
    // get the Function Stat from the map corresponding to the Probe

    const char* papiName = probe.getApiName().c_str();

    CDMW_READER_LOCK_GUARD (m_readWriteMapLock);

    FuncStatMap::iterator iter = m_functionStat.find(papiName);

    if (iter != m_functionStat.end())      // if found
    {
      pfunctionStat = iter->second;
    }
  }

  // catch any exception from Mutex Guard and assert it
  catch (...)
  {
    CDMW_ASSERT(false == true);
  }

  return pfunctionStat; 
}

const Performance::FunctionStat* ProbeCollector::getFunctionStat (const std::string &apiName)
			 throw()
{
  const FunctionStat *pfunctionStat = NULL;
   
  try
  {
    // get the Function Stat from the map corresponding to the Probe

    const char* papiName = apiName.c_str();

    CDMW_READER_LOCK_GUARD (m_readWriteMapLock);

    FuncStatMap::iterator iter = m_functionStat.find(papiName);

    if (iter != m_functionStat.end())      // if found
    {
      pfunctionStat = iter->second;
    }
  }

  // catch any exception from Mutex Guard and assert it
  catch (...)
  {
    CDMW_ASSERT(false == true);
  }

  return pfunctionStat; 
}

bool ProbeCollector::isActivated ()
            throw()
{
  return m_active;
}

void ProbeCollector::setActivity (bool value)
		    throw()
{
  m_active = value;
}


}; // End namespace Performance
}; // End namespace Cdmw

