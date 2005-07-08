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

#ifndef INCL_CDMW_LB_SERVICE_INIT_HPP
#define INCL_CDMW_LB_SERVICE_INIT_HPP

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <LoadBalancing/idllib/CdmwLBService.stub.hpp>

/**
 * Root namespace for CDMW runtime.
 */

namespace Cdmw
{

namespace LB
{

class LBServiceInit
{
public:

    //constructor
    LBServiceInit() {};

    //destructor
    ~LBServiceInit() {}; 

    //Load Balancing Initialization
    static void Init(int argc,char* argv[]) throw (CORBA::SystemException); 

    //Register an user defined policy as Load Balancing Strategy
    static void Register_lb_policy (CdmwLB::StrategyInfo* stgy_info) throw (CORBA::SystemException);
    
}; // End class LB_Service
    
} // End namespace LB

} // End namespace Cdmw


#endif // INCL_CDMW_LB_SERVICE_INIT_HPP

