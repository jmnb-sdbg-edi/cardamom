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

#include "ftreplicationmanager/FallbackOid.hpp"
#include <sstream>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp> // for CDMW_FT_DEBUG macro

#include <FaultTolerance/ftcommon/FTLogger.hpp>

#include <Foundation/logging/FunctionLogger.hpp>

namespace
{
    bool isslash(char c)
    { return c == '/';}
}; // End of anonymous namespace


namespace Cdmw {
namespace FT {
namespace ReplicationManager {



FallbackOid::FallbackOid(::FT::ObjectGroupId         object_group_id,
                         ::FT::ObjectGroupRefVersion group_version,
                         const char *                repository_id) 
    throw ()
    : m_object_group_id(object_group_id),
      m_group_version(group_version),
      m_repository_id(repository_id)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                      "object_group_id="<<object_group_id<<
                      ",group_version="<<group_version<<
                      ",repository_id="<<repository_id);
}

FallbackOid::FallbackOid(const PortableServer::ObjectId & oid) 
    throw (CORBA::BAD_PARAM)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
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
    if (i != end) {
        gid_s = std::string(begin,i);
        std::string::const_iterator j = std::find_if(++i,end,isslash);
        if (j != end) {
            version_s = std::string(i,j);
            type_id_s = std::string(j+1,end);
        }
    }
    if (gid_s.empty() || version_s.empty() || type_id_s.empty()) {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: Fallback oid badly formed!");
        CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAM, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    m_object_group_id = ::atol( gid_s.c_str() );
    m_group_version = ::atol( version_s.c_str() );
    m_repository_id   = type_id_s;
}

FallbackOid::FallbackOid & 
FallbackOid::operator=(const FallbackOid::FallbackOid & rhs)
    throw ()
{
    CDMW_ERROR(FTLogger::GetLogger(),"");
    if (this != &rhs) {
        m_object_group_id = rhs.m_object_group_id;
        m_group_version   = rhs.m_group_version;
        m_repository_id   = rhs.m_repository_id;
    }
    
    return *this;
}

FallbackOid::FallbackOid(const FallbackOid::FallbackOid & rhs)
    throw ()
    : m_object_group_id(rhs.m_object_group_id),
      m_group_version(rhs.m_group_version),
      m_repository_id(rhs.m_repository_id)
{
    CDMW_ERROR(FTLogger::GetLogger(),"");
}

PortableServer::ObjectId * FallbackOid::to_ObjectId() const
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    std::ostringstream oss;
    oss << m_object_group_id << "/" << m_group_version << "/" << m_repository_id;
    PortableServer::ObjectId_var oid =
        PortableServer::string_to_ObjectId(oss.str().c_str());
    CDMW_LOG_FUNCTION_RETURN(oss.str());
    return oid._retn();
}



}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

