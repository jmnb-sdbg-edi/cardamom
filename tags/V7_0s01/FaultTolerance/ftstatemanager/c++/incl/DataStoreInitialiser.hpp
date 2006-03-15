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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_DATASTORE_INITIALISER_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_DATASTORE_INITIALISER_HPP

#include <Foundation/common/Exception.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>

#include <functional>

namespace Cdmw {
namespace FT {


/**
* This template class defines a DataStore initialiser to be registered within a datastore
* to be notified during initialisation of the datastore.
* This will be used within the coordinator.
*
*Features:
*<p> Shall be thread safe and exception safe.
*/
template <class OID, class DATA, class OIDCOMPARE=std::less<typename Cdmw::OrbSupport::type_traits<OID>::_var_type> > 
class DataStoreInitialiser
{
public:
    virtual ~DataStoreInitialiser() throw() {}
    
    typedef typename DataStore<OID,DATA, OIDCOMPARE>::DataInsertIterator DataInsertIterator;

    /**
     * call by the datastore to realise the initialisation on the primary datastore.
     *
     *@param it The iterator on the DataStore to insert data on it
     *@param cold_restart the mode of initialisation (codl or hot)
     */
    virtual void on_init(DataInsertIterator it, bool cold_restart = true) throw (Cdmw::InitErrorException) = 0;
};


}  // End namespace FT
}  // End namespace Cdmw

#endif
