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


#ifndef INCL_PLATFORMMNGT_MULTIPLEIDSERVANT_IMPL_HPP 
#define INCL_PLATFORMMNGT_MULTIPLEIDSERVANT_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
* Purpose:
* <p>
* Servant base class that allows a single servant to be mapped with several
* CORBA references.
*
* A specific POA will be created with the following features :
* - MULTIPLE_ID
* - TRANSIENT
* - RETAIN
* - USER_ID
* - USE_ACTIVE_OBJECT_MAP_ONLY
*/
class MultipleIdServant_impl : virtual public PortableServer::ServantBase
{

public:

    /**
    * Purpose:
    * <p> Destructor.
    */ 
    virtual ~MultipleIdServant_impl();

    /**
    * Purpose:
    * <p> Activates this servant with the specified name as the ObjectId
    * and returns the corresponding CORBA reference.
    *
    * @param name the name that will serve as the ObjectId.
    * @exception BadParameterException if name has already been used.
    */ 
    CORBA::Object_ptr activate(const std::string& name)
        throw (BadParameterException, InternalErrorException);

    /**
    * Purpose:
    * <p> Deactivates the CORBA Object denoted by the specified name
    * as the object id.
    *
    * @param name the name that serves as the ObjectId.
    * @exception BadParameterException if name doesn't denote an existing mapping.
    */ 
    void deactivate(const std::string& name)
        throw (BadParameterException, InternalErrorException);

    /**
    * Purpose:
    * <p> Deactivates all CORBA Objects related with this servant.
    * This servant is no more usable.
    *
    */ 
    void destroy() throw (InternalErrorException);

protected:

    /**
    * Purpose:
    * <p> Constructor.
    *
    * @param orb the initialised ORB.
    * @param poaName the name identifying the POA that will be created.
    */ 
    MultipleIdServant_impl(CORBA::ORB_ptr orb, const std::string& poaName);


    /**
    * Purpose:
    * <p> Returns the name embedded in the ObjectId of the CORBA object
    * in whose context it is called.
    */ 
    std::string getCurrentName()
        throw (OutOfMemoryException, InternalErrorException);


private:

    /**
     * The specific POA in charge of this servant.
     */
    PortableServer::POA_var m_poa;


    /**
     * The PortableServer::Current.
     */
    PortableServer::Current_var m_current;

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_MULTIPLEIDSERVANT_IMPL_HPP

