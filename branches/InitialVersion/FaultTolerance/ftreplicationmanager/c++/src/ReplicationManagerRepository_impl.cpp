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

// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>

// Local Files
#include <ftreplicationmanager/ReplicationManagerRepository_impl.hpp>

#include <idllib/FT.stub.hpp>
#include <idllib/CdmwFTReplicationManager.stub.hpp>

#include <Foundation/logging/FunctionLogger.hpp>
#include <FaultTolerance/ftcommon/FTLogger.hpp>


namespace 
{
    const PortableInterceptor::ORBInitInfo::ObjectId FT_SERVER_REQUEST_INTERCEPTOR_NAME 
        = "FTServerRequestInterceptor";
    const PortableInterceptor::ORBInitInfo::ObjectId FT_REPLICATION_MANAGER_REPOSITORY 
        = "FTReplicationManagerRepository";

} // end of anonymous namespace


namespace Cdmw
{
    
namespace FT
{
ReplicationManagerRepository_impl* ReplicationManagerRepository_impl::mp_instance = 0;

/** Purpose:
 * <p>  
 *  Return the single instance of the GroupRepository_impl 
 *
 *@param  none
 *@exception none
 */ 
ReplicationManagerRepository_impl* 
ReplicationManagerRepository_impl:: Get_instance()
        throw()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (!mp_instance)
    {
        // create instance
        mp_instance = new ReplicationManagerRepository_impl();
    }
    
    return mp_instance;
}

ReplicationManagerRepository_impl::ReplicationManagerRepository_impl()
  : m_is_initialised(false)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

ReplicationManagerRepository_impl::~ReplicationManagerRepository_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}


void 
ReplicationManagerRepository_impl::init(CdmwFT::ReplicationManager_ptr the_replication_manager)
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");

    if (m_is_initialised == true)
    {
        CORBA::BAD_INV_ORDER ex (Cdmw::OrbSupport::BAD_INV_ORDERInitAlreadyDone,
                                 CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    if (CORBA::is_nil(the_replication_manager))
    {
        CORBA::BAD_PARAM ex(Cdmw::OrbSupport::BAD_PARAMNilObjectReference,
                            CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    m_replication_manager = CdmwFT::ReplicationManager::_duplicate(the_replication_manager);

    // no error during initialisation
    m_is_initialised = true;
}

//
// IDL to C++ Mapping
//

CdmwFT::ReplicationManager_ptr
ReplicationManagerRepository_impl::get_replication_manager()
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CdmwFT::ReplicationManager_var the_replication_manager 
        = CdmwFT::ReplicationManager::_duplicate(m_replication_manager.in());

   return the_replication_manager._retn();
}

} // End of namespace FT

} // End of namespace Cdmw

