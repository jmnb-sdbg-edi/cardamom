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
#include "testFTSupervision/SIM_FaultManager.hpp"
#include "testFTSupervision/SIM_FaultNotifier.hpp"
#include "testFTSupervision/SIM_FaultDetector.hpp"
#include "testFTSupervision/TypeRepositoryStub.hpp"
#include "FaultTolerance/ftreplicationmanager/InitUtils.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"

namespace Cdmw {
namespace FT {
    
    // constructor
    SIM_FaultManager::SIM_FaultManager(CORBA::ORB_var m_orb,
                                       PortableServer::POA_var m_rootPOA,
                                       CdmwFT::FaultManagement::FaultDetector_ptr fault_detector)
    {

        // Create the TypeRepositoryStub object and register it
        CdmwFT::TypeRepository_var typeRepository =
            new Cdmw::FT::TypeRepositoryStub();
        m_orb->register_initial_reference("FTTypeRepository", typeRepository.in());
        
        // Create a replication manager, bind to corbaloc
        // then register its initial referance
        std::string ft_domain("ft_domain_test");
        char** argv_tmp = NULL;
        int argc_tmp = 0;
        Cdmw::OsSupport::OS::Timeval cohort_timeout;
        cohort_timeout.seconds=2;
        cohort_timeout.microseconds=0;
        Cdmw::OsSupport::OS::Timeval coordinator_timeout;
        coordinator_timeout.seconds=2;
        coordinator_timeout.microseconds=0;

        m_replication_manager
            = Cdmw::FT::ReplicationManager::InitUtils::create_replication_manager (argc_tmp,
                                                                          argv_tmp,
                                                                          m_orb.in(),
                                                                          m_rootPOA.in(),
                                                                          ft_domain.c_str(),
                                                                          "5555",
                                                                          "ftdomain",
                                                                          "225.1.1.8",
                                                                          100,100,
                                                                          cohort_timeout,
                                                                          coordinator_timeout,
                                                                          "SAFE_SECURITY_LEVEL",
                                                                          "STRICT",
                                                                          std::cout);
    
        // publishes the replication manager corbaloc
        OrbSupport::OrbSupport::bind_object_to_corbaloc (m_orb.in(), 
                                                         "replication_mgr", m_replication_manager.in());

        PortableServer::ServantBase_var fn_servantBase = 
                           new Cdmw::FT::SIM_FaultNotifier();
        Cdmw::FT::SIM_FaultNotifier *fn_servant = 
                           dynamic_cast<Cdmw::FT::SIM_FaultNotifier *> (fn_servantBase.in());
        m_fault_notifier = fn_servant->_this();
        
        m_replication_manager->register_fault_notifier(m_fault_notifier.in());
        
        
    
        PortableServer::ServantBase_var fd_servantBase = 
                           new Cdmw::FT::SIM_FaultDetector();
        Cdmw::FT::SIM_FaultDetector *fd_servant = 
                           dynamic_cast<Cdmw::FT::SIM_FaultDetector *> (fd_servantBase.in());
        m_fault_detector = fd_servant->_this();
    
        m_replication_manager->register_fault_detector(m_fault_detector.in());
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
        throw(CdmwFT::FTManager::NoAlreadyActivated, 
              ::FT::ObjectGroupNotFound,
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
    CdmwFT::FTManagerObjects*  SIM_FaultManager::get_FTManager_objects(const char*, const CdmwFT::StateTransfer::DataStores&, const CdmwFT::LocationList&)
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

