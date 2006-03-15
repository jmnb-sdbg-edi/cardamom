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


#include <iostream>

// Cdmw files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/Codec.hpp>

// Cdmw CCM files
#include <ccmcif/PushConsumer_impl.hpp>

#include <CCMContainer/ccmcommon/CCMUtils.hpp>

// PCR-0049
DEFINE_VALUETYPE_DATA_TRAITS(CORBA::ValueBase)

namespace
{

} // end anonymous namespace


namespace Cdmw {

namespace CCM {

namespace CIF {

    PushConsumer_impl::PushConsumer_impl(Context*                          ctx,
                                         Components::EventConsumerBase_ptr ccm_consumer)
        : LifeCycleObjectBase(ctx),
          m_ccm_consumer(Components::EventConsumerBase::_duplicate(ccm_consumer))
    {
    }

    PushConsumer_impl::~PushConsumer_impl()
      throw()
    {
        PRINT_INFO("~PushConsumer_impl destructor requested!");
    }

    void PushConsumer_impl::disconnect_push_consumer()  throw(CORBA::SystemException)
    {
    }

    void PushConsumer_impl::push(const CORBA::Any& data) throw(CORBA::SystemException)
    {
        Components::EventBase_var evt;
        CORBA::ValueBase_var value_type;

        /* PCR-0049
        data >>= CORBA::Any::to_value(value_type);
        */
        const CORBA::OctetSeq* p_encoded_data;
        data >>= p_encoded_data;
        CORBA::OctetSeq encoded_data(*p_encoded_data);

        Cdmw::OrbSupport::Codec<CORBA::ValueBase> codec;
        codec.decode(encoded_data, value_type.inout());
        CORBA::add_ref(value_type);
        if (!value_type.in())
        {
            PRINT_ERROR("PushConsumer_impl push nil value_type!");
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMBadEventType,
                CORBA::COMPLETED_NO);
        }

        evt = Components::EventBase::_downcast(value_type.in());
        CORBA::add_ref(evt);
        if (!evt.in())
        {
            PRINT_ERROR("PushConsumer_impl push nil event!");
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMBadEventType,
                CORBA::COMPLETED_NO);
        }

        try
        {
            m_ccm_consumer->push_event(evt.in());
        }
        catch (const Components::BadEventType& )
        {
            PRINT_ERROR("PushConsumer_impl push bad event type!");
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMBadEventType,
                CORBA::COMPLETED_NO);

        }
    }


} // end namespace CIF
} // end namespace CCM
} // end namespace Cdmw
