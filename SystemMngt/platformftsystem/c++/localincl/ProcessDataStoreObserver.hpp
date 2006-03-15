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


#ifndef INCL_PLATFORMMNGT_PROCESS_DATASTORE_HPP
#define INCL_PLATFORMMNGT_PROCESS_DATASTORE_HPP

#include "SystemMngt/platformelementlibrary/DataStoreDefinition.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp"


namespace Cdmw
{

namespace PlatformMngt
{
    


/**
 *Purpose:
 *<p>
 */
class ProcessDataStoreObserver : virtual public ProcessDataStore::DataStoreObserver

{

public:
    typedef ProcessDataStore::DataStoreObserver::_Oid_in_type _Oid_in_type;
    typedef ProcessDataStore::DataStoreObserver::_Data_in_type _Data_in_type;

    virtual void on_insert(_Oid_in_type oid, _Data_in_type data) throw ();

    virtual void on_update(_Oid_in_type oid, _Data_in_type data) throw ();

    virtual void on_remove(_Oid_in_type oid) throw ();

    /**
     *Purpose:
     *<p> Constructor.
     *     
     */

    ProcessDataStoreObserver (CdmwPlatformMngt::ElementRepository_ptr element_repository)    
        throw ();

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~ProcessDataStoreObserver() throw();



protected:

private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    ProcessDataStoreObserver (ProcessDataStoreObserver& rhs);

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    ProcessDataStoreObserver& operator = (const ProcessDataStoreObserver& rhs);
    
    /**
     * The repository of the managed elements.
     */
    CdmwPlatformMngt::ElementRepository_var m_element_repository;
    
    /**
     * The managed element datastore home.
     */
    ManagedElementStorageHome* m_storage_home;
                         

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_PROCESS_DATASTORE_HPP

