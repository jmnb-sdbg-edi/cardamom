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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformlibrary/ServiceDefContainer_impl.hpp"


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

//
// initialization of static member of service def container
//
#ifdef AIX

template <class ELEMENT_DATA> Cdmw::OsSupport::Mutex Container<ELEMENT_DATA>::M_existingContainers_mutex;
template <class ELEMENT_DATA> typename Container<ELEMENT_DATA>::Containers Container<ELEMENT_DATA>::M_existingContainers;

template <> Cdmw::OsSupport::Mutex ServiceDefContainer_impl::ServiceDefContainer::M_existingContainers_mutex;
template <> ServiceDefContainer_impl::ServiceDefContainer::Containers ServiceDefContainer_impl::ServiceDefContainer::M_existingContainers;

#else

Cdmw::OsSupport::Mutex ServiceDefContainer_impl::ServiceDefContainer::M_existingContainers_mutex;
ServiceDefContainer_impl::ServiceDefContainer::Containers ServiceDefContainer_impl::ServiceDefContainer::M_existingContainers;

#endif

//
// Constructor
// 
ServiceDefContainer_impl::ServiceDefContainer_impl(PortableServer::POA_ptr poa,
                                                   ServiceDefContainer* pActualContainer,
                                                   CdmwPlatformMngtService::ServiceDefContainer* pParentContainer)
        throw()
        : DeactivableServant_impl (poa),
          m_pActualContainer (pActualContainer)
           
            
{
	// store parent container reference
	m_parentContainer = CdmwPlatformMngtService::ServiceDefContainer::_duplicate (pParentContainer);
}



//
// Destructor
// 
ServiceDefContainer_impl::~ServiceDefContainer_impl()
        throw()
{
}



//
// Get serviceProviderId of service selected by its name from container
//
CdmwPlatformMngtService::ServiceProviderID* ServiceDefContainer_impl::get_service_def (
                                  const char* service_name)
         throw(CdmwPlatformMngtService::ServiceNotFound,
               CORBA::SystemException)
{
	try
	{
		CdmwPlatformMngtService::ServiceProviderID_var serviceProviderId = new CdmwPlatformMngtService::ServiceProviderID();
	    
	    bool result = m_pActualContainer->findElement(service_name, serviceProviderId.inout());
	    
	    if (result == false)
	    {
	    	if (CORBA::is_nil (m_parentContainer.in()))
	    	{
	    	    throw CdmwPlatformMngtService::ServiceNotFound();
	    	}
	    	else
	    	{
	    	    // if service not found in the container, interrogate the parent container
	    	    serviceProviderId = m_parentContainer->get_service_def (service_name);
	    	}
	    }

	    // return serviceProviderId of service name
	    return serviceProviderId._retn();

	}
	catch (CdmwPlatformMngtService::ServiceNotFound& e)
	{
		throw;
	}
	catch(const std::bad_alloc &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const OutOfResourcesException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const OutOfMemoryException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
    catch(const AssertionFailedException &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
    catch(const InternalErrorException &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
	catch(const CORBA::Exception& e)
    {
        throw;
    }
}  
                





ServiceDefContainer_impl::ServiceDefContainer* ServiceDefContainer_impl::getActualContainer () const
{
     return m_pActualContainer;
}
        

} // End namespace PlatformMngt
} // End namespace Cdmw



