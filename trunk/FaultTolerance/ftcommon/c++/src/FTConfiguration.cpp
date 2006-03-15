/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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

#include <FaultTolerance/ftcommon/FTConfiguration.hpp>


#if CDMW_ORB_VDR == tao
#include "ftcommon/TaoFTConfiguration.hpp"
#include <string>
#else
#error "Cdmw: ORB not supported"
#endif


namespace Cdmw {
namespace FT {

#if CDMW_ORB_VDR == tao
typedef TaoFTConfiguration UnderlyingORB;
#else
#error "Cdmw: ORB not supported"
#endif
    /**
     * Set the client_id
     *
     * @param orb the orb.
     * @param client_id the client_id for the orb.
     *
     * @return void 
     */
    void
    FTConfiguration::Set_client_id(CORBA::ORB_ptr orb,
                                   const char* client_id)
        throw(CORBA::SystemException)
    {
        UnderlyingORB::Set_client_id(orb, client_id);
    }
    
    /**
     * Get the client_id
     *
     * @param orb the orb.
     *
     * @return client_id the client_id for the orb..
     */
    const char*
    FTConfiguration::Get_client_id(CORBA::ORB_ptr orb)
        throw(CORBA::SystemException)
    {
        return UnderlyingORB::Get_client_id(orb);
    }
    
    
    /**
     * Get the FT endpoint selector factory
     *
     * @return std::string the name of the FT_endpoint_selector_factory for the ORB
     */
    std::string
    FTConfiguration::Get_FT_endpoint_selector_factory()
    {
        return UnderlyingORB::Get_FT_endpoint_selector_factory();
    }

    
    /**
     * Activate the FT service
     *
     *@param argc The number of parameter received of the command line
     *@param argv The list of parameter received of the command line
     *
     * @return void 
     */
    void
    FTConfiguration::Activate_FT_service(int& argc, char** argv)
        throw(CORBA::SystemException)
    {
        UnderlyingORB::Activate_FT_service(argc, argv);
    }
    

 
}; // namespace FT
}; // namespace Cdmw


