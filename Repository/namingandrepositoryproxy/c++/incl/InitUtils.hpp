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


#ifndef INCL_CDMW_REPOSITORY_INIT_UTILS_HPP
#define INCL_CDMW_REPOSITORY_INIT_UTILS_HPP

#include "Repository/idllib/CdmwRepositoryExt.stub.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"

namespace Cdmw {
namespace NamingAndRepository {

    class InitUtils
    {
    public:
        /**
         * Purpose:
         * <p> Creates a CDMW Repository Proxy object.
         * 
         *@param orb A reference to the ORB pseudo-object
         *
         *@param poa The POA to use for the Proxy
         *
         *@param repos The RepositoryExt to proxy
         *
         *@return A reference to a CDMW Repository object
         *
         *@exception Cdmw::OrbSupport::CORBASystemExceptionWrapper
         */        
        static 
        CdmwNamingAndRepository::Repository_ptr
        create_repository_proxy
        (CORBA::ORB_ptr orb,
         PortableServer::POA_ptr poa,
         CdmwNamingAndRepository::RepositoryExt_ptr repos)
            throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);

    }; // End class InitUtils   
}; // namespace NamingAndRepository
}; // namespace Cdmw

#endif // INCL_CDMW_REPOSITORY_INIT_UTILS_HPP

