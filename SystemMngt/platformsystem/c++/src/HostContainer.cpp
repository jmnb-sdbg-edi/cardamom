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

#include "platformsystem/HostContainer.hpp"


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
#ifdef CDMW_USE_FAULTTOLERANCE
    HostContainer::HostContainer(System_impl* system,
                                 PortableServer::POA_ptr poa,
                                 CdmwFT::FaultManagement::FaultDetector_ptr faultDetector)
#else
    HostContainer::HostContainer(System_impl* system,
                                 PortableServer::POA_ptr poa)
#endif
            throw(BadParameterException)
    {
        if (system == NULL)
        {
            CDMW_THROW2 (BadParameterException, "system", "NULL");
        }
     
        if (CORBA::is_nil(poa))
        {
            CDMW_THROW2 (BadParameterException, "poa", "nil");
        }

        
        // Set the internal data
        m_system = system;
        m_poa = PortableServer::POA::_duplicate(poa);
#ifdef CDMW_USE_FAULTTOLERANCE
        m_faultDetector = CdmwFT::FaultManagement::FaultDetector::_duplicate(faultDetector);
#endif        
    } 



    //
    // Destructor
    // 
    HostContainer::~HostContainer()
            throw()
    {
    } 


    //
    // Add host in container
    //
    CdmwPlatformMngt::HostProxy_ptr 
              HostContainer::addHost(const CdmwPlatformMngt::HostDef& host_def)
                throw (CdmwPlatformMngt::HostAlreadyExists,
                       CORBA::SystemException)
    {
        // get host name
        std::string hostName = host_def.host_name.in();
        
	    try
	    {
	        // Create the HostProxy servant
            HostProxy_impl* p_hostProxy = 
#ifdef CDMW_USE_FAULTTOLERANCE
				       new HostProxy_impl (m_poa.in(), host_def, m_system, m_faultDetector.in());
#else
                       new HostProxy_impl (m_poa.in(), host_def, m_system);
#endif				                                 
                   
	        bool result = addElement(hostName, p_hostProxy);
	    
	        if (result == false)
	        {  
	    	    throw CdmwPlatformMngt::HostAlreadyExists();
	        }
	        
	        // create an object var to take pointer ownership
            PortableServer::ServantBase_var hostProxyServant = p_hostProxy;        
            // activate object
            p_hostProxy->activate();
            
            // start monitoring
            p_hostProxy->start();
            
            // return HostProxy object reference
            CdmwPlatformMngt::HostProxy_var hostProxyObject = p_hostProxy->_this();
            
            return hostProxyObject._retn();
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
        catch (const AlreadyDoneException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch (const OsSupport::ThreadSchedulingException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
        catch(const BadParameterException &)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
        }
	    catch(const CORBA::Exception& e)
        {
            throw;
        }
    } 


        
    //
    // Remove entity from container
    //
    void HostContainer::removeHost(const char* host_name)
                throw (CdmwPlatformMngt::HostNotFound,
                       CORBA::SystemException)
    {
    	// check if host name is valid
	    if (host_name == NULL)
	    {
	        throw CdmwPlatformMngt::HostNotFound();
        }

        if (*host_name == '\0')
        { 
    	    throw CdmwPlatformMngt::HostNotFound();
        }
    
	    try
	    {
	        bool result;
	        
	        // first get the hostProxy_impl object
	        HostProxy_impl* p_hostProxy_impl;
	    
	        result = findElement(host_name, p_hostProxy_impl);
	    
	        if (result == false)
	        {
	    	    throw CdmwPlatformMngt::HostNotFound();
	        }

            // suppress HostProxy_impl object from the container map
	        result = removeElement(host_name);
	    
	        if (result == false)
	        {  
	    	    throw CdmwPlatformMngt::HostNotFound();
	        }
	        
	        // stop the monitoring
	        p_hostProxy_impl->stop();
	        
	        // deactivate the Corba object (object will be deleted by orb)
	        p_hostProxy_impl->deactivate();
	        
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
    // return hosts snapshot
    //
    CdmwPlatformMngt::HostsSnapshot* HostContainer::getSnapshot()
        throw (Cdmw::OutOfResourcesException)
    {
        try
        {
            CdmwPlatformMngt::HostsSnapshot_var snapshot =
                   new CdmwPlatformMngt::HostsSnapshot;
                                     
            // Get the snapshot of all hosts
        
            // set sequence length
            snapshot->length (getNumberOfElements());
        
            // get list of host proxies
            HostProxyList hostProxyList;
            list (hostProxyList);
        
            // fill snapshot sequence
            HostProxyList::iterator hostProxyIt;
            unsigned int i;
        
            for (hostProxyIt=hostProxyList.begin(), i=0;
                 hostProxyIt != hostProxyList.end();
                 hostProxyIt++, i++)
            {
                snapshot[i].host_name = (*hostProxyIt)->name();
                snapshot[i].host_status = (*hostProxyIt)->status();
            }
            
            // return snapshot sequence
            return snapshot._retn();
        }
        
        catch(const std::bad_alloc &)
        {
            CDMW_THROW (Cdmw::OutOfResourcesException);
        }

    }
                     

} // End namespace PlatformMngt
} // End namespace Cdmw



