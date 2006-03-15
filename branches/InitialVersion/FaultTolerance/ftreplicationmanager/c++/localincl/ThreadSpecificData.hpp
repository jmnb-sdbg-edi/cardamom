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
#ifndef _THREAD_SPECIFIC_DATA_H_
#define _THREAD_SPECIFIC_DATA_H_

#include <stdio.h>
#include <pthread.h>

#define DEF_MKEY 0xFFFF

namespace Cdmw {
namespace FT {
namespace ReplicationManager {

  // #### FIXME: this is a POSIX specific version which should be extended to
  //             support other OS such as Windows (using the Thread Specific
  //             Storage API)
  class ThreadSpecificData {
  
    public:
    
      ThreadSpecificData()
          : m_status(0)
          , m_key(DEF_MKEY)
      { 
        m_ctor_status = pthread_key_create (&m_key, NULL);
        if (m_ctor_status) perror("ThreadSpecificData Ctor(): ");
      }

      ThreadSpecificData(void (*key_data_dtor)(void *))
          : m_status(0)
          , m_key(DEF_MKEY)
      { 
        m_ctor_status = pthread_key_create (&m_key, key_data_dtor);
        if (m_ctor_status) perror("ThreadSpecificData Ctor(void (*ThreadKeyDestructor)(void *)): ");
      }

      ~ThreadSpecificData()
      { 
        int status = pthread_key_delete(m_key);
        if (status) perror("ThreadSpecificData Dtor: ");
      }

      void set_value(void* the_value)
      {
        m_status = pthread_setspecific(m_key, the_value);
        if (m_status) perror("ThreadSpecificData::set_value: ");
      }

      void* get_value()
      {
        return pthread_getspecific(m_key);
      }
      
      int   last_error()  { return m_status; }
      int   xtor_error()  { return m_ctor_status; }

    protected:
    
      int              m_status;
      int              m_ctor_status;
      pthread_key_t    m_key;

  };

}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw


#endif // _THREAD_SPECIFIC_DATA_H_
