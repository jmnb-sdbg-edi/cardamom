/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#ifndef INCL_CDMW_TEST_PINGABLE_IMPL_HPP
#define INCL_CDMW_TEST_PINGABLE_IMPL_HPP

/**
 * @file
 * @brief IDL Pingable Interface implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <testlbstrategy/TestPing.skel.hpp>
#include <iostream>
using namespace std;

namespace Cdmw
{

    class Pingable_impl :
        public virtual POA_CdmwLBStrategy::Pingable,
        virtual public PortableServer::RefCountServantBase
    {
        
    public:
        
        /**
         * Constructor
         */
        Pingable_impl(std::string location)
            throw(CORBA::SystemException);
        
        /**
         * Destructor
         */
        virtual
        ~Pingable_impl()
            throw();
        
        /**
         * IDL interface implementation
         *
         */
        virtual void ping ()
	    throw(CORBA::SystemException);
        virtual void pingUserException ()
	    throw(CORBA::SystemException,
		  CdmwLBStrategy::PingableException);
    private:

        // Hide copy constructor/assignment operator
        Pingable_impl(const Pingable_impl& rhs)
            throw();

        Pingable_impl&
        operator=(const Pingable_impl& rhs)
            throw();

        std::string m_location;
    }; // End class Pingable_impl
    
}; // End namespace Cdmw

#endif // INCL_CDMW_TEST_PINGABLE_IMPL_HPP

