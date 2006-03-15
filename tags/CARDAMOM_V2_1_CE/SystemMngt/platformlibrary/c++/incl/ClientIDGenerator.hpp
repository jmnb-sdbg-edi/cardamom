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

#include <string>

#ifndef INCL_CLIENT_ID_GENERATOR_HPP 
#define INCL_CLIENT_ID_GENERATOR_HPP

namespace Cdmw
{
namespace PlatformMngt
{

    //
    // generate unique client id for the system 
    //
    class ClientIDGenerator
    {
    private:
        ClientIDGenerator()
            {};
        
        ~ClientIDGenerator()
            {};

    public:
        // generate unique client id for daemon
        static
        std::string
        generate_daemon_client_id(std::string hostname);
        
        // generate unique client id for the agent
        static
        std::string
        generate_agent_client_id(std::string hostname, std::string applicationname);
        
        // generate unique client id for the supervision
        static
        std::string
        generate_supervision_client_id(std::string hostname);
    };
    
    
    

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_CLIENT_ID_GENERATOR_HPP 

