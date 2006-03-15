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


#ifndef _CDMW_FT_FALLBACK_OID_HPP_
#define _CDMW_FT_FALLBACK_OID_HPP_


#include <Foundation/orbsupport/CORBA.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>
#include <string>


namespace Cdmw {
namespace FT {
namespace ReplicationManager {



    class FallbackOid
    {
    public:
        FallbackOid(::FT::ObjectGroupId         object_group_id,
                    ::FT::ObjectGroupRefVersion group_version,
                    const char *                repository_id) throw ();

        FallbackOid(const PortableServer::ObjectId & oid) throw (CORBA::BAD_PARAM);

        FallbackOid(const FallbackOid & rhs) throw ();
        FallbackOid & operator=(const FallbackOid & rhs) throw ();

        ~FallbackOid() throw() {};

        PortableServer::ObjectId * to_ObjectId() const;

        inline std::string                  get_repository_id()   const { return m_repository_id; }
        inline ::FT::ObjectGroupId          get_object_group_id() const  { return m_object_group_id; }
        inline ::FT::ObjectGroupRefVersion  get_group_version()   const  { return m_group_version; }
    private:
        // Hide default constructor
        FallbackOid();

        ::FT::ObjectGroupId         m_object_group_id;
        ::FT::ObjectGroupRefVersion m_group_version;
        std::string                 m_repository_id;

    };
    




}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw
#endif // _CDMW_FT_FALLBACK_OID_HPP_
