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


#ifndef INCL_PLATFORMMNGT_RWELEMENT_REPOSITORY_HPP 
#define INCL_PLATFORMMNGT_RWELEMENT_REPOSITORY_HPP 

#include <string>

#include "SystemMngt/idllib/CdmwPlatformMngtCommonBase.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.skel.hpp"

#include "SystemMngt/platformlibrary/ElementRepository_impl.hpp"


namespace Cdmw
{

namespace PlatformMngt
{

/**
*Purpose:
*<p>    This class is used to contain the repository for all SystemMngt objects
*       and allow to insert and remove elements
*
*Features:
*<p> Thread safe
*
*
*/
class RWElementRepository_impl : virtual public POA_CdmwPlatformMngt::RWElementRepository,
                                 public ElementRepository_impl
                                 
{
    public:
    
        /**
        * Purpose:
        * <p> Constructor
        *   
        */ 
        RWElementRepository_impl ()
            throw ();
            
            
        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~RWElementRepository_impl()
            throw();

        /**
        *Purpose:
        *<p>
        * Implements the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ElementRepository/add_element:1.0
        * operation
        */
        void add_element(const char* element_path,
                         const CdmwPlatformMngt::ElementType element_type,
                         CdmwPlatformMngt::ManagedElement_ptr element_object)
            throw (CdmwPlatformMngt::ElementPathInvalid,
                   CdmwPlatformMngt::ElementAlreadyExists,
                   CORBA::SystemException);
                   
        /**
        *Purpose:
        *<p>
        * Implements the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ElementRepository/remove_element:1.0
        * operation
        */
        void remove_element(const char* element_path)
            throw (CdmwPlatformMngt::ElementPathInvalid,
                   CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException);
                   
    protected:
    

    private:
    
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        RWElementRepository_impl (const RWElementRepository_impl& rhs)
                throw();


        /**
        * Purpose:
        * <p> Assignement operator
        * 
        */ 
        RWElementRepository_impl& operator=(const RWElementRepository_impl& rhs)
                throw();


		
}; // End class RWElementRepository_impl 

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif // INCL_PLATFORMMNGT_RWELEMENT_REPOSITORY_HPP

