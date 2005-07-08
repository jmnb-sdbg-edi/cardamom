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


#ifndef ___TEST_FTCCMCIF_COMPONENT_WITH_CONSUMER_IMPL_HPP__
#define ___TEST_FTCCMCIF_COMPONENT_WITH_CONSUMER_IMPL_HPP__

#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include <FaultTolerance/idllib/FT.stub.hpp>

#include "testftccmcif/user_ftccmcif.skel.hpp"


namespace TestFtCcmCif
{
//
// IDL:thalesgroup.com/TestFtCcmCif/CCM_ComponentWithConsumer:1.0
//
class CCM_ComponentWithConsumer_impl : 
    virtual public User_CCM_ComponentWithConsumer,
    virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    
    CCM_ComponentWithConsumer_impl(const CCM_ComponentWithConsumer_impl&);
    void operator=(const CCM_ComponentWithConsumer_impl&);

    CCM_ComponentWithConsumer_Context_var m_session_context;

    std::string  m_location;

public:

    CCM_ComponentWithConsumer_impl();
    ~CCM_ComponentWithConsumer_impl();

    
      
    //
    // consumer the_consumer
    //
    virtual void push_the_consumer (TestEvent * event)
        throw (CORBA::SystemException);
    

    //
    // Session component specific operations
    //
    void set_session_context(Components::SessionContext_ptr ctx)
        throw(CORBA::SystemException);
    void ccm_activate()
        throw(CORBA::SystemException);
    void ccm_passivate()
        throw(CORBA::SystemException);
    void ccm_remove()
        throw(CORBA::SystemException);


    CCM_ComponentWithConsumer_Context* get_context()
    {
        return ::TestFtCcmCif::CCM_ComponentWithConsumer_Context::_duplicate(m_session_context.in());
    }
      
};


}; // End of namespace TestFtCcmCif

#endif // ___TEST_FTCCMCIF_COMPONENT_WITH_CONSUMER_IMPL_HPP__

