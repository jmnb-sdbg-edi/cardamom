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


#ifndef INCL_NAMINGANDREPOSITORY_STATE_TRANSFER_CONFIG_HPP
#define INCL_NAMINGANDREPOSITORY_STATE_TRANSFER_CONFIG_HPP

#include <string>
#include <Foundation/commonsvcs/datastore/DataStoreInterface.hpp>

namespace Cdmw
{
namespace NamingAndRepository
{
    static const Cdmw::CommonSvcs::DataStore::DsIdentifier NAME_DOMAIN_DATASTORE_ID    = 31385;
    static const Cdmw::CommonSvcs::DataStore::DsIdentifier NAMING_CONTEXT_DATASTORE_ID = 30295;
 
    static const std::string  REPOSITORY_ID = "IDL:thalesgroup.com/CdmwNamingAndRepository/RepositoryExt:1.0";
    
    static const std::string GROUP_NAME           = "Repository";
    static const std::string COMPOUND_GROUP_NAME  = "repository/object_groups";
}
}

#endif
