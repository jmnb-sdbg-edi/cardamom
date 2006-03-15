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
 * @file
 * @brief The Registry containing all Load Balancing Strategy Factories
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include "lbcommon/LBStrategyRegistry.hpp"

namespace Cdmw
{

namespace LB
{
    //constructor
    LBStrategyRegistry::LBStrategyRegistry()
    {}

    //destructor
    LBStrategyRegistry::~LBStrategyRegistry()
    {
        int count=0;  
        for (pos=m_strategy_vector.begin(); pos!=m_strategy_vector.end();++pos)
        {
            count=count++;
            delete (*pos);
        }
    }

    /**
     * Insert a new strategy factory into registry
     *
     * @param StrategyInfo The strategy info, containing the name and the factory, to add
     *
     * @return void
     */
    
    void LBStrategyRegistry::set_lb_policy_factory(CdmwLB::StrategyInfo *strategy_info)
    {
        m_lock.writeLock();
        m_strategy_vector.push_back(strategy_info);
        m_lock.writeUnlock();
    }
    
    /**
     * Get the load balancing strategy factory specified
     *
     * @param string The strategy name
     *
     * @exception CdmwLB::StrategyNotFound if the specified strategy is not contained in the registry
     * @return void
     */

    CdmwLB::StrategyFactory_ptr  LBStrategyRegistry::get_lb_policy_factory(const std::string policy_name)
        throw (CdmwLB::StrategyNotFound)
    {
        
        m_lock.readLock();
        for (pos=m_strategy_vector.begin(); pos!=m_strategy_vector.end(); ++pos)
        {
            if (!(policy_name.compare(((*(*pos)).strategy_name))))
            {
                return CdmwLB::StrategyFactory::_duplicate((*(*pos)).strategy_factory.in());
            }       
        }
        m_lock.readUnlock();
        
        throw CdmwLB::StrategyNotFound();    
    }
    
 } //end of namespace LB
    
} //end of namespace Cdmw



