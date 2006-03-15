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


#ifndef INCL_CCM_CIF_CONTEXT_HPP
#define INCL_CCM_CIF_CONTEXT_HPP

#include <Foundation/common/Assert.hpp>
#include <CCMContainer/idllib/CdmwCcmContainer.stub.hpp>

namespace Cdmw {

namespace CCM {

namespace CIF {
   
/*
 * This class is the base class for ccmcif context embedding specific CCM 
 * context.
 */
class Context
{
public:
   /**
    * Purpose:
    * <p> Constructor.
    *@param ctx encapsulated context. Each operation of this class are delegated
    * to this context.
    *
    */
   Context(CdmwCcmContainer::CCM2Context_ptr ctx);

    /**
    * Purpose:
    * <p> Destructor.
    */
   virtual ~Context(); 

   /**
    * Purpose:
    * <p> This operation returns the encapsulated ccm context.
    */
   inline CdmwCcmContainer::CCM2Context_ptr get_ccm_context()
   {
      return CdmwCcmContainer::CCM2Context::_duplicate(m_ccm_context.in());
   }
      
   //
   // IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/resolve_initial_references:1.0
   //
   virtual CORBA::Object_ptr resolve_initial_references(const char* identifier)
      throw (CdmwCcmContainer::CCM2Context::InvalidName,
             CORBA::SystemException);

   //
   // IDL:omg.org/Components/CCMContext/get_CCM_home:1.0
   //
   /**
    * Purpose:
    * <p>The get_CCM_home operation is used to obtain a reference to the home 
    * interface.
    *
    *@return A reference to the component's home
    */
   virtual Components::CCMHome_ptr get_CCM_home() 
      throw(CORBA::SystemException);
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
   // IDL:omg.org/Components/Session2Context/create_ref:1.0
   //
   /**
    * Purpose:
    * <p>The create_ref operation is used to create a reference to be exported
    * to clients to invoke operations. The RepositoryId must match the 
    * RepositoryId of the component itself, one of its bases, one of its 
    * supported interfaces, or one of its facets. This operation shall be 
    * implemented by specialised context.
    *
    *@param repid Identifies the RepositoryId associated with the interface for
    * which a reference is being created.
    *
    *@return A newly created object reference.
    */
   virtual CORBA::Object_ptr create_ref(const std::string& rep_id) 
      throw (CORBA::SystemException)= 0;

   //
   // IDL:omg.org/Components/Session2Context/create_ref_from_oid:1.0
   //
   /**
    * Purpose:
    * <p>The create_ref_from_oid_str operation is used to create a reference to be
    * exported to clients which includes information provided by the component
    * which it can use on subsequent operation requests. This operation shall
    * be implemented by specialised context.
    *
    *@param oid Identifies the ObjectIdSeq to be encapsulated in the reference.
    *@param repid Identifies the RepositoryId associated with the interface for
    * which a reference is being created.
    *
    *@return A newly created object reference.
    */
   virtual CORBA::Object_ptr create_ref_from_oid_str(
           const std::string& oid,
           const std::string& rep_id)   
      throw (CORBA::SystemException) = 0;

   //
   // IDL:omg.org/Components/Session2Context/get_oid_from_ref:1.0
   //
   /**
    * Purpose:
    * <p>The get_oid_from_ref operation is used by the component to extract the
    * oid encapsulated in the reference. This operation must be called within an
    * operation invocation. This operation shall be implemented by specialised
    * context.
    *
    *@param objref Specifies the reference which contains the oid.
    *@return the oid.
    *
    *@exception Components::IllegalState Raise if the operation is not called 
    * within an operation invocation.
    *@exception Components::BadComponentReference Raised with the 
    * WRONG_CONTAINER minor code if the reference was not created by this 
    * container.
    */
   virtual char* get_oid_str_from_ref(const CORBA::Object_ptr comp_ref) 
      throw (CORBA::SystemException) = 0;
   
   //
   // IDL:omg.org/Components/Session2Context/get_oid_from_ref:1.0
   //
   /**
    * Purpose:
    * <p>The get_primary_key operation return the primary key when component 
    * is an entity component. This operation shall be implemented by specialised
    * context.
    * *@exception Components::NoKeyAvailable Raise if the component is not an
    * entity one.
    */
   virtual Components::PrimaryKeyBase* get_primary_key() 
      throw(Components::NoKeyAvailable, CORBA::SystemException) = 0;
   
   //
   // IDL:omg.org/Components/CCM2Context/req_passivate:1.0
   //
   /**
    * The req_passivate operation is used by the component to inform the 
    * container that it wishes to be passivated when its current operation 
    * completes. To be valid, the component must have a servant lifetime policy
    * of component or container. If not, the PolicyMismatch exception shall be
    * raised. This operation shall be implemented by specialised context.
    */
   virtual void req_passivate()
      throw (CORBA::SystemException) = 0;
   
   //
   // IDL:omg.org/Components/SessionContext/get_CCM_object:1.0
   //
   /**
    * The get_CCM_object operation is used to get the reference used to invoke 
    * the component. For basic components, this will always be the component 
    * reference. For extended components, this will be a specific facet 
    * reference. If this operation is issued outside of the scope of a callback
    * operation, the IllegalState exception is returned. This operation shall 
    * be implemented by specialised context.
    */
   virtual CORBA::Object_ptr get_CCM_object()
      throw (CORBA::SystemException) = 0;

   
private:
   Context(const Context&);
   void operator=(const Context&);

   CdmwCcmContainer::CCM2Context_var m_ccm_context;

}; // end class Context

} // namespace CIF 
} // end namespace Cdmw 
} // end namespace CCM 

#endif // INCL_CCM_CIF_CONTEXT_HPP
