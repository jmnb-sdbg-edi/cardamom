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


#ifndef INCL_PLATFORMMNGT_PROXY_PROXY_UTIL_HPP
#define INCL_PLATFORMMNGT_PROXY_PROXY_UTIL_HPP

#include <Foundation/common/Exception.hpp>
#include <SystemMngt/platformlibrary/Container.hpp> // AlreadyExistsException
#include <SystemMngt/platformlibrary/StateMachineTemplate.hpp> // UnknownStateException

#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtHost.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtProcess.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtApplication.stub.hpp> 
#include <SystemMngt/idllib/CdmwFTSystMngtEntity.stub.hpp> 

#include <Foundation/orbsupport/ObjectTraits.hpp>
// #include <map>
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>
#include <sstream>
#include <list>

// FIXME move to cpp
#include <Foundation/common/Assert.hpp> 



namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{  

/**
 *Purpose:
 *<p> Provides ProxyUtil.
 */
class ProxyUtil
{

private: 
    /**
     *Purpose:
     *<p> Default constructor is not allowed
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception OutOfMemoryException Lack of memory.
     */
    ProxyUtil() 
	throw (Cdmw::BadParameterException,
	       Cdmw::OutOfMemoryException);

protected:   
    /**
     *Purpose:
     *<p> Constructor.
     *
     *@param orb                The ORB 
     *
     *@exception BadParameterException if the parameter is invalid.
     *@exception OutOfMemoryException Lack of memory.
     */
    ProxyUtil(CORBA::ORB_ptr orb) 
	throw (Cdmw::BadParameterException,
	       Cdmw::OutOfMemoryException);
 
    /**
     *Purpose:
     *<p> Destructor.
     */
     virtual ~ProxyUtil() throw();

    /**
     *Purpose:
     *<p> Constructor.
     *
     *@exception CORBA::SystemException Raises CORBA::OBJECT_NOT_EXIST if conversion of object id to string fails
     */
    std::string getObjectId() throw (CORBA::SystemException); 

private:
    /**
     *Purpose:  
     *<p> The CurrentPOA object
     */
    PortableServer::Current_var m_current;

protected:
    /**
     *Purpose:  
     *<p> The ORB
     */
    CORBA::ORB_var              m_orb;

public:

    /**
     *Purpose:  
     *<p> Creates POA for default servant.
     *
     *@param orb The ORB
     *@param parent_poa The parent POA
     *@param name The unique name of the new POA.
     *
     *@exception Cdmw::BadParameterException One of ORB and POA is nil.
     *@exception Cdmw::PlatformMngt::AlreadyExistsException The name is not unique.
     *@exception Cdmw::OutOfMemoryException Lack of memory.
     */
    static PortableServer::POA_ptr
    createDefaultServantPOA(CORBA::ORB_ptr orb, 
			    PortableServer::POA_ptr parent_poa,
			    const std::string &name) 
	throw (Cdmw::BadParameterException, 
	       Cdmw::PlatformMngt::AlreadyExistsException,
	       Cdmw::OutOfMemoryException);
};

} // End of namespace Proxy
} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif
