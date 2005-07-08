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
 * @brief This class provides an init of the Load Balancing Framework
 * capability.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <lbinit/LBServiceInit.hpp>
#include <LoadBalancing/lbcommon/LBConfiguration.hpp>
#include <lbinit/LBORBInitializer.hpp>
#include <tao/ORBInitializer_Registry.h>


namespace Cdmw
{

namespace LB
{

    // Load Balancing Initialization
    void LBServiceInit::Init(int argc,char* argv[])throw(CORBA::SystemException)
    {
        Cdmw::LB::LBConfiguration::Activate_LB_service(argc,argv);
        std::string str = Cdmw::OsSupport::OS::get_option_value(argc,argv,"--LBInterceptorSide");
        PortableInterceptor::ORBInitializer_ptr initializer = 0;
        if (str.compare("yes")&&str.compare("no"))
        {
            initializer = new Cdmw::LB::LBInit::ORBInitializer(str);
        }
        else
        {
            initializer = new Cdmw::LB::LBInit::ORBInitializer();
        }
            PortableInterceptor::register_orb_initializer(initializer);
    } 

    // Register an user defined policy as Load Balancing Strategy
    void
    LBServiceInit::Register_lb_policy(CdmwLB::StrategyInfo* userdef_strategy_info)
        throw(CORBA::SystemException)
    {
        Cdmw::LB::LBConfiguration::Register_lb_policy(userdef_strategy_info);
    }
}// close namespace LB
    
}// close namespace Cdmw 

