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
 * @brief Implementation for ORB Initializer class.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */

#include "lbinit/LBORBInitializer.hpp"
#include "lbinit/LBClientRequestInterceptor.hpp"
#include "lbinit/LBServerRequestInterceptor.hpp"
#include <LoadBalancing/lbcommon/LBConfiguration.hpp>
#include <LoadBalancing/lbstrategy/RoundRobinFactory_impl.hpp>
#include <LoadBalancing/lbstrategy/RandomFactory_impl.hpp>

#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>
#include "tao/ORBInitInfo.h"

#else
#error "Unsupported ORB"
#endif

#include <iostream>
using namespace std;


namespace Cdmw
{

namespace LB
{

namespace LBInit
{

ORBInitializer::ORBInitializer(void)
{
}

ORBInitializer::~ORBInitializer(void)
{
}


ORBInitializer::ORBInitializer(std::string interceptor_side)
{
    m_interceptor_side = interceptor_side;
}

void
ORBInitializer::pre_init (PortableInterceptor::ORBInitInfo_ptr info)
    throw (CORBA::SystemException)
{
}

void
ORBInitializer::post_init (PortableInterceptor::ORBInitInfo_ptr info)
    throw (CORBA::SystemException)
{
    TAO_ORBInitInfo_var tao_info =
        TAO_ORBInitInfo::_narrow (info);
    TAO_ORB_Core * orb_core = tao_info->orb_core();
    CORBA::ORB_var orb = orb_core->orb();
    Cdmw::LB::IOGRFactory* iogr_factory = new Cdmw::LB::TAO_LB_IOGRFactory(std::cout, orb.in());
    if(m_interceptor_side.compare("Client") == 0)
    {
        // Initialize LB StrategyRegistry
        CdmwLB::StrategyInfo* info1,*info2;
        info1=new CdmwLB::StrategyInfo();
        info2=new CdmwLB::StrategyInfo();
        info1->strategy_name="RoundRobin";
        info1->strategy_factory=new Cdmw::LB::RoundRobinFactory_impl(iogr_factory);
        Cdmw::LB::LBConfiguration::M_lb_strategy_registry->set_lb_policy_factory(info1);
        info2->strategy_name="Random";
        info2->strategy_factory=new Cdmw::LB::RandomFactory_impl(iogr_factory);
        Cdmw::LB::LBConfiguration::M_lb_strategy_registry->set_lb_policy_factory(info2);
        
        PortableInterceptor::SlotId slot_id = info->allocate_slot_id(); 
        
        CORBA::Object_var obj =
            info->resolve_initial_references ("PICurrent");
        PortableInterceptor::Current_var pi_current =
            PortableInterceptor::Current::_narrow (obj.in ());
        
        if (CORBA::is_nil (pi_current.in ()))
            throw new CORBA::INTERNAL();
        
        PortableInterceptor::ClientRequestInterceptor_ptr ci =
            PortableInterceptor::ClientRequestInterceptor::_nil ();
        ci = new ClientRequestInterceptor_impl(orb.in(),
                                           "LBClientInterceptor",
                                               slot_id, 
                                               pi_current.in (), 
                                               iogr_factory);
        
        PortableInterceptor::ClientRequestInterceptor_var ci_interceptor =
            ci;
        
        info->add_client_request_interceptor (ci_interceptor.in ());
    }
    else
    {
        PortableInterceptor::ServerRequestInterceptor_ptr si =
            PortableInterceptor::ServerRequestInterceptor::_nil ();
        si = new ServerRequestInterceptor_impl(orb.in(),
                                               "LBServerInterceptor",
                                               iogr_factory);
        
        PortableInterceptor::ServerRequestInterceptor_var si_interceptor =
            si;
        
        info->add_server_request_interceptor (si_interceptor.in ());
    }    
    }
    
}//End of namespace LBInit

}//End of namespace LB

}//End of namespace Cdmw

