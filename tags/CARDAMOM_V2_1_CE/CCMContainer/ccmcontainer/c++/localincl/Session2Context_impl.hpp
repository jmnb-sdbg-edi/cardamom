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


#ifndef INCL_CDMW_CCM_CONTAINER_SESSION_2_CONTEXT_IMPL_HPP
#define INCL_CDMW_CCM_CONTAINER_SESSION_2_CONTEXT_IMPL_HPP

#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <CCMContainer/ccmcontainer/ComponentServantProvider.hpp>
#include "CCMContainer/idllib/Components.stub.hpp"
//#include "CosPersistentState.stub.hpp"
//#include "SecurityLevel2.stub.hpp"
#include "CCMContainer/idllib/CdmwDeployment.stub.hpp"
#include "CCMContainer/idllib/CdmwCcmContainer.skel.hpp"
#include "Event/idllib/CdmwEvent.stub.hpp"
#include "ccmcontainer/ValueTypeDependencyList.hpp"



namespace Cdmw
{

namespace CCM
{
namespace Container
{
    /**
     * Implementation of the Session2Context internal interface for use with
     * Session container type.
     */
    class Session2Context_impl :
        public CdmwCcmContainer::Session2Context,
        public Cdmw::OrbSupport::RefCountLocalObject
    {
    public:
        /**
        * Purpose:
        * <p> Constructor.
        *
        *@param orb A reference to the ORB pseudo-object
        *@param parent A parent POA to the home instance-specific POA.
        *@param home A reference to the component's home
        *@param home_id Unique Identifier to a home within the context of its container.
        *@param servant_provider A component/facet servant provider.
        *@param servant_lifetime Servant Lifetime
        *@param threading_policy Threading Policy
        *@param home_registration A reference to a HomeRegistration for use to register component
        * homes so they may be located by the HomeFinder
        *@param executor_shlib_handle Handle of the component's shared/dynamically linked library
        *@param valuetype_dependencies A list of value type dependencies
        *
        *@exception CORBA::SystemException TODO:
        */
        Session2Context_impl(CORBA::ORB_ptr                            orb,
                             PortableServer::POA_ptr                   parent,
                             Components::CCMHome_ptr                   home,
                             const std::string &                       home_id,
                             ComponentServantProvider*                 servant_provider,
                             CdmwDeployment::ServantLifetimeValue     servant_lifetime,
                             CdmwDeployment::ThreadingPolicyValue     threading_policy,
                             Components::HomeRegistration_ptr          home_registration,
                             CdmwEvent::EventChannelFactory_ptr       event_channel_factory,
                             const std::string &                       process_name,
                             const std::string &                       application_name,
                             Cdmw::OsSupport::OS::SharedObjectHandle*  executor_shlib_handle,
                             const ValueTypeDependencyList &           valuetype_dependencies)
            throw (CORBA::SystemException);
        
        /**
        * Purpose:
        * <p> Destructor
        * This will call dlclose();
        *
        */
        ~Session2Context_impl()
            throw ();
        
        //--- From CCMContext -----------------------------------------------//
        //
        // IDL:omg.org/Components/CCMContext/get_caller_principal:1.0
        //
        /**
         * Purpose:
         * <p>The get_caller_principal operation obtains the CORBA security credentials in effect
         * for the caller. Security on the server is primarily controlled by the security policy
         * in the deployment descriptor for this component. The component may use this operation to
         * determine the credentials associated with the current client invocation.
         *
         * <p><B>NOTA:</B> This is NOT implemented in this version.
         *
         *@return A reference to the caller pricipal.
         */
        virtual Components::Principal_ptr get_caller_principal()
            throw (CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/CCMContext/get_CCM_home:1.0
        //
        /**
         * Purpose:
         * <p>The get_CCM_home operation is used to obtain a reference to the home interface.
         *
         *@return A reference to the component's home
         */
        virtual Components::CCMHome_ptr get_CCM_home()
            throw (CORBA::SystemException);
 
        //
        // IDL:omg.org/Components/CCMContext/get_rollback_only:1.0
        //
        /**
         * Purpose:
         * <p>The get_rollback_only operation is used by a component to test if the current
         * transaction has been marked for rollback. When get_rollback_only is issued by a
         * component, it results in a CosTransaction::Current::get_status being issued to
         * the CORBA transaction service and the status value returned being tested for the
         * MARKED_ROLLBACK state.
         *
         * <p><B>NOTA:</B> This is NOT implemented in this version.
         *
         @return <B>TRUE</B> if the transaction has been marked for rollback, otherwise it
         * returns <B>FALSE</B>. 
         *
         *@exception Components::IllegalState Raised if no transaction is active.
         */
        virtual CORBA::Boolean get_rollback_only()
            throw (CORBA::SystemException);
 
        //
        // IDL:omg.org/Components/CCMContext/get_user_transaction:1.0
        //
        /**
         * Purpose:
         * <p>The get_user_transaction operation is used to access the Transaction::UserTransaction
         * interface. The UserTransaction interface is used to implement self-managed transactions.
         *
         * <p><B>NOTA:</B> This is NOT implemented in this version.
         *
         *@return A reference to the UserTransaction object
         *
         *@exception Components::IllegalState Raised if this component is using container-managed
         * transactions.
         */
        virtual ::Components::Transaction::UserTransaction_ptr get_user_transaction()
            throw (CORBA::SystemException);
 
        //
        // IDL:omg.org/Components/CCMContext/is_caller_in_role:1.0
        //
        /**
         * Purpose:
         * <p>The is_caller_in_role operation is used by the CORBA component to compare the
         * current credentials to the credentials defined by the role parameter.
         *
         * <p><B>NOTA:</B> This is NOT implemented in this version.
         *
         *@return <B>TRUE</B> if the current credentials match  the credentials defined by
         * the role parameter; <B>FALSE</B> otherwise.
         */
        virtual CORBA::Boolean is_caller_in_role(const char* role)
            throw (CORBA::SystemException);
 
        //
        // IDL:omg.org/Components/CCMContext/set_rollback_only:1.0
        //
        /**
         * Purpose:
         * <p>The set_rollback_only operation is used by a component to mark an existing
         * transaction for abnormal termination.
         * When set_rollback_only is issued by a component, it results in a
         * CosTransaction::Current::rollback_only being issued to the CORBA transaction service.
         *
         * <p><B>NOTA:</B> This is NOT implemented in this version.
         *
         *@exception Components::IllegalState Raised if no transaction is active.
         */
        virtual void set_rollback_only()
            throw (CORBA::SystemException);

        
        //--- From CCM2Context ----------------------------------------------//
        //
        // IDL:omg.org/Components/CCM2Context/get_home_registration:1.0
        //
        /**
         * Purpose:
         * <p>The get_home_registration operation is used to obtain a reference to the
         * HomeRegistration interface. The HomeRegistration is used to register component
         * homes so they may be located by the HomeFinder.
         *
         *@return A reference to the HomeRegistration object.
         */
        virtual Components::HomeRegistration_ptr get_home_registration()
            throw (CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/CCM2Context/req_passivate:1.0
        //
        /**
         * Purpose:
         * <p>The req_passivate operation is used by the component to inform the container that it
         * wishes to be passivated when its current operation completes.
         * The component must have a servant lifetime policy of component or container.
         *
         *@exception Components::PolicyMismatch Raised if the servant lifetime policy is neither
         * component nor container.
         */
        virtual void req_passivate()
            throw (CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/CCM2Context/get_persistence:1.0
        //
        /**
         * Purpose:
         * <p>The get_persistence operation provides the component access to a persistence
         * framework provided by an implementation of the CORBA persistence state service.
         *
         * <p><B>NOTA:</B> This raises a Components::PersistenceNotAvailable exception
         *
         *@param catalog_type_id Identifier of the CatalogBase
         *
         *@return A reference to the CosPersistentState::CatalogBase which serves as an index
         * to the available storage homes.
         *
         *@exception Components::PersistenceNotAvailable Raised if the CatalogBase identified by
         * <I>catalog_type_id</I> is not available on this container.
         */
        virtual Components::CatalogBase_ptr get_persistence(const char* catalog_type_id)
            throw (CORBA::SystemException);

        //--- From SessionContext -------------------------------------------//
        //
        // IDL:omg.org/Components/SessionContext/get_CCM_object:1.0
        //
        /**
         * Purpose:
         * <p>The get_CCM_object operation is used to get the reference used to invoke the
         * component. For basic components, this will always be the component reference. For
         * extended components, this will be a specific facet reference.
         *
         *@return The object reference used to invoke the component.
         *
         *@exception Components::IllegalState Raised if this operation is issued outside of
         * the scope of a callback operation.
        */
        virtual CORBA::Object_ptr get_CCM_object()
            throw (CORBA::SystemException);
        

        //--- From Session2Context ------------------------------------------//
        //
        // IDL:omg.org/Components/Session2Context/create_ref:1.0
        //
        /**
         * Purpose:
         * <p>The create_ref operation is used to create a reference to be exported to clients to
         * invoke operations. The RepositoryId must match the RepositoryId of the component itself,
         * one of its bases, one of its supported interfaces, or one of its facets.
         *
         *@param repid Identifies the RepositoryId associated with the interface for which
         * a reference is being created.
         *
         *@return A newly created object reference.
         */
        virtual CORBA::Object_ptr create_ref(const char* repid)
            throw (CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Session2Context/create_ref_from_oid:1.0
        //
        /**
         * Purpose:
         * <p>The create_ref_from_oid operation is used to create a reference to be exported to
         * clients which includes information provided by the component which it can use on
         * subsequent operation requests.
         *
         *@param oid Identifies the ObjectIdSeq to be encapsulated in the reference.
         *@param repid Identifies the RepositoryId associated with the interface for which
         * a reference is being created.
         *
         *@return A newly created object reference.
         */
        virtual CORBA::Object_ptr create_ref_from_oid(const ::CORBA::OctetSeq& oid,
                                                      const char* repid)
            throw (CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Session2Context/get_oid_from_ref:1.0
        //
        /**
         * Purpose:
         * <p>The get_oid_from_ref operation is used by the component to extract the oid
         * encapsulated in the reference. This operation must be called within an operation
         * invocation.
         *
         *@param objref Specifies the reference which contains the oid.
         *
         *@return
         *
         *@exception Components::IllegalState Raise if the operation is not called within
         * an operation invocation.
         *@exception Components::BadComponentReference Raised with the WRONG_CONTAINER minor
         * code if the reference was not created by this container.
         */
        virtual ::CORBA::OctetSeq* get_oid_from_ref(CORBA::Object_ptr objref)
            throw (CORBA::SystemException);


        //
        // IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/obtain_event_channel_for_emitter:1.0
        //
        virtual ::CosEventChannelAdmin::EventChannel_ptr
        obtain_event_channel_for_emitter(::Components::PortDescription* port)
            throw (CORBA::SystemException);
        
        //
        // IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/create_event_channel_for_publisher:1.0
        //
        virtual ::CosEventChannelAdmin::EventChannel_ptr
        create_event_channel_for_publisher(::Components::PortDescription* port)
            throw (CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/resolve_initial_references:1.0
        //
        virtual CORBA::Object_ptr
        resolve_initial_references(const char* identifier)
            throw (CdmwCcmContainer::CCM2Context::InvalidName, 
                   CORBA::SystemException);


    private:

        /**
         * Generates an ObjectId for use in create_ref()
         */
        PortableServer::ObjectId* create_ObjectId() throw (std::bad_alloc);
        
        /**
         *[Attribute description]
         */ 
        CORBA::ORB_var                                m_orb;
        PortableServer::Current_var                   m_current;
        PortableServer::POA_var                       m_poa;
        Components::CCMHome_var                       m_home;
        Components::HomeRegistration_var              m_home_registration;
        CdmwDeployment::ServantLifetimeValue          m_servant_lifetime;        
        CdmwEvent::EventChannelFactory_var            m_event_channel_factory;
        std::string                                   m_process_name;
        std::string                                   m_application_name;
        Cdmw::OsSupport::OS::SharedObjectHandle*      m_executor_shlib_handle;
        ValueTypeDependencyList                       m_valuetype_dependencies;
        
        Session2Context_impl();
        Session2Context_impl(const Session2Context_impl&);
    };
}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_SESSION_2_CONTEXT_IMPL_HPP

