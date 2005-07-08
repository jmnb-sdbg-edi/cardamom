/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
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
*/
/* ===================================================================== */


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <exception>
#include <iostream>
#include "testplatformdaemon/SIM_FaultManager.hpp"

namespace Cdmw {
namespace FT {
    
    // constructor
    SIM_FaultManager::SIM_FaultManager()
    {
        SIM_FaultNotifier* obj = new SIM_FaultNotifier();   
        m_fault_notifier = obj->_this();        
    }
    
    
    // destructor
    SIM_FaultManager::~SIM_FaultManager()
    {
    }
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_fault_notifier:1.0
     * operation
     */
    ::FT::FaultNotifier_ptr SIM_FaultManager::get_fault_notifier()
        throw(CdmwFT::FTManager::NoAlreadyActivated,
              ::FT::ObjectGroupNotFound,
              CORBA::SystemException )
    {
        return ::FT::FaultNotifier::_duplicate(m_fault_notifier.in());
    }
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_fault_detector:1.0
     * operation
     */
    CdmwFT::FaultManagement::FaultDetector_ptr SIM_FaultManager::get_fault_detector()
        throw(CdmwFT::FTManager::NoAlreadyActivated,
              ::FT::ObjectGroupNotFound,
              CORBA::SystemException )
    {
        return CdmwFT::FaultManagement::FaultDetector::_nil();
    }
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_replication_manager:1.0
     * operation
     */    
    CdmwFT::ReplicationManager_ptr SIM_FaultManager::get_replication_manager()
        throw(CdmwFT::FTManager::NoAlreadyActivated,
              ::FT::ObjectGroupNotFound,
              CORBA::SystemException )
    {
        return CdmwFT::ReplicationManager::_nil();        
    }
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_FTManager_objects:1.0
     * operation
     */
    CdmwFT::FTManagerObjects* SIM_FaultManager::get_FTManager_objects(const char*, const 
   CdmwFT::StateTransfer::DataStores&, const CdmwFT::LocationList&)
        throw(::FT::InterfaceNotFound,
              CORBA::SystemException)
    {
        return new CdmwFT::FTManagerObjects();
    }

    
    /**
     * return the reference of the object group for the id
     */
    CORBA::Object_ptr SIM_FaultManager::get_object_group(::FT::ObjectGroupId id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException)
    {
        return CORBA::Object::_nil();
    }
    
    /**
     * return the object group version for objects groups (all the objects have the same version)
     */
    ::FT::ObjectGroupRefVersion SIM_FaultManager::get_object_group_version(::FT::ObjectGroupId id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException)
    {
        return 0;
    }

    

        
    }; // namespace FT
}; // namespace Cdmw

