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


#ifndef INCL_TEST_SYNC_IMPL_HPP 
#define INCL_TEST_SYNC_IMPL_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>

#include "testftlocationmanager/TestSync.skel.hpp"

#include <string>

namespace Cdmw
{
class SynchInterface_impl :
        public virtual POA_CdmwLocationManager::Synch,
        virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        SynchInterface_impl()
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~SynchInterface_impl()
                throw();
    
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual CORBA::Boolean get_OK ()
            throw(CORBA::SystemException);
     
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual void set_OK (CORBA::Boolean ok)
            throw(CORBA::SystemException);
                  
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 

    virtual ::FT::ObjectGroup_ptr get_result_get_object_group()
        throw(CORBA::SystemException);
    virtual ::CdmwFT::MemberInfos * get_result_get_other_group_members()
        throw(CORBA::SystemException);
    virtual ::FT::ObjectGroupRefVersion get_result_get_object_group_version()
        throw(CORBA::SystemException);

    virtual ::FT::ObjectGroup_ptr get_result_get_object_group_from_gid()
        throw(CORBA::SystemException);
    virtual ::CdmwFT::MemberInfos * get_result_get_other_group_members_from_gid()
        throw(CORBA::SystemException);
    virtual ::FT::ObjectGroupRefVersion get_result_get_object_group_version_from_gid()
        throw(CORBA::SystemException);

    virtual CORBA::Boolean get_result_is_primary()
         throw(CORBA::SystemException);
    virtual CORBA::Boolean get_result_is_primary_from_gid()
          throw(CORBA::SystemException);

    virtual CORBA::Boolean get_activation()
          throw(CORBA::SystemException);

   
    private:

        // Hide copy constructor/assignment operator
        SynchInterface_impl(const SynchInterface_impl& rhs)
                throw();
        
        SynchInterface_impl&
        operator=(const SynchInterface_impl& rhs)
                throw();

    public :
        CORBA::Boolean m_flags;

    ::FT::ObjectGroup_var m_result_get_object_group;
    ::CdmwFT::MemberInfos_var m_result_get_other_group_members;
    ::FT::ObjectGroupRefVersion m_result_get_object_group_version;

    ::FT::ObjectGroup_var m_result_get_object_group_from_gid;
    ::CdmwFT::MemberInfos_var m_result_get_other_group_members_from_gid;
    ::FT::ObjectGroupRefVersion m_result_get_object_group_version_from_gid;

    CORBA::Boolean m_result_is_primary;
    CORBA::Boolean m_result_is_primary_from_gid;

    CORBA::Boolean m_activation;
    
}; // End class SynchInterface_impl 

}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 

