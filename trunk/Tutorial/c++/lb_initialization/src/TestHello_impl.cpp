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

/**
 * @file
 * @brief IDL Hello Interface implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

// Cdmw Files
#include "TestHello_impl.hpp"

namespace Cdmw
{

    HelloInterface_impl::HelloInterface_impl(std::string location)
        throw(CORBA::SystemException)
        :m_location(location)
    {
    }
    

    HelloInterface_impl::~HelloInterface_impl()
        throw()
    {
    }

    char* HelloInterface_impl::hello ()
        throw(CORBA::SystemException)
    {
        std::cerr << m_location  << " <####### HELLO ######> " << std::endl;
	//char* hello_string = (char*) m_location.c_str();
	return CORBA::string_dup(m_location.c_str());
    }
};
