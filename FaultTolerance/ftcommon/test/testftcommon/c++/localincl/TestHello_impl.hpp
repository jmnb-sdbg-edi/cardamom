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


#ifndef INCL_HELLO_INTERFACE_IMPL_HPP 
#define INCL_HELLO_INTERFACE_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "testftcommon/TestHello.skel.hpp"



#include <string>

namespace Cdmw
{
class HelloInterface1_impl :
        public virtual POA_HelloInterface1,
        virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        HelloInterface1_impl()
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~HelloInterface1_impl()
                throw();
    
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual void hello ()
            throw(CORBA::SystemException);
                  

    private:

        // Hide copy constructor/assignment operator
        HelloInterface1_impl(const HelloInterface1_impl& rhs)
                throw();
        
        HelloInterface1_impl&
        operator=(const HelloInterface1_impl& rhs)
                throw();


}; // End class HelloInterface2_impl
class HelloInterface2_impl :
        public virtual POA_HelloInterface2,
        virtual public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        HelloInterface2_impl()
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~HelloInterface2_impl()
                throw();
    
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual void hello ()
            throw(CORBA::SystemException);
                  

    private:

        // Hide copy constructor/assignment operator
        HelloInterface2_impl(const HelloInterface2_impl& rhs)
                throw();
        
        HelloInterface2_impl&
        operator=(const HelloInterface2_impl& rhs)
                throw();


}; // End class HelloInterface2_impl

}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 

