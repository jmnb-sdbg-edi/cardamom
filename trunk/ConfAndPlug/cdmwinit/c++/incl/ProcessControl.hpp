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


#ifndef INCL_CDMW_INIT_PROCESS_CONTROL_HPP
#define INCL_CDMW_INIT_PROCESS_CONTROL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegate.stub.hpp"
#include "Foundation/osthreads/Mutex.hpp"

namespace Cdmw
{
namespace CdmwInit
{


/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class ProcessControl
{

public:

    /**
     * Purpose:
     * <p> The destructor.
     */ 
    virtual ~ProcessControl() throw ();


    /**
     * Purpose:
     * <p> Reference counting operations
     */ 
    virtual void _add_ref();
    virtual void _remove_ref();

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
     * attribute
     */
    virtual CORBA::ULong nb_steps() throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
     * operation
     */
    virtual CORBA::Object_ptr get_service() throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>    This operation informs the pull-based Fault Detector whether the object is able to
     *    accept requests and produce replies.
     *    This operation may be used to perform some user-defined sanity checks ...
     *
     * @return true if the object is alive and ready to take further requests, and false otherwise.
     *
     */
    virtual bool is_alive() throw(CORBA::SystemException)
    {
        return true;
    };

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    virtual void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException) = 0;
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
     * operation
     */
    virtual void on_next_step()
        throw(CdmwPlatformMngt::ProcessDelegate::InvalidStep, 
	      CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    virtual void on_run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
	      CORBA::SystemException) = 0;
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    virtual void on_stop()
        throw(CORBA::SystemException) = 0;


protected:
    
    ProcessControl() : m_ref_count(1) {}

    ProcessControl(const ProcessControl&) 
	     : m_ref_count(1) {}
    
    ProcessControl&
    operator=(const ProcessControl&) { return *this; }

private:
    
    Cdmw::OsSupport::Mutex  m_count_mutex;
    CORBA::ULong            m_ref_count;

};


/**
*Purpose:
*<p>    This class provide a wrapper for safely manipulate ProcessControl objects.
* It automatically calls _add_ref() and _remove_ref() operations when needed.
* See the T_var description within the OMG IDL to C++ language mapping.
*Features:
*<p> Thread safe
*
*/
class ProcessControl_var
{

public:
    
    ProcessControl_var() throw() : m_ctrl(0) { }

    ProcessControl_var(ProcessControl* p) throw() : m_ctrl(p) { }

    ProcessControl_var(const ProcessControl_var& r) throw()
        : m_ctrl(r.m_ctrl) 
    { 
	     if (m_ctrl != 0)
	          m_ctrl->_add_ref();
    }

    ~ProcessControl_var() throw()
    { 
	     if (m_ctrl != 0)
	         m_ctrl->_remove_ref();
    }
    

    ProcessControl_var& operator=(ProcessControl* p) throw()
    {
	     if (m_ctrl != 0) m_ctrl->_remove_ref();
	         m_ctrl = p;

	     return *this;
    }
    
    ProcessControl_var&
    operator=(const ProcessControl_var& b) throw()
    {
	     if (m_ctrl != b.m_ctrl) 
        {
	         if (m_ctrl != 0) m_ctrl->_remove_ref();

	         if ((m_ctrl = b.m_ctrl) != 0)
		          m_ctrl->_add_ref();
	     }

	     return *this;
    }

    ProcessControl* operator->() const throw() { return m_ctrl; }
    
    ProcessControl*  in() const throw() { return m_ctrl; }
    ProcessControl*& inout() throw() { return m_ctrl; }

    ProcessControl*& out() throw()
    {
	     if (m_ctrl != 0) m_ctrl->_remove_ref();
	         m_ctrl = 0;

	     return m_ctrl;
    }

    ProcessControl* _retn() throw()
    {
	     ProcessControl* retval = m_ctrl;
	         m_ctrl = 0;

	     return retval;
    }

private:

    ProcessControl* m_ctrl;

}; // End class ProcessControl_var


}; // End namespace CdmwInit
}; // End namespace Cdmw

#endif // INCL_CDMW_INIT_PROCESS_CONTROL_HPP

