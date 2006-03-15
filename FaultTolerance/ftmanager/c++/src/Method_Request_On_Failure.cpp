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

#include "ftmanager/Method_Request_On_Failure.hpp"
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>
#include <iostream>

namespace Cdmw {
namespace FT {
namespace Scheduler {

    Method_Request_On_Failure::Method_Request_On_Failure (Cdmw::FT::FTManager_impl* ftmanager,
                                                          std::string the_location)
        : m_ftmanager(ftmanager),
          m_the_location(the_location)
    {
    }
    

    Method_Request_On_Failure::~Method_Request_On_Failure (void)
        throw()
    {
    }
    

    void Method_Request_On_Failure::run (void)
        throw()
    {
        // TODO******
        // Do nothing if location is not in FTMAnager DS
        try
        {
            
            FTManagerStorageHome* ftManagerStorageHome = new FTManagerStorageHome(FT_MANAGER_DATA_STORE);
            ftManagerStorageHome->find_by_oid(m_the_location);
        
            try
            {
                DataStoreBase::On_Failure(m_the_location);
            }
            catch(const NotFoundException&)
            {
                //do nothing
            }       
            catch(const CORBA::Exception& ex)
            {
                std::cerr << "[FTManager/Method_Request_On_Failure]=> On_failure()"
                          << " raised an exception : " << ex << std::endl;
            }

 
            try
            {
                m_ftmanager->remove_location(m_the_location.c_str());
            }
            catch (const ::FT::MemberNotFound&)
            {
                //do nothing
            }
            catch(const CORBA::Exception& ex)
            {
                std::cerr << "[FTManager/Method_Request_On_Failure]=> remove_location()"
                          << " raised an exception : " << ex << std::endl;
            }
        }
        catch( const NotFoundException& )
        {
            // do nothing, the location is already removed
        }
        
    }
    

}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw

