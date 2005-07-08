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


#ifndef INCL_PLATFORMMNGT_DEACTIVABLESERVANT_IMPL_HPP 
#define INCL_PLATFORMMNGT_DEACTIVABLESERVANT_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
* Purpose:
* <p>
* Servant base class that provides an easy way to explicitly deactivate one of
* its instance.
*
* If activate() is used, the associated POA must have at least the following features :
* - UNIQUE_ID
* - TRANSIENT
* - RETAIN
* - SYSTEM_ID
* - USE_ACTIVE_OBJECT_MAP_ONLY
*
* If activateWithId() is used, the associated POA must have at least the following features :
* - UNIQUE_ID
* - RETAIN
* - USER_ID
* - USE_ACTIVE_OBJECT_MAP_ONLY
*
* Once a servant has been activated with one of the previous method, a CORBA reference may
* be created with the _this() method.
*
*/
class DeactivableServant_impl : virtual public PortableServer::ServantBase
{

public:

    /**
    * Purpose:
    * <p> Destructor.
    */ 
    virtual ~DeactivableServant_impl();

    /**
    * Purpose:
    * <p> Activates this servant.
    *
    * @exception AlreadyDoneException if the servant is already activated.
    */ 
    void activate() throw (AlreadyDoneException, InternalErrorException);

    /**
    * Purpose:
    * <p> Activates this servant with the specified object id.
    *
    * @param objectId the object id string of the servant.
    * @exception AlreadyDoneException if the servant is already activated.
    */ 
    void activateWithId(const std::string& objectId)
        throw (AlreadyDoneException, InternalErrorException);

    /**
    * Purpose:
    * <p> Deactivates this servant.
    *
    * @exception BadOrderException if the servant was not activated.
    */ 
    void deactivate()
        throw (BadOrderException, InternalErrorException);

    /**
    * Purpose:
    * <p> Redefines the PortableServer::ServantBase method
    * to return the associated POA.
    *
    * @return The m_poa member
    */
    PortableServer::POA_ptr _default_POA();

protected:

    /**
    * Purpose:
    * <p> Constructor.
    *
    * @param orb the initialised ORB.
    * @param poaName the name identifying the POA that will be created.
    */ 
    DeactivableServant_impl(PortableServer::POA_ptr poa);

private:

    /**
     * The specific POA in charge of this servant.
     */
    PortableServer::POA_var m_poa;


    /**
     * The object id of this servant once it has been activated.
     */
    PortableServer::ObjectId_var m_oid;

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_DEACTIVABLESERVANT_IMPL_HPP

