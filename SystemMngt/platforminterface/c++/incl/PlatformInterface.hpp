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


#ifndef INCL_PLATFORMMNGT_PLATFORMINTERFACE_HPP 
#define INCL_PLATFORMMNGT_PLATFORMINTERFACE_HPP 

#include <string>

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"

#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
* Purpose:
* <p>
* The interface to the Platform Management service.
*/
class PlatformInterface
{

public:

    /**
    * Purpose:
    * <p>
    * Returns true if the process has been launched by the platform management.
    * In this case, the command line contains a stringified ProcessCallback
    * reference that is used for the creation acknowledgement.
    */
    static bool isLaunchedByPlatformManagement(int argc, char* argv[]);


    /**
    * Purpose:
    * <p>
    * Sets the interface to the Platform Management service up.
    *
    * @param orb the initialised ORB.
    * @param argc the number of arguments of the command line.
    * @param argv the array of arguments of the command line.
    * @exception BadOrderException if the setup has already been performed.
    * @exception BadParameterException if orb is NIL or the command line doesn't
    * contain the stringified ProcessCallback reference.
    */
    static void setup(CORBA::ORB_ptr orb, int argc, char* argv[])
        throw (BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Acknowledges the creation of the user supplied managed process.
    *
    * @param process the process.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if process is NIL.
    */
    static void acknowledgeCreation(CdmwPlatformMngt::Process_ptr process)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Creates the default implementation of a managed process and
    * acknowledges the latter.
    * If successful, the method takes ownership of the process behaviour
    * which will be deallocated when the default implementation
    * will be deallocated itself.
    *
    * @param behaviour the actual behaviour of the managed process.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if behaviour is NULL.
    */
    static void acknowledgeCreation(ProcessBehaviour *behaviour)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Notifies an applicative message.
    *
    * @param level the level of the message.
    * @param issuer the issuer of the message.
    * @param message the message to notify.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if level is CdmwPlatformMngtBase::FTL,
    * or issuer is NULL or is CdmwPlatformMngtBase::PFMMNG,
    * or message is NULL.
    */
    static void notifyMessage(CdmwPlatformMngtBase::EventLevel level,
            const char* issuer, const char* message)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Notifies a fatal error.
    * If the acknowledgement has not been performed yet, this method indicates
    * that the process cannot be actually created.
    * The process will be killed by the Platform Management service.
    *
    * @param issuer the issuer of the error.
    * @param errorInfo the information about the error.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the setup has not been performed.
    * @exception BadParameterException if issuer is NULL or is
    * CdmwPlatformMngtBase::PFMMNG, or errorInfo is NULL.
    */
    static void notifyFatalError(const char* issuer, const char* errorInfo)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Notifies an applicative user event.
    *
    * @param issuer the issuer of the event.
    * @param data the data to notify.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if level is CdmwPlatformMngtBase::FTL,
    * or issuer is NULL or is CdmwPlatformMngtBase::PFMMNG,
    * or message is NULL.
    */
    static void notifyUserEvent(const char* issuer,
            const CORBA::Any& data)
        throw (CORBA::SystemException, BadOrderException, BadParameterException);


/**
    * Purpose:
    * <p>
    * Returns the name of the application owning the process.
    *
    * @exception BadOrderException if the acknowledgement has not been performed.
    */
    static const char* getApplicationName()
        throw (BadOrderException);


    /**
    * Purpose:
    * <p>
    * Returns the process name.
    *
    * @exception BadOrderException if the acknowledgement has not been performed.
    */
    static const char* getProcessName()
        throw (BadOrderException);


    /**
    * Purpose:
    * <p>
    * Returns the specified service.
    *
    * @param serviceName the name of the service.
    * @exception CdmwPlatformMngtService::ServiceNotFound if serviceName
    * doesn't denote a service that has been defined for the process.
    * @exception CdmwPlatformMngtService::ServiceUnavailable if the specified
    * service is unavailable.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if serviceName is NULL.
    */
    static CORBA::Object_ptr getService(const char* serviceName)
        throw (CdmwPlatformMngtService::ServiceNotFound,
               CdmwPlatformMngtService::ServiceUnavailable,
               CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Sets the status of the specified system entity.
    *
    * @param entityName the name of the entity.
    * @param entityStatus the new status of the entity.
    * @param entityInfo the additional information.
    * @exception CdmwPlatformMngtEntity::EntityNotFound if entityName
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entityName or entityInfo is NULL.
    */
    static void setSystemEntityStatus(const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* entityInfo)
        throw (CdmwPlatformMngtEntity::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Sets the status of the specified application entity.
    *
    * @param entityName the name of the entity.
    * @param entityStatus the new status of the entity.
    * @param entityInfo the additional information.
    * @exception CdmwPlatformMngtEntity::EntityNotFound if entityName
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entityName or entityInfo is NULL.
    */
    static void setApplicationEntityStatus(const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* entityInfo)
        throw (CdmwPlatformMngtEntity::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);


    /**
    * Purpose:
    * <p>
    * Sets the status of the specified process entity.
    *
    * @param entityName the name of the entity.
    * @param entityStatus the new status of the entity.
    * @param entityInfo the additional information.
    * @exception CdmwPlatformMngtEntity::EntityNotFound if entityName
    * doesn't denote an existing entity.
    * @exception CORBA::SystemException the CORBA system exception raised
    * by the operation call.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entityName or entityInfo is NULL.
    */
    static void setProcessEntityStatus(const char* entityName,
            CdmwPlatformMngtEntity::EntityStatus entityStatus,
            const char* entityInfo)
        throw (CdmwPlatformMngtEntity::EntityNotFound,
               CORBA::SystemException, BadOrderException, BadParameterException);

private:

    enum InterfaceStatus
    {
        INITIAL,
        SETUP_PERFORMED,
        ACKNOWLEDGEMENT_PERFORMED
    };

    /**
    * The status of the Platform Management interface.
    */
    static InterfaceStatus M_status;
    
    /**
    * This mutex protects the interface status
    */
    static OsSupport::Mutex M_status_mtx;

    /**
    * The callback provided by the Platform Management service used
    * for the acknowledgement of the process creation.
    */
    static CdmwPlatformMngt::ProcessCallback_var M_processCallback;

    /**
    * The name of the application owning the process.
    */
    static std::string M_applicationName;

    /**
    * The name of the process.
    */
    static std::string M_processName;

    /**
    * The message broker that forwards messages and fatal error to the Platform
    * Management service.
    */
    static CdmwPlatformMngt::ProcessMessageBroker_var M_processMessageBroker;

    /**
    * The service broker that retrieves the defined services for the process.
    */
    static CdmwPlatformMngtService::ServiceBroker_var M_serviceBroker;

    /**
    * The entity observer that forwards the status changes of entities.
    */
    static CdmwPlatformMngtEntity::EntityObserver_var M_entityObserver;

    /**
    * Purpose:
    * <p>
    * Checks the required status.
    *
    * @param requiredStatus the required interface status.
    * @exception BadOrderException if the status of the interface
    * is not the required status.
    */
    static void checkStatus(InterfaceStatus requiredStatus)
        throw (BadOrderException);

    /**
    * Purpose:
    * <p>
    * Checks whether the issuer is valid.
    *
    * @param issuer the issuer.
    * @exception BadParameterException if issuer is NULL or is PFMMNG.
    */
    static void checkIssuer(const char* issuer) throw (BadParameterException);

    /**
    * Purpose:
    * <p>
    * Checks whether the parameter are valids and whether the
    * acknowledgement has been performed.
    *
    * @param entityName the name of the entity.
    * @param entityInfo the additional information.
    * @exception BadOrderException if the acknowledgement has not been performed.
    * @exception BadParameterException if entityName or entityInfo is NULL.
    */
    static void checkEntityStatusParameters(const char* entityName,
            const char* entityInfo)
        throw (BadOrderException, BadParameterException);

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PLATFORMINTERFACE_HPP

