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


#ifndef ___DINNER_CCM_PHILOSOPHER_IMPL_HPP__
#define ___DINNER_CCM_PHILOSOPHER_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>
#include <Foundation/osthreads/Mutex.hpp>

#include "UserPhilo.skel.hpp"

namespace Dinner
{

// forward declaration
class PhilosopherExecutor_impl;

class PhiloActivationThread : public Cdmw::OsSupport::Thread 
{
  public:
  
    PhiloActivationThread(PhilosopherExecutor_impl* philosopher);

    ~PhiloActivationThread() throw();

  protected:
  
    void run() throw();
    
  private:
    
    PhiloActivationThread();
    PhiloActivationThread(const PhiloActivationThread&);
    PhiloActivationThread& operator = (const PhiloActivationThread&);
    
    Dinner::PhilosopherExecutor_impl* m_philosopher;
};


//
// IDL:thalesgroup.com/Dinner/CCM_Philosopher:1.0
//
class PhilosopherExecutor_impl : virtual public User_Philosopher,
                                 virtual public Cdmw::OrbSupport::RefCountLocalObject
{
  friend class PhiloActivationThread;
	 
  public:

    PhilosopherExecutor_impl();
    ~PhilosopherExecutor_impl();
    //
    // IDL:thalesgroup.com/Dinner/CCM_Philosopher_Executor/name:1.0
    //
    virtual char* name()
        throw(CORBA::SystemException);
	 

    void set_session_context(Components::SessionContext_ptr ctx)
		  throw(CORBA::SystemException);
    void ccm_activate()
		  throw(CORBA::SystemException);
    void ccm_passivate()
		  throw(CORBA::SystemException);
    void ccm_remove()
		  throw(CORBA::SystemException);

    // additional operations
    
    void set_name(const char* name);
    
    void push_state();

    bool is_active();
    
    

	
  private:
  
    PhilosopherExecutor_impl(const PhilosopherExecutor_impl&);
    void operator=(const PhilosopherExecutor_impl&);

	CORBA::String_var m_name;
	
	CORBA::ULong m_state;
    CORBA::ULong m_speed;
    
    bool m_is_active;
    bool m_have_left_fork;
    bool m_have_right_fork;
    
    CCM_Philosopher_Context_var m_context;

    PhiloActivationThread*    m_activation_thread;
	Cdmw::OsSupport::Mutex   m_mutex;

};

}; // End of namespace Dinner


#endif // ___DINNER_CCM_PHILOSOPHER_IMPL_HPP__
