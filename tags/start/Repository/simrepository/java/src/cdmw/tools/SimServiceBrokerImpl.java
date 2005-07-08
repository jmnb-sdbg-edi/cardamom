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

import cdmw.namingandrepository.NamingInterface;

/**
 * This class provides an implementation of the ServiceBroker interface
 *
 * Features:
 * Thread safe
 *
 */
public class SimServiceBrokerImpl 
    extends com.thalesgroup.CdmwPlatformMngtService.ServiceBrokerPOA {

    /**
     * Holds a reference to ...
     */
    private org.omg.PortableServer.POA poa;
    private NamingInterface root;
    private com.thalesgroup.CdmwSimulatedRepository.MessageLogger logger;

    /**
     * Constructor
     * 
     */ 
    public SimServiceBrokerImpl(org.omg.PortableServer.POA parent,
        org.omg.CosNaming.NamingContext root, 
        com.thalesgroup.CdmwSimulatedRepository.MessageLogger logger) {
        
        this.poa = parent;
        this.root = new NamingInterface(root);
        this.logger = logger;
        
    }
    
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }
    
    //
    // IDL:thalesgroup.com/CdmwPlatformMngtService/ServiceBroker/get_service:1.0
    //
    public org.omg.CORBA.Object get_service(String serviceName)
        throws com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound,
        com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable {
        
        String header = "ServiceBroker::get_service(" + serviceName + ")";

        org.omg.CORBA.Object obj = null;
        try {
            obj = root.resolve(serviceName);
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            logger.log(header,"throw CdmwPlatformMngtService::ServiceNotFound");
            throw new com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound();
        } catch (Exception e) {
            logger.log(header,"throw CORBA::INTERNAL");
            throw new org.omg.CORBA.INTERNAL();
        }

        logger.log(header,"service found.");

        return obj;
        
    }
    
    public void set_service(String serviceName, org.omg.CORBA.Object objectRef) {
        
        try {
            root.bind(serviceName, objectRef, true);
        } catch (Exception e) {
            throw new org.omg.CORBA.INTERNAL();
        }
        
    }

}

