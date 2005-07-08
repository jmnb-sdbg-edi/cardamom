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


#ifndef INCL_PLATFORMMNGT_APPLICATION_OBSERVER_IMPL_HPP
#define INCL_PLATFORMMNGT_APPLICATION_OBSERVER_IMPL_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSystem.skel.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class System_impl;


/**
 *Purpose:
 *<p> Provides the event notifications to the processes of an application.
 */
class ApplicationObserver_impl : virtual public POA_CdmwPlatformMngt::ApplicationObserver,
                                 virtual public PortableServer::RefCountServantBase
{

public:

    /**
     *Purpose:
     *<p> Constructor.
     */
    ApplicationObserver_impl (System_impl* system);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationObserver/notify_initialisation:1.0
     * operator
     */
    void notify_initialisation (const char* application_name)        
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CORBA::SystemException);


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationObserver/notify_degradation:1.0
     * operator
     */
    void notify_degradation (const char* application_name)        
            throw (CdmwPlatformMngt::ApplicationNotFound,
                   CORBA::SystemException);
                   
                                      
                   
private:

    /**
     * The system reponsible for the applications.
     */
    System_impl* m_system;

    /**
     *Purpose:
     *<p> Default constructor is not allowed.
     */
    ApplicationObserver_impl()
    {};

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ApplicationObserver_impl (ApplicationObserver_impl& rhs)
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ApplicationObserver_impl& operator= (const ApplicationObserver_impl& rhs)
    {
        return *this;
    };

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_OBSERVER_IMPL_HPP

