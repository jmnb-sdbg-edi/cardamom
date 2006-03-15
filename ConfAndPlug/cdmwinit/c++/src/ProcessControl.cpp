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


#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Assert.hpp"

namespace Cdmw
{
namespace CdmwInit
{

ProcessControl::~ProcessControl()
    throw ()
{ 
}

void ProcessControl::_add_ref()
{ 
    CDMW_MUTEX_GUARD(m_count_mutex);
    ++m_ref_count;
}

void ProcessControl::_remove_ref()
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

CORBA::ULong 
ProcessControl::nb_steps() 
    throw(CORBA::SystemException)
{
    return 1;
}
    
CORBA::Object_ptr 
ProcessControl::get_service() 
    throw(CORBA::SystemException)
{
    return CORBA::Object::_nil();
}
  

void ProcessControl::on_next_step()
    throw(CdmwPlatformMngt::ProcessDelegate::InvalidStep, CORBA::SystemException)
{
    throw CdmwPlatformMngt::ProcessDelegate::InvalidStep();
}
    

}; // End namespace CdmwInit
}; // End namespace Cdmw


