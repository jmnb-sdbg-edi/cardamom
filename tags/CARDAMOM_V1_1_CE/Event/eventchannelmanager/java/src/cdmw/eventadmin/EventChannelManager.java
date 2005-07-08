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


package cdmw.eventadmin;

import cdmw.common.Options;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;

import cdmw.cdmwinit.CdmwInterface;

/**
 * This class provides a process that embeds two default event channel 
 * factories (a generic one and a typed one), and an event channel profile 
 * manager allowing to create, modify or remove event channel profile 
 * from the configuration. 
 * This executable reads the Event Channel profile XML file 
 * at its initialisation. 
 * Only one EventChannelManager is allowed to run in a system.
 *
 */
public class EventChannelManager {
    
    /**
     * Default thread pool size of the Root POA.
     */
    public static final int POA_THREAD_POOL_SIZE = 5;
    
    /**
     * Display a help message about the usage of the EventChannelProfileManager
     */
    public static void usage() {
        System.out.println("usage: EventChannelProfileManager " 
            + Options.CALLBACK_IOR_OPTION + "=<callbackIOR> "
            + Options.PROCESS_XML_FILE_OPTION + "=<xmlFileName> " 
            + InitUtils.PROFILE_XML_FILE_OPTION + "=<xmlFileName> " 
            + "[" + Options.VALIDATE_XML_OPTION + "]");
    }
    
    /**
     * Start the EventChannelProfileManager
     *
     * @param args Options of the command line
     */
    public static void main(String[] args) {
    
        org.omg.CORBA.ORB orb = null;
        org.omg.PortableServer.POA rootPOA;
        boolean validate = false;
    
        if (args.length < 2) {
            usage();
            System.exit(1);
        }

        try {
            // Get the XML file Name : mandatory argument
            String xmlFilePath = InitUtils.getProfileXmlFile(args);

            // Get validation option for xml file
            validate = InitUtils.getXmlValidationOption(args);

            // Init ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = ORBSupport.ORBInit(args, orbStrategies);

            // Resolve Root POA.
            rootPOA = org.omg.PortableServer.POAHelper.narrow(
                orb.resolve_initial_references("RootPOA"));
            
            // Activate POA manager
            rootPOA.the_POAManager().activate();

            // Create the single instance of the profile manager
            EventChannelProfileManagerImpl profileManager 
                = EventChannelProfileManagerImpl.init(
                    orb, rootPOA, xmlFilePath, validate);

            if (profileManager == null) {
                System.out.println("ERROR: Unable to instantiate the EventChannelProfileManager object");
            }

            ManagerProcessControl mngCtrl 
                = new ManagerProcessControl(profileManager);

            // init Cdmw
            CdmwInterface.cdmwInit(orb ,args, 
                mngCtrl, "cdmw.generated.ServerCdmwInterface");

            // wait for event
            orb.run();

        } catch(Exception e) {
            e.printStackTrace();
        }

        if (orb!=null) {
            try {
                CdmwInterface.cdmwCleanup(
                    orb , "cdmw.generated.ServerCdmwInterface");
                orb.destroy();
            } catch(Exception e) {
                e.printStackTrace();
            }
        }

    }
    
}


