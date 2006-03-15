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


#ifndef INCL_CCM_CIF_SESSION_CONTEXT_HPP
#define INCL_CCM_CIF_SESSION_CONTEXT_HPP

#include <CCMContainer/idllib/CdmwCcmContainer.stub.hpp>
#include <CCMContainer/ccmcif/Context.hpp>

namespace Cdmw {

namespace CCM {

namespace CIF {
   
/*
 * This class is the base class for ccmcif context embedding specific CCM 
 * context.
 */
class SessionContext : public Context
{
public:
   /**
    * Purpose:
    * <p> Constructor.
    *@param ctx encapsulated context. Each operation of this class are delegated
    * to this context.
    *
    */
   SessionContext(CdmwCcmContainer::Session2Context_ptr ctx);
   
   /**
    * Purpose:
    * <p> Destructor.
    */
   virtual ~SessionContext();

   /**
    * Purpose:
    * <p> Return the session context.
    */
   inline CdmwCcmContainer::Session2Context_ptr get_session_context()
      throw(CORBA::SystemException)
   {
       CdmwCcmContainer::CCM2Context_var context = get_ccm_context();
       CdmwCcmContainer::Session2Context_var session_context 
          = CdmwCcmContainer::Session2Context::_narrow(context.in());

       return session_context._retn();
   }


   //
   // IDL:omg.org/Components/Session2Context/create_ref:1.0
   //
   /**
    * Purpose:
    * <p>The create_ref operation is used to create a reference to be exported 
    * to clients to invoke operations. The RepositoryId must match the 
    * RepositoryId of the component itself, one of its bases, one of its 
    * supported interfaces, or one of its facets.
    *
    *@param repid Identifies the RepositoryId associated with the interface for
    * which a reference is being created.
    *
    *@return A newly created object reference.
    */

   virtual CORBA::Object_ptr create_ref(const std::string& rep_id) 
       throw(CORBA::SystemException);

   //
   // IDL:omg.org/Components/Session2Context/create_ref_from_oid:1.0
   //
   /**
    * Purpose:
    * <p>The create_ref_from_oid_str operation is used to create a reference to be
    * exported to clients which includes information provided by the component 
    * which it can use on subsequent operation requests.
    *
    *@param oid Identifies the stringified ObjectIdSeq to be encapsulated in the reference.
    *@param repid Identifies the RepositoryId associated with the interface for
    * which a reference is being created.
    *
    *@return A newly created object reference.
    */
   virtual CORBA::Object_ptr create_ref_from_oid_str(
           const std::string& oid,
           const std::string& rep_id)   
       throw (CORBA::SystemException);

   //
   // IDL:omg.org/Components/Session2Context/get_oid_from_ref:1.0
   //
   /**
    * Purpose:
    * <p>The get_oid_str_from_ref operation is used by the component to extract the
    * oid encapsulated in the reference. This operation must be called within an
    * operation invocation.
    *
    *@param objref Specifies the reference which contains the oid.
    *@return the stringified oid.
    *
    *@exception Components::IllegalState Raise if the operation is not called 
    * within an operation invocation.
    *@exception Components::BadComponentReference Raised with the 
    * WRONG_CONTAINER minor code if the reference was not created by this 
    * container.
    */
   virtual char* get_oid_str_from_ref(const CORBA::Object_ptr comp_ref)
       throw(CORBA::SystemException);

   //
   // IDL:omg.org/Components/Entity2Context/get_primary_key:1.0
   //
   /**
    * Purpose:
    * <p>The get_primary_key operation return the primary key when component 
    * is an entity component. Because we are in a session context this operation
    * will always raise NoKeyAvailable.
    */
   virtual Components::PrimaryKeyBase* get_primary_key() 
       throw(Components::NoKeyAvailable, CORBA::SystemException);

   //
   // IDL:omg.org/Components/SessionContext/get_CCM_object:1.0
   //
   /**
    * The get_CCM_object operation is used to get the reference used to invoke 
    * the component. For basic components, this will always be the component 
    * reference. For extended components, this will be a specific facet 
    * reference. If this operation is issued outside of the scope of a callback
    * operation, the IllegalState exception is returned.
    */
   virtual CORBA::Object_ptr get_CCM_object()
      throw (CORBA::SystemException);

   //
   // IDL:omg.org/Components/CCM2Context/req_passivate:1.0
   //
   /**
    * The req_passivate operation is used by the component to inform the 
    * container that it wishes to be passivated when its current operation 
    * completes. To be valid, the component must have a servant lifetime policy
    * of component or container. If not, the PolicyMismatch exception shall be
    * raised.
    */
   virtual void req_passivate()
       throw (CORBA::SystemException);

   
private:
   SessionContext(const SessionContext&);
   void operator=(const SessionContext&);


}; // end class SessionContext

} // namespace CIF 
} // end namespace Cdmw 
} // end namespace CCM 


#endif // INCL_CCM_CIF_SESSION_CONTEXT_HPP
