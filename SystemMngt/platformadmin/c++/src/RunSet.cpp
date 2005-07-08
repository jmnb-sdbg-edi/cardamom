/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include <util/PlatformUtils.hpp>

#include "Foundation/orbsupport/OrbSupport.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"

#include "SystemMngt/platformlibrary/LogMngr.hpp"

#include "platformadmin/RunSet.hpp"
#include "platformadmin/RunSetParser.hpp"
#include "platformadmin/AdminLogMessageId.hpp"

namespace Cdmw {         // Begin namespace Cdmw
namespace PlatformAdmin { // Begin namespace PlatformAdmin

CdmwPlatformMngt::RunSetDef* RunSet::get_definition(
    const RunSetParser& runSetParser )
{
    try
    {
        // Get the run set's name
        CdmwPlatformMngt::RunSetDef_var runSetDef
            = new CdmwPlatformMngt::RunSetDef();
        runSetDef->set_name = CORBA::string_dup( runSetParser.name().c_str() );

        // Add the run set's elements
        unsigned int runSetElementCount
            = runSetParser.get_run_set_element_count();

        runSetDef->run_set.length( runSetElementCount );
        for( unsigned int runSetElementIndex = 0;
             runSetElementIndex < runSetElementCount;
             runSetElementIndex++ )
        {
            std::string newRunSetElementName
                = runSetParser.get_run_set_element_name(
                    runSetElementIndex ).c_str();

            // Checks there is no run set element redefinition
            for( unsigned int l_runSetElementIndex = 0;
                 l_runSetElementIndex < runSetElementIndex;
                 l_runSetElementIndex++ )
            {
                std::string runSetElementName =
                     runSetDef->run_set[ l_runSetElementIndex ].in();

                if( runSetElementName == newRunSetElementName )
                    throw CdmwPlatformMngt::DuplicateRunSetElement(
                        newRunSetElementName.c_str() );
            }

            runSetDef->run_set[ runSetElementIndex ] =
                CORBA::string_dup( newRunSetElementName.c_str() );
        }

        return runSetDef._retn();
    }
    catch( const std::bad_alloc& )
    {
        CDMW_THROW( OutOfMemoryException );
    }
    catch( const CdmwPlatformMngt::DuplicateRunSetElement& e )
    {
        PlatformMngt::LogMngr::logMessage(
            PlatformMngt::ERR,
            MSG_ID_ADMIN_DUPLICATE_RUNSET_ELEMENT,
            e.element.in() );

        throw LoggedException();
    }
}

} // End namespace PlatformAdmin
} // End namespace Cdmw

