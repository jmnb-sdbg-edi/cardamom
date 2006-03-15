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


package cdmw.ccm.cif.test;


import cdmw.testutils.Testable;
import cdmw.ossupport.OS;

public class ClientThread extends Thread {
    
    private org.omg.CORBA.ORB orb;
    
    private String containerIOR;
    
    private String consumerIOR;

    public ClientThread(
        org.omg.CORBA.ORB orb, 
        String containerIOR, 
        String consumerIOR) {
        this.orb = orb;
        this.containerIOR = containerIOR;
        this.consumerIOR = consumerIOR;
    }
    
    public void run() {
        
        int timescale = Testable.getTimescale();
        try {
            OS.sleep(timescale * 3000);
        } catch(InterruptedException ie) {}
        // Testing PhiloTest
        PhiloTest philoTest = 
            new PhiloTest(this.orb, this.containerIOR, this.consumerIOR);
        philoTest.start();

        try {
            OS.sleep(timescale * 3000);
        } catch(InterruptedException ie) {}
        // Testing TypedPhiloTest
        TypedPhiloTest typedPhiloTest = 
            new TypedPhiloTest(this.orb, this.containerIOR);
        typedPhiloTest.start();

        this.orb.shutdown(false);
        
    }

}


