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


package cdmw.platformmngt.test;
import org.omg.CORBA.StringHolder;

import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;

import cdmw.platformmngt.PlatformInterface;

import com.thalesgroup.CdmwPlatformMngt.Entity;
import com.thalesgroup.CdmwPlatformMngtBase.ERR;

/**
 * SystemEntityToUpdate.
 */
public class SystemEntityToUpdate extends EntityToUpdate {

    /**
     * Constructor.
     */ 
    public SystemEntityToUpdate(String entityName,
                                String hostName,
                                String applicationName,
                                String processName) {
        super(entityName, hostName, applicationName, processName);
    }

    /**
     * Executes the updating.
     */
    public void update() throws BadParameterException, BadOrderException {

        if (entityName.length() != 0 && !errorSet) {
            String infoMsg = "";

            try {
                updatingCounter++;
                if (updatingCounter == ENTITY_UPDATE_MAX_COUNTER) {
                    PlatformInterface.notifyFatalError(processName, "Fatal Error");
                } 
                else if (updatingCounter == 1) {
                    PlatformInterface.setSystemEntityStatus(entityName,
                                                          "functioning",
                                                          "the system entity status is : functioning");

                    StringHolder info = new StringHolder();
                    String status;   
                    status = PlatformInterface.getSystemEntityStatus(entityName,
                                                                   info);


                }
                else if (updatingCounter < ENTITY_UPDATE_MAX_COUNTER) {

                    StringHolder info = new StringHolder();
                    String status;   
                    status = PlatformInterface.getSystemEntityStatus(entityName,
                                                                   info);
                    
                    if (status.equals("disfunction"))
                        {
                            PlatformInterface.setSystemEntityStatus(entityName,
                                                                  "functioning",
                                                                  "the system entity status is : functioning");
                        } else {
                        PlatformInterface.setSystemEntityStatus(entityName,
                                                              "disfunction",
                                                              "the system entity status is : disfunction");;                      
                    }
                }




            } catch (com.thalesgroup.CdmwPlatformMngt.EntityNotFound enf) {
                errorSet = true;
                infoMsg = "Entity not found : " + entityName;
            } catch (org.omg.CORBA.SystemException se) {
                errorSet = true;
                infoMsg = "setSystemEntityStatus system exception : "
                    + entityName + "\n" + se.toString();            
            } catch (cdmw.common.BadOrderException boe) {
                errorSet = true;
                infoMsg = "setSystemEntityStatus bad order exception";
            } catch (cdmw.common.BadParameterException bpe) {
                errorSet = true;
                infoMsg = "setSystemEntityStatus bad parameter exception";
            }

            if (errorSet) {
                PlatformInterface.notifyMessage(ERR.value, processName, infoMsg);
            }
            
        }
        
    }

}

