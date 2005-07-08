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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/RWEntityContainer_impl.hpp"
#include "SystemMngt/platformlibrary/SupervisionEventNotifier.hpp"
#include "SystemMngt/platformapplicationlibrary/Application_impl.hpp"
#include "platformapplicationlibrary/EntityObserver_impl.hpp"
#include "platformapplicationlibrary/ManagedProcessProxy_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

EntityObserver_impl::EntityObserver_impl(
    Application_impl* application )
throw( BadParameterException )
: MultipleIdServant_impl(
    Configuration::M_orb.in(),
    (std::string( "EntityObserverPOA|" ) +
        application->m_applicationAck->application_name.in()).c_str() )
{
    if( application == NULL )
        CDMW_THROW2(
            BadParameterException,
            "application",
            "NULL" );
        
    m_application = application;
}

void EntityObserver_impl::set_system_entity_status(
    const char* entity_name,
    CdmwPlatformMngtEntity::EntityStatus entity_status,
    const char* entity_info )
throw( CdmwPlatformMngtEntity::EntityNotFound,
       CORBA::SystemException )
{
    m_application->m_systemEntityObserver->set_system_entity_status(
        entity_name,
        entity_status,
        entity_info );
}

void EntityObserver_impl::set_application_entity_status(
    const char* entity_name,
    CdmwPlatformMngtEntity::EntityStatus entity_status,
    const char* entity_info )
throw( CdmwPlatformMngtEntity::EntityNotFound,
       CORBA::SystemException )
{
    m_application->m_rwEntityContainer->set_entity_status(
        entity_name,
        entity_status,
        entity_info );

    // Log and notify the change of status
    m_application->notifyEntityStatusChangeEvent(
            entity_name,
            entity_status,
            entity_info );
}

void EntityObserver_impl::set_process_entity_status(
    const char* entity_name,
    CdmwPlatformMngtEntity::EntityStatus entity_status,
    const char* entity_info )
throw( CdmwPlatformMngtEntity::EntityNotFound,
       CORBA::SystemException )
{
    try
    {
        ManagedProcessProxy_impl* managedProcessProxy
            = dynamic_cast< ManagedProcessProxy_impl* > (
                m_application->getProcessProxy( getCurrentName().c_str() ) );
        if( managedProcessProxy == NULL )
            throw CORBA::BAD_OPERATION(
                OrbSupport::BAD_OPERATION,
                CORBA::COMPLETED_NO );
            
        managedProcessProxy->m_rwEntityContainer->set_entity_status(
            entity_name,
            entity_status,
            entity_info );
            
        // Log and notify the change of status
        managedProcessProxy->notifyEntityStatusChangeEvent(
                entity_name,
                entity_status,
                entity_info );
    }
    catch( const OutOfMemoryException& )
    {
        throw CORBA::NO_MEMORY( OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO );
    }
    catch( const InternalErrorException& )
    {
        throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );
    }
}

} // End namespace PlatformMngt
} // End namespace Cdmw

