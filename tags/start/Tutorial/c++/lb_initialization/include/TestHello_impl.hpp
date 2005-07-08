/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */
                                                                                                 
#ifndef INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP
#define INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP

/**
 * @file
 * @brief IDL Hello Interface implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "Hello.skel.hpp"
#include <iostream>
using namespace std;

namespace Cdmw
{

    class HelloInterface_impl :
        public virtual POA_CdmwLBInit::HelloInterface,
        virtual public PortableServer::RefCountServantBase
    {
        
    public:
        
        /**
         * Constructor
         */
        HelloInterface_impl(std::string location)
            throw(CORBA::SystemException);
        
        /**
         * Destructor
         */
        virtual
        ~HelloInterface_impl()
            throw();
        
        /**
         * IDL interface implementation
         *
         */
        virtual char* hello ()
                throw(CORBA::SystemException);
        private:

        // Hide copy constructor/assignment operator
        HelloInterface_impl(const HelloInterface_impl& rhs)
            throw();

        HelloInterface_impl&
        operator=(const HelloInterface_impl& rhs)
            throw();

        std::string m_location;
        
    }; // End class HelloInterface_impl
    
}; // End namespace Cdmw

#endif // INCL_CDMW_TEST_HELLO_INTERFACE_IMPL_HPP

