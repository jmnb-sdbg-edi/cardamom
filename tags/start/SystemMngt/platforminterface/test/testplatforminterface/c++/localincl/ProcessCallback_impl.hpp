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


#ifndef INCL_PLATFORMSIMU_PROCESSCALLBACK_IMPL_HPP 
#define INCL_PLATFORMSIMU_PROCESSCALLBACK_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/platformlibrary/AcknowledgementCallback_impl.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.skel.hpp"



namespace Cdmw
{
namespace PlatformInterface
{


/**
* Purpose:
* <p>
* implements the
* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback:1.0
* interface
*/
class ProcessCallback_impl : virtual public POA_CdmwPlatformMngt::ProcessCallback,
                             virtual public PortableServer::RefCountServantBase,
                             virtual public PlatformMngt::AcknowledgementCallback_impl
{
public:

    /**
    * Purpose:
    * <p> Constructor
    */ 
    ProcessCallback_impl(
              const CdmwPlatformMngt::ProcessCallback::ProcessAck& acknowledgementData);

    /**
    * Purpose:
    * <p> Destructor
    */ 
    virtual ~ProcessCallback_impl() throw();

    /**
    * Purpose:
    * <p> Returns the acknowledged managed process.
    */ 
    CdmwPlatformMngt::Process_ptr getProcess();


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/set_ready:1.0
    * operation
    */
    virtual CdmwPlatformMngt::ProcessCallback::ProcessAck* set_ready(
            CdmwPlatformMngt::Process_ptr the_process)
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessCallback/notify_error:1.0
    * operation
    */
     virtual void notify_error(const char* issuer, const char* error_info)
         throw(CORBA::SystemException);


protected:

    /**
    * The data returned to the process upon its acknowledgemnt.
    */
    CdmwPlatformMngt::ProcessCallback::ProcessAck_var m_acknowledgementData;

    /**
    * The reference to the managed process.
    */
    CdmwPlatformMngt::Process_var m_process;


};


} // End namespace PlatformSimu
} // End namespace Cdmw

#endif // INCL_PLATFORMSIMU_PROCESSCALLBACK_IMPL_HPP

