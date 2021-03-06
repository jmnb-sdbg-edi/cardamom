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

#ifndef INCL_HELLO_INTERFACE_IMPL_HPP 
#define INCL_HELLO_INTERFACE_IMPL_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <LifeCycle/lifecycle/LifeCycleSingleObject.hpp>

#include "Hello.skel.hpp"



#include <string>

namespace Cdmw
{
namespace Tutorial
{


class HelloInterface_impl :
        public virtual POA_Hello::HelloInterface,
        public Cdmw::LifeCycle::LifeCycleSingleObject,
        public virtual Cdmw::OrbSupport::RefCountServantBase
{

    public:

 
        HelloInterface_impl(const Cdmw::LifeCycle::ObjectDeactivator & obj_d)
            throw(CORBA::SystemException);


        virtual 
        ~HelloInterface_impl()
                throw();
    
         
        //
        // IDL:thalesgroup.com/Hello/HelloInterface/display_hello:1.0
        //
        virtual void display_hello ()
            throw(CORBA::SystemException);
                  

    private:

        // Hide default constructor
        HelloInterface_impl()
            throw(CORBA::SystemException);
            
            
        // Hide copy constructor/assignment operator
        HelloInterface_impl(const HelloInterface_impl& rhs)
                throw();
        
        HelloInterface_impl&
        operator=(const HelloInterface_impl& rhs)
                throw();


}; // End class HelloInterface_impl 

}; // End namespace Tutorial
}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 
