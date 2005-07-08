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


// Cdmw Files
#include <Foundation/common/Assert.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/ossupport/OS.hpp>


#include <ccm_philo/CCM_Philosopher_impl.hpp>
#include <ccm_philo/StatusInfo_impl.hpp>


namespace 
{
	
void MySrand ()
{
  static bool initialized = false;

  if (!initialized) 
  {
	  Cdmw::OsSupport::OS::Timeval tv 
		  = Cdmw::OsSupport::OS::get_time ();
	  Cdmw::OsSupport::OS::ProcessId pid 
		  = Cdmw::OsSupport::OS::get_processId();
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

/**
*  This class provides an object to test the ccm_activate.
*
**/

void 
Dinner::test_ccm_philosopher_activate::do_tests()
  {
      CDMW_ASSERT(m_philosopher);

	  ::Dinner::Registration_var register_server;
     ::Dinner::CCM_Philosopher_Context_var philosopher_context = m_philosopher->get_context();
	  
     TEST_INFO("Philo: Get_connection_to_register...");
	  bool test_ok = false;
	  try
	  {
	      register_server = philosopher_context->get_connection_register();

			if (!CORBA::is_nil(register_server.in()))
			{
				test_ok = true;
			}
	  }
	  catch (...)
	  {
		  TEST_INFO("Philo: Unexpected exception raised!");
	  }
	  TEST_CHECK(test_ok);

	  // register philosopher
	  TEST_INFO("Philo: Register philosopher...");
     test_ok = false;
	  try
	  {
 			if (!CORBA::is_nil(register_server.in()))
			{
	          CORBA::String_var philosopher_name = register_server->_cxx_register();
	          m_philosopher->set_name(philosopher_name.in());
				 std::cout << "Philo: "  << philosopher_name.in() << " is active, speed is " << m_philosopher->m_speed << std::endl;
			    test_ok = true;
         }
	  }
	  catch (...)
	  {
		    TEST_INFO("Philo: Unexpected exception raised!");
	  }
	  TEST_CHECK(test_ok);

	  // push config from emitter port 
	  TEST_INFO("Philo: push_config using emitter port...");
     test_ok = false;
	  try
	  {
		    Dinner::StatusInfo_var si = new StatusInfo_impl;
          si->name (m_philosopher->m_name.in());
	       si->state (Dinner::THINKING);
          si->secs_since_last_meal (m_philosopher->m_state);
          si->has_left_fork (m_philosopher->m_have_left_fork);
          si->has_right_fork (m_philosopher->m_have_right_fork);

          m_philosopher->m_context->push_config (si.in());
			 test_ok = true;
	  }
	  catch (...)
	  {
		    TEST_INFO("Philo: Unexpected exception raised!");
	  }
	  TEST_CHECK(test_ok);

	 
     //get the connection to the server 
	  TEST_INFO("Philo: Connect to the fork facet...");
	  Dinner::Fork_var left;
	  Dinner::Fork_var right;
     test_ok = false;
	  try
	  {
         left = philosopher_context->get_connection_left();
         right = philosopher_context->get_connection_right();

			if ( (!CORBA::is_nil(left.in())) && (!CORBA::is_nil(right.in())) )
			{
				test_ok = true;
			}
			else
			{
		      TEST_INFO("Philo: Facet reference is nil!");
			}
	  }
	  catch (...)
	  {
		    TEST_INFO("Philo: Unexpected exception raised!");
	  }
	  TEST_CHECK(test_ok);

	  // Let philosopher try to eat
	  try
	  {
	     m_philosopher->m_mutex.lock();
	     while (m_philosopher->m_is_active)
	     {
	        m_philosopher->m_mutex.unlock();
			 int timescale = Cdmw::TestUtils::Testable::get_timescale();
		     Cdmw::OsSupport::OS::sleep(timescale*m_philosopher->m_speed);

	        m_philosopher->m_mutex.lock();
			  if (!m_philosopher->m_is_active)
			  {
				  // philosopher has been desactivated: get out from loop
				  break;
			  }
	        m_philosopher->m_mutex.unlock();

		     if (m_philosopher->m_have_left_fork && m_philosopher->m_have_right_fork) 
		     {
  	            /*
	             * Eating
	             */
	            if (m_philosopher->m_state > 3) 
			   	{
	                m_philosopher->m_state -= 3;
	                m_philosopher->push_state ();
	                m_philosopher->m_mutex.lock();
			   		 continue;
	            }
	            m_philosopher->m_state = 0;

	            try 
			   	{
	                left->release ();
	                m_philosopher->m_have_left_fork = false;
	                right->release ();
	                m_philosopher->m_have_right_fork = false;
	            } 
			   	catch (...) 
		   		{
	            }
		   		std::cout << "Philo: " << m_philosopher->m_name.in() << " has eaten." << std::endl;
	            m_philosopher->push_state ();
	            m_philosopher->m_mutex.lock();
	            continue;
           }

           m_philosopher->m_state++;

           if (m_philosopher->m_state < 3) 
		     {
		   	   std::cout << "Philo: " << m_philosopher->m_name.in() << " is thinking." << std::endl;
	            m_philosopher->push_state ();
	            m_philosopher->m_mutex.lock();
	            continue;
           }

           if (m_philosopher->m_state == 3) 
		     {
		   	  std::cout  << "Philo: "<< m_philosopher->m_name.in() << " is becoming hungry." << std::endl;
           }
           else if (m_philosopher->m_state >= 10) 
		     {
			     std::cout  << "Philo: "<< m_philosopher->m_name.in() << " is starving." << std::endl;
           }

           if (!m_philosopher->m_have_left_fork) 
		     {
	            try 
		   		{
	                left->get ();
	                m_philosopher->m_have_left_fork = true;
	            }
	            catch (const Dinner::InUse& ) 
			   	{
	                std::cout  << "Philo: "<< m_philosopher->m_name.in() << " has its left fork used." << std::endl;
	            }
					catch (const CORBA::Exception&)
					{
					}

	            if (m_philosopher->m_have_left_fork) 
			   	{
	                std::cout  << "Philo: "<< m_philosopher->m_name.in() << " has got the left fork." << std::endl;
	                m_philosopher->push_state ();
	                m_philosopher->m_mutex.lock();
	                continue;
	            }
	            else 
			   	{
	                // std::cout << _name << " did not get the left fork." << std::endl;
	            }
           }


           if (!m_philosopher->m_have_right_fork) 
		     {
	            try 
		   		{
	                right->get ();
	                m_philosopher->m_have_right_fork = true;
	            }
	            catch (const Dinner::InUse& ) 
			   	{
	                std::cout  << "Philo: "<< m_philosopher->m_name.in() << " has its right fork used." << std::endl;
	            }
					catch (const CORBA::Exception&)
			   	{
	            }

	            if (m_philosopher->m_have_right_fork) 
			   	{
	                std::cout  << "Philo: "<< m_philosopher->m_name.in() << " has got the right fork." << std::endl;
	                m_philosopher->push_state ();
	                m_philosopher->m_mutex.lock();
	                continue;
	            }
	            else 
			   	{
	                // std::cout << m_philosopher->m_name.in() << " did not get the right fork." << std::endl;
	            }
           }

           if (((m_philosopher->m_have_left_fork && !m_philosopher->m_have_right_fork) ||
	            (!m_philosopher->m_have_left_fork && m_philosopher->m_have_right_fork)) && ((rand() % 3) == 0)) 
		     {
	            if (m_philosopher->m_have_left_fork) 
		   		{
	                try 
		   			 {
	                    left->release ();
	                    m_philosopher->m_have_left_fork = false;
	                    std::cout  << "Philo: "<< "Deadlock prevention! " << m_philosopher->m_name.in()
		                           << " drops the left fork." << std::endl;
	                }
	                catch (...) 
			   		 {
	                }
	            }
	            if (m_philosopher->m_have_right_fork) 
			   	{
	                try 
			   		 {
	                    right->release ();
	                    m_philosopher->m_have_right_fork = false;
			   			  std::cout  << "Philo: "<< "Deadlock prevention! " << m_philosopher->m_name.in()
		                           << " drops the right fork." << std::endl;
	                }
	                catch (...) 
			   		 {
	                }
	            }
           }

           m_philosopher->push_state ();
	        m_philosopher->m_mutex.lock();
       } // End of while (is_active)
	    m_philosopher->m_mutex.unlock();
	    std::cout << "End of dinner for " << m_philosopher->m_name.in() << std::endl;
	  }
	  catch (const CORBA::SystemException& e)
	  {
		  std::cout << "System Exception raised: " << e << std::endl;
	  }
	  catch (const CORBA::Exception& e)
	  {
		  std::cout << "CORBA Exception raised: " << e << std::endl;
	  }
  } // End of do_test()

	

//
// IDL:acme.com/Dinner/CCM_Philosopher:1.0
//
Dinner::CCM_Philosopher_impl::CCM_Philosopher_impl()
	: m_name(""),
	  m_is_active(false),
	  m_state(0),
	  m_have_left_fork(false),
	  m_have_right_fork(false),
	  m_context(Dinner::CCM_Philosopher_Context::_nil())
{
	  m_speed = 1500 + (MyRand() % 13) * 300;
	  m_activation_thread = new PhiloActivationThread(this);
}

Dinner::CCM_Philosopher_impl::~CCM_Philosopher_impl()
{
    std::cout << "CCM_Philosopher_impl::~CCM_Philosopher_impl called!" << std::endl;
}


//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void
Dinner::CCM_Philosopher_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout  << "Philo: "<< "Set_session_context called!" << std::endl;
   
    m_context = ::Dinner::CCM_Philosopher_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void
Dinner::CCM_Philosopher_impl::ccm_activate()
   throw(CORBA::SystemException)
{
   if (! m_is_active)
	{
		m_is_active = true;
	   //test_ccm_philosopher_activate test(this);
	   //test.start();
		
		// Start thread
      m_activation_thread->start();
	}
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void
Dinner::CCM_Philosopher_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout  << "Philo: "<< "Ccm_passivate called!" << std::endl;

	m_mutex.lock();
   if (!m_is_active) 
	{
	    m_mutex.unlock();
	    return;
   }

   m_is_active = false;
	m_mutex.unlock();

	std::cout  << "Philo: "<< m_name.in() << " is inactive." << std::endl;

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

   if (m_have_left_fork) 
	{
	    try 
		 {
	        std::cout  << "Philo: "<< m_name.in() << " release left fork!" << std::endl;
	        Dinner::Fork_var left = m_context->get_connection_left ();
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

   if (m_have_right_fork) 
	{
	    try 
		 {
	        std::cout  << "Philo: "<< m_name.in() << " release right fork!" << std::endl;
	        Dinner::Fork_var right = m_context->get_connection_right ();
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
void
Dinner::CCM_Philosopher_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout  << "Philo: "<< "Ccm_remove called!" << std::endl;
}

// push status info event
void 
Dinner::CCM_Philosopher_impl::push_state ()
{
    Dinner::StatusInfo_var si = new StatusInfo_impl;
    si->name (m_name.in());

    if (m_have_left_fork && m_have_right_fork) 
	 {
	     si->state (Dinner::EATING);
    }
    else if (m_state < 3) 
	 {
	     si->state (Dinner::THINKING);
    }
    else if (m_state < 10) 
	 {
	     si->state (Dinner::HUNGRY);
    }
    else if (m_state < 40) 
	 {
	     si->state (Dinner::STARVING);
    }
    else 
	 {
	     si->state (Dinner::DEAD);
    }

    si->secs_since_last_meal (m_state);
    si->has_left_fork (m_have_left_fork);
    si->has_right_fork (m_have_right_fork);

    m_context->push_info (si.in());
}


//
// IDL:acme.com/Dinner/CCM_Philosopher_Executor/name:1.0
//
char*
Dinner::CCM_Philosopher_impl::name()
    throw(CORBA::SystemException)
{
    char* name = CORBA::string_dup(m_name);
    return name;
}

