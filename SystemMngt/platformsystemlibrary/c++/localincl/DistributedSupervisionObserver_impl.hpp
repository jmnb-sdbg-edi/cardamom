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


#ifndef INCL_PLATFORMMNGT_DISTRIBUTED_SUPERVISION_OBSERVER_HPP
#define INCL_PLATFORMMNGT_DISTRIBUTED_SUPERVISION_OBSERVER_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtSupervisionObserver.skel.hpp"

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw 
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt 
{

class SupervisionEventNotifier;


class DistributedSupervisionObserver_impl: 
                                virtual public POA_CdmwPlatformMngt::SupervisionObserver,
                                virtual public PortableServer::RefCountServantBase
{
    
public:

    /**
     *Purpose:
     *<p> Default constructor.
     *     
     *@param eventNotifier the supervision observer to be notified of events.
     *
     *@exception BadParameterException if the parameter is invalid.
     */
    DistributedSupervisionObserver_impl (SupervisionEventNotifier* eventNotifier)
            throw (BadParameterException);
        
        
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/SupervisionObserver/notify:1.0
     * operation
     */
                    
    void notify (CdmwPlatformMngt::Event* an_event)
        throw (CORBA::SystemException);
        
private:

    /**
     * The supervision event notifier.
     */
    SupervisionEventNotifier* m_eventNotifier;
    
    
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_DISTRIBUTED_SUPERVISION_OBSERVER_HPP

