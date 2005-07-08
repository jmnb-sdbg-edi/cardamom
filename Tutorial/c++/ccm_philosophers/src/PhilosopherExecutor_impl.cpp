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


// Unix Files
#include <stdlib.h>


// Cardamom Files
#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>


#include "PhilosopherExecutor_impl.hpp"
#include "StatusInfo_impl.hpp"


// anonymous namespace

namespace 
{
	
void MySrand ()
{
  static bool initialized = false;

  if (!initialized) 
  {
	  Cdmw::OsSupport::OS::Timeval tv =
		         Cdmw::OsSupport::OS::get_time ();
		         
	  Cdmw::OsSupport::OS::ProcessId pid =
		         Cdmw::OsSupport::OS::get_processId();
		         
      ::srand ((unsigned int) (tv.microseconds + pid) % 991);
    
      initialized = true;
  }
}

int MyRand ()
{
  MySrand ();
  
  return (int) rand ();
}

} // End of anonymous namespace


namespace Dinner 
{
    
PhiloActivationThread::PhiloActivationThread(PhilosopherExecutor_impl* philosopher)
		 : m_philosopher(philosopher)
{
}

PhiloActivationThread::~PhiloActivationThread() throw() 
{
}


  
void PhiloActivationThread::run() throw()
{
    try
    {
        // get register connection
        Registration_var register_server =
                       m_philosopher->m_context->get_connection_register();
        
        // register the philosopher and get its name              
        CORBA::String_var philosopher_name = register_server->_cxx_register();
        
        // store the phisopher name
	    m_philosopher->set_name(philosopher_name.in());
	    std::cout << "Philo: "  << philosopher_name.in() 
	              << " is active, speed is " << m_philosopher->m_speed 
	              << std::endl;
	              
	    // push config from emitter port 
	    std::cout << "Philo: push_config using emitter port..." << std::endl; 
     
		StatusInfo_var si = new StatusInfo_impl;
        si->name (m_philosopher->m_name.in());
	    si->state (THINKING);
        si->secs_since_last_meal (m_philosopher->m_state);
        si->has_left_fork (m_philosopher->m_have_left_fork);
        si->has_right_fork (m_philosopher->m_have_right_fork);

        m_philosopher->m_context->push_config (si.in());
        
        
        //get the connection to the Fork manager
	    std::cout << "Philo: Get the fork connections..." << std::endl;
	    
	    Fork_var left = m_philosopher->m_context->get_connection_left();
	    Fork_var right = m_philosopher->m_context->get_connection_right();
	    
	    // let philosopher try to eat
	    
	    // lock the mutex
	    m_philosopher->m_mutex.lock();
	    
	    // while thread is not requested to stop
	    while (m_philosopher->m_is_active)
	    {
	        // unlock the mutex
	        m_philosopher->m_mutex.unlock();
	        
	        // sleep
		    Cdmw::OsSupport::OS::sleep(m_philosopher->m_speed);

            // if philosopher has been desactivated: get out from loop
	        m_philosopher->m_mutex.lock();	        	        
			if (!m_philosopher->m_is_active)
			{
		        break;
			}
	        m_philosopher->m_mutex.unlock();


            // if philosopher has the two forks left and right
		    if (m_philosopher->m_have_left_fork && m_philosopher->m_have_right_fork) 
		    {
  	            /*
	             * philosopher is eating
	             */
	             
	            // if remaining time is more than 3 sec
	            if (m_philosopher->m_state > 3) 
			   	{
			   	    // decrement time of 3 sec
	                m_philosopher->m_state -= 3;
	                
	                // send status
	                m_philosopher->push_state ();
	                
	                // lock the mutex
	                m_philosopher->m_mutex.lock();
	                
	                // then goto loop
			   		continue;
	            }
	            
	            // no more time to eat (<= 3sec)
	            m_philosopher->m_state = 0;

                // release the left and right forks
	            left->release ();
	            m_philosopher->m_have_left_fork = false;
	            right->release ();
	            m_philosopher->m_have_right_fork = false;

		   		std::cout << "Philo: " << m_philosopher->m_name.in() 
		   		          << " has eaten." << std::endl;
		   		  
		   		// send status          
	            m_philosopher->push_state ();
	            
	            // lock the mutex
	            m_philosopher->m_mutex.lock();
	            
	            // then goto loop
	            continue;
            }
            
            
            // Philosopher does not eat
            
            // increment time without eating
            m_philosopher->m_state++;

            // if < 3sec he is thinking
            if (m_philosopher->m_state < 3) 
		    {
		   	   std::cout << "Philo: " << m_philosopher->m_name.in() 
		   	             << " is thinking." << std::endl;
		   	   
		   	   // send status          
	           m_philosopher->push_state ();
	           
	           // lock the mutex
	           m_philosopher->m_mutex.lock();
	           
	           // then goto loop
	           continue;
            }

            // if = 3sec he is becoming hungry
            if (m_philosopher->m_state == 3) 
		    {
		   	    std::cout  << "Philo: "<< m_philosopher->m_name.in() 
		   	               << " is becoming hungry." << std::endl;
            }
            
            // if = 10sec he is becoming starving
            else if (m_philosopher->m_state == 10) 
		    {
			    std::cout  << "Philo: "<< m_philosopher->m_name.in() 
			               << " is becoming starving." << std::endl;
            }
            
            // if > 10sec and < 40sec he is starving
            else if (m_philosopher->m_state > 10 &&
                     m_philosopher->m_state < 40) 
		    {
			    std::cout  << "Philo: "<< m_philosopher->m_name.in() 
			               << " is starving." << std::endl;
            }
            
            // if = 40sec he is dead
            else if (m_philosopher->m_state == 40) 
		    {
			    std::cout  << "Philo: "<< m_philosopher->m_name.in() 
			               << " is dead." << std::endl;
            }
            
            // if left fork is missing
            if (!m_philosopher->m_have_left_fork) 
		    {
		        // try to get a fork on left facet
		        try
		        {
	                left->get ();
	                m_philosopher->m_have_left_fork = true;
	            }
	            catch (const InUse& ) 
			   	{
			   	    // left fork is already used
	                std::cout  << "Philo: "<< m_philosopher->m_name.in() 
	                           << " has its left fork used." << std::endl;
	            }

                // if philosopher has got the left fork
	            if (m_philosopher->m_have_left_fork) 
			   	{
	                std::cout  << "Philo: "<< m_philosopher->m_name.in() 
	                           << " has got the left fork." << std::endl;
	                       
	                // send status
	                m_philosopher->push_state ();
	                
	                // lock the mutex
	                m_philosopher->m_mutex.lock();
	                
	                // then goto loop
	                continue;
	            }
            }
            
            // if right fork is missing
            if (!m_philosopher->m_have_right_fork) 
		    {
		        // try to get a fork on right facet
	            try 
		   		{
	                right->get ();
	                m_philosopher->m_have_right_fork = true;
	            }
	            catch (const InUse& ) 
			   	{
			   	    // right fork is already used
	                std::cout  << "Philo: "<< m_philosopher->m_name.in() 
	                           << " has its right fork used." << std::endl;
	            }

                // if philosopher has got the right fork
	            if (m_philosopher->m_have_right_fork) 
			   	{
	                std::cout  << "Philo: "<< m_philosopher->m_name.in() 
	                           << " has got the right fork." << std::endl;
	                           
	                // send status
	                m_philosopher->push_state ();
	                
	                // lock the mutex
	                m_philosopher->m_mutex.lock();
	                
	                // then goto loop
	                continue;
	            }
           }
           
           // if one of the forks is missing
           if (((m_philosopher->m_have_left_fork && !m_philosopher->m_have_right_fork) ||
	            (!m_philosopher->m_have_left_fork && m_philosopher->m_have_right_fork)) 
	            && ((rand() % 3) == 0)) 
		   {
		        // if philosopher has left fork : randomly drop it
	            if (m_philosopher->m_have_left_fork) 
		   		{
	                left->release ();
	                m_philosopher->m_have_left_fork = false;
	                std::cout  << "Philo: "<< "Deadlock prevention! " << m_philosopher->m_name.in()
		                       << " drops the left fork." << std::endl;
	            }
	            
	            // if philosopher has right fork : randomly drop it
	            if (m_philosopher->m_have_right_fork) 
			   	{
	                right->release ();
	                m_philosopher->m_have_right_fork = false;
			   	    std::cout  << "Philo: "<< "Deadlock prevention! " << m_philosopher->m_name.in()
		                       << " drops the right fork." << std::endl;
	            }
           }
           
           // send status
           m_philosopher->push_state ();
           
           // lock the mutex
	       m_philosopher->m_mutex.lock();
	       
        } // End of while (is_active)
       
        // unlock the mutex
	    m_philosopher->m_mutex.unlock();
	    
	    std::cout << "End of dinner for " << m_philosopher->m_name.in() 
	              << std::endl;
     
    }
    
    catch (const CORBA::UserException & e) 
    {
        std::cout << "CORBA User Exception : \n" << e << std::endl;
    }
    
    catch (const CORBA::SystemException & e) 
    {
        std::cout << "CORBA System Exception : \n" << e << std::endl;
    }
    
    catch (...)
	{
	    std::cout << "Philo: Unexpected exception raised!" << std::endl;
	}
}



//
// IDL:acme.com/Dinner/CCM_Philosopher:1.0
//
PhilosopherExecutor_impl::PhilosopherExecutor_impl()
	: m_name(""),
	  m_state(0),
	  m_is_active(false),	  
	  m_have_left_fork(false),
	  m_have_right_fork(false),
	  m_context(CCM_Philosopher_Context::_nil())
{
    // set speed
	m_speed = 1500 + (MyRand() % 13) * 300;
	  
	// create thread
	m_activation_thread = new PhiloActivationThread(this);
}



PhilosopherExecutor_impl::~PhilosopherExecutor_impl()
{
    std::cout << "PhilosopherExecutor_impl::~PhilosopherExecutor_impl called!" << std::endl;
}


//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void PhilosopherExecutor_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout  << "Philo: "<< "Set_session_context called!" << std::endl;
   
    m_context = CCM_Philosopher_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void PhilosopherExecutor_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    // if thread not yet started, start it
   if (! m_is_active)
   {
      m_is_active = true;
      
      m_activation_thread->start();
   }
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//
void PhilosopherExecutor_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout  << "Philo: "<< "Ccm_passivate called!" << std::endl;

   // first lock mutex
   m_mutex.lock();
   
   // if thread not started
   if (!m_is_active) 
   {
       // unlock mutex and return
	    m_mutex.unlock();
	    return;
   }

   // thread has been started so stop it then unlock mutex
   m_is_active = false;
   m_mutex.unlock();

   std::cout  << "Philo: "<< m_name.in() << " is inactive." << std::endl;

   // join the thread
   try
   {
	   m_activation_thread->join();
   }
   catch (const Cdmw::OsSupport::DeadlockException& )
   {
	   std::cout  << "Deadlock Exception raised!" << std::endl;
   }
   catch (const Cdmw::InternalErrorException& )
   {
	   std::cout  << "InternalError Exception raised!" << std::endl;
   }

   std::cout  << "Philo: "<< m_name.in() << " thread has join" << std::endl;


   // if philosopher has the left fork, release it
   if (m_have_left_fork) 
   {
       try 
	   {
	       std::cout  << "Philo: "<< m_name.in() << " release left fork!" << std::endl;
	       
	       Fork_var left = m_context->get_connection_left ();
	       
		   if (!CORBA::is_nil(left.in()))
		   {
	           left->release ();
	           
	           std::cout  << "Philo: "<< m_name.in() << " has released left fork!" << std::endl;
			   m_have_left_fork = false;
		   }
	   } 
	   catch (...) 
	   {
	       std::cout  << "Philo: "<< m_name.in() << " Exception raised" << std::endl;
	   }
   }

   // if philosopher has the right fork, release it
   if (m_have_right_fork) 
   {
       try 
	   {
	       std::cout  << "Philo: "<< m_name.in() << " release right fork!" << std::endl;
	       
	       Fork_var right = m_context->get_connection_right ();
	       
		   if (!CORBA::is_nil(right.in()))
		   {
	           right->release ();
	           
	           std::cout  << "Philo: "<< m_name.in() << " has released right fork!" << std::endl;
	           m_have_right_fork = false;
		   }
	    } 
		catch (...) 
		{
	        std::cout  << "Philo: "<< m_name.in() << " Exception raised" << std::endl;
	    }
   }
} 



//
// ccm_remove function : it destroys the component (call by the container)
//
void PhilosopherExecutor_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout  << "Philo: "<< "Ccm_remove called!" << std::endl;
}



//
// IDL:acme.com/Dinner/CCM_Philosopher_Executor/name:1.0
//
char* PhilosopherExecutor_impl::name()
    throw(CORBA::SystemException)
{
    char* name = CORBA::string_dup(m_name.in());
    return name;
}

void PhilosopherExecutor_impl::set_name(const char* name)
{
    m_mutex.lock();
    m_name = CORBA::string_dup(name);
    m_mutex.unlock();
}


//
// push status info event
//
void PhilosopherExecutor_impl::push_state ()
{
    // create a new status info event
    StatusInfo_var si = new StatusInfo_impl;
    
    si->name (m_name.in());

    // if philosopher has two forks (left and right)
    if (m_have_left_fork && m_have_right_fork) 
	{
	     si->state (EATING);
    }
    
    // else he cannot eat so
    // he is thinking if time since last meal is < 3 sec
    else if (m_state < 3) 
	{
	     si->state (THINKING);
    }
    
    // else he is hungry if time since last meal is < 10 sec
    else if (m_state < 10) 
	{
	     si->state (HUNGRY);
    }
    
    // else he is starving if time since last meal is < 40 sec
    else if (m_state < 40) 
	{
	     si->state (STARVING);
    }
    
    // else he is dead
    else 
	{
	     si->state (DEAD);
    }

    si->secs_since_last_meal (m_state);
    si->has_left_fork (m_have_left_fork);
    si->has_right_fork (m_have_right_fork);


    // send the event to consummers
    m_context->push_info (si.in());
}


bool PhilosopherExecutor_impl::is_active()
{
    m_mutex.lock();
    bool result = m_is_active;
    m_mutex.unlock();
    
    return result;
}
	 
	 
} // End namespace Dinner

