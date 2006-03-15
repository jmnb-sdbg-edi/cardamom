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

#ifndef INCL_CDMW_LB_ROUND_ROBIN_FACTORY_IMPL_H
#define INCL_CDMW_LB_ROUND_ROBIN_FACTORY_IMPL_H

/**
 * @brief Round Robin strategy Factory Implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */


#include <LoadBalancing/idllib/CdmwLBService.skel.hpp>
#include <LoadBalancing/lbcommon/LB_IOGRFactory.hpp>

namespace Cdmw
{

namespace LB
{

    class RoundRobinFactory_impl : public CdmwLB::StrategyFactory
    {
    public:
        
        // Constructor.
        RoundRobinFactory_impl(Cdmw::LB::IOGRFactory* iogr_factory);

        // Destructor
        ~RoundRobinFactory_impl();

        /**
         * Methods required by the CdmwLB::Strategy interface.
         */

        /**
         * Create a new Round Robin Strategy for the specified object group
         *
         * @param og The object group
         *
         * @exception CdmwLB::StrategyNotCreated if the strategy is not created
         */
        
        virtual CdmwLB::Strategy_ptr create_strategy (PortableGroup::ObjectGroup_ptr og)
            throw (CORBA::SystemException,
                   CdmwLB::StrategyNotCreated);
        
    private:
        // The IOGR Factory reference
        Cdmw::LB::IOGRFactory* m_iogr_factory;
    };
    
};//close namespace LB
    
};//close namespace Cdmw

#endif //INCL_CDMW_LB_ROUND_ROBIN_FACTORY_IMPL_H
