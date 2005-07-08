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


import cdmw.common.Assert;
import cdmw.testutils.Testable;
import cdmw.ossupport.OS;
import cdmw.ossupport.Mutex;
import cdmw.ossupport.Timeval;

//
// IDL:thalesgroup.com/Dinner/CCM_Philosopher:1.0
//
public class CCMPhilosopherImpl extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.UserDinner.User_CCM_Philosopher {
    
    public String name = "";
    
    public boolean isActive = false;
    
    public int state = 0;
    
    public int speed;
    
    public boolean haveLeftFork = false;
    
    public boolean haveRightFork = false;
    
    public com.acme.Dinner.CCM_Philosopher_Context context = null;
    
    public PhiloActivationThread activationThread;
    
    public Mutex mutex = new Mutex();
    
    private java.util.Random rand;
    
    public CCMPhilosopherImpl() {
        Timeval tv = OS.getTime();
        rand = new java.util.Random(tv.getMicroseconds() % 991);
        this.speed = 1500 + Math.abs((rand.nextInt() % 13) * 300);
        this.activationThread = new PhiloActivationThread(this);
    }
    
    /**
     *  This class provides an object to test the ccm_activate.
     */
    public class CCMPhilosopherActivateTest extends Testable {
        
        private CCMPhilosopherImpl philosopher = null;

        public CCMPhilosopherActivateTest(CCMPhilosopherImpl philosopher) {
            this.name = "Test ccm_activate on Philosopher";
            this.out = System.out;
            this.philosopher = philosopher;
        }
    
        public void doTests() {
        
            // set number of requested successfull tests
            setNbOfRequestedTestOK(4);
        
            Assert.check(this.philosopher != null);
        
            com.acme.Dinner.Registration registerServer = null;
            com.acme.Dinner.CCM_Philosopher_Context philosopherContext = 
                this.philosopher.get_context();
              
            println("Philo: Get_connection_to_register...");
            boolean testOk = false;
            try {
                registerServer = philosopherContext.get_connection_register();
        
                if (registerServer != null) {
                    testOk = true;
                }
            } catch (Exception e) {
                println("Philo: Unexpected exception raised!");
            }
            check(testOk);
        
            // register philosopher
            println("Philo: Register philosopherException e");
            testOk = false;
            try {
                if (registerServer != null) {
                    String philosopherName = registerServer.register();
                    this.philosopher.set_name(philosopherName);
                    System.out.println("Philo: " + philosopherName 
                        + " is active, speed is " + this.philosopher.speed);
                    testOk = true;
                 }
            } catch (Exception e) {
                println("Philo: Unexpected exception raised!");
            }
            check(testOk);
       /* 
            // push config from emitter port 
            println("Philo: push_config using emitter portException e");
            testOk = false;
            try {
                com.acme.Dinner.StatusInfo si = new StatusInfoImpl();
                si.name = this.philosopher.name;
                si.state = com.acme.Dinner.PhilosopherState.THINKING;
                si.secs_since_last_meal = this.philosopher.state;
                si.has_left_fork = this.philosopher.haveLeftFork;
                si.has_right_fork = this.philosopher.haveRightFork;
        
                this.philosopher.context.push_config(si);
                testOk = true;
            } catch (Exception e) {
                println("Philo: Unexpected exception raised!");
		e.printStackTrace();
            }*/
            check(testOk);


            //get the connection to the server 
            println("Philo: Connect to the fork facetException e");
            com.acme.Dinner.Fork left = null;
            com.acme.Dinner.Fork right = null;
            testOk = false;
            try {
                left = philosopherContext.get_connection_left();
                right = philosopherContext.get_connection_right();
        
                if ( left != null && right != null ) {
                    testOk = true;
                } else {
                    println("Philo: Facet reference is nil!");
                }
            } catch (Exception e) {
                println("Philo: Unexpected exception raised!");
            }
            check(testOk);
        
            // Let philosopher try to eat
            try {
                this.philosopher.mutex.lock();
                while (this.philosopher.isActive) {
                    this.philosopher.mutex.unlock();
                    int timescale = Testable.getTimescale();
                    OS.sleep(timescale * this.philosopher.speed);
        
                    this.philosopher.mutex.lock();
                    if (!this.philosopher.isActive) {
                        // philosopher has been desactivated: get out from loop
                        break;
                    }
                    this.philosopher.mutex.unlock();
        
                    if (this.philosopher.haveLeftFork 
                        && this.philosopher.haveRightFork) {
                        /*
                         * Eating
                         */
                        if (this.philosopher.state > 3) {
                            this.philosopher.state -= 3;
                            this.philosopher.push_state();
                            this.philosopher.mutex.lock();
                            continue;
                        }
                        this.philosopher.state = 0;
        
                        try {
                            left.release();
                            this.philosopher.haveLeftFork = false;
                            right.release();
                            this.philosopher.haveRightFork = false;
                        } catch (Exception e) {}
                        System.out.println("Philo: " + 
                            this.philosopher.name + " has eaten.");
                        this.philosopher.push_state();
                        this.philosopher.mutex.lock();
                        continue;
                    }
        
                    this.philosopher.state++;
        
                    if (this.philosopher.state < 3) {
                        System.out.println("Philo: " + 
                            this.philosopher.name + " is thinking.");
                        this.philosopher.push_state();
                        this.philosopher.mutex.lock();
                        continue;
                    }
        
                    if (this.philosopher.state == 3) {
                        System.out.println("Philo: " + 
                            this.philosopher.name + " is becoming hungry.");
                    } else if (this.philosopher.state >= 10) {
                        System.out.println("Philo: " + 
                            this.philosopher.name + " is starving.");
                    }
        
                    if (!this.philosopher.haveLeftFork) {
                        try {
                            left.get();
                            this.philosopher.haveLeftFork = true;
                        } catch (com.acme.Dinner.InUse iu) {
                            System.out.println("Philo: " + 
                                this.philosopher.name + 
                                " has its left fork used.");
                        } catch (org.omg.CORBA.UserException ue) {}
        
                        if (this.philosopher.haveLeftFork) {
                            System.out.println("Philo: " + 
                                this.philosopher.name + 
                                " has got the left fork.");
                            this.philosopher.push_state();
                            this.philosopher.mutex.lock();
                            continue;
                        } else {
                            System.out.println(this.philosopher.name + " did not get the left fork.");
                        }
                    }        
        
                    if (!this.philosopher.haveRightFork) {
                        try {
                            right.get ();
                            this.philosopher.haveRightFork = true;
                        } catch (com.acme.Dinner.InUse iu) {
                            System.out.println("Philo: " + 
                                this.philosopher.name + 
                                " has its right fork used.");
                        } catch (org.omg.CORBA.UserException ue) {}
        
                        if (this.philosopher.haveRightFork) {
                            System.out.println("Philo: " + 
                                this.philosopher.name + 
                                " has got the right fork.");
                            this.philosopher.push_state();
                            this.philosopher.mutex.lock();
                            continue;
                        } else {
                            System.out.println(this.philosopher.name + " did not get the right fork.");
                        }
                    }
        
                    if ( ( (this.philosopher.haveLeftFork 
                            && !this.philosopher.haveRightFork) ||
                        (!this.philosopher.haveLeftFork 
                            && this.philosopher.haveRightFork)
                        ) && ((rand.nextInt() % 3) == 0) ) {

                        if (this.philosopher.haveLeftFork) {
                            try {
                                left.release ();
                                this.philosopher.haveLeftFork = false;
                                System.out.println("Philo: " + 
                                    "Deadlock prevention! "+ 
                                    this.philosopher.name +
                                    " drops the left fork.");
                            } catch (Exception e)  {}
                        }
                        
                        if (this.philosopher.haveRightFork) {
                            try {
                                right.release();
                                this.philosopher.haveRightFork = false;
                                System.out.println("Philo: " + 
                                    "Deadlock prevention! " + 
                                    this.philosopher.name +
                                    " drops the right fork.");
                            } catch (Exception e) {}
                        }
                    }
        
                    this.philosopher.push_state();
                    this.philosopher.mutex.lock();
                } // End of while (is_active)
                this.philosopher.mutex.unlock();
                System.out.println(
                    "End of dinner for " + this.philosopher.name);
            } catch (org.omg.CORBA.SystemException se) {
                System.out.println("System Exception raised: " + se.toString());
            } catch (InterruptedException ie) {
                System.out.println("Interrupted Exception raised: " + ie.toString());
            }
        }

    }

    public class PhiloActivationThread extends Thread {
        
        private CCMPhilosopherImpl philosopher;

        public PhiloActivationThread(CCMPhilosopherImpl philosopher) {
            this.philosopher = philosopher;
        }

        public void run() {
          CCMPhilosopherActivateTest test = 
            new CCMPhilosopherActivateTest(this.philosopher);
          test.start();
        }
    }
    
    //
    // IDL:thalesgroup.com/Dinner/CCM_Philosopher_Executor/name:1.0
    //
    public String name() {
        return this.name;
    }

    public void set_session_context(org.omg.Components.SessionContext ctx) {
        System.out.println("Philo: set_session_context called!");
        this.context = 
            com.acme.Dinner.CCM_Philosopher_ContextHelper.narrow(ctx);
    }
    
    /**
     * Actives the component (call by the container)
     */
    public void ccm_activate() {
        if (!this.isActive) {
            this.isActive = true;
            // Start thread
            this.activationThread.start();
        }
    }
    
    /**
     * Makes stop the component (call by the container)
     */
    public void ccm_passivate() {
        System.out.println("Philo: ccm_passivate called!");

        try {
            this.mutex.lock();
            if (!this.isActive) {
                this.mutex.unlock();
                return;
            }
        
            this.isActive = false;
            this.mutex.unlock();
        
            System.out.println("Philo: " + this.name + " is inactive.");
        
            try {
                this.activationThread.join();
            } catch (InterruptedException ie) {
                System.out.println(ie.toString());
            }
        
            System.out.println("Philo: " + this.name + " thread has join");
        
            if (this.haveLeftFork) {
                try {
                    System.out.println(
                        "Philo: " + this.name + " release left fork!");
                    com.acme.Dinner.Fork left = 
                        this.context.get_connection_left();
                    if (left != null) {
                        left.release();
                        System.out.println("Philo: " + 
                            this.name + " has released left fork!");
                        this.haveLeftFork = false;
                    }
                } catch (Exception e) {
                    System.out.println("Philo: " + this.name + " Exception raised");
                }
            }
        
            if (this.haveRightFork) {
                try {
                    System.out.println(
                        "Philo: " + this.name + " release right fork!");
                    com.acme.Dinner.Fork right = 
                        this.context.get_connection_right();
                    if (right != null) {
                        right.release ();
                        System.out.println("Philo: " + 
                            this.name + " has released right fork!");
                        this.haveRightFork = false;
                      }
                } catch (Exception e) {
                    System.out.println("Philo: " + this.name + " Exception raised");
                }
            }
        } catch(InterruptedException ie) {
            System.out.println("Interrupted Exception raised: " + ie.toString());
        }
    }
    
    /**
     * Destroys the component (call by the container)
     */
    public void ccm_remove() {
        System.out.println("Philo: ccm_remove called!");
    }

    /**
     * Push status info event
     */
    public void push_state() {
        com.acme.Dinner.StatusInfo si = new StatusInfoImpl();
        si.name = this.name;
    
        if (this.haveLeftFork && this.haveRightFork) {
            si.state = com.acme.Dinner.PhilosopherState.EATING;
        } else if (this.state < 3) {
            si.state = com.acme.Dinner.PhilosopherState.THINKING;
        } else if (this.state < 10) {
            si.state = com.acme.Dinner.PhilosopherState.HUNGRY;
        } else if (this.state < 40) {
            si.state = com.acme.Dinner.PhilosopherState.STARVING;
        } else {
            si.state = com.acme.Dinner.PhilosopherState.DEAD;
        }
    
        si.secs_since_last_meal = this.state;
        si.has_left_fork = this.haveLeftFork;
        si.has_right_fork = this.haveRightFork;
    
        this.context.push_info(si);
    }

    public boolean is_active() {
        boolean active = false;
        try {
            this.mutex.lock();
            active = this.isActive;
            this.mutex.unlock();
        } catch(InterruptedException ie) {
            System.err.println(ie.toString());
        }
        return active;
    }

    public void set_name(String name) {
        try {
            this.mutex.lock();
            this.name = name;
            this.mutex.unlock();
        } catch(InterruptedException ie) {
            System.err.println(ie.toString());
        }
    }

    public com.acme.Dinner.CCM_Philosopher_Context get_context() {
        return this.context;
    }

}


