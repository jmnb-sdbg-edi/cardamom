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


#ifndef INCL_PLATFORMMNGT_PROCESS_IMPL_HPP 
#define INCL_PLATFORMMNGT_PROCESS_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtProcess.skel.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


class ProcessBehaviour;


/**
* Purpose:
* <p>
* The default implementation of a managed process.
*/
class Process_impl : virtual public POA_CdmwPlatformMngt::Process,
    virtual public PortableServer::RefCountServantBase
{

public:

    /**
    * Purpose:
    * <p> Constructor.
    *  Takes ownership of the supplied behaviour which will be destroyed
    *  by the destructor.
    */ 
    Process_impl(ProcessBehaviour *behaviour);


    /**
    * Purpose:
    * <p> Destructor.
    */ 
    virtual ~Process_impl() throw ();

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
    * attribute
    */
    virtual CORBA::ULong nb_steps()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_activity_points:1.0
	* attribute
	*/
    virtual CORBA::ULong nb_activity_points()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
	* operation
	*/
    virtual CORBA::Object_ptr get_service()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_pull_monitorable:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::PullMonitorable_ptr get_pull_monitorable()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_push_monitorable:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::PushMonitorable_ptr get_push_monitorable()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_activity_point:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::Process::ActivityPointInfo* get_activity_point(CORBA::ULong point_index)
        throw(CdmwPlatformMngt::Process::OutOfRange,
              CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_all_activity_points:1.0
	* operation
	*/
    virtual CdmwPlatformMngt::Process::ActivityPointInfos* get_all_activity_points()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
	* operation
	*/
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::Process::BadOrder, CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
	* operation
	*/
    virtual void next_step()
        throw(CdmwPlatformMngt::Process::BadOrder,
              CdmwPlatformMngt::Process::InvalidStep,
              CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
	* operation
	*/
    virtual void run()
        throw(CdmwPlatformMngt::Process::NotReadyToRun,
              CdmwPlatformMngt::Process::AlreadyDone,
              CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
	* operation
	*/
    virtual void stop()
        throw(CORBA::SystemException);


private:

    /**
     *Purpose:
     *<p> The actual behaviour of the process.
     */
    ProcessBehaviour *m_behaviour;


};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROCESS_IMPL_HPP

