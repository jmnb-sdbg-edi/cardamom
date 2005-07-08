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


#ifndef INCL_PERFORMANCELIBRARY_INIT_UTILS_HPP 
#define INCL_PERFORMANCELIBRARY_INIT_UTILS_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"

#include "TraceAndPerf/performancelibrary/PerformanceMngr.hpp"

#include <string>
#include <cstddef>

namespace Cdmw 
{
namespace Performance
{
    
/**
 * Purpose:
 * <p> This class provides helper functions to be used by the CDMW generated 
 * CdmwInterface.cpp file.
 *
 *@see CDMW_init
 *@see CDMW_cleanup
 */
 
class InitUtils
{
    
  public:
  

    /**
     * Purpose:
     * <p> Initialise the CDMW Performance Library.
     * 
     *@param performancePOA       The POA to be used by Performance Library
     *@param application_name     The application name owning the CDMW Process     
     *@param process_name         The process name of the CDMW Process
     *@param performance_filename The performance file name to store results
     *
     *@exception CORBA::INTERNAL Internal failure. The following CDMW minor codes
     * details the reason:  
     * <UL>
     * <LI><B>INTERNALCdmwRepositoryError</B> when its a CDMW Repository Error.
     * This may happen if the Performance admin name domain is not present in the
     * Repository, or if the ADMIN_ROOT_CONTEXT_ID is not set.
     * <LI><B>INTERNAL</B> when an Internal Error occurs during Init of PerformanceMngr,
     * or within OsSupport library.
     * <LI><B>INTERNALCdmwRepositoryNotExists</B> when the CDMW Repository object
     * does not exist (has been deleted!)
     * </UL>
     *
     *@exception CORBA::BAD_PARAM Invalid parameter. The following CDMW minor codes
     * details the reason:  
     * <B>BAd_PARAMInvalidPerformanceAdminLocation</B> when the location of the
     * PerformanceMngr object is not valid.
     * <B>BAD_PARAMInvalidPerformanceAdminName</B> when the name of the
     * PerformanceMngr object is not valid.
     *
     *@exception CORBA::NO_MEMORY. The <B>NO_MEMORY</B> CDMW minor code indicates
     * that CDMW could not acquire memory from the system.
     *
     *@exception CORBA::TRANSIENT The <B>TRANSIENTCannotReachApplicationAgent</B>
     * minor code indicates that it cannot reach the CDMW ApplicationAgent now.
     *
     *@exception CORBA::SystemException Any CORBA system exception that can be
     * raised by the undelying ORB.
     *
     *@see Cdmw::Performance::PerformanceMngr::init
     */
     
    static
    void init_performance_library (PortableServer::POA_ptr  performancePOA,
				                   const std::string & application_name,
				                   const std::string & process_name,
				                   const std::string & performance_filename)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);

}; // End class InitUtils
        

}; // End namespace Performance
}; // End namespace Cdmw 

#endif // INCL_PERFORMANCELIBRARY_INIT_UTILS_HPP


