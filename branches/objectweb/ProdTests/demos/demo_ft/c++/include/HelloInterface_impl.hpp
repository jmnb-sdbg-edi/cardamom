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


#ifndef INCL_DEMO_FT_HELLO_INTERFACE_IMPL_HPP
#define INCL_DEMO_FT_HELLO_INTERFACE_IMPL_HPP


#include <string>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>

#include "TestHello.skel.hpp"


class HelloInterface_impl : public virtual POA_DemoFT::HelloInterface,
                            public virtual Cdmw::OrbSupport::RefCountServantBase
{

public:

    // Constructor
    HelloInterface_impl(std::string name,
                        PortableServer::POA_ptr parentPOA);

    // Destructor
    virtual ~HelloInterface_impl()
        throw();

    // returns the POA of this servant
    virtual PortableServer::POA_ptr _default_POA();
        
        
    // IDL:thalesgroup.com/DemoFT/HelloInterface/hello()
    virtual void hello()
        throw( CORBA::SystemException );

    // IDL:omg.org/FT/Checkpointable/get_state()
    virtual FT::State* get_state()
        throw (FT::NoStateAvailable);

    // IDL:omg.org/FT/Checkpointable/get_state()
    virtual void set_state(const FT::State& s)
        throw (FT::InvalidState);
    
    // return true if state has changed and get_state() has not been called
    virtual bool has_state_changed();
    
private:

    // Hide copy constructor/assignment operator
    HelloInterface_impl(
        const HelloInterface_impl& rhs )
            throw();

    HelloInterface_impl& operator =(
        const HelloInterface_impl& rhs )
            throw();

    // hello counter
    unsigned long m_nb_hello;
    
    // process name
    std::string m_name;
    
    // servant's POA
    PortableServer::POA_var m_poa;
    
    // flag indicating if state has changed
    bool m_state_changed;
    
    
}; // End class HelloInterface_impl 


#endif // INCL_DEMO_FT_HELLO_INTERFACE_IMPL_HPP
 

