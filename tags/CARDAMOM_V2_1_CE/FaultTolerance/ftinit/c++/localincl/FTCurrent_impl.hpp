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


#ifndef INCL_CDMW_FT_FTINIT_CURRENT_IMPL_HPP 
#define INCL_CDMW_FT_FTINIT_CURRENT_IMPL_HPP 

// Cdmw Files
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include <idllib/CdmwFTCurrent.skel.hpp>

//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{
    
namespace FT
{



//
// IDL:thalesgroup.com/CdmwFT/Current:1.0
//
class Current_impl : virtual public CdmwFT::Current, 
                     virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:

    Current_impl(PortableInterceptor::Current_ptr   pi_current,
                 const PortableInterceptor::SlotId  ft_domain_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_ref_version_slot_id,
                 const PortableInterceptor::SlotId  client_id_slot_id,
                 const PortableInterceptor::SlotId  retention_id_slot_id,
                 const PortableInterceptor::SlotId  expiration_time_slot_id);
    ~Current_impl();

    //
    // IDL:thalesgroup.com/CdmwFT/Current/get_ft_domain_id:1.0
    //
    virtual ::FT::FTDomainId get_ft_domain_id()
        throw(CdmwFT::Current::NoContext, CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Current/get_object_group_id:1.0
    //
    virtual ::FT::ObjectGroupId get_object_group_id()
        throw(CdmwFT::Current::NoContext, CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Current/get_object_group_ref_version:1.0
    //
    virtual ::FT::ObjectGroupRefVersion get_object_group_ref_version()
        throw(CdmwFT::Current::NoContext, CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Current/get_client_id:1.0
    //
    virtual char* get_client_id()
        throw(CdmwFT::Current::NoContext, CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Current/get_retention_id:1.0
    //
    virtual CORBA::Long get_retention_id()
        throw(CdmwFT::Current::NoContext, CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Current/get_expiration_time:1.0
    //
    virtual TimeBase::TimeT get_expiration_time()
        throw(CdmwFT::Current::NoContext, CORBA::SystemException);

private:
    // hide default constructor, copy constructor and operator =
    Current_impl();
    Current_impl(const Current_impl&);
    void operator=(const Current_impl&);

    // Current object that is used specifically by portable Interceptors 
    // to transfer thread context information to a request context.
    PortableInterceptor::Current_ptr   m_pi_current;

    // current object ft domain id slot id
    PortableInterceptor::SlotId  m_ft_domain_id_slot_id;

    // current object ft group id
    PortableInterceptor::SlotId  m_object_group_id_slot_id;

    // current object ft group ref version
    PortableInterceptor::SlotId  m_object_group_ref_version_slot_id;

    // current object request client id
    PortableInterceptor::SlotId  m_client_id_slot_id;

    // current object request retention id
    PortableInterceptor::SlotId  m_retention_id_slot_id;

    // current object request expiration time
    PortableInterceptor::SlotId  m_expiration_time_slot_id;

};


} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_FTINIT_CURRENT_IMPL_HPP

