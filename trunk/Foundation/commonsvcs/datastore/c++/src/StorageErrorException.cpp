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

#include "Foundation/commonsvcs/datastore/StorageErrorException.hpp"


// Anonymous namespace
namespace
{
    // TODO: [i18n]put strings in a message catalog
    const char* STORAGE_ERROR = "Cannot modify storage!";
} // End of Anonymous namespace

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {
/**
 *Implementation notes: [if no pertinent write none ]
 *<p>
 *Portability issues: [if no pertinent write none ]
 *<p>
 */
    
    StorageErrorException::StorageErrorException()
        throw()
        : Exception(Exception::SAFE, STORAGE_ERROR)
    {
        // does nothing
    }
    
    
    StorageErrorException::~StorageErrorException()
        throw()
    {
        // does nothing
    }
    
    
    StorageErrorException::StorageErrorException(const std::string& reason)
        throw()
        : Exception(Exception::SAFE, reason)
    {
        // does nothing
    }
    
    StorageErrorException::StorageErrorException(const StorageErrorException& rhs)
        throw()
        : Exception(rhs)
    {
        // does nothing
    }

}  // End namespace DataStore  
}  // End namespace CommonSvcs 
}  // End namespace Cdmw

