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


#include <cstring>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

#include "platforminterface/Process_impl.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

PlatformInterface::InterfaceStatus PlatformInterface::M_status = PlatformInterface::INITIAL;

OsSupport::Mutex PlatformInterface::M_status_mtx;

CdmwPlatformMngt::ProcessCallback_var PlatformInterface::M_processCallback;

std::string PlatformInterface::M_applicationName;
std::string PlatformInterface::M_processName;

CdmwPlatformMngt::ProcessMessageBroker_var PlatformInterface::M_processMessageBroker;

CdmwPlatformMngtService::ServiceBroker_var PlatformInterface::M_serviceBroker;

CdmwPlatformMngtEntity::EntityObserver_var PlatformInterface::M_entityObserver;


void PlatformInterface::checkStatus(InterfaceStatus requiredStatus)
    throw (BadOrderException)
{
    CDMW_MUTEX_GUARD(M_status_mtx);
    
    if (M_status != requiredStatus)
        CDMW_THROW(BadOrderException);
}


bool PlatformInterface::isLaunchedByPlatformManagement(int argc, char* argv[])
{
    bool ret = true;

    std::string callbackIor_option =
        OsSupport::OS::get_option_value(argc, argv, Common::Options::CALLBACK_IOR_OPTION);

    if (callbackIor_option == "no" || callbackIor_option == "yes")
    {
        ret = false;
    }

    return ret;
}


void PlatformInterface::setup(CORBA::ORB_ptr orb, int argc, char* argv[])
    throw (BadOrderException, BadParameterException)
{

    checkStatus(INITIAL);

    if (CORBA::is_nil(orb))
        CDMW_THROW2(BadParameterException, "orb", "nil");

    std::string callbackIor_option =
        OsSupport::OS::get_option_value(argc, argv, Common::Options::CALLBACK_IOR_OPTION);

    if (callbackIor_option == "no" || callbackIor_option == "yes")
    {
        CDMW_THROW2(BadParameterException, "argv", "doesn't contain the callback IOR");
    }
    else
    {
        try
        {
            // gets the acknowledgement callback
            CORBA::Object_var callback_obj = orb->string_to_object(callbackIor_option.c_str());

            CdmwPlatformMngt::ProcessCallback_var callback = 
                CdmwPlatformMngt::ProcessCallback::_narrow(callback_obj.in());

            if (CORBA::is_nil(callback.in()))
            {
                CDMW_THROW2(BadParameterException, "argv", "doesn't contain a valid callback IOR");
            }

            M_processCallback = callback;

        }
        catch(const CORBA::SystemException&)
        {
            CDMW_THROW2(BadParameterException, "argv", "contains an invalid IOR");
        }

    }

    CDMW_MUTEX_GUARD(M_status_mtx);
    
    M_status = SETUP_PERFORMED;

}


void PlatformInterface::acknowledgeCreation(CdmwPlatformMngt::Process_ptr process)
    throw (CORBA::SystemException, BadOrderException, BadParameterException)
{

    checkStatus(SETUP_PERFORMED);

    if (CORBA::is_nil(process))
        CDMW_THROW2(BadParameterException, "process", "nil");
        
        
    CDMW_MUTEX_GUARD(M_status_mtx);

    CdmwPlatformMngt::ProcessCallback::ProcessAck_var ackData =
        M_processCallback->set_ready(process);

    M_applicationName = ackData->application_name;

    M_processName = ackData->process_name;

    CDMW_ASSERT(!CORBA::is_nil((ackData->process_message_broker).in()));
    M_processMessageBroker =
        CdmwPlatformMngt::ProcessMessageBroker::_duplicate((ackData->process_message_broker).in());

    CDMW_ASSERT(!CORBA::is_nil((ackData->service_broker).in()));
    M_serviceBroker =
        CdmwPlatformMngtService::ServiceBroker::_duplicate((ackData->service_broker).in());

    CDMW_ASSERT(!CORBA::is_nil((ackData->entity_observer).in()));
    M_entityObserver =
        CdmwPlatformMngtEntity::EntityObserver::_duplicate((ackData->entity_observer).in());

    M_status = ACKNOWLEDGEMENT_PERFORMED;

}


void PlatformInterface::acknowledgeCreation(ProcessBehaviour *behaviour)
    throw (CORBA::SystemException, BadOrderException, BadParameterException)
{

    checkStatus(SETUP_PERFORMED);

    if (behaviour == NULL)
        CDMW_THROW2(BadParameterException, "behaviour", "NULL");

    // creates the default managed process servant
    PortableServer::ServantBase_var process_servant
        = new Process_impl(behaviour);

    Process_impl *process_impl =
        dynamic_cast<Process_impl*>(process_servant.in());

    // activates the servant and creates the CORBA object
    CdmwPlatformMngt::Process_var process = process_impl->_this();

    acknowledgeCreation(process.in());

}


void PlatformInterface::checkIssuer(const char* issuer) throw (BadParameterException)
{

    if (issuer == NULL)
        CDMW_THROW2(BadParameterException, "issuer", "NULL");

    size_t len = ::strlen(CdmwPlatformMngtBase::PFMMNG);

    if (::strncmp(CdmwPlatformMngtBase::PFMMNG, issuer, len) == 0)
        CDMW_THROW2(BadParameterException, "issuer", "PFMMNG");

}


void PlatformInterface::notifyMessage(CdmwPlatformMngtBase::EventLevel level,
        const char* issuer, const char* message)
    throw (CORBA::SystemException, BadOrderException, BadParameterException)
{
    checkStatus(ACKNOWLEDGEMENT_PERFORMED);

    if (level == CdmwPlatformMngtBase::FTL)
        CDMW_THROW2(BadParameterException, "level", "FTL");

    checkIssuer(issuer);

    if (message == NULL)
        CDMW_THROW2(BadParameterException, "message", "NULL");

    CdmwPlatformMngtBase::EventHeader header =
            Cdmw::PlatformMngt::EventHeaderFactory::createHeader(level);

    M_processMessageBroker->notify_message(header, issuer, message);

}



void PlatformInterface::notifyFatalError(const char* issuer,
        const char* errorInfo)
    throw (CORBA::SystemException, BadOrderException, BadParameterException)
{
    CDMW_MUTEX_GUARD(M_status_mtx);

    if (M_status == INITIAL)
        CDMW_THROW(BadOrderException);

    checkIssuer(issuer);

    if (errorInfo == NULL)
        CDMW_THROW2(BadParameterException, "errorInfo", "NULL");

    if (M_status == SETUP_PERFORMED)
    {
        // the acknowledgement has not been performed yet
        M_processCallback->notify_error(issuer, errorInfo);
    }
    else
    {
        // the acknowledgement has already been performed

        CdmwPlatformMngtBase::TimeStamp timeStamp =
                Cdmw::PlatformMngt::EventHeaderFactory::createTimeStamp();

        M_processMessageBroker->notify_error(timeStamp, issuer, errorInfo);

    }

}


void PlatformInterface::notifyUserEvent(const char* issuer,
        const CORBA::Any& data)
    throw (CORBA::SystemException, BadOrderException, BadParameterException)
{
    checkStatus(ACKNOWLEDGEMENT_PERFORMED);

    M_processMessageBroker->notify_user_event(issuer, data);

}



const char* PlatformInterface::getApplicationName()
    throw (BadOrderException)
{
    checkStatus(ACKNOWLEDGEMENT_PERFORMED);

    return M_applicationName.c_str();
}


const char* PlatformInterface::getProcessName()
    throw (BadOrderException)
{
    checkStatus(ACKNOWLEDGEMENT_PERFORMED);

    return M_processName.c_str();
}


CORBA::Object_ptr PlatformInterface::getService(const char* serviceName)
    throw (CdmwPlatformMngtService::ServiceNotFound,
           CdmwPlatformMngtService::ServiceUnavailable,
           CORBA::SystemException, BadOrderException, BadParameterException)
{
    checkStatus(ACKNOWLEDGEMENT_PERFORMED);

    if (serviceName == NULL)
        CDMW_THROW2(BadParameterException, "serviceName", "NULL");

    return M_serviceBroker->get_service(serviceName);

}


void PlatformInterface::checkEntityStatusParameters(const char* entityName,
        const char* entityInfo)
    throw (BadOrderException, BadParameterException)
{

    checkStatus(ACKNOWLEDGEMENT_PERFORMED);

    if (entityName == NULL)
        CDMW_THROW2(BadParameterException, "entityName", "NULL");

    if (entityInfo == NULL)
        CDMW_THROW2(BadParameterException, "entityInfo", "NULL");

}


void PlatformInterface::setSystemEntityStatus(const char* entityName,
        CdmwPlatformMngtEntity::EntityStatus entityStatus,
        const char* entityInfo)
    throw (CdmwPlatformMngtEntity::EntityNotFound,
           CORBA::SystemException, BadOrderException, BadParameterException)
{

    checkEntityStatusParameters(entityName, entityInfo);

    M_entityObserver->set_system_entity_status(
        entityName, entityStatus, entityInfo);

}


void PlatformInterface::setApplicationEntityStatus(const char* entityName,
        CdmwPlatformMngtEntity::EntityStatus entityStatus,
        const char* entityInfo)
    throw (CdmwPlatformMngtEntity::EntityNotFound,
           CORBA::SystemException, BadOrderException, BadParameterException)
{

    checkEntityStatusParameters(entityName, entityInfo);

    M_entityObserver->set_application_entity_status(
        entityName, entityStatus, entityInfo);

}


void PlatformInterface::setProcessEntityStatus(const char* entityName,
        CdmwPlatformMngtEntity::EntityStatus entityStatus,
        const char* entityInfo)
    throw (CdmwPlatformMngtEntity::EntityNotFound,
           CORBA::SystemException, BadOrderException, BadParameterException)
{

    checkEntityStatusParameters(entityName, entityInfo);

    M_entityObserver->set_process_entity_status(
        entityName, entityStatus, entityInfo);

}


} // End namespace PlatformMngt
} // End namespace Cdmw


