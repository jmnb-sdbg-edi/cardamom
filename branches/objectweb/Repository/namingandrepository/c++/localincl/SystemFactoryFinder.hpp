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


#ifndef INCL_NAMINGANDREPOSITORY_SYSTEMFACTORYFINDER_HPP 
#define INCL_NAMINGANDREPOSITORY_SYSTEMFACTORYFINDER_HPP 


#include "Foundation/orbsupport/CORBA.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.skel.hpp"
#include "Foundation/orbsupport/CosNaming.skel.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/common/Exception.hpp"

#include <string>


namespace Cdmw
{
namespace NamingAndRepository
{

class PersistentNameDomain;


enum Location { Local_Criterion, Global_Criterion };

enum Selection { FactoryName_Criterion, ObjectInterface_Criterion, All_Criterion };


/**
*Purpose:
*<p>
*The search criteria.
*
*Features:
*<p>
*Thread safe.
*/
class Criteria
{

public:

    /**
    * Indicates whether to search locally or globally
    */
    Location m_location;

    /**
    * Indicates whether a selection should be applied
    */
    Selection m_selection;

    /**
    * Contains the object interface in the ObjectInterface_Criterion case.
    * Contains the factory name in the FactoryName_Criterion case.
    */
    std::string m_selectionFilter;

    /**
    * The default constructor.
    * The default criteria is : Local_Criterion and All_Criterion
    */
    Criteria();

};


/**
*Purpose:
*<p>
*The factory finder in charge of system name domains.
*
*Features:
*<p>
*Thread safe.
*/
class SystemFactoryFinder
{

public:
    
    /**
    * Purpose:
    * <p>
    * Search the factories corresponding to the criteria within
    * the system name domain attached to this factory finder and
    * within each sub name domains.
    *
    *@param factory_key Contains the actual name domain path and the criteria at the end 
    *@param criteria    A simplest form of the criteria that is embedded in factory_key
    *@param relevantDepth The number of name component within factory_key
    * that forms the actual name domain path.
    */
    virtual CosLifeCycle::Factories* find_factories(
        const CosLifeCycle::Key& factory_key, const Criteria& criteria,
        CORBA::ULong relevantDepth)
        throw(OutOfResourcesException, InternalErrorException); 

      
    /**
    * Purpose:
    * The constructor
    */ 
    SystemFactoryFinder(PersistentNameDomain *systemNameDomain);


    /**
    * <p>
    * Returns the factories list corresponding to the concatenation
    * of the two factories lists.
    */
    static CosLifeCycle::Factories* concatenate(const CosLifeCycle::Factories& f1,
        const CosLifeCycle::Factories& f2)
        throw (OutOfResourcesException);


protected:

    /**
    * The system name domain where to find factories
    */
    PersistentNameDomain *m_systemNameDomain;


protected:

    /**
    * Purpose:
    * <p>
    * Finds factories corresponding to the criteria within the system name domain
    * attached to this factory finder.
    */
    CosLifeCycle::Factories* find_factories(const Criteria& criteria)
        throw(OutOfResourcesException, InternalErrorException); 

    /**
    * Purpose:
    * <p>
    * Finds factories corresponding to the criteria within the sub name domains
    * of the system name domain attached to this factory finder.
    */
    CosLifeCycle::Factories* find_factories(
        const CosLifeCycle::Key& factory_key, const Criteria& criteria)
        throw(OutOfResourcesException, InternalErrorException); 


}; // End class FactoryFinder_impl 

} // End namespace NamingAndRepository
} // End namespace Cdmw
#endif // INCL_NAMINGANDREPOSITORY_FACTORYFINDER_IMPL_HPP

