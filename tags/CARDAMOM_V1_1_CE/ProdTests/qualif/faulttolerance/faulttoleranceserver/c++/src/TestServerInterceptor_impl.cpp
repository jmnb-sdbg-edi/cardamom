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

#include <faulttoleranceserver/TestServerInterceptor_impl.hpp>

// C++ include
#include <iostream>
#include <cstdlib>


TestServerInterceptor_impl::TestServerInterceptor_impl(PortableInterceptor::ORBInitInfo_ptr info,
                                                       PortableInterceptor::SlotId slotId, std::string i)
    : m_slotId(slotId), i(i), m_is_forwared(false)
{

}

//
// IDL to C++ Mapping
//
char*
TestServerInterceptor_impl::name()
    throw (CORBA::SystemException)
{
    return CORBA::string_dup("TestServerInterceptor");
}

void
TestServerInterceptor_impl::destroy()
    throw (CORBA::SystemException)
{
}

void
TestServerInterceptor_impl::forward_references (CORBA::Object_ptr obj1)
    throw(CORBA::SystemException)
{
//    ::CdmwFT::Location::MembersAdmin_var testMembersAdmin = ::CdmwFT::Location::MembersAdmin::_narrow(obj1);
    ::CdmwFT::Location::PrimaryBackupAdmin_var testPrimaryBackupAdmin =  ::CdmwFT::Location::PrimaryBackupAdmin::_narrow(obj1);
    
//    m_testMembersAdmin = ::CdmwFT::Location::MembersAdmin::_duplicate(testMembersAdmin.in());
    m_testPrimaryBackupAdmin = ::CdmwFT::Location::PrimaryBackupAdmin::_duplicate(testPrimaryBackupAdmin.in());
    
}



void
TestServerInterceptor_impl::receive_request_service_contexts(
                                  PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    std::cout << "\n\nINFO: receive_request_service_contexts" <<i<< std::endl;


}

void
TestServerInterceptor_impl::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    std::cout << "\n\n------------------>>>>INFO: receive_request" <<i<<"  "<<ri->operation()<<"  "<<m_is_forwared<< std::endl;
    std::string operation_name = ri->operation();
    



    if ((strcmp(ri->operation(), "update_group_view") == 0) && (m_is_forwared == false))
    {
        std::cout<<"---------->>>>ri->operation() "<<ri->operation()<<std::endl;
        
        m_is_forwared = true;

        std::cout<<"---------->>>>ForwardRequest"<<std::endl;
        
        throw PortableInterceptor::ForwardRequest(m_testPrimaryBackupAdmin.in(), 1);

    }


//     if (m_is_forwared == true)
//     {
//         m_is_forwared = false;
            
//     }
    
        // il faut que le forward soit activer pour la requete en cours et desactiver pour la requete forwardé
        
    

}

void
TestServerInterceptor_impl::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException)
{
    std::cout << "\n\nINFO: send_reply" <<i<< std::endl;

}

void
TestServerInterceptor_impl::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    std::cout << "\n\nINFO: send_other" <<i<< std::endl;


}

void
TestServerInterceptor_impl::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
        std::cout << "\n\nINFO: send_exception" <<i<< std::endl;



}

