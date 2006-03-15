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


package cdmw.platformmngt;

import cdmw.common.Assert;
import cdmw.common.Options;

import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;

import cdmw.ossupport.OS;

import com.thalesgroup.CdmwPlatformMngtEntity.EntityStatus;

/**
 * The interface to the Platform Management service.
 *
 */
public class PlatformInterface {

    /**
     * Initial status of the interface
     */
    private static final int INITIAL = 0;

    /**
     * Status of the interface after setup
     */
    private static final int SETUP_PERFORMED = 1;

    /**
     * Status of the interface after acknowledgement
     */
    private static final int ACKNOWLEDGEMENT_PERFORMED = 2;

    /**
     * The status of the Platform Management interface.
     */
    private static int status = INITIAL;

    /**
     * The callback provided by the Platform Management service used
     * for the acknowledgement of the process creation.
     */
    private static com.thalesgroup.CdmwPlatformMngt.ProcessCallback
        processCallback;

    /**
     * The name of the application owning the process.
     */
    private static String applicationName;

    /**
     * The name of the process.
     */
    private static String processName;

    /**
     * The message broker that forwards messages and fatal error
     * to the Platform Management service.
     */
    private static com.thalesgroup.CdmwPlatformMngt.ProcessMessageBroker
        processMessageBroker;

    /**
     * The service broker that retrieves the defined services for the process.
     */
    private static com.thalesgroup.CdmwPlatformMngtService.ServiceBroker
        serviceBroker;

    /**
     * The entity observer that forwards the status changes of entities.
     */
    private static com.thalesgroup.CdmwPlatformMngtEntity.EntityObserver
        entityObserver;

    /**
     * The initialised ORB
     */
    private static org.omg.CORBA.ORB initialisedOrb;

    /**
     * Sets the interface to the Platform Management service up.
     *
     * @param orb the initialised ORB.
     * @param args arguments of the command line.
     * @exception BadOrderException if the setup has already been performed.
     * @exception BadParameterException if orb is NIL or the command line
     * doesn't contain the stringified ProcessCallback reference.
     */
    public static synchronized void setup(org.omg.CORBA.ORB orb, String[] args)
        throws BadOrderException, BadParameterException {

        initialisedOrb = orb;
        checkStatus(INITIAL);

        if (orb==null) {
            throw new BadParameterException("orb","null");
        }

        String callbackIorOption =
            OS.getOptionValue(args, Options.CALLBACK_IOR_OPTION);

        if (callbackIorOption.equals("no")
            || callbackIorOption.equals("yes")) {
            throw new BadParameterException("args",
                "doesn't contain the callback IOR");
        } else {
            try {
                // gets the acknowledgement callback
                org.omg.CORBA.Object callbackObj
                    = orb.string_to_object(callbackIorOption);

                com.thalesgroup.CdmwPlatformMngt.ProcessCallback callback =
                    com.thalesgroup.CdmwPlatformMngt.ProcessCallbackHelper.narrow(callbackObj);

                if (callback==null) {
                    throw new BadParameterException("args",
                        "doesn't contain a valid callback IOR");
                }

                processCallback = callback;

            } catch(org.omg.CORBA.SystemException se) {
                throw new BadParameterException("args",
                    "contains an invalid IOR");
            }

        }

        status = SETUP_PERFORMED;
    }

    /**
     * Checks the required status.
     *
     * @param requiredStatus the required interface status.
     * @exception BadOrderException if the status of the interface
     * is not the required status.
     */
    private static synchronized void checkStatus(int requiredStatus)
        throws BadOrderException {

        if (status != requiredStatus) {
            throw new BadOrderException();
        }
    }

    /**
     * Returns true if the process has been launched by the platform management.
     * In this case, the command line contains a stringified ProcessCallback
     * reference that is used for the creation acknowledgement.
     *
     * @param args Parameters of the command line.
     * @return True if the process has been launched by the platform management
     * false otherwise.
     */
    public static boolean isLaunchedByPlatformManagement(String[] args) {
        boolean ret = true;

        String callbackIorOption =
            OS.getOptionValue(args, Options.CALLBACK_IOR_OPTION);

        if (callbackIorOption.equals("no")
            || callbackIorOption.equals("yes")) {
            ret = false;
        }

        return ret;
    }

    /**
     * Acknowledges the creation of the user supplied managed process.
     *
     * @param process the process.
     * @exception BadOrderException if the setup has not been performed.
     * @exception BadParameterException if process is NIL.
     */
    public static synchronized void acknowledgeCreation(
        com.thalesgroup.CdmwPlatformMngt.Process process)
        throws BadOrderException, BadParameterException {

        checkStatus(SETUP_PERFORMED);

        if (process==null) {
            throw new BadParameterException("process","null");
        }

        com.thalesgroup.CdmwPlatformMngt.ProcessCallbackPackage.ProcessAck ackData =
            processCallback.set_ready(process);

        applicationName = ackData.application_name;

        processName = ackData.process_name;

        Assert.check(ackData.process_message_broker!=null);
        processMessageBroker = ackData.process_message_broker;

        Assert.check(ackData.service_broker!=null);
        serviceBroker = ackData.service_broker;

        Assert.check(ackData.entity_observer!=null);
        entityObserver = ackData.entity_observer;

        status = ACKNOWLEDGEMENT_PERFORMED;

    }

    /**
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
    public static void acknowledgeCreation(ProcessBehaviour behaviour)
        throws BadOrderException, BadParameterException {

        checkStatus(SETUP_PERFORMED);

        if (behaviour == null) {
            throw new BadParameterException("behaviour","null");
        }

        // creates the default managed process servant
        ProcessImpl processImpl = new ProcessImpl(behaviour);

        // activates the servant and creates the CORBA object
        com.thalesgroup.CdmwPlatformMngt.Process process
            = processImpl._this(initialisedOrb);

        acknowledgeCreation(process);

    }

    /**
     * Checks whether the issuer is valid.
     *
     * @param issuer the issuer.
     * @exception BadParameterException if issuer is NULL or is PFMMNG.
     */
    private static void checkIssuer(String issuer)
        throws BadParameterException {
        if (issuer == null) {
            throw new BadParameterException("issuer", "null");
        }

        if (com.thalesgroup.CdmwPlatformMngtBase.PFMMNG.value.equals(issuer)) {
            throw new BadParameterException("issuer", "PFMMNG");
        }
    }

    /**
     * Notifies an applicative message.
     *
     * @param level the level of the message.
     * @param issuer the issuer of the message.
     * @param message the message to notify.
     * @exception BadOrderException if the acknowledgement
     * has not been performed.
     * @exception BadParameterException if level is CdmwPlatformMngtBase.FTL,
     * or issuer is NULL or is CdmwPlatformMngtBase.PFMMNG,
     * or message is NULL.
     */
    public static void notifyMessage(short level,
        String issuer, String message)
        throws BadOrderException, BadParameterException {

        checkStatus(ACKNOWLEDGEMENT_PERFORMED);

        if (level == com.thalesgroup.CdmwPlatformMngtBase.FTL.value) {
            throw new BadParameterException("level","FTL");
        }

        checkIssuer(issuer);

        if (message == null) {
            throw new BadParameterException("message","null");
        }

        com.thalesgroup.CdmwPlatformMngtBase.EventHeader header =
            createHeader(level);

        processMessageBroker.notify_message(header, issuer, message);

    }

    /**
     * Notifies a fatal error.
     * If the acknowledgement has not been performed yet, this method indicates
     * that the process cannot be actually created.
     * The process will be killed by the Platform Management service.
     *
     * @param issuer the issuer of the error.
     * @param errorInfo the information about the error.
     * @exception BadOrderException if the setup has not been performed.
     * @exception BadParameterException if issuer is NULL or is
     * CdmwPlatformMngtBase.PFMMNG, or errorInfo is NULL.
     */
    public static synchronized void
        notifyFatalError(String issuer, String errorInfo)
        throws BadOrderException, BadParameterException {

        if (status == INITIAL)
            throw new BadOrderException();

        checkIssuer(issuer);

        if (errorInfo == null) {
            throw new BadParameterException("errorInfo","null");
        }

        if (status == SETUP_PERFORMED) {
            // the acknowledgement has not been performed yet
            processCallback.notify_error(issuer, errorInfo);
        } else {
            // the acknowledgement has already been performed
            com.thalesgroup.CdmwPlatformMngtBase.TimeStamp timeStamp =
                createTimeStamp();

            processMessageBroker.notify_error(timeStamp, issuer, errorInfo);
        }

    }

    /**
     * Returns the name of the application owning the process.
     *
     * @exception BadOrderException if the acknowledgement
     * has not been performed.
     */
    public static String getApplicationName()
        throws BadOrderException {
        checkStatus(ACKNOWLEDGEMENT_PERFORMED);
        return applicationName;
    }


    /**
     * Returns the process name.
     *
     * @exception BadOrderException if the acknowledgement
     * has not been performed.
     */
    public static String getProcessName()
        throws BadOrderException {
        checkStatus(ACKNOWLEDGEMENT_PERFORMED);
        return processName;
    }


    /**
     * Returns the specified service.
     *
     * @param serviceName the name of the service.
     * @exception com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound
     * if serviceName doesn't denote a service that has been defined
     * for the process.
     * @exception com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable
     * if the specified service is unavailable.
     * @exception BadOrderException if the acknowledgement has not been performed.
     * @exception BadParameterException if serviceName is NULL.
     */
    public static org.omg.CORBA.Object getService(String serviceName)
        throws com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound,
              com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable,
              BadOrderException, BadParameterException {
        checkStatus(ACKNOWLEDGEMENT_PERFORMED);
        if (serviceName == null) {
            throw new BadParameterException("serviceName","null");
        }
        return serviceBroker.get_service(serviceName);
    }

    /**
     * Checks whether the parameter are valids and whether the
     * acknowledgement has been performed.
     *
     * @param entityName the name of the entity.
     * @param entityInfo the additional information.
     * @exception BadOrderException if the acknowledgement has not been performed.
     * @exception BadParameterException if entityName or entityInfo is NULL.
     */
    private static void checkEntityStatusParameters(String entityName,
        String entityInfo)
        throws BadOrderException, BadParameterException {

        checkStatus(ACKNOWLEDGEMENT_PERFORMED);

        if (entityName == null) {
            throw new BadParameterException("entityName","null");
        }

        if (entityInfo == null) {
            throw new BadParameterException("entityInfo","null");
        }

    }

    /**
     * Sets the status of the specified system entity.
     *
     * @param entityName the name of the entity.
     * @param entityStatus the new status of the entity.
     * @param entityInfo the additional information.
     * @exception com.thalesgroup.CdmwPlatformMngtEntity.EntityNotFound
     * if entityName doesn't denote an existing entity.
     * @exception BadOrderException if the acknowledgement
     * has not been performed.
     * @exception BadParameterException if entityName or entityInfo is NULL.
     */
    public static void setSystemEntityStatus(String entityName,
        EntityStatus entityStatus, String entityInfo)
        throws com.thalesgroup.CdmwPlatformMngtEntity.EntityNotFound,
               BadOrderException, BadParameterException {

        checkEntityStatusParameters(entityName, entityInfo);
        entityObserver.set_system_entity_status(
            entityName, entityStatus, entityInfo);
    }


    /**
     * Sets the status of the specified application entity.
     *
     * @param entityName the name of the entity.
     * @param entityStatus the new status of the entity.
     * @param entityInfo the additional information.
     * @exception com.thalesgroup.CdmwPlatformMngtEntity.EntityNotFound
     * if entityName doesn't denote an existing entity.
     * @exception BadOrderException if the acknowledgement has not been performed.
     * @exception BadParameterException if entityName or entityInfo is NULL.
     */
    public static void setApplicationEntityStatus(String entityName,
        EntityStatus entityStatus, String entityInfo)
        throws com.thalesgroup.CdmwPlatformMngtEntity.EntityNotFound,
               BadOrderException, BadParameterException {

        checkEntityStatusParameters(entityName, entityInfo);

        entityObserver.set_application_entity_status(
            entityName, entityStatus, entityInfo);

    }


    /**
     * Sets the status of the specified process entity.
     *
     * @param entityName the name of the entity.
     * @param entityStatus the new status of the entity.
     * @param entityInfo the additional information.
     * @exception com.thalesgroup.CdmwPlatformMngtEntity.EntityNotFound
     * if entityName doesn't denote an existing entity.
     * @exception BadOrderException if the acknowledgement has not been performed.
     * @exception BadParameterException if entityName or entityInfo is NULL.
     */
    public static void setProcessEntityStatus(String entityName,
        EntityStatus entityStatus, String entityInfo)
        throws com.thalesgroup.CdmwPlatformMngtEntity.EntityNotFound,
               BadOrderException, BadParameterException {

        checkEntityStatusParameters(entityName, entityInfo);

        entityObserver.set_process_entity_status(
            entityName, entityStatus, entityInfo);

    }

    /**
     * Returns the header embedding the specified level and the timestamp
     * of the operation call.
     *
     * @param level the level of the message.
     * @return Header of the operation call.
     */
    private static com.thalesgroup.CdmwPlatformMngtBase.EventHeader
        createHeader(short level) {

        com.thalesgroup.CdmwPlatformMngtBase.EventHeader eventHeader
            = new com.thalesgroup.CdmwPlatformMngtBase.EventHeader();

        eventHeader.time_stamp = createTimeStamp();
        eventHeader.level = level;

        return eventHeader;

    }

    /**
     * Returns the timestamp of the operation call.
     *
     * @return Timestamp of the operation call
     */
    private static com.thalesgroup.CdmwPlatformMngtBase.TimeStamp
        createTimeStamp() {

        com.thalesgroup.CdmwPlatformMngtBase.TimeStamp timeStamp =
            new com.thalesgroup.CdmwPlatformMngtBase.TimeStamp();

        java.util.Calendar now = java.util.Calendar.getInstance();

        timeStamp.year =  (short) now.get(java.util.Calendar.YEAR);
        // month value starts oddly from 0 for january
        timeStamp.month = (short) (now.get(java.util.Calendar.MONTH)+1);
        timeStamp.day =   (short) now.get(java.util.Calendar.DAY_OF_MONTH);

        timeStamp.hour = (short) now.get(java.util.Calendar.HOUR_OF_DAY);
        timeStamp.minute = (short) now.get(java.util.Calendar.MINUTE);
        timeStamp.second = (short) now.get(java.util.Calendar.SECOND);

        return timeStamp;

    }

}
