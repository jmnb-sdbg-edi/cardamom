/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

/**
 * @brief Random Strategy Factory Implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "testlbinit/UserDefinedStrategyFactory_impl.hpp"
#include "testlbinit/UserDefinedStrategy_impl.hpp"

#include <stdlib.h>

#include <iostream>
using namespace std;

namespace Cdmw
{
    // Constructor.
    UserDefinedStrategyFactory_impl::UserDefinedStrategyFactory_impl(Cdmw::LB::IOGRFactory* iogr_factory)
        :m_iogr_factory(iogr_factory)
    {
    }

    // Destructor
    UserDefinedStrategyFactory_impl::~UserDefinedStrategyFactory_impl()
    {
    }
    
    /**
     * Create a new UserDefinedStrategy Strategy for the specified object group
     *
     * @param og The object group
     *
     * @exception CdmwLB::StrategyNotCreated if the strategy is not created
     */
    
    CdmwLB::Strategy_ptr UserDefinedStrategyFactory_impl::create_strategy(PortableGroup::ObjectGroup_ptr og)
        throw (CORBA::SystemException,CdmwLB::StrategyNotCreated)
    {
        
        CdmwLB::Strategy_ptr lb_strategy;
        try
        {
            lb_strategy = new UserDefinedStrategy_impl(CORBA::Object::_duplicate(og),
                                                       m_iogr_factory,
                                                       "User Defined Strategy");
        }catch(const CORBA::INV_OBJREF&)
        {
            std::cout << "CORBA Exception: Invalid Object Reference" << std::endl;
            throw CdmwLB::StrategyNotCreated();
        }
        catch(const CORBA::SystemException&)
        {
            std::cout << "CORBA SystemException" << std::endl;
            throw;
        }
        return CdmwLB::Strategy::_duplicate(lb_strategy);
    }
    
}; //close namepsace Cdmw
