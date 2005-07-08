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


package cdmw.tools;

import com.thalesgroup.CdmwPlatformMngtEntity.EntityStatus;
import com.thalesgroup.CdmwPlatformMngtEntity.EntityNotFound;

/**
 * Provides an implementation of the EntityObserver interface
 *
 * Features:
 * Thread safe
 *
 */
public class SimEntityObserverImpl 
    extends com.thalesgroup.CdmwPlatformMngtEntity.EntityObserverPOA {

    private static final String ENTITY_FUNCTIONING_STRING
        = "The entity is functioning correctly";
    private static final String ENTITY_DYSFUNCTION_STRING
        = "The entity has an applicative dysfunction";
    private static final String ENTITY_FAILED_NO_RESPONSE_STRING
        = "The entity doesn't respond";
    private static final String ENTITY_FAILED_DEATH_STRING
        = "The entity is technically dead";

    /**
     * The current POA
     */
    private org.omg.PortableServer.POA poa;
    
    /**
     * The logger object
     */
    private com.thalesgroup.CdmwSimulatedRepository.MessageLogger logger;

    /**
     * Constructor
     * 
     * @param parent The parent POA to use
     * @param logger The logger object to use
     */ 
    public SimEntityObserverImpl(org.omg.PortableServer.POA parent, 
        com.thalesgroup.CdmwSimulatedRepository.MessageLogger logger) {
        this.poa = parent;
        this.logger = logger;
    }
    
    /**
     * Returns the default POA
     *
     * @return The default POA.
     */
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngtEntity/SystemEntityObserver/set_system_entity_status:1.0
    //
    /**
     * Sets the status of the specified system entity
     *
     * @param entityName Name of the entity
     * @param entityStatus Status of the entity
     * @param entityInfo Information about the entity
     */
    public void set_system_entity_status(String entityName, 
        EntityStatus entityStatus, String entityInfo) throws EntityNotFound {
        
        String header = "EntityObserver::set_system_entity_status";
        String body = getStatusMessage(entityName,entityStatus,entityInfo);
        logger.log(header,body);
        
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityObserver/set_application_entity_status:1.0
    //
    /**
     * Sets the status of the specified application entity
     *
     * @param entityName Name of the entity
     * @param entityStatus Status of the entity
     * @param entityInfo Information about the entity
     */
    public void set_application_entity_status(String entityName, 
        EntityStatus entityStatus, String entityInfo) throws EntityNotFound {
        
        String header = "EntityObserver::set_application_entity_status";
        String body = getStatusMessage(entityName,entityStatus,entityInfo);
        logger.log(header,body);
        
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngtEntity/EntityObserver/set_process_entity_status:1.0
    //
    /**
     * Sets the status of the specified process
     *
     * @param entityName Name of the entity
     * @param entityStatus Status of the entity
     * @param entityInfo Information about the entity
     */
    public void set_process_entity_status(String entityName, 
        EntityStatus entityStatus, String entityInfo) throws EntityNotFound {
        
        String header = "EntityObserver::set_process_entity_status";
        String body = getStatusMessage(entityName,entityStatus,entityInfo);
        logger.log(header,body);
        
    }
    
    /**
     * Build the status message.
     *
     * @param entityName Name of the entity
     * @param entityStatus Status of the entity
     * @param entityInfo Information about the entity
     */
    private static String getStatusMessage(String entityName,
        EntityStatus entityStatus, String entityInfo) {
        
        String body = "(" + entityName + ",";

        switch (entityStatus.value()) {
        case EntityStatus._ENTITY_FUNCTIONING:
            body += ENTITY_FUNCTIONING_STRING;
            break;
        case EntityStatus._ENTITY_DYSFUNCTION:
            body += ENTITY_DYSFUNCTION_STRING;
            break;
        case EntityStatus._ENTITY_FAILED_NO_RESPONSE:
            body += ENTITY_FAILED_NO_RESPONSE_STRING;
            break;
        case EntityStatus._ENTITY_FAILED_DEATH:
            body += ENTITY_FAILED_DEATH_STRING;
            break;
        }
        body += "," + entityInfo + ")";
        return body;
    }

}

