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


#include "performancelibrary/FunctionStat.hpp"


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

FunctionStat::FunctionStat(const std::string &apiName)
    throw(Cdmw::OutOfMemoryException)
{
  try
  {
    m_functionName = apiName;
    m_minElapsedTime = 0;
    m_maxElapsedTime = 0;
    m_sumElapsedTime = 0;
    m_hitCount = 0;
  } 
  
  catch (const std::bad_alloc &)
  {
	CDMW_THROW(Cdmw::OutOfMemoryException);
  }
}


FunctionStat::~FunctionStat()
    throw()
{

}

FunctionStat::FunctionStat(const FunctionStat& rhs)
    throw()
{
  m_functionName = rhs.m_functionName;
  m_minElapsedTime = rhs.m_minElapsedTime;
  m_maxElapsedTime = rhs.m_maxElapsedTime;
  m_sumElapsedTime = rhs.m_sumElapsedTime;
  m_hitCount = rhs.m_hitCount;
}

FunctionStat& FunctionStat::operator=(const FunctionStat& rhs)
    throw()
{
  m_functionName = rhs.m_functionName;
  m_minElapsedTime = rhs.m_minElapsedTime;
  m_maxElapsedTime = rhs.m_maxElapsedTime;
  m_sumElapsedTime = rhs.m_sumElapsedTime;
  m_hitCount = rhs.m_hitCount;

  return *this;
}


void FunctionStat::update(long elapsedTime)
        throw()
{
  if (m_minElapsedTime == 0 || elapsedTime < m_minElapsedTime)
  {
    m_minElapsedTime = elapsedTime;
  }
  
  if (elapsedTime > m_maxElapsedTime)
  {
    m_maxElapsedTime = elapsedTime;
  }
   
  m_sumElapsedTime += elapsedTime;
  m_hitCount++;
}

}; // End namespace Performance
}; // End namespace Cdmw

