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

#ifndef __CDMW_SERVICE_LOCATOR_IMPL_HPP__
#define __CDMW_SERVICE_LOCATOR_IMPL_HPP__


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/idllib/CdmwServiceLocator.skel.hpp"
#include "idllib/CdmwFTManager.stub.hpp"
#include "idllib/CdmwFTCommon.stub.hpp"
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include "ftmanager/FTManager_impl.hpp"

#include <iostream>


namespace Cdmw {
namespace FT {

class  ServiceLocator_impl :
        public virtual POA_CdmwServiceLocator::ServiceLocator,
        public virtual PortableServer::RefCountServantBase
{

public :
    
    ServiceLocator_impl(CORBA::ORB_ptr orb, int dsid);
    
    ~ServiceLocator_impl();

    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwServiceLocator/locate_service:1.0
     * operation
     */    
    virtual void locate_service (const char * name,
                                 const char * sender)
        throw(CORBA::SystemException);


private:
    int m_dsid;
	typedef Cdmw::CommonSvcs::DataStore::StorageHome< int, ::CdmwFT::IOGRInfo > IOGRInfoStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< int, ::CdmwFT::IOGRInfo > IOGRInfoStorageObject;

	IOGRInfoStorageHome* m_iogrInfoStorageHome;
    CORBA::ORB_var m_orb;
};
    
}
}

#endif
