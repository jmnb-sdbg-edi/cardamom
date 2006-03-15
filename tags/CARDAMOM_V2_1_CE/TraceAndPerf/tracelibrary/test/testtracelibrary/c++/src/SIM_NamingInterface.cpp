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


#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/commonsvcs/naming/InvalidNameException.hpp"

namespace Cdmw
{
namespace CommonSvcs
{
namespace Naming
{
    
// Default chunk size for CosNaming::NamingContext::list operation
const CORBA::ULong NamingInterface::LIST_CHUNK_SIZE(100);

// The fake invalid name implementation
InvalidNameException::~InvalidNameException()
    throw()
{
}


InvalidNameException::InvalidNameException(const std::string& reason) 
    throw()
    : Exception(Exception::SAFE,reason) 
{
}

InvalidNameException::InvalidNameException(const InvalidNameException& rhs) 
    throw()
    : Exception(rhs)
{
}

}; // End namespace Naming
}; // End namespace CommonSvcs
}; // End namespace Cdmw





namespace Cdmw
{
namespace CommonSvcs
{
namespace Naming
{

CORBA::Object_ptr the_AdminInterfaceObject(CORBA::Object::_nil());

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/

NamingInterface::NamingInterface() 
    throw(CORBA::SystemException)
    : m_context(CosNaming::NamingContext::_nil())
{
    // does nothing by default
}


NamingInterface::NamingInterface(CosNaming::NamingContext_ptr nc)
    throw(CORBA::SystemException)
    : m_context(CosNaming::NamingContext::_duplicate(nc))
{
   if (CORBA::is_nil(nc)) throw CORBA::BAD_PARAM(); 
}
   

 
NamingInterface::NamingInterface(const NamingInterface & rhs)
    throw(CORBA::SystemException)
    : m_context(CosNaming::NamingContext::_nil())
{
    // does nothing
}



NamingInterface::~NamingInterface()
    throw()
{
    // does nothing
}




NamingInterface& NamingInterface::operator=(const NamingInterface & rhs)
    throw(CORBA::SystemException)
{
    return *this;
}

void
NamingInterface::append_chunk(const CosNaming::BindingList & bl,
                              BindingList&                   blist)
{

}



bool 
NamingInterface::list_context(BindingList& blist,
                              CORBA::ULong how_many)
    throw()
{
    return true;
}




bool
NamingInterface::clear_chunk(CosNaming::NamingContext_ptr nc, 
                             const CosNaming::BindingList & bl)
    throw()
{
    bool done = true;
    return done;
}



// Depth first
bool 
NamingInterface::clear_context(CosNaming::NamingContext_ptr nc)
    throw()
{
    bool all_done = false;
    return all_done;
}




// Forces creation of intermediate naming contexts if requested
CosNaming::NamingContext_ptr
NamingInterface::bind_new_context(const CosNaming::Name & name, bool force)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::AlreadyBound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    return NULL;
}




//
// Operations from CosNaming::NamingContext
//
void 
NamingInterface::bind(const std::string& s, CORBA::Object_ptr o, bool force)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{
    the_AdminInterfaceObject = CORBA::Object::_duplicate (o);
}

void
NamingInterface::rebind(const std::string& s, CORBA::Object_ptr o, bool force)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

}






void
NamingInterface::bind_context(const std::string&           s, 
                              CosNaming::NamingContext_ptr nc, 
                              bool                         force)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

}




void
NamingInterface::bind_context(const std::string&           s, 
                              const NamingInterface&       ni, 
                              bool                         force)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CosNaming::NamingContext::AlreadyBound,
          CORBA::SystemException)
{

}





void
NamingInterface::rebind_context(const std::string&           s, 
                                CosNaming::NamingContext_ptr nc, 
                                bool                         force)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

}


void
NamingInterface::rebind_context(const std::string&           s, 
                                const NamingInterface&       ni,
                                bool                         force)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{  

}





CORBA::Object_ptr
NamingInterface::resolve(const std::string& s)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    if (s == "the_Admin" ) 
	{ 
        return the_AdminInterfaceObject;
    }
    
    return CORBA::Object::_nil();
}




void
NamingInterface::unbind(const std::string& s)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{

}





NamingInterface
NamingInterface::new_context()
    throw(CORBA::SystemException)
{
    CosNaming::NamingContext_var nc 
        = m_context->new_context();
    return NamingInterface(nc.in());
}

NamingInterface
NamingInterface::bind_new_context(const std::string& s, bool force)
    throw(CosNaming::NamingContext::NotFound,
          CosNaming::NamingContext::AlreadyBound,
          CosNaming::NamingContext::CannotProceed,
          CosNaming::NamingContext::InvalidName,
          CORBA::SystemException)
{
    return NamingInterface();
}




void
NamingInterface::destroy()
    throw(CosNaming::NamingContext::NotEmpty,
          CORBA::SystemException)
{
    m_context->destroy();
}

bool
NamingInterface::destroy_all()
    throw(CORBA::SystemException)
{
    bool done = false;
	return done;
}





void
NamingInterface::list(CORBA::ULong  how_many, 
                      CosNaming::BindingList_out  bl,
                      CosNaming::BindingIterator_out bi)
    throw(CORBA::SystemException)
{
    m_context->list(how_many,bl,bi);
}




//
// Additional local operations from CosNaming::NamingContextExt
// interface.
//
std::string NamingInterface::to_string(const CosNaming::Name& name)
    throw(InvalidNameException)
{
    return "";
}




CosNaming::Name* NamingInterface::to_name(const std::string& s)
        throw(InvalidNameException)
{
    return NULL;
}

}; // End namespace Naming
}; // End namespace CommonSvcs
}; // End namespace Cdmw


