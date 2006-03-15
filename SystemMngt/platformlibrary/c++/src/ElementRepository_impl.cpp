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

#include "SystemMngt/platformlibrary/ElementRepository_impl.hpp"

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw
{

namespace PlatformMngt
{



//
// Ctor
//
//
ElementRepository_impl::ElementRepository_impl ()
    throw ()
{

}


//
// Dtor
// 
ElementRepository_impl::~ElementRepository_impl()
    throw()
{

}


CdmwPlatformMngt::ElementRepositoryData* 
ElementRepository_impl::get_element_data(const char* element_path)
     throw (CdmwPlatformMngt::ElementPathInvalid,
            CdmwPlatformMngt::ManagedElementNotFound,
            CORBA::SystemException)
{
	CDMW_READER_LOCK_GUARD(m_rwLock);

    try
    {
        CdmwPlatformMngt::ElementRepositoryData_var element_data =
            new CdmwPlatformMngt::ElementRepositoryData ();
            
        CdmwPlatformMngt::ElementRepositoryData& element_data_ref = element_data.inout();
              
        Elements::iterator it = m_elements.find(element_path);

        if (it != m_elements.end())
        {
            element_data_ref = it->second;
        }
        else
        {
            throw CdmwPlatformMngt::ManagedElementNotFound (element_path);
        }
        
        
        return element_data._retn();
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
}

                   
CdmwPlatformMngt::ManagedElement_ptr 
ElementRepository_impl::get_element(const char* element_path)
            throw (CdmwPlatformMngt::ElementPathInvalid,
                   CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException)
{
    CDMW_READER_LOCK_GUARD(m_rwLock);
    
    try
    {
        CdmwPlatformMngt::ManagedElement_var element;
          
        Elements::iterator it = m_elements.find(element_path);

        if (it != m_elements.end())
        {
        	// copy the element var
            element = (it->second).element;
        }
        else
        {
            throw CdmwPlatformMngt::ManagedElementNotFound (element_path);
        }
        
        return element._retn();
    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
}

}; // End namespace PlatformMngt
}; // End namespace Cdmw

