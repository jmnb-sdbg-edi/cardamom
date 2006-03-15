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


#ifndef INCL_PLATFORMMNGT_ELEMENT_REPOSITORY_HPP 
#define INCL_PLATFORMMNGT_ELEMENT_REPOSITORY_HPP 

#include <string>
#include <map>

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommonBase.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtManagedElement.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.skel.hpp"


namespace Cdmw
{

namespace PlatformMngt
{

/**
*Purpose:
*<p>    This class is used to contain the repository for all SystemMngt objects
*       and allow to get the elements
*
*Features:
*<p> Thread safe
*
*
*/
class ElementRepository_impl : virtual public POA_CdmwPlatformMngt::ElementRepository,
                               virtual public PortableServer::RefCountServantBase
{
    public:
    
        /**
        * Purpose:
        * <p> Constructor
        *   
        */ 
        ElementRepository_impl ()
            throw ();
            
            
        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~ElementRepository_impl()
            throw();

        /**
        *Purpose:
        *<p>
        * Implements the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ElementRepository/get_element_data:1.0
        * operation
        */
        CdmwPlatformMngt::ElementRepositoryData*
            get_element_data(const char* element_path)
            throw (CdmwPlatformMngt::ElementPathInvalid,
                   CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException);
                   
        /**
        *Purpose:
        *<p>
        * Implements the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ElementRepository/get_element:1.0
        * operation
        */
        CdmwPlatformMngt::ManagedElement_ptr 
            get_element(const char* element_path)
            throw (CdmwPlatformMngt::ElementPathInvalid,
                   CdmwPlatformMngt::ManagedElementNotFound,
                   CORBA::SystemException);
                    

    protected:
    
        /**
        *Purpose:
        *<p> The type used to manage the element data.
        */
        typedef std::map<std::string, CdmwPlatformMngt::ElementRepositoryData> Elements;


       /**
       * The reader/writer lock used to manage concurrent thread safe access to
       * Elements.
       */
       Cdmw::OsSupport::ReaderWriterLock m_rwLock;
    
       /**
       * The elements contained in this container
       */
       Elements m_elements;



    private:
    
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        ElementRepository_impl (const ElementRepository_impl& rhs)
                throw();


        /**
        * Purpose:
        * <p> Assignement operator
        * 
        */ 
        ElementRepository_impl& operator=(const ElementRepository_impl& rhs)
                throw();
                
                
}; // End class ElementRepository_impl 

} // End namespace PlatformMngt
} // End namespace Cdmw
#endif // INCL_PLATFORMMNGT_ELEMENT_REPOSITORY_HPP

