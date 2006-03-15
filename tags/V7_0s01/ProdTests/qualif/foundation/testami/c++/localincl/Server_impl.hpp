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


#ifndef INCL_SERVER_IMPL_HPP 
#define INCL_SERVER_IMPL_HPP 

#include <iostream>
#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/Codec.hpp>

#include "testami/TestAMI.skel.hpp"


namespace Cdmw
{
namespace TestAMI
{

class Server_impl : 
    virtual public POA_Cdmw::TestAMI::Server,
    virtual public PortableServer::RefCountServantBase
{
    public:
  
        Server_impl(CORBA::ORB_ptr orb)
            throw(CORBA::SystemException);
        
        virtual ~Server_impl()
            throw();
        
        virtual CORBA::Short request_sleeping(CORBA::Short time_to_sleep, 
                                              CORBA::Short& a, 
                                              CORBA::Short& b)
           throw (CORBA::SystemException);    

        virtual void request_sending_exception()
            throw (CORBA::SystemException, TestException);
            
            
    private:

        Server_impl(const Server_impl& rhs)
            throw();
        
        Server_impl&
        operator=(const Server_impl& rhs)
            throw();
       

        CORBA::ORB_var m_orb;

};  // End class Server_impl


}; // End namespace TestAMI
}; // End namespace Cdmw

#endif // INCL_SERVER_IMPL_HPP

