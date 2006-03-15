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
 * @brief A generic LoadBalancing Strategy Factory Implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "lbstrategy/StrategyFactory_impl.hpp"
#include <LoadBalancing/lbcommon/LBConfiguration.hpp>
#include <LoadBalancing/lbcommon/MacroDef.hpp>

#define ECHO_HEADER() \
    "[CDMW LBStrategyFactory] "

#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment << std::endl;\
} while(0)

#endif

namespace Cdmw
{

namespace LB
{
    // Constructor.
    StrategyFactory_impl::StrategyFactory_impl(Cdmw::LB::IOGRFactory* iogr_factory)
        :m_iogr_factory(iogr_factory)
    {
    }

    // Destructor
    StrategyFactory_impl::~StrategyFactory_impl()
    {
    }
    
    /**
     * Create a new Strategy for the specified object group
     *
     * @param og The object group
     *
     * @exception CdmwLB::StrategyNotCreated if the strategy is not created
     */
    
    CdmwLB::Strategy_ptr StrategyFactory_impl::create_strategy(PortableGroup::ObjectGroup_ptr og)
        throw (CORBA::SystemException,
               CdmwLB::StrategyNotCreated)
      {
          CdmwLB::Strategy_var lb_strategy;
          CdmwLB::StrategyFactory_var lb_factory;
          CdmwLB::TagLBGroupTaggedComponent* lb_group_tag = 0;
          try
          {
              lb_group_tag = m_iogr_factory->decode_profile_with_group_tag(CORBA::Object::_duplicate(og), 0);
          }catch(const CdmwLB::TagNotFound&)
          {
              DEBUG_DUMP("Error occured when creating the LB Strategy");
              throw CdmwLB::StrategyNotCreated();
          }
          if(((LBConfiguration::M_override_lb_policy_default)
              && (strcmp(lb_group_tag->lb_policy, "default") == 0))
             || (LBConfiguration::M_override_lb_policy_force))
          {
              lb_factory = LBConfiguration::M_lb_strategy_registry->get_lb_policy_factory(
                  LBConfiguration::M_override_lb_policy);
              DEBUG_DUMP( "  Overriding IOGR LB Policy with: " << LBConfiguration::M_override_lb_policy);
          }
          else
          {
              std::string str((const char*)lb_group_tag->lb_policy);
              if (str == "default")
                  str = CORBA::string_dup("RoundRobin");
              lb_factory = LBConfiguration::M_lb_strategy_registry->get_lb_policy_factory(str);
              DEBUG_DUMP( "  Using IOGR LB Policy: " << lb_group_tag->lb_policy);
              
          }
          lb_strategy = lb_factory->create_strategy(CORBA::Object::_duplicate(og));
          return lb_strategy._retn();
      }
    
}; //close namepsace LB
    
}; //close namespace Cdmw
