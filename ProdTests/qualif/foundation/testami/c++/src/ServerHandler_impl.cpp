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

#include <string>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "testami/ServerHandler_impl.hpp"


namespace 
{
    CORBA::Short EXPECTED_RETURN_VALUE = 10;
    CORBA::Short EXPECTED_A_VALUE = 20;
    CORBA::Short EXPECTED_B_VALUE = 30;
}



namespace Cdmw
{
namespace TestAMI
{


ServerHandler_impl::ServerHandler_impl(CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
{
    m_request_sleeping_result = RESULT_NOT_RECEIVED;
    m_request_sending_exception_result = RESULT_NOT_RECEIVED;
}

ServerHandler_impl::~ServerHandler_impl()
    throw()
{
}


            
void ServerHandler_impl::request_sleeping (CORBA::Short ami_return_val, 
                                           CORBA::Short a, 
                                           CORBA::Short b)
     throw(CORBA::SystemException)                  
{
    std::cout << "request_sleeping() result received." << std::endl;    
    m_request_sleeping_result = RESULT_CORRECT;
    
    if (ami_return_val != EXPECTED_RETURN_VALUE) {
        std::cerr << "Return value of request_sleeping() is not the one expected: "
                  << ami_return_val << std::endl;
        m_request_sleeping_result = RESULT_NOT_CORRECT;
    }
    
    if (a != EXPECTED_A_VALUE) {
        std::cerr << "intout 'a' value of request_sleeping() is not the one expected: "
                  << a << std::endl;
        m_request_sleeping_result = RESULT_NOT_CORRECT;
    }
    
    if (b != EXPECTED_B_VALUE) {
        std::cerr << "out 'b' value of request_sleeping() is not the one expected: "
                  << b << std::endl;
        m_request_sleeping_result = RESULT_NOT_CORRECT;
    }
}

void ServerHandler_impl::request_sleeping_excep(
        Cdmw::TestAMI::AMI_ServerExceptionHolder* excep_holder)
    throw (CORBA::SystemException)
{
    std::cout << "request_sleeping() result received: it is an Exception !!" << std::endl;    
    m_request_sleeping_result = RESULT_NOT_CORRECT;
}    


void ServerHandler_impl::request_sending_exception()
    throw (CORBA::SystemException)
{
    std::cout << "request_sending_exception() result received: it is not an Exception !!" << std::endl;    
    m_request_sending_exception_result = RESULT_NOT_CORRECT;
}


void ServerHandler_impl::request_sending_exception_excep(
        Cdmw::TestAMI::AMI_ServerExceptionHolder * excep_holder)
    throw (CORBA::SystemException)
{
    std::cout << "request_sending_exception() result received." << std::endl;    
    
    try {
        excep_holder->raise_request_sending_exception();

        std::cerr << "request_sending_exception() has'nt send an exception !!" << std::endl;
        m_request_sending_exception_result = RESULT_NOT_CORRECT;
        
    } catch (const TestException& e) {
        m_request_sending_exception_result = RESULT_CORRECT;
    } catch (const CORBA::Exception& e) {
        std::cerr << "request_sending_exception() has'nt send the expected exception: \n"
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                  << std::endl;
        m_request_sending_exception_result = RESULT_NOT_CORRECT;
    } catch (...) {
        std::cerr << "request_sending_exception() has'nt send the expected exception: \n"
                  << "Unknown non-CORBA exception"
                  << std::endl;
        m_request_sending_exception_result = RESULT_NOT_CORRECT;
    }
    
}


int ServerHandler_impl::check_request_sleeping_result() {
    return m_request_sleeping_result;
}


int ServerHandler_impl::check_request_sending_exception_result() {
    return m_request_sending_exception_result;
}



}; // End namespace TestAMI
}; // End namespace Cdmw

