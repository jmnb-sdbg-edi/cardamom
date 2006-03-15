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


package cdmw.tools;

import cdmw.commonsvcs.naming.NamingInterface;

import com.thalesgroup.CdmwPlatformMngt.ProcessCallbackPackage.ProcessStartingData;

/**
 * Provides an implementation of the ProcessCallback interface
 *
 * Features:
 * Thread safe
 *
 */
public class SimProcessCallbackImpl
    extends com.thalesgroup.CdmwPlatformMngt.ProcessCallbackPOA {

    private static final String PROCESS_PREFIX="PROC_";

    /**
     * Holds a reference to ...
     */
    private org.omg.PortableServer.POA poa;
    private NamingInterface root;
    private com.thalesgroup.CdmwPlatformMngt.ProcessMessageBroker messageBroker;
    private com.thalesgroup.CdmwPlatformMngtService.ServiceBroker serviceBroker;
    private com.thalesgroup.CdmwSimulatedRepository.MessageLogger logger;
    private String applicationName;
    private int processNumber = 0;

    /**
     * Constructor
     * 
     */ 
    public SimProcessCallbackImpl(org.omg.PortableServer.POA parent,
        org.omg.CosNaming.NamingContext root,
        com.thalesgroup.CdmwPlatformMngt.ProcessMessageBroker messageBroker,
        com.thalesgroup.CdmwPlatformMngtService.ServiceBroker serviceBroker,
        com.thalesgroup.CdmwSimulatedRepository.MessageLogger logger,
        String applicationName) {
        
        this.poa = parent;
        this.root = new NamingInterface(root);
        this.messageBroker = messageBroker;
        this.serviceBroker = serviceBroker;
        this.logger = logger;
        this.applicationName = applicationName;
        
    }
    
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_ready:1.0
    //
    /**
     * Not thread safe!
     */
    public com.thalesgroup.CdmwPlatformMngt.ProcessCallbackPackage.ProcessStartingData
        set_ready(com.thalesgroup.CdmwPlatformMngt.ProcessDelegate theProcess) {
        
        return get_starting_info (theProcess);
    }
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_ready:1.0
    //
    /**
     * Not thread safe!
     */
    public com.thalesgroup.CdmwPlatformMngt.ProcessCallbackPackage.ProcessStartingData
        get_starting_info(com.thalesgroup.CdmwPlatformMngt.ProcessDelegate theProcess) {
        
        ProcessStartingData starting_data = new ProcessStartingData();
        
        // The name of the application owning the process.
        starting_data.application_name = this.applicationName;
        // The process name.
        char[] procNumber = String.valueOf(this.processNumber).toCharArray();
        char[] procNb = { '0', '0', '0' };
        int i = 0;
        while(i < procNumber.length) {
            procNb[procNb.length - procNumber.length + i] = procNumber[i];
            i++;
        }
        
        processNumber++;
        starting_data.process_name = PROCESS_PREFIX + String.valueOf(procNb);

        // The broker used by the managed process to notify its
        // applicative messages and fatal errors.
        starting_data.process_message_broker = this.messageBroker;

        String body = this.applicationName + "/" + starting_data.process_name;
        logger.log("ProcessCallback::set_ready", body);

        // Register theProcess in the naming context
        try {
            root.bind(body, theProcess, true);
        } catch (Exception e) {
            logger.log(e.toString(),"Unable to bind the process " 
                + starting_data.process_name + " with name " + body);
        }
        
        return starting_data;
        
    }
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_creation_done:1.0
    //
    public void set_creation_done() {

    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/notify_error:1.0
    //
    public void notify_error(String issuer,String errorInfo) {
    
        String body = "(" + issuer + "," + errorInfo + ")";
        logger.log("ProcessCallback::notify_error", body);
    
    }

}

