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

#ifndef INCL_HELLO_PUSH_CONSUMER_HPP 
#define INCL_HELLO_PUSH_CONSUMER_HPP 


#include "CosEventComm.skel.hpp"


namespace Cdmw 
{
namespace Tutorial
{
    
    
class HelloPushConsumer_impl : virtual public POA_CosEventComm::PushConsumer,
                               virtual public PortableServer::RefCountServantBase
{

public:
 
    HelloPushConsumer_impl (CORBA::ORB_ptr orb, 
                            PortableServer::POA_ptr poa);


	virtual
    ~HelloPushConsumer_impl();
    
    
    void push(const CORBA::Any& data)
	     throw(CORBA::SystemException);


    void disconnect_push_consumer()
	    throw(CORBA::SystemException);


    PortableServer::POA_ptr _default_POA();   

    
private :

    /**
    * The ORB reference.
    */
    CORBA::ORB_var m_orb;
    
    /**
    * The Root POA reference.
    */
    PortableServer::POA_var m_poa;

    
       
}; // End 


}; // End namespace Tutorial

}; // End namespace Cdmw 


#endif // INCL_HELLO_PUSH_CONSUMER_HPP
