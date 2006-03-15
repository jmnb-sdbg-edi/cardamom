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

#ifndef INCL_COMMONSVCS_DATASTORE_DATASTOREOBSERVER_HPP
#define INCL_COMMONSVCS_DATASTORE_DATASTOREOBSERVER_HPP

#include "Foundation/orbsupport/TypeTraits.hpp"

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {


/**
* This template class defines a DataStore observer to be registered within a datastore
* to be notified whenever there an insertion, update, or removal of a record in the datastore.
*
*Features:
*<p> Shall be thread safe and exception safe.
*/
template <class Oid, class Data> 
class DataStoreObserver
{
public:
    virtual ~DataStoreObserver() throw() {}
    
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_in_type      _Oid_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<Data>::_in_type     _Data_in_type;
    
    /**
     * call by the datastore when the insert method is called on it.
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    virtual void on_insert(const _Oid_in_type oid, const _Data_in_type data)
        throw () = 0;

    /**
     * call by the datastore when the update method is called on it.
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    virtual void on_update(const _Oid_in_type oid, const _Data_in_type data)
        throw () = 0;

    /**
     * call by the datastore when the remove method is called on it.
     *
     *@param oid is the Object Id key
     *@param data is the data associated with Oid key
     */
    virtual void on_remove(const _Oid_in_type oid)
        throw () = 0;
};


}  // End namespace DataStore
}  // End namespace CommonSvcs
}  // End namespace Cdmw

#endif /* INCL_COMMONSVCS_DATASTORE_DATASTOREOBSERVER_HPP */
