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


#ifndef INCL_PLATFORMMNGT_HOST_CONTAINER_IMPL_HPP 
#define INCL_PLATFORMMNGT_HOST_CONTAINER_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtHostProxy.skel.hpp"
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


/**
*Purpose: Host Def container implementation
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class HostContainer_impl : virtual public POA_CdmwPlatformMngt::HostContainer,
                           virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        *
        *@param pActualContainer actual container to associate
        * 
        */ 
        HostContainer_impl (PlatformMngt::HostContainer* pActualContainer)
                throw();



        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~HostContainer_impl()
                throw();



        /**
		* Purpose:
		* <p> Get number of Hosts from container
		* 
		*@return the number of hosts
		*
		*@exception CORBA::SystemException
		*
		*/ 
		CORBA::ULong get_number_of_hosts()
                throw(CORBA::SystemException);

        /**
		* Purpose:
		* <p> Get all host names from container
		* 
		*@param how_many     nbr of host names to get
		*@param entity_names sequence of host names returned
		*@param rest         iterator on host names container returned
		*
		*@exception CORBA::SystemException
		*
		*/
		void get_all_host_names (CORBA::ULong how_many,
                                 CdmwPlatformMngt::HostNames_out host_names,
                                 CdmwPlatformMngt::HostNamesIterator_out rest)
                throw(CORBA::SystemException);

        /**
		* Purpose:
		* <p> Get HostProxy selected by its name from container
		* 
		*@param host_name name of host
		*
		*@return pointer to HostProxy
		*
		*@exception CdmwPlatformMngtEntity::HostNotFound
		*@exception CORBA::SystemException
		*/ 
		CdmwPlatformMngt::HostProxy_ptr get_host(const char* host_name)
                throw(CdmwPlatformMngt::HostNotFound,
                      CORBA::SystemException);

        /**
		* Purpose:
		* <p> Get sequence of HostProxies from container
		* 
		*@param host_names   sequence of host name
		*@param hosts        sequence of HostProxy
		*
		*@return true if all returned hosts are valid
		*        false if at least one host is not valid
		*
		*@exception CORBA::SystemException
		*/ 
		CORBA::Boolean get_hosts(const CdmwPlatformMngt::HostNames& host_names,
                                 CdmwPlatformMngt::HostProxies_out hosts)
                 throw(CORBA::SystemException);
        


        /**
		* Purpose:
		* <p> Get all HostProxies from container
		* 
		*@param how_many     nbr of hosts to get
		*@param host         sequence of hosts returned
		*@param rest         iterator on hosts container returned
		*
		*@exception CORBA::SystemException
		*
		*/
		void get_all_hosts(CORBA::ULong how_many,
                           CdmwPlatformMngt::HostProxies_out hosts,
                           CdmwPlatformMngt::HostProxiesIterator_out rest)
                throw(CORBA::SystemException);


        /**
        * Purpose:
        * <p> get the Host container
        * 
        *@return the host container pointer
        *@exception
        */
        PlatformMngt::HostContainer* getActualContainer () const;
        
        
        
        

    private:

        /**
        * Define the HostProxyNameList
        */
        typedef SimpleContainer<HostProxy_impl*>::ElementNameList HostNameList;
    
        /**
        * Define the HostProxyList
        */
        typedef SimpleContainer<HostProxy_impl*>::ElementList HostList;
        
        
        
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        HostContainer_impl (const HostContainer_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        HostContainer_impl&
        operator=(const HostContainer_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Reference to Host Container
        * 
        */
        PlatformMngt::HostContainer* m_actualContainer;
        
       

};  // End class HostContainer_impl 

}   // End namespace PlatformMngt
}   // End namespace Cdmw



#endif // INCL_PLATFORMMNGT_HOST_CONTAINER_IMPL_HPP


