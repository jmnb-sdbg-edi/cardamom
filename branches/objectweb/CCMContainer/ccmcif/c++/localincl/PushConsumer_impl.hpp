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


#ifndef INCL_CCM_CIF_PUSHCONSUMER_IMPL_HPP
#define INCL_CCM_CIF_PUSHCONSUMER_IMPL_HPP

// Cdmw Files
#include <Foundation/orbsupport/RefCountServantBase.hpp>

#include <CCMContainer/ccmcif/LifeCycleObjectBase.hpp>


#include <CCMContainer/idllib/CdmwCcmCif.skel.hpp>
#include <CCMContainer/idllib/CdmwCcmContainer.stub.hpp>



namespace Cdmw {

namespace CCM {

namespace CIF {


//
// IDL:omg.org/Components/HomeConfiguration:1.0
//
class PushConsumer_impl : virtual public POA_CdmwCcmCif::PushConsumer,
                          virtual public LifeCycleObjectBase,
                          virtual public OrbSupport::RefCountServantBase

{

public:

    PushConsumer_impl(CdmwCcmContainer::CCM2Context_ptr ctx, 
                      Components::EventConsumerBase_ptr ccm_consumer);
    ~PushConsumer_impl() throw();

    //
    // IDL:omg.org/CosEventComm/PushConsumer/disconnect_push_consumer:1.0
    //
    virtual void disconnect_push_consumer()  throw(CORBA::SystemException);

    //
    // IDL:omg.org/CosEventComm/PushConsumer/push:1.0
    //
    virtual void push(const CORBA::Any& data) throw(CORBA::SystemException);


private:
    PushConsumer_impl();
    PushConsumer_impl(const PushConsumer_impl&);
    void operator=(const PushConsumer_impl&);

    Components::EventConsumerBase_var   m_ccm_consumer;  // non cdmw consumer
};




} // End of namespace CIF

} // End of namespace CCM

} // End of namespace Cdmw

#endif  // INCL_CCM_CIF_PUSHCONSUMER_IMPL_HPP

