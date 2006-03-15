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

/**
 * This class provides an implementation of ProcessMessageBroker interface
 *
 * Features:
 * Thread safe
 *
 */
public class SimProcessMessageBrokerImpl 
    extends com.thalesgroup.CdmwPlatformMngt.ProcessMessageBrokerPOA {

    /**
     * Holds a reference to ...
     */
    private org.omg.PortableServer.POA poa;
    private com.thalesgroup.CdmwPlatformMngtService.ServiceBroker serviceBroker;
    private com.thalesgroup.CdmwSimulatedRepository.MessageLogger logger;

    /**
     * Constructor
     * 
     */ 
    public SimProcessMessageBrokerImpl(org.omg.PortableServer.POA parent,
        com.thalesgroup.CdmwPlatformMngtService.ServiceBroker serviceBroker,
        com.thalesgroup.CdmwSimulatedRepository.MessageLogger logger) {
        this.poa = parent;
        this.serviceBroker = serviceBroker;
        this.logger = logger;
    }
    
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_message:1.0
    //
    public void
    notify_message(com.thalesgroup.CdmwPlatformMngtBase.EventHeader header,
        String issuer, String message) {
        
        String body = "( {" + getTimeStamp(header.time_stamp) + ";" 
            + header.level + "}," + issuer + "," + message + ")";
        logger.log("ProcessMessageBroker::notify_message",body);
        
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_error:1.0
    //
    public void 
    notify_error(com.thalesgroup.CdmwPlatformMngtBase.TimeStamp timeStamp,
        String issuer, String errorInfo) {
        
        String body = "(" + getTimeStamp(timeStamp) + ";" 
            + issuer + "," + errorInfo + ")";
        logger.log("ProcessMessageBroker::notify_message",body);
        
    }
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_process_error:1.0
    //
    public void 
    notify_process_error(String failed_process_name,
        com.thalesgroup.CdmwPlatformMngtBase.TimeStamp timeStamp,
        String issuer, String errorInfo) {
        
        String body = "(" + failed_process_name + getTimeStamp(timeStamp) + ";" 
            + issuer + "," + errorInfo + ")";
        logger.log("ProcessMessageBroker::notify_process_error",body);
        
    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/notify_user_event:1.0
    //
    public void
	notify_user_event(String issuer, 
			  org.omg.CORBA.Any data)
     
    {
	    String body = "(" + issuer + ",data)";
        logger.log("ProcessMessageBroker::notify_message",body);
    }
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_service:1.0
    //
    public org.omg.CORBA.Object get_service(String serviceName)
        throws com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound,
        com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable {

        return this.serviceBroker.get_service("","","", serviceName);

    }

    //
    // IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessageBroker/get_managed_element:1.0
    //
    public com.thalesgroup.CdmwPlatformMngt.ManagedElement get_managed_element(
            String element_path) 
        throws com.thalesgroup.CdmwPlatformMngt.ElementPathInvalid,
               com.thalesgroup.CdmwPlatformMngt.ManagedElementNotFound {
                
        com.thalesgroup.CdmwPlatformMngt.ManagedElement element = null;

        return element;
    }

    
    private static String getTimeStamp(
        com.thalesgroup.CdmwPlatformMngtBase.TimeStamp timeStamp) {
        
        return  Short.toString(timeStamp.year)   + "/"
              + Short.toString(timeStamp.month)  + "/"
              + Short.toString(timeStamp.day)    + " "
              + Short.toString(timeStamp.hour)   + ":"
              + Short.toString(timeStamp.minute) + ":"
              + Short.toString(timeStamp.second);
    }

	public java.lang.String 
        get_system_entity_status(java.lang.String entity_name,
                                 org.omg.CORBA.StringHolder entity_info)
        throws com.thalesgroup.CdmwPlatformMngt.EntityNotFound
    {
        entity_info.value = "";
        return "";
    }

	public java.lang.String 
        get_host_entity_status(java.lang.String host_name,
                               java.lang.String entity_name,
                               org.omg.CORBA.StringHolder entity_info)
        throws com.thalesgroup.CdmwPlatformMngt.EntityNotFound,com.thalesgroup.CdmwPlatformMngt.HostNotFound
    {
        entity_info.value = "";
        return "";
    }

	public java.lang.String 
        get_application_entity_status(java.lang.String application_name,
                                      java.lang.String entity_name,
                                      org.omg.CORBA.StringHolder entity_info)
        throws com.thalesgroup.CdmwPlatformMngt.ApplicationNotFound,com.thalesgroup.CdmwPlatformMngt.EntityNotFound
    {
        entity_info.value = "";
        return "";
    }
    
	public java.lang.String 
        get_process_entity_status(java.lang.String application_name,
                                  java.lang.String process_name,
                                  java.lang.String host_name,
                                  java.lang.String entity_name,
                                  org.omg.CORBA.StringHolder entity_info)
        throws com.thalesgroup.CdmwPlatformMngt.EntityNotFound,com.thalesgroup.CdmwPlatformMngt.ProcessNotFound
    {
        entity_info.value = "";
        return "";
    }
    
	public void 
        set_system_entity_status(java.lang.String entity_name,
                                 java.lang.String entity_status,
                                 java.lang.String entity_info)
        throws com.thalesgroup.CdmwPlatformMngt.EntityNotFound
    {
    }
    
	public void 
        set_host_entity_status(java.lang.String host_name,
                               java.lang.String entity_name,
                               java.lang.String entity_status,
                               java.lang.String entity_info)
        throws com.thalesgroup.CdmwPlatformMngt.EntityNotFound,com.thalesgroup.CdmwPlatformMngt.HostNotFound
    {
    }
    
	public void 
        set_application_entity_status(java.lang.String application_name,
                                      java.lang.String entity_name,
                                      java.lang.String entity_status,
                                      java.lang.String entity_info)
        throws com.thalesgroup.CdmwPlatformMngt.ApplicationNotFound,com.thalesgroup.CdmwPlatformMngt.EntityNotFound
    {
    }
    
	public void 
        set_process_entity_status(java.lang.String application_name,
                                  java.lang.String process_name,
                                  java.lang.String host_name,
                                  java.lang.String entity_name,
                                  java.lang.String entity_status,
                                  java.lang.String entity_info)
        throws com.thalesgroup.CdmwPlatformMngt.EntityNotFound,com.thalesgroup.CdmwPlatformMngt.ProcessNotFound
    {
    }


}

