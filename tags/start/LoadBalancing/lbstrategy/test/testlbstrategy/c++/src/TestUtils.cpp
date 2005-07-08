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

#include <testlbstrategy/TestUtils.hpp>
#include <fstream>
#include <iostream>
using namespace std;

namespace
{

    const int NB_LOCATION = 3;

} // end anonymous namespace

namespace Cdmw
{

namespace LB
{
    const char* TestUtils::M_locations[NB_LOCATION] = {"host1", "host2", "host3"};
    const char* TestUtils::M_files[NB_LOCATION] = {"hello1", "hello2", "hello3"};

    CdmwLBStrategy::Pingable_ptr
    TestUtils::Get_hello_ref_from_file(CORBA::ORB_ptr orb, std::string file_name)
        throw (CORBA::SystemException)
    {
        CORBA::Object_var objref;
        CdmwLBStrategy::Pingable_var pingable_ref;

        // import the object reference from the file
        std::ifstream is(file_name.c_str());
        if (is.good())
        {
            std::string objref_str;
            is >> objref_str;
            try
            {
                objref = orb->string_to_object(objref_str.c_str());
                pingable_ref = CdmwLBStrategy::Pingable::_narrow(objref.in());
                
            }
            catch( CORBA::SystemException& e )
                {
                    std::cerr << " CORBA System Exception raised: " << e << std::endl;
                    std::cerr << " **** TEST FAILED AT LINE " << __LINE__
                              << " IN FILE: " << __FILE__ << std::endl;
                    throw;
                }
            catch (...)
                {
                    throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);
                }
            
            is.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: " << file_name << std::endl;
            throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);
        }
        
        return pingable_ref._retn();
    }

}; // end namespace LB

}; //end namespace Cdmw

