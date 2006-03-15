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

#include <exception>
#include <iostream>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include <FaultTolerance/ftreplicationmanager/InitUtils.hpp>
#include <FaultTolerance/ftcommon/FactoryInfos.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>

#include "testFTSupervision/SIM_FaultManager.hpp"
#include "testFTSupervision/SIM_FaultNotifier.hpp"
#include "testFTSupervision/SIM_FaultDetector.hpp"
#include "testFTSupervision/TypeRepositoryStub.hpp"

namespace Cdmw {
namespace FT {
    
    // constructor
    SIM_FaultManager::SIM_FaultManager(CORBA::ORB_ptr orb,
                                       PortableServer::POA_ptr rootPOA,
                                       CdmwFT::FaultManagement::FaultDetector_ptr fault_detector,
                                       CdmwFT::ReplicationManager_ptr replication_manager)
        : m_fault_detector(CdmwFT::FaultManagement::FaultDetector::_duplicate(fault_detector)),
          m_replication_manager(CdmwFT::ReplicationManager::_duplicate(replication_manager))
    {
        // Create the TypeRepositoryStub object and register it
        CdmwFT::TypeRepository_var typeRepository =
            new Cdmw::FT::TypeRepositoryStub();
        orb->register_initial_reference("FTTypeRepository", typeRepository.in());
        
        m_fault_notifier = m_replication_manager->get_fault_notifier();
        m_replication_manager->register_fault_detector(m_fault_detector.in());
    }
    
    
    // destructor
    SIM_FaultManager::~SIM_FaultManager()
    {
       std::cerr << "SIM_FaultManager destructor" << std::endl;
    }
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_fault_notifier:1.0
     * operation
     */
    ::FT::FaultNotifier_ptr SIM_FaultManager::get_fault_notifier()
        throw(::FT::ObjectGroupNotFound,
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
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException )
    {
        return CdmwFT::FaultManagement::FaultDetector::_duplicate(m_fault_detector.in());
    }
    
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_replication_manager:1.0
     * operation
     */    
    CdmwFT::ReplicationManager_ptr SIM_FaultManager::get_replication_manager()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException )
    {
        return CdmwFT::ReplicationManager::_duplicate(m_replication_manager.in());
    }


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_FTManager_objects:1.0
     * operation
     */
    CdmwFT::FTManagerObjects*  SIM_FaultManager::get_FTManager_objects(
        const char *,
        CdmwFT::StateTransfer::DataStoreGroup_ptr,
        const CdmwFT::LocationList &)
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

