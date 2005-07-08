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


package cdmw.trace.test;



class OrbThread extends Thread {
 
    private org.omg.CORBA.ORB orb;
    
    private org.omg.PortableServer.POA poa;
    
    
    
    public OrbThread() {
        super("ORBThread");
        
        cdmw.orbsupport.StrategyList strategyList = 
            new cdmw.orbsupport.StrategyList();
        
        strategyList.addORBThreaded();
        try {
            strategyList.addPOAThreadPool(10);
        } catch (cdmw.common.BadParameterException e) {
            System.err.println("ERROR in strategyList.addPOAThreadPool(10)");
            e.printStackTrace();
            System.exit(0);
        }
            
        orb = cdmw.orbsupport.ORBSupport.ORBInit(new String[0], strategyList, "");
        
        try {
            org.omg.CORBA.Object obj = orb.resolve_initial_references("RootPOA");
            poa = org.omg.PortableServer.POAHelper.narrow(obj);
            org.omg.PortableServer.POAManager poaMgr = poa.the_POAManager();
            poaMgr.activate();
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            System.err.println("ERROR in orb.resolve_initial_references(\"RootPOA\")");
            e.printStackTrace();
            System.exit(0);
        } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive e) {
            System.err.println("poaMgr.activate()");
            e.printStackTrace();
            System.exit(0);
        }        
        
    }
    
    public void shutdown() {
        orb.shutdown(false);
        orb.destroy();   
    }
    
    public org.omg.CORBA.ORB getORB() {
        return orb;   
    }
    
    public org.omg.PortableServer.POA getPOA() {
        return poa;   
    }
    
    public void run() {
        orb.run();  
    }
    
}



  

