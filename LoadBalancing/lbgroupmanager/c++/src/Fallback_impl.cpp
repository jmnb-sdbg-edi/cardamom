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


#include "lbgroupmanager/Fallback_impl.hpp"
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp> // for CDMW_ASSERT macro


#define ECHO_HEADER() \
    "[CDMW LBGroup Manager] (file: " << __FILE__ << ", line: " << __LINE__ << ")\n -->"

#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment:\
} while(0)

#endif


namespace
{
    bool isslash(char c)
    { return c == '/';}
}; // End of anonymous namespace

    
    
namespace Cdmw {
namespace LB {

namespace GroupManager {
    

static const char* CORBA_OBJECT_REPID = 
    "IDL:omg.org/CORBA/Object:1.0";
   
    
    
    /**
     * Creates a fallback default servant
     *
     * @param orb ...
     * @param poa ...
     */
    Fallback_impl::Fallback_impl(CORBA::ORB_ptr          orb,
                                 PortableServer::POA_ptr poa)
        throw(CORBA::SystemException)
        : m_orb(CORBA::ORB::_duplicate(orb)),
          m_poa(PortableServer::POA::_duplicate(poa)),
          m_current(PortableServer::Current::_nil()),
          m_type_repository(CdmwLB::TypeRepository::_nil())
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));
        CORBA::Object_var obj =
            orb->resolve_initial_references("POACurrent");
        m_current = PortableServer::Current::_narrow(obj.in());
	obj = orb->resolve_initial_references("LBTypeRepository");
        m_type_repository = ::CdmwLB::TypeRepository::_narrow(obj.in());
        CDMW_ASSERT(!CORBA::is_nil(m_type_repository.in()));
    }
        
    
    /**
     * Destroys the fallback default servant.
     */
    Fallback_impl::~Fallback_impl(void) throw ()
    {
    }
    
    
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
    CORBA::Boolean Fallback_impl::_is_a (const char *logical_type_id)
    {
        // always return true if logical_type_id == CORBA::Object
        if (strcmp(logical_type_id, CORBA_OBJECT_REPID) == 0) {
            return true;
        }
        // The servant determines the type id represented by self.
        PortableServer::ObjectId_var oid = m_current->get_object_id ();
        PortableServer::POA_var poa = m_current->get_POA ();
        CORBA::String_var type_id = this->_primary_interface (oid.in(),poa.in());
        
       CORBA::Boolean result;
        try {
            // Ask TypeRepository if type_id inherits from logical_type_id
            result = m_type_repository->has_base_interface(type_id.in(), logical_type_id);
        
        } catch (const CdmwLB::TypeRepository::TypeNotFound& e) {
	    DEBUG_DUMP("");
	    DEBUG_ECHO( << "WARNING: type " <<  type_id.in() 
			<< " is unknown from LBGroupManager. Check that its config file" 
			<< " contains this type as <SupportedInterface> element." 
			<< std::endl);
            // TypeRepository dosn't know the type_id
            result = false;
        
        } catch (const CORBA::Exception& e) {
            DEBUG_DUMP("");
	    DEBUG_ECHO(  "ERROR: unexpected exception in _is_a() implementation: "
			 << e << std::endl);
            result = false;
        } 
	return result;
        
    }

    
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
    CORBA::RepositoryId 
    Fallback_impl::_primary_interface (const PortableServer::ObjectId &oid,
                                       PortableServer::POA_ptr poa)
    {  
      // First convert the id into a string
      CORBA::String_var oids = PortableServer::ObjectId_to_string (oid);
    
      // The ObjectId has the following pattern:
    //            object_group_id/group_version/type_id
      std::string s__(oids.in());
      std::string::const_iterator begin = s__.begin();
      std::string::const_iterator end   = s__.end();
      // find end of object_group_id
      std::string::const_iterator i = std::find_if(begin,end,isslash);
    // copy the characters in [begin,i)
      std::string gid_s, version_s,type_id_s;
      gid_s = std::string(begin,i);
        
      if (i != end) {
        std::string::const_iterator j = std::find_if(++i,end,isslash);
        if (j != end) {
	  version_s = std::string(i,j);
	  type_id_s = std::string(j+1,end);
        }
    }
        return CORBA::string_dup(type_id_s.c_str());
    }
       
        
    /**
     * The invoke() method receives requests issued to any CORBA
     * object incarnated by the DSI servant and performs the
     * processing necessary to execute the request.
     *
     * @param request A ServerRequest local object containing request information.
     *
     * @exception TODO...
     */
    void Fallback_impl::invoke (CORBA::ServerRequest_ptr request)
    {
        // Get the operation name for this request
        const char *operation = request->operation ();
        
        if (strcmp (operation,"_is_a") == 0) {
            this->handle_is_a (request);
        } else {
            // Raise CORBA::INTERNAL instead of CORBA::NO_IMPLEMENT because this is caused by
            // the PortableInterceptor no filtering out non-_is_a operations!
            DEBUG_DUMP("ERROR: Fallback object servicing other that is_a operation!");
            throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        }
    }
    
    /**
     * Helper function to handle the _is_a() operation.
     *
     */
    void  Fallback_impl::handle_is_a(CORBA::ServerRequest_ptr request)
    {
        // get argument list of _is_a request
        CORBA::NVList_ptr list;
        m_orb->create_list (0, list);
        CORBA::Any any;
	any <<= "";
        list->add_value ("value",
                         any,
                         CORBA::ARG_IN);
        request->arguments (list);

        // get unique argument of list
        const char *value;
        CORBA::NamedValue_ptr nv = list->item (0);
        CORBA::Any_ptr ap = nv->value ();
        *ap >>= value;
        
        // call _is_a
        CORBA::Boolean result = this->_is_a(value);

        // set return value to request
        CORBA::Any result_any;
        CORBA::Any::from_boolean from_boolean (result);
        result_any <<= from_boolean;
        
        request->set_result (result_any);
    }
    

}; // namespace GroupManager
}; // namespace LB
}; // namespace Cdmw

