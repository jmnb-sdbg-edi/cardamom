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


#include <iostream>
#include <sstream>

#include "SystemMngt/platformlibrary/RWElementRepository_impl.hpp"

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{

namespace PlatformMngt
{



//
// Ctor
//
//
RWElementRepository_impl::RWElementRepository_impl ()
    throw ()
{

}


//
// Dtor
// 
RWElementRepository_impl::~RWElementRepository_impl()
    throw()
{

}

void RWElementRepository_impl::add_element(
                         const char* element_path,
                         const CdmwPlatformMngt::ElementType element_type,
                         CdmwPlatformMngt::ManagedElement_ptr element_object)
     throw (CdmwPlatformMngt::ElementPathInvalid,
            CdmwPlatformMngt::ElementAlreadyExists,
            CORBA::SystemException)
{
    // Add managed element into the map

    CDMW_WRITER_LOCK_GUARD(m_rwLock);
        
    CdmwPlatformMngt::ElementRepositoryData element_data;
    
    element_data.element_type = element_type;
    element_data.element = CdmwPlatformMngt::ManagedElement::_duplicate (element_object);
        

    try
    {
        Elements::iterator it = m_elements.find(element_path);

        if (it == m_elements.end())
        {
            // add the element
            m_elements[element_path] = element_data;
        }
        else
        {
            throw CdmwPlatformMngt::ElementAlreadyExists();
        }
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
}

void RWElementRepository_impl::remove_element(const char* element_path)
            throw (CdmwPlatformMngt::ElementPathInvalid,
                   CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException)                   
{
	// remove managed element
	
    CDMW_WRITER_LOCK_GUARD(m_rwLock);
        
    try
    {
        Elements::iterator it = m_elements.find(element_path);

        if (it != m_elements.end())
        {
            m_elements.erase(element_path);
        }
        else
        {
            throw CdmwPlatformMngt::ManagedElementNotFound (element_path);
        }
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }    
}



}; // End namespace PlatformMngt
}; // End namespace Cdmw

