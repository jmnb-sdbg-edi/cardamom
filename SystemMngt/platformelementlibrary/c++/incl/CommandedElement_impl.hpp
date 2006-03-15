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


#ifndef INCL_PLATFORMMNGT_COMMANDED_ELEMENT_IMPL_HPP
#define INCL_PLATFORMMNGT_COMMANDED_ELEMENT_IMPL_HPP


#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommandedElement.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"



#include <string>

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt
{



class CommandedElement_impl : virtual public POA_CdmwPlatformMngt::CommandedElement,
                              virtual public PortableServer::RefCountServantBase
{

public:


    /**
     *Purpose:
     *<p> Constructor.
     *     
     */
    CommandedElement_impl ()    
        throw ();

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~CommandedElement_impl();

      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/get_command_status:1.0
     * operation
     */
    CdmwPlatformMngt::CommandStatus get_command_status(CORBA::String_out status_info)
          throw (CORBA::SystemException);
      
      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/call_command:1.0
     * operation
     */  
    void call_command (
        const char * command_name,
        const CdmwPlatformMngt::CmdParameterValues & command_args,
        CdmwPlatformMngt::CommandResponseCallback_ptr response_callback)
          throw (CdmwPlatformMngt::CommandNotFound,
                 CdmwPlatformMngt::CommandArgumentInvalid,
                 CdmwPlatformMngt::CommandRejected,
                 CORBA::SystemException);
          

protected:

                               
                       
private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    CommandedElement_impl (CommandedElement_impl& rhs)
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    CommandedElement_impl& operator = (const CommandedElement_impl& rhs)

    {
        return *this;
    };
          
};

class CommandContainer_impl : virtual public POA_CdmwPlatformMngt::CommandContainer,
                              public CommandedElement_impl
{

public:


    /**
     *Purpose:
     *<p> Constructor.
     *     
     */
    CommandContainer_impl ()    
        throw ();

    /**
     *Purpose:
     *<p> Default destructor.
     */
    virtual ~CommandContainer_impl();

      
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/add_command:1.0
     * operation
     */
    void add_command (
        const char * command_name,
        CdmwPlatformMngt::Timeout time_out,
        const CdmwPlatformMngt::CmdParameterDescriptors & command_args)
          throw (CdmwPlatformMngt::CommandAlreadyExists,
                 CdmwPlatformMngt::CommandArgumentAlreadyExists,
                 CORBA::SystemException);
                   
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ManagedElement/add_command:1.0
     * operation
     */               
    void remove_command (const char * command_name)
          throw (CdmwPlatformMngt::CommandNotFound,
                 CORBA::SystemException);

protected:

                               
                       
private:

    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    CommandContainer_impl (CommandContainer_impl& rhs)
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    CommandContainer_impl& operator = (const CommandContainer_impl& rhs)

    {
        return *this;
    };
          

};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_COMMANDED_ELEMENT_IMPL_HPP

