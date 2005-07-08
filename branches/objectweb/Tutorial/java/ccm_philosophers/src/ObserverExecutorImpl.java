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

package Dinner;



class ObserverExecutorImpl
    extends org.omg.CORBA.LocalObject
    implements com.acme.Dinner.CCM_Observer 
{
    
    private int indice;
    
    private String name;
    
    
    public ObserverExecutorImpl() {
        this.indice = 0;
        this.name = "";
    }
    
    
    //
    // IDL:acme.com/Dinner/Observer/push_info:1.0
    //
    public void push_info (com.acme.Dinner.StatusInfo event) {
        // print status of philosopher
        
        System.out.print("Observer: " + event.name + ": ");
    
        switch (event.state.value()) 
        {
          case com.acme.Dinner.PhilosopherState._EATING:   
                System.out.print("Eating     "); 
                break;
          case com.acme.Dinner.PhilosopherState._THINKING: 
                System.out.print("Thinking   "); 
                break;
          case com.acme.Dinner.PhilosopherState._HUNGRY:   
                System.out.print("Hungry     "); 
                break;
          case com.acme.Dinner.PhilosopherState._STARVING: 
                System.out.print("Starving   "); 
                break;
          case com.acme.Dinner.PhilosopherState._DEAD:     
                System.out.print("Dead       "); 
                break;
        }
    
    
        // print if philosopher has a fork in left hand
        
        if (event.has_left_fork) {
            System.out.print("* ");
        } else {
            System.out.print("  ");
        }
    
    
        // print if philosopher has a fork in right hand
        if (event.has_right_fork) {
             System.out.print("* ");
        } else {
             System.out.print("  ");
        }
    
        // print the number of sec since last meal
        
        System.out.println(event.secs_since_last_meal + "s since last meal");
    
        System.out.println();
    }
    
}

