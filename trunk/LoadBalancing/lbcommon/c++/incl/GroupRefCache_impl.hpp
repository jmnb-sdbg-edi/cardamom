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

#ifndef _CDMW_LB_GROUP_REF_CACHE_IMPL_HPP_
#define _CDMW_LB_GROUP_REF_CACHE_IMPL_HPP_

#include "LoadBalancing/lbcommon/GroupRefCache.skel.hpp"
#include "LoadBalancing/lbcommon/GroupRefCacheCommandHandler.hpp"
#include "LoadBalancing/lbcommon/GroupRefEncoder.hpp"
#include <map>



namespace Cdmw {
    namespace LB {
        class GroupRefCache_impl;
    }
}

class Cdmw::LB::GroupRefCache_impl
    : public virtual CdmwLB::GroupRefCache,
      public virtual Cdmw::LB::GroupRefCacheCommandHandler
            
{
public:
    GroupRefCache_impl(CORBA::ORB_ptr orb, ::Cdmw::LB::GroupRefUpdateMessageCodec_t& codec);

public: 
    virtual void set(::CORBA::Short id,
				  ::PortableGroup::ObjectGroupRefVersion iogr_version,
				  ::CORBA::Object_ptr iogr)
      throw(CORBA::SystemException);
    
    virtual void remove(::CORBA::Short id)
      throw(CORBA::SystemException,
	    ::CdmwLB::GroupRefCache::UnknownGroupReference); 
    
    virtual CdmwLB::CacheData* get(::CORBA::Short id)
        throw(CORBA::SystemException,
              ::CdmwLB::GroupRefCache::UnknownGroupReference);

public:
    virtual void handle_set(CORBA::Short id, 
			    PortableGroup::ObjectGroupRefVersion iogr_version,
			    CORBA::Object_ptr iogr);
    
    virtual void handle_remove(CORBA::Short id);
    
private:
    CORBA::ORB_var m_orb;
    Cdmw::LB::GroupRefEncoder m_encoder;
    
    // Maps an object group id to an ObjectGroupData structure.
    typedef std::map<short, CdmwLB::CacheData*> GroupCacheMap;
    GroupCacheMap m_group_map;
    GroupCacheMap::iterator m_group_pos;
};

#endif /* _CDMW_LB_GROUP_REF_CACHE_IMPL_HPP_ */
