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


#include "SystemMngt/platformelementlibrary/CommandedElement_impl.hpp"

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"



namespace Cdmw
{
namespace PlatformMngt
{



// ----------------------------------------------------------------------
// CommandedElement_impl class.
// ----------------------------------------------------------------------

CommandedElement_impl::CommandedElement_impl ()
          throw ()
{

}


CommandedElement_impl::~CommandedElement_impl()
{

}


CdmwPlatformMngt::CommandStatus CommandedElement_impl::get_command_status(
       CORBA::String_out status_info)
          throw (CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
      
      
 
void CommandedElement_impl::call_command (
        const char * command_name,
        const CdmwPlatformMngt::CmdParameterValues & command_args,
        CdmwPlatformMngt::CommandResponseCallback_ptr response_callback)
          throw (CdmwPlatformMngt::CommandNotFound,
                 CdmwPlatformMngt::CommandArgumentInvalid,
                 CdmwPlatformMngt::CommandRejected,
                 CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


// ----------------------------------------------------------------------
// CommandContainer_impl class.
// ----------------------------------------------------------------------

CommandContainer_impl::CommandContainer_impl ()
          throw ()
{

}


CommandContainer_impl::~CommandContainer_impl()
{

}

void CommandContainer_impl::add_command (
        const char * command_name,
        CdmwPlatformMngt::Timeout time_out,
        const CdmwPlatformMngt::CmdParameterDescriptors & command_args)
          throw (CdmwPlatformMngt::CommandAlreadyExists,
                 CdmwPlatformMngt::CommandArgumentAlreadyExists,
                 CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
                   
             
void CommandContainer_impl::remove_command (const char * command_name)
          throw (CdmwPlatformMngt::CommandNotFound,
                 CORBA::SystemException)
{
	// TODO
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}
      
      

} // End namespace PlatformMngt
} // End namespace Cdmw

