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


#include <iostream>


#include "platformsystemlibrary/DistributedSupervisionObserver_impl.hpp"

#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"


namespace Cdmw 
{
namespace PlatformMngt 
{
    
    
// ----------------------------------------------------------------------
// DistributedSupervisionObserver_impl class.
// ----------------------------------------------------------------------
DistributedSupervisionObserver_impl::DistributedSupervisionObserver_impl (
                          SupervisionEventNotifier* eventNotifier)
    throw (BadParameterException)
{
    if (eventNotifier == NULL)
    {
        CDMW_THROW2 (BadParameterException, "eventNotifier", "NULL");
    }
    
    // Set the internal data
    m_eventNotifier = eventNotifier;
}  

  
void
DistributedSupervisionObserver_impl::notify (CdmwPlatformMngt::Event* an_event)
    throw (CORBA::SystemException)
{
    // nothing to in centralized
}

} // End namespace PlatformMngt
} // End namespace Cdmw

