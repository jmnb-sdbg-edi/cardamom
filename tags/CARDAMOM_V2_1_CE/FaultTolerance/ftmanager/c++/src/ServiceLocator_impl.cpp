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


#include "ftmanager/ServiceLocator_impl.hpp"
#include "idllib/FT.stub.hpp"
#include "ftstatemanager/DataStore.hpp"

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

namespace Cdmw {
namespace FT {

    ServiceLocator_impl::ServiceLocator_impl(CORBA::ORB_ptr orb, int dsid)
        : m_orb(CORBA::ORB::_duplicate(orb))
        
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_iogrInfoStorageHome = new IOGRInfoStorageHome(dsid);
    }
    
    
    ServiceLocator_impl::~ServiceLocator_impl()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }
    

    //
    // location service 
    //
    void ServiceLocator_impl::locate_service (const char * name,
                                              const char * sender)
        throw(CORBA::SystemException)
        
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"name="<<name<<",sender="<<sender);

        CORBA::Object_var service_registration_group = m_orb->string_to_object(sender);

       ::CdmwServiceLocator::ServiceRegistration_var sender_ = 
            CdmwServiceLocator::ServiceRegistration::_unchecked_narrow (service_registration_group.in());

        CDMW_DEBUG(FTLogger::GetLogger(),"locate_service "<<name);
        
        if (!strcmp(name, "FaultNotifier"))
        {
            try
            {
                       
                // get a storage object for the fault notifier
                IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
                // get the data
                ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
                
                ::FT::FaultNotifier_var fn = ::FT::FaultNotifier::_narrow(iogrInfo->fault_notifier.in());
                
                // return the reference of the object group
                sender_->register_service(name, fn._retn());
            }
            catch(const Cdmw::FT::NotFoundException & ex)
            {
                CDMW_DEBUG(FTLogger::GetLogger(),"Received exception '" << ex << "'. Ignored.");
                // do nothing
            }
        }
        
        if (!strcmp(name, "FaultDetector"))
        {
            try
            {
                // get a storage object for the fault notifier
                IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
                // get the data
                ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
                CdmwFT::FaultManagement::FaultDetector_var fd = CdmwFT::FaultManagement::FaultDetector::_narrow(iogrInfo->fault_detector.in());
                // return the reference of the object group
                sender_->register_service(name, fd._retn());
            }
            catch(const Cdmw::FT::NotFoundException & ex)
            {
                CDMW_DEBUG(FTLogger::GetLogger(),"Received exception '" << ex << "'. Ignored.");
                // do nothing
            }
        }
        
        if (!strcmp(name, "ReplicationManager"))
        {
            try
            {
                // get a storage object for the fault notifier
                IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
                // get the data
                ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
                CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(iogrInfo->replication_manager.in());
                // return the reference of the object group
                sender_->register_service(name, rm._retn());
            }
            catch(const Cdmw::FT::NotFoundException & ex)
            {   
                CDMW_DEBUG(FTLogger::GetLogger(),"Received exception '" << ex << "'. Ignored.");
                // do nothing
            }
        }
    }


    
}
}
