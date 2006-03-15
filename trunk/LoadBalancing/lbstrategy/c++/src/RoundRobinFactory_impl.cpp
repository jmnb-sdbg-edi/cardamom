/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
 * @brief Round Robin strategy Factory Implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "lbstrategy/RoundRobinFactory_impl.hpp"
#include "lbstrategy/RoundRobin_impl.hpp"

#include <stdlib.h>
#include <iostream>
using namespace std;

namespace Cdmw
{

namespace LB
{
    // Constructor.
    RoundRobinFactory_impl::RoundRobinFactory_impl(Cdmw::LB::IOGRFactory* iogr_factory)
        :m_iogr_factory(iogr_factory)
    {
    }

    // Destructor
    RoundRobinFactory_impl::~RoundRobinFactory_impl()
    {
	delete m_iogr_factory;
    }

    /**
     * Create a new Round Robin Strategy for the specified object group
     *
     * @param og The object group
     *
     * @exception CdmwLB::StrategyNotCreated if the strategy is not created
     */
    
    CdmwLB::Strategy_ptr RoundRobinFactory_impl::create_strategy(PortableGroup::ObjectGroup_ptr og)
        throw (CORBA::SystemException,
               CdmwLB::StrategyNotCreated)
    {
        CdmwLB::Strategy_ptr lb_strategy;
        try
        {
            lb_strategy = new RoundRobin_impl(CORBA::Object::_duplicate(og),
                                              m_iogr_factory,
                                              "Round Robin Strategy");
        }catch(const CORBA::INV_OBJREF&)
        {
            std::cout << "CORBA Exception: Invalid Object Reference" << std::endl;
            throw CdmwLB::StrategyNotCreated();
        }
	return CdmwLB::Strategy::_duplicate(lb_strategy);
    }
}; //close namespace LB
    
}; // close namespace Cdmw
