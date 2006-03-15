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

#include "ftmanager/Method_Request_On_Start.hpp"

#include <iostream>

namespace Cdmw {
namespace FT {
namespace Scheduler {

    Method_Request_On_Start::Method_Request_On_Start (Cdmw::FT::FTManager_impl* ftmanager,
                                                      std::string the_location)
        : m_ftmanager(ftmanager),
          m_the_location(the_location)
    {
    }
    

    Method_Request_On_Start::~Method_Request_On_Start (void)
        throw()
    {
    }
    

    void Method_Request_On_Start::run (void)
        throw()
    {
        try {
            m_ftmanager->add_location(m_the_location.c_str());
        } catch (const CORBA::Exception & ex) {
            std::cerr << "[FTManager/Method_Request_On_Start]=> add_location()"
                      << " raised an exception : " << ex << std::endl;
        }
    }
    

}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw

