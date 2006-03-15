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


#include "SystemMngt/platformlibrary/EndingCallback.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

namespace Cdmw
{

namespace PlatformMngt
{

//
// Constructor.
//
EndingCallback::EndingCallback () 
{
  m_counter = 0;
  m_delete = false;
}


//
// Destructor.
//
EndingCallback::~EndingCallback()
    throw()
{
}


void EndingCallback::increaseCounter()
{
  CDMW_MUTEX_GUARD(m_mutex);

  m_counter += 1;
}


// Note : only the last callback user set the counter to 0 and is authorised to delete it,
//        the result is protected by the mutex)
bool EndingCallback::decreaseCounter()
{
  CDMW_MUTEX_GUARD(m_mutex);
  
  bool result = false;

  if (m_counter > 0)
  {
      m_counter -= 1;
      
      // if counter set to 0 and if is deletable
      // the caller can delete this object
      if (m_counter == 0 && m_delete == true)
      {
          result = true;
      }    
  }
  
  return result;
}


bool EndingCallback::isDeletable()
{
  CDMW_MUTEX_GUARD(m_mutex);

  if (m_delete == true && m_counter == 0) return true;
  else return false; 
}


bool EndingCallback::isCallable()
{
  CDMW_MUTEX_GUARD(m_mutex);

  if (m_delete == false) return true;
  else return false; 
}


void EndingCallback::setDeleteCallback()
{
  CDMW_MUTEX_GUARD(m_mutex);

  m_delete = true;
}

} // End namespace PlatformMngt
} // End namespace Cdmw



