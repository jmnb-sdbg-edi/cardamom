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


// Standard Files 
#include <string>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>


#include <idllib/FT.stub.hpp>
#include <testftreplicationmanagerinit/TestHello.stub.hpp>
#include <idllib/CdmwFTReplicationManager.stub.hpp>


namespace Cdmw 
{

namespace FT 
{

class TestUtils
{
public:
    static  int   M_current_primary_index;
    static const char* M_locations[3];
    static const char* M_files[3];
    static const char* M_files2[3];

    static  CdmwFT::Test::HelloInterface_ptr
    get_hello_ref_from_file(CORBA::ORB_ptr orb, std::string file_name)
        throw (CORBA::SystemException);

    static CdmwFT::FaultManagement::FaultDetector_ptr
    get_fault_detector_ref_from_file(CORBA::ORB_ptr orb, std::string file_name)
        throw (CORBA::SystemException);

    static ::FT::ObjectGroup_ptr
    create_group(CORBA::ORB_ptr orb, 
                 CdmwFT::ReplicationManager_ptr replication_manager,
                 const char * rep_id)
        throw (CORBA::Exception);

    static ::FT::ObjectGroup_ptr
    change_primary(CORBA::ORB_ptr orb,
                   ::FT::ObjectGroup_ptr object_group,
                   CdmwFT::ReplicationManager_ptr replication_manager, 
                   int primary_index)
        throw (CORBA::Exception);

private:
    TestUtils();
};

} // end namespace FT

} // end namespace Cdmw

