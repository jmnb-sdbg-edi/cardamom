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

/**
 * @brief Random Strategy Implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include "lbstrategy/Random_impl.hpp"

#include <cstdlib>
 
namespace Cdmw
{

namespace LB
{

    // Costructor with arguments
    Random_impl::Random_impl(PortableGroup::ObjectGroup_ptr og,
                             Cdmw::LB::IOGRFactory* iogr_factory,
                             const char* name)
        :m_iogr_factory(iogr_factory)
    {
        m_name = CORBA::string_dup(name);
        init(og);
    }

    // Destructor
    Random_impl::~Random_impl()
    {
        for(CORBA::ULong i = 0; i < m_og_members.size(); i++)
            delete (m_og_members[i]);
        for(CORBA::ULong i = 0; i < m_fallback_members.size(); i++)
            delete (m_fallback_members[i]);
	delete m_iogr_factory;
    }

    // IDL specific methods
    char* Random_impl::name()throw (CORBA::SystemException)
    {return CORBA::string_dup(m_name);}


    /**
     * Return the next member to invoke, according to the Random Strategy
     *
     * @param ClientRequestInfo the request to invoke
     */
    
    CORBA::Object_ptr Random_impl::next_member(PortableInterceptor::ClientRequestInfo_ptr request)
        throw(CORBA::SystemException)
    {
	if(m_replicaNumber == 0)
	    return CORBA::Object::_duplicate(m_fallback_members[0]);

        m_next = rand() % m_replicaNumber;
        return CORBA::Object::_duplicate(m_og_members[m_next]);
    }

    // Initialize all strategy attributes
    void
    Random_impl::init (PortableGroup::ObjectGroup_ptr og)
        throw (CORBA::SystemException)
    {
        CORBA::Long n_profile = m_iogr_factory->profile_count(CORBA::Object::_duplicate(og));
        for(CORBA::Long index = 0; index < n_profile; index++)
            try
            {
                CdmwLB::TagLBGroupTaggedComponent* lb_group_tag = m_iogr_factory->decode_profile_with_group_tag(CORBA::Object::_duplicate(og), index);
                m_og_version = lb_group_tag->object_group_ref_version;
                try
                {
                    m_iogr_factory->decode_profile_with_fallback_tag(CORBA::Object::_duplicate(og), index);
                    m_fallback_members.push_back(m_iogr_factory->get_member_ref(CORBA::Object::_duplicate(og), index));
                }catch(const PortableGroup::MemberNotFound&)
                {
                    throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
                }
                catch(const CdmwLB::TagNotFound&)
                {
                    try
                    {
                        m_og_members.push_back(m_iogr_factory->get_member_ref(CORBA::Object::_duplicate(og), index));
                    }catch(const PortableGroup::MemberNotFound&)
                    {
                        throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
                    }
                }
        }catch(const CdmwLB::TagNotFound&)
        {
            throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
        }
        m_replicaNumber = m_og_members.size();
        m_next = 0;
        
    }
}//end of namespace LB
    
}//end of namespace Cdmw
