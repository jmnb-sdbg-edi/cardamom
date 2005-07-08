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


package cdmw.ccm.cif.test;


import com.acme.Dinner.PhilosopherState;

public class NonCdmwStatusInfoConsumerImpl 
    extends com.acme.Dinner.StatusInfoConsumerPOA {
    
    private org.omg.PortableServer.POA poa;

    public NonCdmwStatusInfoConsumerImpl(org.omg.PortableServer.POA poa) {
        this.poa = poa;
    }
    
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    public void push_StatusInfo(com.acme.Dinner.StatusInfo theStatusInfo) {
        
        System.out.println("NonCdmwStatusInfoConsumer received a status info message");
        System.out.print("NonCdmwStatusInfoConsumer: " + theStatusInfo.name + ": ");

        switch (theStatusInfo.state.value()) {
            case PhilosopherState._EATING:
              System.out.print("Eating     "); 
              break;
            case PhilosopherState._THINKING: 
              System.out.print( "Thinking   "); 
              break;
            case PhilosopherState._HUNGRY:   
              System.out.print( "Hungry     "); 
              break;
            case PhilosopherState._STARVING: 
              System.out.print( "Starving   "); 
              break;
            case PhilosopherState._DEAD:     
              System.out.print( "Dead       "); 
              break;
        }

        if (theStatusInfo.has_left_fork) {
            System.out.print("* ");
        } else {
            System.out.print("  ");
        }

        if (theStatusInfo.has_right_fork) {
            System.out.print("* ");
        } else {
            System.out.print("  ");
        }

        System.out.println(theStatusInfo.secs_since_last_meal + "s since last meal");
       
    }
    
    public void push_event(org.omg.Components.EventBase evt) 
        throws org.omg.Components.BadEventType {
        
        System.out.println("NonCdmwStatusInfoConsumer received an event");

        if (evt == null) {
            System.out.println("NonCdmwStatusInfoConsumer received a nil event type");
            throw new org.omg.Components.BadEventType();
        }

        try {
            com.acme.Dinner.StatusInfo infoEvt = (com.acme.Dinner.StatusInfo) evt;
            System.out.println("NonCdmwStatusInfoConsumer push_StatusInfo called");
            this.push_StatusInfo(infoEvt);
        } catch(ClassCastException cce) {
            System.out.println("NonCdmwStatusInfoConsumer received a bad event type");
            throw new org.omg.Components.BadEventType();
        }

    }

}


