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

#include "testftreplicationmanager/TypeRepositoryStub.hpp"



namespace Cdmw {
namespace FT {


static const char* CORBA_OBJECT_REPID = 
    "IDL:omg.org/CORBA/Object:1.0";
static const char* HELLO_INTERFACE_1_REPID = 
    "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface1:1.0";
static const char* HELLO_INTERFACE_2_REPID = 
    "IDL:thalesgroup.com/CdmwReplicationManager/HelloInterface2:1.0";
static const char* BASE_INTERFACE_REPID = 
    "IDL:thalesgroup.com/CdmwReplicationManager/BaseInterface:1.0";
        
    
CORBA::Boolean 
TypeRepositoryStub::has_base_interface(const char* type_id, const char* base_type_id)
    throw(::CdmwFT::TypeRepository::TypeNotFound)
{
    // if base interface is CORBA::Object, return true
    if (strcmp(base_type_id, CORBA_OBJECT_REPID) == 0) {
        return true;
    }
    
    // accept only CdmwReplicationManager::HelloInterface1
    // and CdmwReplicationManager::HelloInterface2 as type_id
    if ( (strcmp(type_id, HELLO_INTERFACE_1_REPID) != 0) &&
         (strcmp(type_id, HELLO_INTERFACE_2_REPID) != 0) )
    {
        throw ::CdmwFT::TypeRepository::TypeNotFound();
    }
         
    // if RepositoryId are the same, return true
    if (strcmp(type_id, base_type_id) == 0) {
        return true;
    }

    // HelloInterface1 inherits from BaseInterface
    if ( (strcmp(type_id, HELLO_INTERFACE_1_REPID) == 0) &&
         (strcmp(base_type_id, BASE_INTERFACE_REPID) == 0) )
    {
        return true;
    }
    
    return false;
    
}
    

    
}  // End namespace FT
}  // End namespace Cdmw
