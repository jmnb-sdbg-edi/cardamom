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

// Standard Files
#include <string>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include <loadbalancingserverft/TestUtils.hpp>
#include <iostream>
using namespace std;

namespace Cdmw
{

namespace LB
{
    CdmwLBInit::HelloInterface_ptr
    TestUtils::Get_hello_ref_from_repository(CORBA::ORB_ptr orb, std::string rep_name)
        throw (CORBA::SystemException)
    {
        CORBA::Object_var objref;
        CdmwLBInit::HelloInterface_var hello_ref;

        // Get the Naming Context reference
        CdmwNamingAndRepository::Repository_var repository
            = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
        Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                              repository.in());

        Cdmw::CommonSvcs::Naming::NamingInterface ni =
            Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");

        std::string full_name = "dom3/" + rep_name;
        try
        {
            objref = ni.resolve (full_name);
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cerr << " Exception raised: " << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__
                      << " IN FILE: " << __FILE__ << std::endl;
            throw;
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cerr << " Exception raised: " << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__
                      << " IN FILE: " << __FILE__ << std::endl;
            throw;
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cerr << " Exception raised: " << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__
                      << " IN FILE: " << __FILE__ << std::endl;
            throw;
        }
        catch(const CORBA::SystemException& e )
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
        
        hello_ref = CdmwLBInit::HelloInterface::_narrow(objref.in());    
        return hello_ref._retn();
    }
    
}; // end namespace LB
    
}; //end namespace Cdmw

