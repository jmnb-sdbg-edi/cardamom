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


#include <idllib/PortableGroup.stub.hpp>
#include <loadbalancingserver/TestHello.stub.hpp>
#include <idllib/CdmwLBGroupManager.stub.hpp>
#include <idllib/CdmwLBIOGRFactory.stub.hpp>

namespace Cdmw
{

namespace LB
{

    class TestUtils
    {
    public:

        static  CdmwLBInit::HelloInterface_ptr
        Get_hello_ref_from_repository(CORBA::ORB_ptr orb, std::string rep_name)
            throw (CORBA::SystemException);
        
    private:
        TestUtils();
    };
    
} // end namespace LB
    
} // end namespace Cdmw

