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

#ifndef INCL_COMMONSVCS_DATASTORE_DATASTOREBASE_HPP
#define INCL_COMMONSVCS_DATASTORE_DATASTOREBASE_HPP

#include <Foundation/common/AlreadyExistException.hpp>
#include <Foundation/common/NotFoundException.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/common/Assert.hpp>

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {

typedef Cdmw::Common::AlreadyExistException AlreadyExistException;
typedef Cdmw::Common::NotFoundException     NotFoundException;

/**
* A unique identifier for a Data Store instance.
*/
typedef CORBA::ULong DsIdentifier;

/**
* Base class of any DataStore instance.
*
*Features:
*<p> Thread safe. Exception safe.
*/
class DataStoreBase
{
public:

    /**
    * Accessor for the identifier of this Data Store.
    *@return the identifier of this DataStore.
    */
    inline DsIdentifier get_dsid() const throw ()
    {
        CDMW_ASSERT(m_ds_id != 0L);
        return m_ds_id;
    };
    

    /**
    * Purpose:
    * <p> Constructor. Creates a data instance and registers it with the
    * DataStoreDictionnary.
    *
    *@param string dsid Data Store Identifier
    */
    DataStoreBase(DsIdentifier dsid);

protected:

    /**
    * Purpose:
    * <p> Destructor.
    */
    virtual ~DataStoreBase();


    /**
    * Assignement operator is not allowed.
    */
    DataStoreBase& operator=(const DataStoreBase& rhs);

private:
    /**
    * Purpose:
    * <p> Default constructor.
    */
    DataStoreBase()
        : m_ds_id(0L)
    {
    }


    DsIdentifier m_ds_id;
};

}  // End namespace DataStore  
}  // End namespace CommonSvcs 
}  // End namespace Cdmw

#endif /* INCL_COMMONSVCS_DATASTORE_DATASTOREBASE_HPP */
