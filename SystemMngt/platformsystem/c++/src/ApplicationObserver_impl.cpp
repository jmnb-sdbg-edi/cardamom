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


#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "platformsystem/ApplicationObserver_impl.hpp"
#include "platformsystem/System_impl.hpp"
#include "platformsystem/RunSetManager.hpp"
#include "platformsystem/SystemLogMessageId.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// ApplicationObserver_impl class.
// ----------------------------------------------------------------------
ApplicationObserver_impl::ApplicationObserver_impl (System_impl* system)
{
    m_system = system;
}



void ApplicationObserver_impl::notify_initialisation (const char* application_name)  
     throw (CdmwPlatformMngt::ApplicationNotFound,
            CORBA::SystemException)
{
    // return if system is in initial or in stopped state
    // (inhibition of runsets in this case)
    CORBA::String_var statusInfo;
    CdmwPlatformMngt::SystemStatus status =
                     m_system->get_status (statusInfo.out());

    if ((status == CdmwPlatformMngt::SYSTEM_INITIAL) ||
        (status == CdmwPlatformMngt::SYSTEM_STOPPED))
    {
        return;
    }

    try
    {
        // check if application exist, application not found exception is raised
        CdmwPlatformMngt::Application_var application = 
                    m_system->get_application (application_name);
                    
        
        // acknowledge application element in runset            
        std::string element = application_name;
        std::string runSetName;
        
        RunSet::RunSetDef* p_runSetDef = NULL;
                        
        bool result = m_system->m_runSetManager->acknowledgeElement (
                                element, runSetName, p_runSetDef);
        
        // if runset acknowledged
        if (result && (p_runSetDef != NULL))
        {
            // take owner ship of runset pointer
            std::auto_ptr <RunSet::RunSetDef> runSetDef (p_runSetDef);
        
            // start the runset
            LogMngr::logMessage(INF, MSG_ID_START_RUNSET, runSetName.c_str());
            m_system->m_runSetManager->startRunSet (runSetName, p_runSetDef);
        }
    }
    catch (const RunSetElementNoFoundException&)
    {
        // nothing to do : the application is not defined in a runset
    }
    catch (const std::bad_alloc&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch( const OutOfMemoryException& )
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch( const OutOfResourcesException&)
    {
        throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
    }
    catch( const InternalErrorException&)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }    
    catch (const RunSetAckMisuseException&)
    {
        throw CORBA::INTERNAL (OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
    }
}


void ApplicationObserver_impl::notify_degradation (const char* application_name)  
     throw (CdmwPlatformMngt::ApplicationNotFound,
            CORBA::SystemException)
{
    // check application is defined (exception is raised if undefined)
    m_system->get_application (application_name);
    
    m_system->degradation_event();
}

} // End namespace PlatformMngt
} // End namespace Cdmw

