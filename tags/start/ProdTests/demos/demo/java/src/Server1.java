/* =========================================================================== *
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
 * =========================================================================== */


package cdmw.validation;

public class Server1 {

    public static final int POA_THREAD_POOL_SIZE = 5;

    public static final String SERVER_NAME = "server1";

    public static final String CHANNEL_NAME = "EventChannel1";
    public static final String COMPLETE_CHANNEL_NAME = "EVENT_CHANNELS/EventChannel1";
    public static final String TYPED_CHANNEL_NAME = "TypedEventChannel1";
    public static final String COMPLETE_TYPED_CHANNEL_NAME = "EVENT_CHANNELS/TypedEventChannel1";

    public static void main(String[] args) {

        org.omg.CORBA.ORB orb = null;

        try {
            // Initialize the ORB
            cdmw.orbsupport.StrategyList orbStrategies =
                new cdmw.orbsupport.StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);

            // Get the root POA
            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));

            // Run implementation (Activate the POA manager)
            rootPOA.the_POAManager().activate();

            cdmw.cdmwinit.ProcessControl ctrl =
                new cdmw.resources.ServerProcessControl(orb,
                                                         rootPOA,
                                                         SERVER_NAME,
                                                         CHANNEL_NAME,
                                                         COMPLETE_CHANNEL_NAME,
                                                         COMPLETE_TYPED_CHANNEL_NAME);

            cdmw.cdmwinit.CdmwInterface.cdmwInit(orb, args, ctrl,
                "cdmw.generated.Server1CdmwInterface");

            // wait for CORBA message
            orb.run();

        } catch (Exception ex) {
            ex.printStackTrace();
        }

        if (orb != null) {
            try {
                cdmw.cdmwinit.CdmwInterface.cdmwCleanup(orb,
                    "cdmw.generated.Server1CdmwInterface");
                orb.destroy();
            } catch(Exception ex) {
                ex.printStackTrace();
            }
        }

    }

}

