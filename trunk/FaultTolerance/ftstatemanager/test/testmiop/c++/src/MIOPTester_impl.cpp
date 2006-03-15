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

#include <fstream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>

#include <testmiop/MIOPTester_impl.hpp>



namespace CdmwFT
{
namespace Test
{


MIOPTester_impl::MIOPTester_impl(CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
{ 
}


void MIOPTester_impl::test_miop_request(CORBA::UShort client_id,
                                        CORBA::UShort counter)
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(M_map_mutex);
    std::cout << "     =====> received from " << client_id << ": " << counter << std::endl;
    m_received_requests[client_id][counter] = true;
}


}; // End namespace Test

}; // End namespace Cdmw

