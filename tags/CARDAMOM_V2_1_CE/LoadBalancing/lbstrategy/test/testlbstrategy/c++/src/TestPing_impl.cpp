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

/**
 * @file
 * @brief IDL Pingable Interface implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

// Cdmw Files
#include <testlbstrategy/TestPing_impl.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
namespace Cdmw
{

    Pingable_impl::Pingable_impl(std::string location)
        throw(CORBA::SystemException)
        :m_location(location)
    {
    }
    

    Pingable_impl::~Pingable_impl()
        throw()
    {
    }

    void Pingable_impl::ping ()
        throw(CORBA::SystemException)
    {
      if (!m_location.compare("host5"))
	std::cerr << "[NEW REPLICA]==>"<< m_location  << " <####### PING ######> " << "<==[NEW REPLICA]" <<std::endl;
      else
        std::cerr << m_location  << " <####### PING ######> " << std::endl;
    }


    void Pingable_impl::pingUserException ()
        throw(CORBA::SystemException,
	      CdmwLBStrategy::PingableException)
    {
	std::cerr << m_location  << " <####### PING USER EXCEPTION RAISED######> " << std::endl;
	throw CdmwLBStrategy::PingableException();
    }

};
