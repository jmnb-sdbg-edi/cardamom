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

#ifndef INCL_SIM_FAULT_MANAGER_HPP
#define INCL_SIM_FAULT_MANAGER_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/testutils/TestManager.hpp>
#include "idllib/FT.skel.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.skel.hpp"
#include "FaultTolerance/idllib/CdmwFTMembersAdmin.stub.hpp"
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include "testFTSupervision/SIM_FaultNotifier.hpp"


namespace Cdmw {
namespace FT {

class SIM_FaultManager : virtual public POA_CdmwFT::FTManager,
                         public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    // constructor
    SIM_FaultManager(CORBA::ORB_ptr orb,
                     PortableServer::POA_ptr rootPOA,
                     CdmwFT::FaultManagement::FaultDetector_ptr fault_detector,
                     CdmwFT::ReplicationManager_ptr replication_manager);
    
    // destructor
    virtual ~SIM_FaultManager();
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_fault_notifier:1.0
     * operation
     */
    ::FT::FaultNotifier_ptr get_fault_notifier()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException );
    

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_fault_detector:1.0
     * operation
     */
    CdmwFT::FaultManagement::FaultDetector_ptr get_fault_detector()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_replication_manager:1.0
     * operation
     */    
    CdmwFT::ReplicationManager_ptr get_replication_manager()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_FTManager_objects:1.0
     * operation
     */
    CdmwFT::FTManagerObjects* get_FTManager_objects(const char *,
     CdmwFT::StateTransfer::DataStoreGroup_ptr,
     const CdmwFT::LocationList &)
        throw(::FT::InterfaceNotFound,
              CORBA::SystemException);


    /**
     * return the reference of the object group for the id
     */
    CORBA::Object_ptr get_object_group(::FT::ObjectGroupId id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);
    /**
     * return the object group version for objects groups (all the objects have the same version)
     */
    ::FT::ObjectGroupRefVersion get_object_group_version(::FT::ObjectGroupId id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);

    
    /** 
     * return true if the manager is primary
     */
    CORBA::Boolean is_activated()
        throw(CORBA::SystemException )
    {
        return true;
    };
    
    void dump_datastore(CdmwFT::FaultManagerObject obj)
        throw (CORBA::SystemException)
    {
    };

    
private:
    ::FT::FaultNotifier_var m_fault_notifier;
    CdmwFT::FaultManagement::FaultDetector_var m_fault_detector;
    CdmwFT::ReplicationManager_var m_replication_manager;
    

};
}; // namespace FT
}; // namespace Cdmw

#endif //INCL_SIM_FAULT_MANAGER_HPP

