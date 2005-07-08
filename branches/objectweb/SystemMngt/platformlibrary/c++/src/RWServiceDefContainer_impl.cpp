/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformlibrary/RWServiceDefContainer_impl.hpp"


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
    // Constructor
    //
    RWServiceDefContainer_impl::RWServiceDefContainer_impl(
                 PortableServer::POA_ptr poa,
                 ServiceDefContainer_impl::ServiceDefContainer* pActualContainer,
                 CdmwPlatformMngtService::ServiceDefContainer* pParentContainer)
            throw()
            : DeactivableServant_impl (poa),
              ServiceDefContainer_impl(poa, pActualContainer, pParentContainer)
    {
    } 



    //
    // Destructor
    // 
    RWServiceDefContainer_impl::~RWServiceDefContainer_impl()
            throw()
    {
    } 


    //
    // Add service in container
    //
    void RWServiceDefContainer_impl::add_service_def (
                                  const CdmwPlatformMngtService::ServiceDef& service_def)
		    throw(CdmwPlatformMngtService::ServiceAlreadyExists,
		          CORBA::SystemException)
    
    {
        std::string name = service_def.service_name.in();
        
	    try
	    {
	        bool result = getActualContainer()->addElement (name,
	                                                        service_def.service_provider);
	    
	        if (result == false)
	        {  
	    	    throw CdmwPlatformMngtService::ServiceAlreadyExists();
	        }
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


        
    //
    // Remove service from container
    //
    void RWServiceDefContainer_impl::remove_service_def (const char* service_name)
             throw(CdmwPlatformMngtService::ServiceNotFound,
                   CORBA::SystemException)
    {
	    try
	    {
	        bool result = getActualContainer()->removeElement(service_name);
	    
	        if (result == false)
	        {  
	    	    throw CdmwPlatformMngtService::ServiceNotFound();
	        }
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
                      
                      
                      
                      
                      
    


} // End namespace PlatformMngt
} // End namespace Cdmw



