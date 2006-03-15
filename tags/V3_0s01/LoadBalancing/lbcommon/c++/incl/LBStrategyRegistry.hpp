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

#ifndef INCL_CDMW_LB_LBSTRATEGY_REGISTRY_HPP
#define INCL_CDMW_LB_LBSTRATEGY_REGISTRY_HPP


/**
 * @file
 * @brief The Registry containing all Load Balancing Strategy Factories
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it> 
 */


// Cdmw Files
#include <Foundation/osthreads/ReaderWriterLock.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <LoadBalancing/idllib/CdmwLBService.stub.hpp>

#include <vector>

namespace Cdmw
{

namespace LB
{

    class LBStrategyRegistry
    {
    public:
        
        //constructor
        LBStrategyRegistry();
        
        //destructor
        ~LBStrategyRegistry(); 
        
        /**
         * Initialize the registry, adding RoundRobin and Random strategy factories
         *
         * @return void
         */
        void init();
        
        /**
         * Insert a new strategy factory into registry
         *
         * @param StrategyInfo The strategy info, containing the name and the factory, to add
         *
         * @return void
         */
        void set_lb_policy_factory(CdmwLB::StrategyInfo*);

        /**
         * Get the load balancing strategy factory specified
         *
         * @param string The strategy name
         *
         * @exception CdmwLB::StrategyNotFound if the specified strategy is not contained in the registry
         * @return void
         */
        
        CdmwLB::StrategyFactory_ptr get_lb_policy_factory(const std::string) throw (CdmwLB::StrategyNotFound);
        
    private:
        
        //The Reader Writer Lock managing the access to the registry
        Cdmw::OsSupport::ReaderWriterLock m_lock;
        
        //The vector containing the strategy info
        typedef std::vector<CdmwLB::StrategyInfo*> StrategyVector;
        StrategyVector m_strategy_vector;
        StrategyVector::iterator pos;
    };
    
}; // close namespace LB
    
}; // close namespace Cdmw


#endif // INCL_CDMW_LB_LBSTRATEGY_REGISTRY_HPP

