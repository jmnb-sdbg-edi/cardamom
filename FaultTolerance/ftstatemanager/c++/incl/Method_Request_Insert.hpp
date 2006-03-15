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

#ifndef _CDMW_FT_METHOD_REQUEST_INSERT_HPP_
#define _CDMW_FT_METHOD_REQUEST_INSERT_HPP_
#include <FaultTolerance/ftstatemanager/TransactionEngineCoordinatorBase.hpp>
#include <Foundation/osthreads/Runnable.hpp>
#include <Foundation/orbsupport/TypeTraits.hpp>

namespace Cdmw {
namespace FT {
namespace Scheduler {


template <class OID, class DATA> 
/**
 * This class used to insert data to be propagated during the state tranfer
 */
class Method_Request_Insert : public Cdmw::OsSupport::Runnable
{
public:
    typedef typename Cdmw::OrbSupport::type_traits<OID>::_var_type     _Oid_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<OID>::_in_type      _Oid_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<OID>::_retn_type    _Oid_retn_type;

    typedef typename Cdmw::OrbSupport::type_traits<DATA>::_var_type     _Data_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<DATA>::_in_type      _Data_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<DATA>::_retn_type    _Data_retn_type;

    Method_Request_Insert (Cdmw::FT::TransactionEngineCoordinatorBase<OID,DATA> * coordinator,
                           _Oid_in_type oid,
                           _Data_in_type data,
                           ACE_Future<TransactionResult> & future);

    virtual ~Method_Request_Insert (void) throw();

    virtual void run() throw();
    
private:
    // TODO: Only keep a reference to Oid and a reference to Data as these shall not be deleted
    //       before this object is destroyed. The same applies to all other requests!
    _Oid_in_type  m_oid;
    _Data_in_type m_data;
    ACE_Future<TransactionResult> m_future;
    Cdmw::FT::TransactionEngineCoordinatorBase<OID,DATA> * m_coordinator;
};
 
//
// Implementation part
//
template <class OID, class DATA>
Method_Request_Insert<OID,DATA>::Method_Request_Insert
(Cdmw::FT::TransactionEngineCoordinatorBase<OID,DATA> * coordinator,
 _Oid_in_type oid,
 _Data_in_type data,
 ACE_Future<TransactionResult> & future)
    : m_oid(oid),
      m_data(data),
      m_future(future),
      m_coordinator(coordinator)
{
}
template <class OID, class DATA>
Method_Request_Insert<OID,DATA>::~Method_Request_Insert (void) throw()
{
}


template <class OID, class DATA>
void Method_Request_Insert<OID,DATA>::run () throw()
{
    try {
        m_coordinator->insert_i(m_oid, m_data, m_future);
    } catch (const InternalErrorException & ) {
        // Ignore for the moment
    }
}



}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_METHOD_REQUEST_INSERT_HPP_
