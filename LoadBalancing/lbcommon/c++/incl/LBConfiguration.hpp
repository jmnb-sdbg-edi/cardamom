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

#ifndef INCL_CDMW_LB_LBCONFIGURATION_HPP_
#define INCL_CDMW_LB_LBCONFIGURATION_HPP_

/**
 * @file
 * @brief Activate the Load Balancing Service
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */


#include <Foundation/orbsupport/CORBA.hpp>
#include "LoadBalancing/lbcommon/LBStrategyRegistry.hpp"

#include "Foundation/osthreads/ReaderWriterLock.hpp"


namespace Cdmw
{
    
namespace LB
{


    class LBConfiguration 
    {
    public:
        
        /**
         * Activate the LB service
         *
         *@param argc The number of parameter received of the command line
         *@param argv The list of parameter received of the command line
         *
         * @return void 
         */

        static
        void
        Activate_LB_service(int argc,char* argv[])
            throw(CORBA::SystemException);
        
        
        /**
         * Register an User Defined Load Balancing Strategy
         *
         * @param StrategyInfo The StrategyInfo, containing the name and the StrategyFactory, to add
         * in the Strategy Registry
         *
         * @return void
         */
        
        static 
        void 
        Register_lb_policy(CdmwLB::StrategyInfo*)
            throw(CORBA::SystemException);
        
        
        /**
         * Get the Strategy Registry containing the Load Balancing strategies
         *
         * @return The Strategy Registry
         */
        static Cdmw::LB::LBStrategyRegistry* GetLbStrategyRegistry();
        
        
        // The Policy overriding default Load Balancing Policy defined in the IOGR
        static std::string M_override_lb_policy;
        
        // True if the client overrides "default" policy defined in the IOGR
        static CORBA::Boolean M_override_lb_policy_default;
        
        // True if the client overrides every policy defined in the IOGR
        static CORBA::Boolean M_override_lb_policy_force;

        // The Strategy Registry containing all Load Balancing strategies
        static Cdmw::LB::LBStrategyRegistry* M_lb_strategy_registry;
        
    };    
    
}; // namespace LB
    
}; // namespace Cdmw

#endif // INCL_CDMW_LB_LBCONFIGURATION_HPP_
