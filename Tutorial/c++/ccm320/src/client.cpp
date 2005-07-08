/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
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
 * =========================================================================== */

#include <iostream>

#include <Foundation/common/System.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <Repository/naminginterface/NamingUtil.hpp>

#include "Hello_cif.stub.hpp"


namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    CosNaming::NamingContext_ptr
    get_naming_service(CORBA::ORB_ptr orb)
        throw(CORBA::TRANSIENT)
    {
        CosNaming::NamingContext_var nc = CosNaming::NamingContext::_nil();
        //
        // Get naming service
        //        
        try {
            CORBA::Object_var obj = orb->resolve_initial_references("NameService");
            
            nc = CosNaming::NamingContext::_narrow(obj.in() );
        } catch (const CORBA::SystemException & ex) {
            std::cerr << "Couldn't retrieve reference of the Name Service"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ") :" 
                      << ex << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                   CORBA::COMPLETED_NO);
        } catch (const CORBA::Exception & ex) {
            std::cerr << "Couldn't retrieve reference of the Name Service"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" 
                      << ex << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                   CORBA::COMPLETED_NO);
        }
        if (CORBA::is_nil(nc.in())) {
            std::cerr << "Couldn't retrieve reference of the Name Service"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" 
                      << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                   CORBA::COMPLETED_NO);
        }

        return nc._retn();
    }

}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    int status = SUCCESS;

    CORBA::ORB_var orb;  


	 // Get Repository host
    std::string caller_id;
    try {
        caller_id = Cdmw::OsSupport::OS::get_option_value(argc, argv, 
                                         "--caller-id=");
    } catch (const Cdmw::OutOfMemoryException & ) {
		std::cerr << "Not enought memory to get option value!" << std::endl;
        return FAILURE;
    } catch (const Cdmw::InternalErrorException & ) {
		std::cerr << "Internal error!" << std::endl;
        return FAILURE;
    }
    
    if ((caller_id == "no") ||
        (caller_id == "yes")) {
        caller_id="";
    }
  
   
    try 
    {
        // ===================================================
        // Initialize the ORB
        // ===================================================
        orb = CORBA::ORB_init(argc, argv);

        // Get reference of the naming service
        CosNaming::NamingContext_var nc = get_naming_service(orb.in());
        using namespace Cdmw::NamingAndRepository;
        NamingInterface ni(nc.in());

        // Get a reference to a component facet

        Hello::Display_var display =
            NamingUtil<Hello::Display>::resolve_name
            (ni,"demo_ccm_lb/hello_servers/hello_server_facet.group"); 
        // Call the facet MAX times and then exit.
        const int MAX=1;
        for (int i=0;i<MAX;++i) {
            try {
                display->display_hello (caller_id.c_str());
            } catch(const CORBA::SystemException & ex) {
                std::cerr << "Received a System exceptiond"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" 
                          << ex << std::endl;
            }
        }
    } 
    catch (const Cdmw::Exception & ex) 
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } 
    catch(const CORBA::Exception& ex)  
    {        
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
       
    // =====================================================
    // Destroy orb
    // =====================================================
    if(!CORBA::is_nil(orb.in()))
    {
        try 
        {
            orb -> destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}

