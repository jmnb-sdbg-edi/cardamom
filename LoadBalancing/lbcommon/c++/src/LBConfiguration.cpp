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
 * @file Activate the Load Balancing Service
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */

#include "lbcommon/LBConfiguration.hpp"

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>

#if CDMW_ORB_VDR == tao
#include "LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp"

#else
#error "Unsupported ORB"
#endif


namespace Cdmw
{
namespace LB
{

    Cdmw::LB::LBStrategyRegistry* Cdmw::LB::LBConfiguration::M_lb_strategy_registry = new Cdmw::LB::LBStrategyRegistry();
    std::string  LBConfiguration::M_override_lb_policy;
    CORBA::Boolean LBConfiguration::M_override_lb_policy_default;
    CORBA::Boolean LBConfiguration::M_override_lb_policy_force;
    
    /**
     * Get the Strategy Registry containing the Load Balancing strategies
     *
     * @return The Strategy Registry
     */

    Cdmw::LB::LBStrategyRegistry* LBConfiguration::GetLbStrategyRegistry()
    {
        return M_lb_strategy_registry; 
    }

    /**
     * Activate the LB service
     *
     *@param argc The number of parameter received of the command line
     *@param argv The list of parameter received of the command line
     *
     * @return void
     */

    void LBConfiguration::Activate_LB_service(int argc,char* argv[])    
        throw(CORBA::SystemException)
    {
        M_override_lb_policy_default=false;
        M_override_lb_policy_force=false;
        
        std::string str=Cdmw::OsSupport::OS::get_option_value(argc,argv,"--CdmwLBPolicyDefault");
        
        if (str.compare("yes")&&str.compare("no"))
            {
                M_override_lb_policy=(CORBA::string_dup((char *)
                                                      (Cdmw::OsSupport::OS::get_option_value(argc,argv,"--CdmwLBPolicyDefault")).c_str()));
                M_override_lb_policy_default=true;
            }
        else
            {
                str=Cdmw::OsSupport::OS::get_option_value(argc,argv,"--CdmwLBPolicyForce");
                if (str.compare("yes")&&str.compare("no"))
                    {
                        M_override_lb_policy=(CORBA::string_dup((char *)
                                                              (Cdmw::OsSupport::OS::get_option_value(argc,argv,"--CdmwLBPolicyForce")).c_str()));
                        M_override_lb_policy_force=true;
                    };
            };
    }

    /**
     * Register an User Defined Load Balancing Strategy
     *
     * @param StrategyInfo The StrategyInfo, containing the name and the StrategyFactory, to add
     * in the Strategy Registry
     *
     * @return void
     */
    
    void LBConfiguration::Register_lb_policy(CdmwLB::StrategyInfo* userdef_strategy_info)
        throw(CORBA::SystemException)
    {
        M_lb_strategy_registry->set_lb_policy_factory(userdef_strategy_info);
    }
    
}; // namespace LB

}; // namespace Cdmw

