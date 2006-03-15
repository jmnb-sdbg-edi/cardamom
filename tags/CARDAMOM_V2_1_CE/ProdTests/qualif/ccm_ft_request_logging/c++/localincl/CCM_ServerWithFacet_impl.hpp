/* =========================================================================== *
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
 * =========================================================================== */


#ifndef ___CCM_FT_TEST_CCM_SERVER_WITH_FACET_IMPL_HPP__
#define ___CCM_FT_TEST_CCM_SERVER_WITH_FACET_IMPL_HPP__

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <FaultTolerance/ftrequestlogging/FTReplyRecording.hpp>

#include <ccm_ft_request_logging/User_ccm_ft_request_logging.skel.hpp>
#include <ccm_ft_request_logging/TestObserver.hpp>

namespace CcmFtTest
{
//
// IDL:thalesgroup.com/CcmFtTest/CCM_ServerWithFacet:1.0
//
class CCM_ServerWithFacet_impl : 
    virtual public ::UserCcmFtTest::User_CCM_ServerWithFacet,
    virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    
    CCM_ServerWithFacet_impl(const CCM_ServerWithFacet_impl&);
    void operator=(const CCM_ServerWithFacet_impl&);


    ::CcmFtTest::CCM_ServerWithFacet_Context_var m_session_context;
    Cdmw::FT::RequestLogging::ReplyRecording::ReplyStore* m_store;
    Cdmw::Observer_impl* m_observer;


public:

    CCM_ServerWithFacet_impl();
    ~CCM_ServerWithFacet_impl();

    
    //
    // facet serverHostInfo
    //
    virtual ::CcmFtTest::CCM_HostInfo_ptr get_serverHostInfo()
        throw (CORBA::SystemException);
    
    virtual char* get_hostname()
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


    ::CcmFtTest::CCM_ServerWithFacet_Context* get_context()
    {
        return ::CcmFtTest::CCM_ServerWithFacet_Context::_duplicate(m_session_context.in());
    }
      
};


}; // End of namespace CcmFtTest

#endif // ___CCM_FT_TEST_CCM_SERVER_WITH_FACET_IMPL_HPP__

