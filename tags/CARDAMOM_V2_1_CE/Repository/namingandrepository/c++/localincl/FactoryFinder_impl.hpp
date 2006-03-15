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


#ifndef INCL_NAMINGANDREPOSITORY_FACTORYFINDER_IMPL_HPP 
#define INCL_NAMINGANDREPOSITORY_FACTORYFINDER_IMPL_HPP 


#include "Foundation/orbsupport/CORBA.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
#include "Foundation/orbsupport/CosNaming.skel.hpp"
#include "Foundation/common/Exception.hpp"

#include "namingandrepository/SystemFactoryFinder.hpp"

#include <string>


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*Implementation of the CdmwLifeCycle::FactoryFinder IDL interface.
*
*Features:
*<p>
*Thread safe.
*
*/
class FactoryFinder_impl : virtual public POA_CdmwLifeCycle::FactoryFinder,
        virtual public PortableServer::RefCountServantBase
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
    FactoryFinder_impl(CosNaming::NamingContextExt_ptr defaultRootContext);

    /**
    * Purpose:
    * The destructor
    */ 
    virtual 
    ~FactoryFinder_impl();


protected:

    /**
    * The default root context
    */
    CosNaming::NamingContextExt_var m_defaultRootContext;



private:

    /**
    * Purpose:
    * <p>
    * Analyse the key and returns the criteria that is embedded within the key.
    */
    Criteria analyze_key(const CosLifeCycle::Key& factory_key,
        CORBA::ULong& relevantDepth)
        throw(BadParameterException, OutOfResourcesException);


}; // End class FactoryFinder_impl 

} // End namespace NamingAndRepository
} // End namespace Cdmw
#endif // INCL_NAMINGANDREPOSITORY_FACTORYFINDER_IMPL_HPP

