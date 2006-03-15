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


#include <cstdlib>
#include <string>
#include <sstream>

#include "TraceAndPerf/performancelibrary/InitUtils.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/NamingUtil.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"


namespace Cdmw
{

namespace Performance
{


void 
InitUtils::init_performance_library (
                          PortableServer::POA_ptr performancePOA,
		                  const std::string & application_name,
                          const std::string & process_name,
                          const std::string & performance_filename)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
    using Cdmw::Performance::PerformanceMngr;
    using Cdmw::NamingAndRepository::RepositoryInterface;
    using Cdmw::Common::Locations;
    
    using namespace Cdmw::CommonSvcs::Naming;
    using namespace Cdmw::OrbSupport;
    
    // init of PerformanceMngr
    CdmwPerformanceAdmin::PerformanceAdmin_var perfo_manager = 
	                      CdmwPerformanceAdmin::PerformanceAdmin::_nil();
    
    try 
    {
        perfo_manager = PerformanceMngr::init (performancePOA,
					                           performance_filename);
	
    } 
    catch (const Cdmw::OutOfMemoryException & ex) 
    {
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                      CORBA::NO_MEMORY(NO_MEMORY,CORBA::COMPLETED_NO),
                      ex.what());
    } 
    catch (const Cdmw::BadParameterException & ex) 
    {
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAM,CORBA::COMPLETED_NO),
                      ex.what());
    }
    
    // Register PerformanceMngr object
    std::string perfo_manager_name;        
    
    // Build the name of the Performance Manager object
    // Pattern is :
    //    hostname/appli_name/process_name/PERFORMANCE/PerformanceAdmin

    perfo_manager_name = Cdmw::OsSupport::OS::get_hostname();
    perfo_manager_name += std::string("/") + application_name;
    perfo_manager_name +=  std::string("/") 
	                       + process_name 
	                       + std::string("/PERFORMANCE/PerformanceAdmin");
    
    // Get "AdminRootContext" root context and then
    // register PerformanceMngr object
    CosNaming::NamingContext_var admin_root = CosNaming::NamingContext::_nil();
    
    try 
    {
        CdmwNamingAndRepository::Repository_var rep =
	                      RepositoryInterface::get_repository();
	
        admin_root = rep->resolve_root_context(Locations::ADMIN_ROOT_CONTEXT_ID);
	
    } 
    catch (const CdmwNamingAndRepository::Repository::NoRootContext & ) 
    {
        std::string s(Locations::ADMIN_ROOT_CONTEXT_ID);
        s += " is not a Root Context!";
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO),
                      s );
    } 
    catch (const CORBA::SystemException & ex) 
    {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex);
    }   
    
    try 
    {
        Cdmw::CommonSvcs::Naming::NamingInterface ni (admin_root.in());
        // If already registered, force its registration
        ni.bind(perfo_manager_name,perfo_manager.in(),true);
    } 
    catch (const CosNaming::NamingContext::NotFound & ) 
    {
        // Should not happen!
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMInvalidPerformanceAdminLocation,CORBA::COMPLETED_NO));
    } 
    catch (const CosNaming::NamingContext::CannotProceed & )
    {
        // Should not happen!
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO));
    } 
    catch (const CosNaming::NamingContext::InvalidName & ) 
    {
        std::string s(perfo_manager_name);
        s += " is not valid!";
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMInvalidPerformanceAdminName,CORBA::COMPLETED_NO),
                      s);
    } 
    catch (const CosNaming::NamingContext::AlreadyBound & ) 
    {
        // Should not happen because force is set to true!
        std::string s(perfo_manager_name);
        s += " is already bound!";
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO),
                      s);
    } 
    catch (const CORBA::SystemException & ex) 
    {
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex);
    }
}



}; // namespace Performance
}; // namespace Cdmw


