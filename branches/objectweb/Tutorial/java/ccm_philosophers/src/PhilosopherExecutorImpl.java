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



class PhilosopherExecutorImpl
    extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.UserDinner.User_Philosopher 
{
    
    String name;

    int state;
    int speed;

    boolean isActive;
    boolean haveLeftFork;
    boolean haveRightFork;

    com.acme.Dinner.CCM_Philosopher_Context context;
    
    private PhiloActivationThread activationThread;
    
    cdmw.ossupport.Mutex mutex;
     
    
    public PhilosopherExecutorImpl() {
        this.name = "";
        this.state = 0;
        this.isActive = false;
        this.haveLeftFork = false;
        this.haveRightFork = false;
        this.context = null;

        // set speed
        this.speed = 1500 + (((int)(Math.random()*Integer.MAX_VALUE)) % 13) * 300;
        
        // create thread
        this.activationThread = new PhiloActivationThread(this);
        
        this.mutex = new cdmw.ossupport.Mutex();
    }
    
    
    //
    // set_session_context function : it fixes the session context of the component (call by the container)
    //
    public void set_session_context(org.omg.Components.SessionContext ctx)  {
        System.out.println("Philo: Set_session_context called!");
       
        context = com.acme.Dinner.CCM_Philosopher_ContextHelper.narrow(ctx); 
    }
       
        
    //
    // ccm_activate function : it actives the component (call by the container)
    //
    public void ccm_activate()  {
        // if thread not yet started, start it
       if (! isActive) {
          isActive = true;
          
          activationThread.start();
       }
    }
    
    
    //
    // ccm_passivate function : it makes stop the component (call by the container)
    //
    public void ccm_passivate()  {
        System.out.println("Philo: Ccm_passivate called!");
    
        // first lock mutex
        try {
            mutex.lock();
        } catch (InterruptedException e) {
            System.out.println("Interrupted Exception raised!");
        }
       
        // if thread not started
        if (!isActive) {
            // unlock mutex and return
            mutex.unlock();
            return;
        }
    
        // thread has been started so stop it then unlock mutex
        isActive = false;
        mutex.unlock();
    
        System.out.println("Philo: " + name + " is inactive.");
    
        // join the thread
        try {
            activationThread.join();
        } catch (InterruptedException e) {
            System.out.println("Interrupted Exception raised!");
        }
    
        System.out.println("Philo: " + name + " thread has join");
    
    
        // if philosopher has the left fork, release it
        if (haveLeftFork) {
            try {
                System.out.println("Philo: " + name + " release left fork!");
               
                com.acme.Dinner.Fork left = context.get_connection_left ();
               
                if (left != null) {
                    left.release ();
                   
                    System.out.println("Philo: " + name + " has released left fork!");
                    haveLeftFork = false;
                }
            } catch (Exception e) {
                System.out.println("Philo: " + name + " Exception raised: " + e.toString());
            }
        }
    
        // if philosopher has the right fork, release it
        if (haveRightFork) {
            try {
                System.out.println("Philo: " + name + " release right fork!");
                
                com.acme.Dinner.Fork right = context.get_connection_right ();
                
                if (right != null) {
                    right.release ();
                    
                    System.out.println("Philo: " + name + " has released right fork!");
                    haveRightFork = false;
                }
            } catch (Exception e) {
                System.out.println("Philo: " + name + " Exception raised: " + e.toString());
            }
        }
    } 
    
    
    
    //
    // ccm_remove function : it destroys the component (call by the container)
    //
    public void ccm_remove() {
       System.out.println("Philo: Ccm_remove called!");
    }
    
    
    
    //
    // IDL:acme.com/Dinner/CCM_Philosopher_Executor/name:1.0
    //
    public String name() {
        return name;
    }
    
    public void set_name(String name) {
        try {
            mutex.lock();
        } catch (InterruptedException e) {
            System.out.println("Interrupted Exception raised!");
        }
        this.name = name;
        mutex.unlock();
    }
    
    
    //
    // push status info event
    //
    public void push_state () {
        // create a new status info event
        com.acme.Dinner.StatusInfo si = new StatusInfoImpl();
        
        si.name = name;
    
        // if philosopher has two forks (left and right)
        if (haveLeftFork && haveRightFork) {
             si.state = com.acme.Dinner.PhilosopherState.EATING;
        }
        
        // else he cannot eat so
        // he is thinking if time since last meal is < 3 sec
        else if (state < 3) {
             si.state = com.acme.Dinner.PhilosopherState.THINKING;
        }
        
        // else he is hungry if time since last meal is < 10 sec
        else if (state < 10)  {
             si.state = com.acme.Dinner.PhilosopherState.HUNGRY;
        }
        
        // else he is starving if time since last meal is < 40 sec
        else if (state < 40) {
             si.state = com.acme.Dinner.PhilosopherState.STARVING;
        }
        
        // else he is dead
        else {
             si.state = com.acme.Dinner.PhilosopherState.DEAD;
        }
    
        si.secs_since_last_meal = state;
        si.has_left_fork = haveLeftFork;
        si.has_right_fork = haveRightFork;
    
    
        // send the event to consummers
        context.push_info (si);
    }
    
    
    public boolean isActive() {
        return isActive;
    }
    
}

