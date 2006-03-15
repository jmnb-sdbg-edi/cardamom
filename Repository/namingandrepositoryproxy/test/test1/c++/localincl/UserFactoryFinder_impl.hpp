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


#ifndef INCL_NAMINGANDREPOSITORY_USERFACTORYFINDER_IMPL_HPP 
#define INCL_NAMINGANDREPOSITORY_USERFACTORYFINDER_IMPL_HPP 


#include "Foundation/orbsupport/CORBA.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.skel.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"
#include "Foundation/common/Exception.hpp"

#include <string>


/**
*Purpose:
*<p>
*User-supplied implementation of the CdmwLifeCycle::FactoryFinder IDL interface
*for unit tests.
*/
class UserFactoryFinder_impl : virtual public POA_CdmwLifeCycle::FactoryFinder
{

public:
    
    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:omg.org/CosLifeCycle/FactoryFinder/find_factories:1.0
    * operation
    */
    virtual CosLifeCycle::Factories* find_factories(const CosLifeCycle::Key& factory_key)
        throw(CosLifeCycle::NoFactory,
              CORBA::SystemException);
      
    /**
    * Purpose:
    * <p>
    * Implements the
    * IDL:thalesgroup.com/CdmwLifeCycle/FactoryFinder/find_factory:1.0
    * operation
    */
    virtual CdmwLifeCycle::FactoryBase_ptr find_factory(const char* factory_key)
        throw(CosLifeCycle::NoFactory,
              CORBA::SystemException);

    /**
    * Purpose:
    * The constructor
    */ 
    UserFactoryFinder_impl(CosNaming::NamingContextExt_ptr context);

    /**
    * Purpose:
    * The destructor
    */ 
    virtual 
    ~UserFactoryFinder_impl();


protected:

    /**
    * The context
    */
    CosNaming::NamingContextExt_var m_context;

}; // End class UserFactoryFinder_impl 

#endif // INCL_NAMINGANDREPOSITORY_USERFACTORYFINDER_IMPL_HPP

