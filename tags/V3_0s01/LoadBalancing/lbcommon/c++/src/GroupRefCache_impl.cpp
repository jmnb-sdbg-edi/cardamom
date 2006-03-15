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

#include "LoadBalancing/lbcommon/GroupRefCache_impl.hpp"
#include <iostream>
#include "Foundation/commonsvcs/federation/LocalTopicUpdateManager.hpp"

static const int TOPIC_ID = 1003;

Cdmw::LB::GroupRefCache_impl::GroupRefCache_impl(CORBA::ORB_ptr orb,
                                                     ::Cdmw::LB::GroupRefUpdateMessageCodec_t& codec)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_encoder(codec)
{
}


void
Cdmw::LB::GroupRefCache_impl::set(::CORBA::Short id,
				  ::PortableGroup::ObjectGroupRefVersion iogr_version,
				  ::CORBA::Object_ptr iogr)
    throw(CORBA::SystemException)
{
    m_encoder.data().command_id = CdmwLB::SET;
    m_encoder.data().group_id = id;
    m_encoder.data().object_group_ref_version = iogr_version;
    m_encoder.data().iogr = CORBA::Object::_duplicate(iogr);
    ::Cdmw::CommonSvcs::LocalTopicUpdateManager::instance().update_topic(TOPIC_ID,
									 m_encoder);
}
    
void
Cdmw::LB::GroupRefCache_impl::remove(::CORBA::Short id)
    throw(CORBA::SystemException,
          ::CdmwLB::GroupRefCache::UnknownGroupReference)
{
    m_encoder.data().command_id = CdmwLB::REMOVE;
    m_encoder.data().group_id = id;

    ::Cdmw::CommonSvcs::LocalTopicUpdateManager::instance().update_topic(TOPIC_ID,
                                                                         m_encoder);
    
}

CdmwLB::CacheData*
Cdmw::LB::GroupRefCache_impl::get(::CORBA::Short id)
    throw(CORBA::SystemException,
          ::CdmwLB::GroupRefCache::UnknownGroupReference)
{
    CdmwLB::CacheData* cache_data = 0;
    m_group_pos = m_group_map.find(id);
    if(m_group_pos==m_group_map.end())
      throw ::CdmwLB::GroupRefCache::UnknownGroupReference();
    else
    {
	cache_data = m_group_pos->second;
    }
    return cache_data;
      
}


void
Cdmw::LB::GroupRefCache_impl::handle_set(CORBA::Short id, ::PortableGroup::ObjectGroupRefVersion iogr_version, CORBA::Object_ptr iogr)
{
   CdmwLB::CacheData* cache_data = 0;
   m_group_pos = m_group_map.find(id);
   if(m_group_pos!=m_group_map.end())
   {
       cache_data = new CdmwLB::CacheData();
       m_group_map.erase(m_group_pos);
       cache_data->object_group_ref_version = iogr_version;
       cache_data->iogr = CORBA::Object::_duplicate(iogr);
       m_group_map.insert(std::make_pair(id, cache_data));
   }
   else
   {
       cache_data = new CdmwLB::CacheData();
       cache_data->object_group_ref_version = iogr_version;
       cache_data->iogr = CORBA::Object::_duplicate(iogr);
       m_group_map.insert(std::make_pair(id, cache_data));
   }
}
    
void
Cdmw::LB::GroupRefCache_impl::handle_remove(CORBA::Short id)
{
   m_group_pos = m_group_map.find(id);
   if(m_group_pos!=m_group_map.end())
   {
       m_group_map.erase(m_group_pos);
   }
}


