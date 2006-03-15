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


#include <CCMContainer/ccmcif/LifeCycleObjectBase.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>


namespace 
{

} // end anonymous namespace


namespace Cdmw {

namespace CCM {

namespace CIF {



LifeCycleObjectBase::LifeCycleObjectBase(Context* ctx)
    throw(CORBA::SystemException)
    : m_context(ctx),
       m_is_removed(false)
{
    CDMW_ASSERT(ctx);
}

LifeCycleObjectBase::~LifeCycleObjectBase()
    throw()
{
    // do nothing
}

//
// IDL:omg.org/CosLifeCycle/LifeCycleObject/copy:1.0
//
CosLifeCycle::LifeCycleObject_ptr 
LifeCycleObjectBase::copy(CosLifeCycle::FactoryFinder_ptr there,
                          const CosLifeCycle::Criteria& the_criteria)
    throw(CosLifeCycle::NoFactory,
          CosLifeCycle::NotCopyable,
          CosLifeCycle::InvalidCriteria,
          CosLifeCycle::CannotMeetCriteria,
          CORBA::SystemException)
{
    using namespace Cdmw::OrbSupport;
    throw CORBA::NO_IMPLEMENT(NO_IMPLEMENT,CORBA::COMPLETED_NO);
}

//
// IDL:omg.org/CosLifeCycle/LifeCycleObject/move:1.0
//
void 
LifeCycleObjectBase::move(CosLifeCycle::FactoryFinder_ptr there,
                          const CosLifeCycle::Criteria& the_criteria)
    throw(CosLifeCycle::NoFactory,
          CosLifeCycle::NotMovable,
          CosLifeCycle::InvalidCriteria,
          CosLifeCycle::CannotMeetCriteria,
          CORBA::SystemException)
{
    using namespace Cdmw::OrbSupport;
    throw CORBA::NO_IMPLEMENT(NO_IMPLEMENT,CORBA::COMPLETED_NO);
}

//
// IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
//
// This should only be called with the context of a request invocation
void 
LifeCycleObjectBase::remove()
    throw(CosLifeCycle::NotRemovable,
          CORBA::SystemException)
{
    try {
        if (m_is_removed)
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO);

          // deactivate object
        m_context->req_passivate();
          
        m_is_removed = true;

    } catch (const PortableServer::Current::NoContext & ) {
        // Not called within the context of an operation invocation
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                              CORBA::COMPLETED_NO);
    } catch (const CORBA::SystemException & ) {
        // rethrow
        throw;
    }
}

} // end namespace CIF
} // end namespace CCM
} // end namespace Cdmw

