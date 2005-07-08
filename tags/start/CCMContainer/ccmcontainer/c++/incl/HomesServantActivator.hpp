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


#ifndef INCL_CDMW_CCM_CONTAINER_HOMES_SERVANT_ACTIVATOR_HPP
#define INCL_CDMW_CCM_CONTAINER_HOMES_SERVANT_ACTIVATOR_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>


namespace Cdmw
{
namespace CCM
{
namespace Container
{

    /**
     * A Servant Activator for home servants.
     */
    class HomesServantActivator 
        : public virtual PortableServer::ServantActivator,
          public virtual Cdmw::OrbSupport::RefCountLocalObject
    {
    public:
        HomesServantActivator ();
        ~HomesServantActivator ();
        
        virtual PortableServer::Servant 
        incarnate(const PortableServer::ObjectId& oid, 
                  PortableServer::POA_ptr         poa)
            throw(PortableServer::ForwardRequest, CORBA::SystemException);

        /**
         * The main purpose of this operation is to provide a hook
         * that will be called by the ORB during its shutdown phase
         * to etherealize component homes objects.
         */ 
        virtual void 
        etherealize(const PortableServer::ObjectId& oid,
                    PortableServer::POA_ptr         poa,
                    PortableServer::Servant         servant,
                    CORBA::Boolean                  cleanup_in_progress,
                    CORBA::Boolean                  remaining_activations)
            throw(CORBA::SystemException);
    };

}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_HOMES_SERVANT_ACTIVATOR_HPP

