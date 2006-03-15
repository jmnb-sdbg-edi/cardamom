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


#ifndef INCL_PLATFORMMNGT_SYSTEM_OBSV_DATASTORE_HPP
#define INCL_PLATFORMMNGT_SYSTEM_OBSV_DATASTORE_HPP

#include "platformsystemlibrary/System_impl.hpp"
#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"



namespace Cdmw
{

namespace PlatformMngt
{
    



/**
 *Purpose:
 *<p>
 */
class SystemObsvDataStoreObserver : virtual public SystemObserverRegistrationDataStore::DataStoreObserver

{

public:
    typedef SystemObserverRegistrationDataStore::DataStoreObserver::_Oid_in_type _Oid_in_type;
    typedef SystemObserverRegistrationDataStore::DataStoreObserver::_Data_in_type _Data_in_type;

    virtual void on_insert(_Oid_in_type oid, _Data_in_type data) throw ();

    virtual void on_update(_Oid_in_type oid, _Data_in_type data) throw ();

    virtual void on_remove(_Oid_in_type oid) throw ();

    /**
     *Purpose:
     *<p> Constructor.
     *     
     */

    SystemObsvDataStoreObserver (System_impl* p_system)    
        throw ();

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~SystemObsvDataStoreObserver() throw ();



protected:

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    SystemObsvDataStoreObserver (SystemObsvDataStoreObserver& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    SystemObsvDataStoreObserver& operator = (const SystemObsvDataStoreObserver& rhs);
    
    /**
     * The system object.
     */
    System_impl* m_system;
    
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_SYSTEM_OBSV_DATASTORE_HPP

