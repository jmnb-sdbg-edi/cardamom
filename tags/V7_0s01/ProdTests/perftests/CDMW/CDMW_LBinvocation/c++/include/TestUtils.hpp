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

// Standard Files
#include <string>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>


#include <TestPingableC.h>
#include <idllib/CdmwLBGroupManager.stub.hpp>
#include <idllib/CdmwLBIOGRFactory.stub.hpp>

namespace Cdmw
{

namespace LB
{

    class TestUtils
    {
    public:
        static const char* M_locations[3];
        static const char* M_files[3];

        static  CdmwLBPerf::PingableInterface_ptr
        Get_pingable_ref_from_file(CORBA::ORB_ptr orb, std::string file_name)
            throw (CORBA::SystemException);
        
        
        static CORBA::Object_ptr
            Create_group(CORBA::ORB_ptr orb,
                         CdmwLB::LBGroupManager_ptr group_manager,
                         const char * rep_id,
                         const char * policy)
            throw (CORBA::Exception);

        
    private:
        TestUtils();
    };
    
} // end namespace LB
    
} // end namespace Cdmw

