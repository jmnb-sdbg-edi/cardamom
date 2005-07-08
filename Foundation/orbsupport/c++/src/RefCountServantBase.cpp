/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "Foundation/orbsupport/RefCountServantBase.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Assert.hpp"

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Implementation notes: none 
        *<p>
        *Portability issues: none
        *<p>
        */

        RefCountServantBase::~RefCountServantBase()
        {
            CDMW_ASSERT(m_ref_count == 0);
        }

        void RefCountServantBase::_add_ref()
        {
            CDMW_MUTEX_GUARD(m_count_mutex);
            ++m_ref_count;
        }

        void RefCountServantBase::_remove_ref()
        {
            bool del = false;
            {
                CDMW_MUTEX_GUARD(m_count_mutex);
                CDMW_ASSERT(m_ref_count > 0);

                if (--m_ref_count == 0)
                    del = true;
            }

            if (del)
                delete this;
        }

        CORBA::ULong RefCountServantBase::_get_ref_count() const
        {
            // TODO: Do we really need a lock here?
            return m_ref_count;
        }

        // This is to set the Reference counter value. Returns old value.
        CORBA::ULong RefCountServantBase::_set_ref_count(CORBA::ULong count)
        {
            CDMW_MUTEX_GUARD(m_count_mutex);
            CORBA::ULong previous_count = m_ref_count;
            m_ref_count = count;
            return previous_count;
        }


    } // End namespace OrbSupport
} // End namespace Cdmw

