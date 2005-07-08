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


#ifndef INCL_SERVER_HANDLER_IMPL_HPP 
#define INCL_SERVER_HANDLER_IMPL_HPP 

#include <iostream>
#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/Codec.hpp>

#include "testami/TestAMI.skel.hpp"


namespace {
    const int RESULT_NOT_RECEIVED = 0;   
    const int RESULT_NOT_CORRECT  = -1;   
    const int RESULT_CORRECT      = 1;   
}


namespace Cdmw
{
namespace TestAMI
{

class ServerHandler_impl : 
    virtual public POA_Cdmw::TestAMI::AMI_ServerHandler,
    virtual public PortableServer::RefCountServantBase
{
    public:
  
        ServerHandler_impl(CORBA::ORB_ptr orb)
            throw(CORBA::SystemException);
        
        virtual ~ServerHandler_impl()
            throw();
        
        virtual void request_sleeping(CORBA::Short ami_return_val, 
                                      CORBA::Short a, 
                                      CORBA::Short b)
           throw (CORBA::SystemException);    

        virtual void request_sleeping_excep(
                Cdmw::TestAMI::AMI_ServerExceptionHolder* excep_holder)
           throw (CORBA::SystemException);    
           
        virtual void request_sending_exception()
            throw (CORBA::SystemException);
            
        virtual void request_sending_exception_excep(
                Cdmw::TestAMI::AMI_ServerExceptionHolder * excep_holder)
            throw (CORBA::SystemException);
            

        int check_request_sleeping_result();
        int check_request_sending_exception_result();
            
    private:

        ServerHandler_impl(const ServerHandler_impl& rhs)
            throw();
        
        ServerHandler_impl&
        operator=(const ServerHandler_impl& rhs)
            throw();
       

        CORBA::ORB_var m_orb;
        
        int m_request_sleeping_result;
        int m_request_sending_exception_result;
        

};  // End class ServerHandler_impl


}; // End namespace TestAMI
}; // End namespace Cdmw

#endif // INCL_SERVER_HANDLER_IMPL_HPP
