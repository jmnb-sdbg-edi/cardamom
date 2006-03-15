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


#ifndef INCL_FTSYSTMNGT_HOST_CONTAINER_ACCESSOR_IMPL_HPP 
#define INCL_FTSYSTMNGT_HOST_CONTAINER_ACCESSOR_IMPL_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwFTSystMngtHost.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtManagedElement.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"


/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt
{


/**
*Purpose: Host Container accessor
*<p> 
*
*/
class HostContainerAccessor_impl : virtual public POA_CdmwFTSystMngt::HostContainerAccessor,
                                   virtual public PortableServer::RefCountServantBase
{

    public:

        /**
         *Purpose:
         *<p> Constructor.
         *
         */
        HostContainerAccessor_impl(
            CdmwPlatformMngt::ElementRepository_ptr element_repository)
                throw();
        

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~HostContainerAccessor_impl();


        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/HostContainerAccessor/get_number_of_hosts:1.0
          * operation
          */ 
		CORBA::ULong get_number_of_hosts(const char* target_element_path)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);


        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/HostContainerAccessor/get_all_host_names:1.0
          * operation
          */
		void get_all_host_names (const char* target_element_path,
		                         CORBA::ULong how_many,
                                 CdmwPlatformMngt::ElementNames_out host_names,
                                 CdmwPlatformMngt::ElementNamesIterator_out rest)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);


        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/HostContainerAccessor/get_host:1.0
          * operation
          */
		char* get_host(const char* target_element_path,
		               const char* host_name)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CdmwPlatformMngt::HostNotFound,
                      CORBA::SystemException);
                      
        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/HostContainerAccessor/get_hosts:1.0
          * operation
          */
		CORBA::Boolean get_hosts (
		                const char* target_element_path,
                        const CdmwPlatformMngt::ElementNames& host_names,
                        CdmwPlatformMngt::ElementPaths_out hosts)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);
                      

        /**
         *Purpose:
          *<p>
          * Implements the
          * IDL:thalesgroup.com/CdmwFTSystMngt/HostContainerAccessor/get_all_hosts:1.0
          * operation
          */
		void get_all_hosts(const char* target_element_path,
		                   CORBA::ULong how_many,
                           CdmwPlatformMngt::ElementPaths_out hosts,
                           CdmwPlatformMngt::ElementPathsIterator_out rest)
                throw(CdmwPlatformMngt::ManagedElementNotFound,
                      CORBA::SystemException);

    private:
    
       /**
         * The repository of the managed elements.
         */
        CdmwPlatformMngt::ElementRepository_var m_host_element_repository;
        
    
       /**
        *Purpose:
        *<p> Get the HostContainer object.
        */
       CdmwPlatformMngt::HostContainer_ptr get_host_container (const char* target_element_path)
              throw(CdmwPlatformMngt::ManagedElementNotFound);


        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        HostContainerAccessor_impl (const HostContainerAccessor_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        HostContainerAccessor_impl&
        operator=(const HostContainerAccessor_impl& rhs)
                throw();


    

};  // End class HostContainerAccessor_impl 

}   // End namespace PlatformMngt
}   // End namespace Cdmw



#endif // INCL_FTSYSTMNGT_HOST_CONTAINER_ACCESSOR_IMPL_HPP


