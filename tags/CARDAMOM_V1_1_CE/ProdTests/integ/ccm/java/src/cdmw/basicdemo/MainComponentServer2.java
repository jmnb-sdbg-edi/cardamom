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

package cdmw.basicdemo;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.cdmwinit.CdmwInterface;

public class MainComponentServer2 {
    
    public final static int POA_THREAD_POOL_SIZE = 5;

    public static void main(String[] args) {
        
        org.omg.CORBA.ORB orb = null;
       
        try {
            // Initialize the ORB
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
    
            orb = ORBSupport.ORBInit(args, orbStrategies);

            CdmwInterface.cdmwInit(
                orb, args, null, "cdmw.generated.CdmwInterface2");

            // Get the root POA 
            org.omg.CORBA.Object obj = 
                orb.resolve_initial_references("RootPOA");
            org.omg.PortableServer.POA rootPOA = 
                org.omg.PortableServer.POAHelper.narrow(obj);

            // Run implementation (Activate the POA manager)
            rootPOA.the_POAManager().activate();
    
            orb.run();
        } catch (Exception ex) {
            System.err.println(ex.getMessage());
        }
    
        if (orb != null) {
            try {
                CdmwInterface.cdmwCleanup(
                    orb, "cdmw.generated.CdmwInterface2");
                orb.destroy();
            } catch(Exception ex) {
                System.err.println(ex.getMessage());
            }
        }
    
    }

}

