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


#ifndef INCL_TOOLS_SIMULATED_REPOSITORY_MESSAGE_LOGGER_IMPL_HPP
#define INCL_TOOLS_SIMULATED_REPOSITORY_MESSAGE_LOGGER_IMPL_HPP

#include "Foundation/common/System.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "simrepository/MessageLogger.skel.hpp"
#include <fstream>

namespace Cdmw
{
namespace Tools
{

/**
*Purpose:
*<p> 
*
*Features:
*<p> Log method is not thread-safe.
*
*@see  
*
*/
class MessageLogger_impl : 
        public POA_CdmwSimulatedRepository::MessageLogger,
        public virtual PortableServer::RefCountServantBase
{

public:

    /**
     * Purpose:
     * <p> Constructor
     * 
     */ 
    MessageLogger_impl(PortableServer::POA_ptr,
		       const char*)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p> Destructor
     * 
     */ 
    virtual 
    ~MessageLogger_impl()
        throw();
    
    PortableServer::POA_ptr _default_POA();

    //
    // IDL:thalesgroup.com/CdmwSimulatedRepository/MessageLogger/log:1.0
    //
    virtual
    void
    log(const char* header,
        const char* body)
        throw(CORBA::SystemException);
    
protected:
    
private:
    
    /**
     * Purpose:
     * <p>  Copy constructor
     * 
     */ 
    MessageLogger_impl (const MessageLogger_impl& rhs)
        throw();
    
    
    /**
     * Purpose:
     * <p> Assignment operator
     * 
     */ 
    MessageLogger_impl&
    operator=(const MessageLogger_impl& rhs)
        throw();

    /**
     * Holds a reference to ...
     */
    PortableServer::POA_var                 m_poa;
    std::ofstream                           m_os;

}; // End class MessageLogger_impl 

} // End namespace Tools
} // End namespace Cdmw

#endif // INCL_TOOLS_SIMULATED_REPOSITORY_MESSAGE_LOGGER_IMPL_HPP

