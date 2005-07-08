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



class PhiloActivationThread
    extends Thread
{
    
    private PhilosopherExecutorImpl philosopher;
    
    public PhiloActivationThread(PhilosopherExecutorImpl philosopher) {
        this.philosopher = philosopher;
        
    }
    
    
    
    public void run() {
        try {
            // get register connection
            com.acme.Dinner.Registration registerServer =
                philosopher.context.get_connection_register();
            
            // register the philosopher and get its name              
            String philosopherName = registerServer.register();
            
            // store the phisopher name
            philosopher.set_name(philosopherName);
            System.out.println("Philo: "  + philosopherName + 
                               " is active, speed is " + philosopher.speed);
                      
            // push config from emitter port 
            System.out.println("Philo: push_config using emitter port..."); 
         
            com.acme.Dinner.StatusInfo si = new StatusInfoImpl();
            si.name = philosopher.name;
            si.state = com.acme.Dinner.PhilosopherState.THINKING;
            si.secs_since_last_meal = philosopher.state;
            si.has_left_fork = philosopher.haveLeftFork;
            si.has_right_fork = philosopher.haveRightFork;
    
            philosopher.context.push_config (si);
            
            
            //get the connection to the Fork manager
            System.out.println("Philo: Get the fork connections...");
            
            com.acme.Dinner.Fork left = philosopher.context.get_connection_left();
            com.acme.Dinner.Fork right = philosopher.context.get_connection_right();
            
            // let philosopher try to eat
            
            // lock the mutex
            philosopher.mutex.lock();
            
            // while thread is not requested to stop
            while (philosopher.isActive)
            {
                // unlock the mutex
                philosopher.mutex.unlock();
                
                // sleep
                cdmw.ossupport.OS.sleep(philosopher.speed);
    
                // if philosopher has been desactivated: get out from loop
                philosopher.mutex.lock();	        	        
                if (!philosopher.isActive)
                {
                    break;
                }
                philosopher.mutex.unlock();
    
    
                // if philosopher has the two forks left and right
                if (philosopher.haveLeftFork && philosopher.haveRightFork) 
                {
                    /*
                     * philosopher is eating
                     */
                     
                    // if remaining time is more than 3 sec
                    if (philosopher.state > 3) 
                    {
                        // decrement time of 3 sec
                        philosopher.state -= 3;
                        
                        // send status
                        philosopher.push_state ();
                        
                        // lock the mutex
                        philosopher.mutex.lock();
                        
                        // then goto loop
                        continue;
                    }
                    
                    // no more time to eat (<= 3sec)
                    philosopher.state = 0;
    
                    // release the left and right forks
                    left.release ();
                    philosopher.haveLeftFork = false;
                    right.release ();
                    philosopher.haveRightFork = false;
    
                    System.out.println("Philo: " + philosopher.name + " has eaten.");
                      
                    // send status          
                    philosopher.push_state ();
                    
                    // lock the mutex
                    philosopher.mutex.lock();
                    
                    // then goto loop
                    continue;
                }
                
                
                // Philosopher does not eat
                
                // increment time without eating
                philosopher.state++;
    
                // if < 3sec he is thinking
                if (philosopher.state < 3) 
                {
                   System.out.println("Philo: " + philosopher.name + " is thinking.");
                   
                   // send status          
                   philosopher.push_state ();
                   
                   // lock the mutex
                   philosopher.mutex.lock();
                   
                   // then goto loop
                   continue;
                }
    
                // if = 3sec he is becoming hungry
                if (philosopher.state == 3) 
                {
                    System.out.println("Philo: " + philosopher.name + " is becoming hungry.");
                }
                
                // if = 10sec he is becoming starving
                else if (philosopher.state == 10) 
                {
                    System.out.println("Philo: " + philosopher.name + " is becoming starving.");
                }
                
                // if > 10sec and < 40sec he is starving
                else if (philosopher.state > 10 &&
                         philosopher.state < 40) 
                {
                    System.out.println("Philo: " + philosopher.name + " is starving.");
                }
                
                // if = 40sec he is dead
                else if (philosopher.state == 40) 
                {
                    System.out.println("Philo: " + philosopher.name + " is dead.");
                }
                
                // if left fork is missing
                if (!philosopher.haveLeftFork) 
                {
                    // try to get a fork on left facet
                    try
                    {
                        left.get ();
                        philosopher.haveLeftFork = true;
                    }
                    catch (com.acme.Dinner.InUse e) 
                    {
                        // left fork is already used
                        System.out.println("Philo: " + philosopher.name + " has its left fork used.");
                    }
    
                    // if philosopher has got the left fork
                    if (philosopher.haveLeftFork) 
                    {
                        System.out.println("Philo: " + philosopher.name + " has got the left fork.");
                               
                        // send status
                        philosopher.push_state ();
                        
                        // lock the mutex
                        philosopher.mutex.lock();
                        
                        // then goto loop
                        continue;
                    }
                }
                
                // if right fork is missing
                if (!philosopher.haveRightFork) 
                {
                    // try to get a fork on right facet
                    try 
                    {
                        right.get ();
                        philosopher.haveRightFork = true;
                    }
                    catch (com.acme.Dinner.InUse e) 
                    {
                        // right fork is already used
                        System.out.println("Philo: " + philosopher.name + " has its right fork used.");
                    }
    
                    // if philosopher has got the right fork
                    if (philosopher.haveRightFork) 
                    {
                        System.out.println("Philo: " + philosopher.name + " has got the right fork.");
                                   
                        // send status
                        philosopher.push_state ();
                        
                        // lock the mutex
                        philosopher.mutex.lock();
                        
                        // then goto loop
                        continue;
                    }
               }
               
               // if one of the forks is missing
               if (((philosopher.haveLeftFork && !philosopher.haveRightFork) ||
                    (!philosopher.haveLeftFork && philosopher.haveRightFork)) 
                    && ((Math.random() % 3) == 0)) 
               {
                    // if philosopher has left fork : randomly drop it
                    if (philosopher.haveLeftFork) 
                    {
                        left.release ();
                        philosopher.haveLeftFork = false;
                        System.out.println("Philo: Deadlock prevention! " + philosopher.name +
                                           " drops the left fork.");
                    }
                    
                    // if philosopher has right fork : randomly drop it
                    if (philosopher.haveRightFork) 
                    {
                        right.release ();
                        philosopher.haveRightFork = false;
                        System.out.println("Philo: Deadlock prevention! " + philosopher.name +
                                           " drops the right fork.");
                    }
               }
               
               // send status
               philosopher.push_state ();
               
               // lock the mutex
               philosopher.mutex.lock();
               
            } // End of while (is_active)
           
            // unlock the mutex
            philosopher.mutex.unlock();
            
            System.out.println("End of dinner for " + philosopher.name);
         
        }
        
        catch (org.omg.CORBA.SystemException e) 
        {
            System.out.println("CORBA System Exception : \n" + e.toString());
        }
        
        catch (Exception e)
        {
            System.out.println("Philo: Unexpected exception raised!\n" + e.toString());
            e.printStackTrace();
        }        
    }
    
}

