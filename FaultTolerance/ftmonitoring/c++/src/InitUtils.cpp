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


#include <ftmonitoring/InitUtils.hpp>
#include <ftmonitoring/MonitoringLibraryInterface_impl.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <string>
#include <list>

namespace Cdmw {
namespace FT {

    typedef std::list<std::string> ArgvList;

    CdmwFT::MonitoringLibrary::MonitoringLibraryInterface_ptr
    InitUtils::Create_monitoring_library(int                     argc, 
                                         char*                   argv[],
                                         CORBA::ORB_ptr          orb)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
    {
        
        ArgvList argList;
        for (int i=0; i<argc; i++) 
            argList.push_back(argv[i]);

        CdmwFT::MonitoringLibrary::MonitoringLibraryInterface_impl::Init(orb, argList);
        
        CORBA::Object_var monitLibObj = orb->resolve_initial_references("MonitoringLibrary");

        CdmwFT::MonitoringLibrary::MonitoringLibraryInterface_var the_monitoring_library 
            = CdmwFT::MonitoringLibrary::MonitoringLibraryInterface::_narrow(monitLibObj.in());

        return the_monitoring_library._retn();
    }
    
    
}; // namespace FT
}; // namespace Cdmw

