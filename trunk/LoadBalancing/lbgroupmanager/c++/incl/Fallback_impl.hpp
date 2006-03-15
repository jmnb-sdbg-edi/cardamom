/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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


#ifndef _CDMW_LB_GROUP_MANAGER_FALLBACK_IMPL_HPP_
#define _CDMW_LB_GROUP_MANAGER_FALLBACK_IMPL_HPP_

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <Foundation/orbsupport/POADynamicImplementation.hpp>
#include "LoadBalancing/idllib/CdmwLBTypeRepository.stub.hpp"



namespace Cdmw {
namespace LB {
namespace GroupManager {

    /**
     * Default Implementation for fallback objects.
     * A Fallback object only replies to is_a() operations for checking type of the object
     * group but should not process other operations.
     * NOTA:
     *   It is expected that the <invoke> and <_primary_interface>
     *   methods will be only invoked by the POA in the context of
     *   serving a CORBA request. Invoking this method in other
     *   circumstances may lead to unpredictable results.
     */
    class Fallback_impl
        : public virtual PortableServer::DynamicImplementation,
          public virtual Cdmw::OrbSupport::RefCountServantBase
    {

    public:
        /**
         * Creates a fallback default servant
         *
         * @param orb ...
         * @param poa ...
         */
        Fallback_impl(CORBA::ORB_ptr orb, 
                      PortableServer::POA_ptr poa)
            throw(CORBA::SystemException);
        
        /**
         * Destroys the fallback default servant.
         */
        ~Fallback_impl(void) throw ();


        /**
         * The _is_a() method receives a RepositoryId and returns
         * true if the invoked CORBA::Object is an instance of the
         * type with the specified RepositoryId (or an instance of
         * a inherited type).
         * This operation is called instead of the invoke() operation
         * in case of co-localised call.
         *
         * @param logical_type_id The RepositoryId to be compared.
         *
         * @return true if the invoked CORBA::Object is an instance
         * of the type with the specified RepositoryId.
         */
        virtual CORBA::Boolean _is_a (const char *logical_type_id);
        
        /**
         * The invoke() method receives requests issued to any CORBA
         * object incarnated by the DSI servant and performs the
         * processing necessary to execute the request.
         *
         * @param request A ServerRequest local object containing request information.
         *
         * @exception TODO...
         */
        virtual void invoke (CORBA::ServerRequest_ptr request);
        /**
         * Returns the default POA for this servant.
         *
         * @return Default POA for this servant.
         *
         */
        virtual PortableServer::POA_ptr _default_POA() {
            return PortableServer::POA::_duplicate(m_poa.in()); }
        /**
         * The _primary_interface() method receives an ObjectId value and
         * a POA_ptr as input parameters and returns a valid RepositoryId
         * representing the most-derived interface for that oid.
         *
         * @param oid ObjectId of the target object.
         * @param poa PortableServer::POA pseudo-object
         *
         * @return The repository id of the most-derived interface for that oid.
         *
         * @exception TODO...
         */
        virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &oid,
                                                        PortableServer::POA_ptr poa);
        

    private:
        // The ORB pseudo-object.
        CORBA::ORB_var m_orb;
        
        // The parent POA.
        PortableServer::POA_var m_poa;

        // POA Current.
        PortableServer::Current_var m_current;

	// CARDAMOM LB Type Repository.
        CdmwLB::TypeRepository_var m_type_repository;

        /**
         * Helper function to handle the _is_a() operation.
         *
         */
        void handle_is_a(CORBA::ServerRequest_ptr request);


        // Default constructor.
        Fallback_impl(void)
            throw();

        
        // Hide the copy constructor.
        Fallback_impl(const Fallback_impl & fallback)
            throw();
        
        // Hide the copy assignment operator.
        Fallback_impl &
        operator=(const Fallback_impl & fallback)
            throw();
    };

}; // namespace GroupManager
}; // namespace LB
}; // namespace Cdmw

#endif // _CDMW_LB_GROUP_MANAGER_FALLBACK_IMPL_HPP_

