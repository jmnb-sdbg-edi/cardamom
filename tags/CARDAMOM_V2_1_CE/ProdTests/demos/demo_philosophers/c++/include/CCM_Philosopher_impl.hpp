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
#include "Foundation/osthreads/Mutex.hpp"
#include <Foundation/testutils/Testable.hpp>

#include <UserPhilo.skel.hpp>

namespace Dinner
{

// forward declaration
class CCM_Philosopher_impl;

/**
*  This class provides an object to test the ccm_activate.
*
**/

class test_ccm_philosopher_activate : public Cdmw::TestUtils::Testable
{
  Dinner::CCM_Philosopher_impl* m_philosopher;

 public:
  test_ccm_philosopher_activate(Dinner::CCM_Philosopher_impl* philosopher)
      : Testable("Test ccm_activate on Philosopher"),
        m_philosopher(philosopher)
  {
  };

  ~test_ccm_philosopher_activate() {};


  void do_tests();

};



class PhiloActivationThread : public Cdmw::OsSupport::Thread 
{
public:
    PhiloActivationThread(Dinner::CCM_Philosopher_impl* philosopher)
		 : m_philosopher(philosopher)
	 {
	 }

    ~PhiloActivationThread() throw() {};

protected:
    void
    run() throw()
	 {
 	      test_ccm_philosopher_activate test(m_philosopher);
	      test.start();
	 }
    
private:
    
    PhiloActivationThread();
    PhiloActivationThread(const PhiloActivationThread&);
    PhiloActivationThread& operator = (const PhiloActivationThread&);
    
    Dinner::CCM_Philosopher_impl* m_philosopher;
};


//
// IDL:thalesgroup.com/Dinner/CCM_Philosopher:1.0
//
class CCM_Philosopher_impl : virtual public ::Dinner::User_CCM_Philosopher,
                        virtual public Cdmw::OrbSupport::RefCountLocalObject
{
	 friend class Dinner::test_ccm_philosopher_activate;

    CCM_Philosopher_impl(const CCM_Philosopher_impl&);
    void operator=(const CCM_Philosopher_impl&);

	 CORBA::String_var                     m_name;
	 bool                                  m_is_active;
	 CORBA::ULong                          m_state;
    CORBA::ULong                          m_speed;
    bool                                  m_have_left_fork;
    bool                                  m_have_right_fork;
    ::Dinner::CCM_Philosopher_Context_var m_context;

	 PhiloActivationThread*                m_activation_thread;
	 Cdmw::OsSupport::Mutex               m_mutex;

	 
	 
public:

    CCM_Philosopher_impl();
    ~CCM_Philosopher_impl();
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
;

 // additional operation
    void push_state();

    bool is_active()
	 {
		 m_mutex.lock();
		 bool result = m_is_active;
		 m_mutex.unlock();
		 return result;
	 }

	 void set_name(const char* name)
	 {
		 m_mutex.lock();
		 m_name = CORBA::string_dup(name);
		 m_mutex.unlock();
	 }

	 ::Dinner::CCM_Philosopher_Context* get_context()
	 {
		 return ::Dinner::CCM_Philosopher_Context::_duplicate(m_context.in());
	 }

};

}; // End of namespace Dinner


#endif // ___DINNER_CCM_PHILOSOPHER_IMPL_HPP__

