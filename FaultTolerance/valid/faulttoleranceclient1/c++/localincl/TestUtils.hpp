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

#ifndef INCL_TESTUTILS_HPP 
#define INCL_TESTUTILS_HPP 

// Standard Files 
#include <string>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>


#include <FaultTolerance/idllib/FT.stub.hpp>
#include <faulttoleranceclient1/TestHello.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>


namespace Cdmw 
{

namespace FT 
{

class TestUtils
{
public:

    static int 
    get_object_group_version(CORBA::ORB_ptr orb, ::FT::ObjectGroup_ptr object_group)
        throw (CORBA::Exception);

    static int 
    get_number_of_member(CORBA::ORB_ptr orb, ::FT::ObjectGroup_ptr object_group)
        throw (CORBA::Exception);

    static ::FT::ObjectGroup_ptr
    update_group_with_primary_outside_cdmw_ft
                         (CORBA::ORB_ptr orb, 
                          ::FT::ObjectGroup_ptr object_group,
                          CdmwFT::ReplicationManager_ptr replication_manager,
                          ::FT::Location & primary_location)
        throw (CORBA::Exception);
private:
    TestUtils();
};

} // end namespace FT

} // end namespace Cdmw

#endif //INCL_TESTUTILS_HPP 

