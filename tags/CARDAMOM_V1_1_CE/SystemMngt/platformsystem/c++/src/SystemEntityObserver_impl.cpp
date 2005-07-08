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


#include "platformsystem/SystemEntityObserver_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/RWEntityContainer_impl.hpp"
#include "platformsystem/System_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

SystemEntityObserver_impl::SystemEntityObserver_impl (System_impl* system, 
                                                      RWEntityContainer_impl* entityContainer)    
    throw (BadParameterException)
{
    if (system == NULL)
    {
        CDMW_THROW2 (BadParameterException, "system", "NULL");
    }
     
    if (entityContainer == NULL)
    {
        CDMW_THROW2 (BadParameterException, "entityContainer", "NULL");
    }
    
    m_system = system;             
    m_entityContainer = entityContainer;
}

void SystemEntityObserver_impl::set_system_entity_status (
                     const char* entity_name,
                     CdmwPlatformMngtEntity::EntityStatus entity_status,
                     const char* entity_info)
    throw (CdmwPlatformMngtEntity::EntityNotFound,
           CORBA::SystemException)
{
    m_entityContainer->set_entity_status (
                               entity_name,
                               entity_status,
                               entity_info);

    m_system->notifyEntityStatusChangeEvent (
                               entity_name,
                               entity_status,
                               entity_info);
}


} // End namespace PlatformMngt
} // End namespace Cdmw

