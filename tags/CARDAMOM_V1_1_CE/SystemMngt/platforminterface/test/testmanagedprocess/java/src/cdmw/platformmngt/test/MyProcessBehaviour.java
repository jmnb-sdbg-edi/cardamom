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


package cdmw.platformmngt.test;

import cdmw.testutils.Testable;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;
import cdmw.ossupport.OS;

import cdmw.platformmngt.PlatformInterface;
import cdmw.platformmngt.ProcessBehaviour;

import com.thalesgroup.CdmwPlatformMngtBase.INF;
import com.thalesgroup.CdmwPlatformMngtBase.ERR;

/**
 * The abstract process behaviour.
 *
 */
public class MyProcessBehaviour extends ProcessBehaviour {

    /**
     * The ORB.
     */
    private org.omg.CORBA.ORB orb;

    /**
     * To indicate if supervised by platform management.
     */
    private boolean platformManaged;

    /**
     * The time out for process auto ending after run.
     * -1 if no auto ending
     */
    private long autoEndTimeOut = -1;

    /**
     * The time interval to update entity
     */
    private long entityUpdateInterval = 5000;

    /**
     * The number of init step defined for this process.
     */
    private int nbrOfInitSteps = 1;

    /**
     * The current init step.
     */
    private int currentInitStep = 0;

    /**
     * The application name.
     */
    private String applicationName;

    /**
     * The process name.
     */
    private String processName;

    /**
     * The service name to request.
     */
    private String serviceName = "";

    /**
     * The system entity name to set.
     */
    private String sysEntityName = "";

    /**
     * The application entity name to set.
     */
    private String appEntityName = "";

    /**
     * The process entity name to set.
     */
    private String procEntityName = "";

    /**
     * The system entity to update.
     */
    private UpdateEntity updateSystemEntity = null;

    /**
     * The application entity to update.
     */
    private UpdateEntity updateApplicationEntity = null;

    /**
     * The process entity to update.
     */
    private UpdateEntity updateProcessEntity = null;

    /**
     * The startup mode.
     *     < 0 if initialisation time out in msec
     *     > 0 if running time out in msec
     */
    private short startupMode = 0;

    /**
    * The AutoEnding thread class.
    */
    private AutoEnding autoEnding = null;


    /**
     * The constructor.
     *
     */
    public MyProcessBehaviour(
        org.omg.CORBA.ORB orb,
        boolean platformManaged,
        String processExe,
        AutoEnding autoEnding,
        long autoEndTimeOut,
        long entityUpdateInterval,
        int nbrOfInitSteps) {

        this.orb = orb;
        this.platformManaged = platformManaged;
        this.processName = processExe;
        this.autoEndTimeOut = autoEndTimeOut;
        this.entityUpdateInterval = entityUpdateInterval;
        this.nbrOfInitSteps = nbrOfInitSteps;

        this.autoEnding = autoEnding;
        if (this.autoEnding != null) {
            this.autoEnding.setProcessBehaviour(this);
        }

    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
     * attribute
     */
    public int nbSteps() {
        return nbrOfInitSteps;
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
     * operation
     */
    public org.omg.CORBA.Object getService() {
        // example of using the PlatformInterface for notifying a message
        try {
            PlatformInterface.notifyMessage(INF.value,
                processName, "Service requested.NIL returned");
        } catch(BadParameterException bpe) {
            bpe.printStackTrace();
        } catch(BadOrderException boe) {
            boe.printStackTrace();
        }
        return null;
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {

        try {

            if (platformManaged) {
                // get application and process names
                applicationName = PlatformInterface.getApplicationName();
                processName = PlatformInterface.getProcessName();

                // example of using the PlatformInterface for notifying a message
                PlatformInterface.notifyMessage(INF.value,
                    processName, "Initialisation requested");
            } else {
                System.out.println(processName + ": Initialisation requested");
            }

            // init current step
            currentInitStep = 0;


            // set startup mode
            startupMode = startupKind.startup_mode;

            // if timeout for init phase
            if (startupMode > 0) {
                if (platformManaged) {
                    String msgInfo = "Init Timeout requested :" + startupMode + " msec";
                    PlatformInterface.notifyMessage(INF.value, processName, msgInfo);
                } else {
                    System.out.println(processName + 
                        ": Init Timeout requested: " + startupMode + " msec");
                }

                OS.sleep(Testable.getTimescale() * startupMode);
            }

        } catch(BadParameterException bpe) {
            bpe.printStackTrace();
        } catch(BadOrderException boe) {
            boe.printStackTrace();
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }

    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    public void nextStep()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {
        
        try {
            // increment init step
            if (currentInitStep < nbrOfInitSteps) {
              currentInitStep++;
            }

            String msgInfo = "Step requested: " + currentInitStep;

            if (platformManaged) {
                // example of using the PlatformInterface for notifying a message
                PlatformInterface.notifyMessage(INF.value, processName, msgInfo);
            } else {
                System.out.println(processName + ": " + msgInfo);
            }

            // if invalid init step
            if (currentInitStep >= nbrOfInitSteps) {
                throw new com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep();
            }

        } catch(BadParameterException bpe) {
            bpe.printStackTrace();
        } catch(BadOrderException boe) {
            boe.printStackTrace();
        }

    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public void run()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun {

        try {

            if (platformManaged) {
                // example of using the PlatformInterface for notifying a message
                PlatformInterface.notifyMessage(INF.value, processName, "Run requested");
            } else {
                System.out.println(processName + ": Run requested");
            }


            if (serviceName.length() != 0) {
                boolean errorSet = false;
                String infoMsg = "";

                try {
                    org.omg.CORBA.Object serviceObject
                        = PlatformInterface.getService(serviceName);
                } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound snf) {
                    errorSet = true;
                    infoMsg = "Service not found : " + serviceName;
                } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable su) {
                    errorSet = true;
                    infoMsg = "Service not available : " + serviceName;
                } catch (org.omg.CORBA.SystemException se) {
                    errorSet = true;
                    infoMsg = "getService system exception : "
                        + serviceName + "\n" + se.toString();
                } catch (cdmw.common.BadOrderException boe) {
                    errorSet = true;
                    infoMsg = "getService bad order exception";
                } catch (cdmw.common.BadParameterException bpe) {
                    errorSet = true;
                    infoMsg = "getService bad parameter exception";
                }

                if (errorSet) {
                    PlatformInterface.notifyMessage(ERR.value, processName, infoMsg);
                } else {
                    infoMsg = "Service has been found : " + serviceName;
                    PlatformInterface.notifyMessage(INF.value, processName, infoMsg);
                }
            }

            // creates the EntityToUpdate classes
            SystemEntityToUpdate systemEntityToUpdate
                = new SystemEntityToUpdate(sysEntityName, processName);

            ApplicationEntityToUpdate applicationEntityToUpdate
                = new ApplicationEntityToUpdate(appEntityName, processName);

            ProcessEntityToUpdate processEntityToUpdate
                = new ProcessEntityToUpdate(procEntityName, processName);

            // if system entity to update
            if (sysEntityName.length() != 0) {
                try {
                    // create the UpdateEntity class
                    this.updateSystemEntity
                        = new UpdateEntity(systemEntityToUpdate, entityUpdateInterval);
                    // start thread
                    System.out.println(processName + ": Starting SystemEntity thread...");
                    this.updateSystemEntity.start();
                } catch (Exception e) {
                    PlatformInterface.notifyMessage(ERR.value,
                        processName, "System Entity Update cannot be started");
                }
            }

            // if application entity to update
            if (appEntityName.length() != 0) {
                try {
                    // create the UpdateEntity class
                    this.updateApplicationEntity
                        = new UpdateEntity(applicationEntityToUpdate, entityUpdateInterval);
                    // start thread
                    System.out.println(processName + ": Starting ApplicationEntity thread...");
                    this.updateApplicationEntity.start();
                }
                catch (Exception e) {
                    PlatformInterface.notifyMessage(ERR.value,
                        processName, "Application Entity Update cannot be started");
                }
            }

            // if process entity to update
            if (procEntityName.length() != 0) {
                try {
                    // create the UpdateEntity class
                    this.updateProcessEntity
                        = new UpdateEntity(processEntityToUpdate, entityUpdateInterval);
                    // start thread
                    System.out.println(processName + ": Starting ProcessEntity thread...");
                    this.updateProcessEntity.start();
                } catch (Exception e) {
                    PlatformInterface.notifyMessage(ERR.value,
                        processName, "Process Entity Update cannot be started");
                }
            }

            // if process auto ending
            if (autoEndTimeOut > 0 && autoEnding != null) {
                try {
                    // start thread
                    System.out.println(processName + ": Starting AutoEnding thread...");
                    autoEnding.start();
                } catch (Exception e) {
                    PlatformInterface.notifyMessage(ERR.value,
                        processName, "AutoEnding cannot be started");
                }
            }

            // if timeout for running phase
            if (startupMode > 0) {
                if (platformManaged) {
                    String msgInfo = "Running Timeout requested: "
                        + startupMode + " msec";
                    PlatformInterface.notifyMessage(INF.value, processName, msgInfo);
                } else {
                    System.out.println(processName + 
                        ": Running Timeout requested: " + startupMode + " msec");
                }

                System.out.println(processName + ": Sleeping " + 
                    Testable.getTimescale() * startupMode + " ms...");
                OS.sleep(Testable.getTimescale() * startupMode);
            }

        } catch(BadParameterException bpe) {
            bpe.printStackTrace();
        } catch(BadOrderException boe) {
            boe.printStackTrace();
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }
        
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public void stop() {

        try {

            if (platformManaged) {
                // example of using the PlatformInterface for notifying a message
                PlatformInterface.notifyMessage(INF.value,
                    processName, "Stop requested");
            } else {
                System.out.println(processName + ": Stop requested");
            }

            try {
                // if process auto ending request to stop thread
                //    if not already stopped
                if (autoEndTimeOut > 0 && autoEnding != null) {
                    System.out.println(processName + ": Stopping AutoEnding thread...");
                    autoEnding.end();
                }

                // if system entity updating to stop
                if (sysEntityName.length() != 0) {
                    // stop thread
                    System.out.println(processName + ": Stopping SystemEntity thread...");
                    updateSystemEntity.end();
                }

                // if application entity updating to stop
                if (appEntityName.length() != 0) {
                    // stop thread
                    System.out.println(processName + ": Stopping ApplicationEntity thread...");
                    updateApplicationEntity.end();
                }

                // if process entity updating to stop
                if (procEntityName.length() != 0) {
                    // stop thread
                    System.out.println(processName + ": Stopping ProcessEntity thread...");
                    updateProcessEntity.end();
                }
            } catch (Exception e) {
                // nothing to do
            }

            // stop timeout using startup mode
            if (startupMode > 0) {
                if (platformManaged) {
                    String msgInfo = "Stop Timeout requested: " + startupMode + " msec";
                    PlatformInterface.notifyMessage(INF.value, processName, msgInfo);
                } else {
                    System.out.println(processName +
                        ": Stop Timeout requested: " + startupMode + " msec");
                }

                System.out.println(processName + ": Sleeping " + 
                    Testable.getTimescale() * startupMode + " ms...");
                OS.sleep(Testable.getTimescale() * startupMode);
            }

        } catch(BadParameterException bpe) {
            bpe.printStackTrace();
        } catch(BadOrderException boe) {
            boe.printStackTrace();
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }

        System.out.println(processName + ": Calling orb shutdown...");
        orb.shutdown(false);
        
    }

    /**
     * Auto ends the process
     */
    public void autoEnd() {
    
        if (platformManaged) {
            // example of using the PlatformInterface for notifying a message
            try {
                PlatformInterface.notifyMessage(INF.value,
                    processName, "Process auto ending");
            } catch(BadParameterException bpe) {
                bpe.printStackTrace();
            } catch(BadOrderException boe) {
                boe.printStackTrace();
            }
        } else {
            System.out.println(processName + ": Process auto ending");
        }

        System.out.println(processName + ": Calling orb shutdown...");
        orb.shutdown(true);
        
    }


    /**
     * Sets service name to request.
     */
    public void setServiceToRequest(String serviceName){
        if (serviceName != null) {
            this.serviceName = serviceName;
        }
    }


    /**
     * Sets system entity name to set.
     */
    public void setSysEntityToSet(String entityName) {
        if (entityName != null) {
            this.sysEntityName = entityName;
        }
    }

    /**
     * Sets application entity name to set.
     */
    public void setAppEntityToSet(String entityName) {
        if (entityName != null) {
            this.appEntityName = entityName;
        }
    }

    /**
     * Sets process entity name to set.
     */
    public void setProcEntityToSet(String entityName) {
        if (entityName != null) {
            this.procEntityName = entityName;
        }
    }

}

