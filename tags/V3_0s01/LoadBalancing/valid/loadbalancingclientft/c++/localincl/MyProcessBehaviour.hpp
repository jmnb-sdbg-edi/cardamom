/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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
 
#ifndef INCL_MY_PROCESS_BEHAVIOUR_HPP 
#define INCL_MY_PROCESS_BEHAVIOUR_HPP 

// Standard Files 
#include <string>

// Cdmw Files
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include "loadbalancingclientft/TestLBInitThread.hpp"
#include "LoadBalancing/lbgroupmanager/LBGroupManager_impl.hpp"
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include "LoadBalancing/lbcommon/StateTransferConfig.hpp"

/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class MyProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
     * Purpose:
     * <p> The constructor.
     */
    MyProcessBehaviour(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr rootPOA,
        std::string host_name1,
        std::string host_name2,
	std::string host_name3)
        :  m_host_name1(host_name1),
           m_host_name2(host_name2),
	   m_host_name3(host_name3)
        {
            m_orb = CORBA::ORB::_duplicate( orb );
            m_rootPOA = PortableServer::POA::_duplicate( rootPOA );
            m_test_thread = NULL;
        }

    /**
     * Purpose:
     * <p> The destructor.
     */
    virtual ~MyProcessBehaviour() throw ()
    {
    }

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CORBA::SystemException )
    {

    }

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    virtual void on_run()
    throw( CORBA::SystemException,
           CdmwPlatformMngt::ProcessDelegate::NotReadyToRun )
    {
   
	 std::cout << "   -------- Client initialisation -----------" << std::endl;
        
	 // get NamingInterface to object_groups (for reading)
	 Cdmw::CommonSvcs::Naming::NamingInterface objGroupsNamingInterface =
	     Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(REPOSITORY_GROUP_MANAGER_GROUP_NAME.c_str());
	 
	 // get reference to Database ObjectGroup from NamingInterface
	 CORBA::Object_var obj;
	 try
	 {
	     std::cout << "     Client: retrieve group reference of Database " << std::endl;
	     obj =
		 objGroupsNamingInterface.resolve (GROUP_MANAGER_OBJECT_GROUP_NAME);
	     
	     m_group_mgr =CdmwLB::LBGroupManager::_narrow( obj.in() );
	 }
	 catch(const CosNaming::NamingContext::NotFound& e)
	 {
	     std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
	     std::cerr << "Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::NotFound raised!" << std::endl;
	 }
	 catch(const CosNaming::NamingContext::CannotProceed& e)
	 {
	     std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
	     std::cerr << "Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::CannotProceed raised!" << std::endl;
	 }
	 catch(const CosNaming::NamingContext::InvalidName& e)
	 {
	     std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
	     std::cerr << "Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => NamingContext::InvalidName raised!" << std::endl;
	 }
	 catch (const CORBA::SystemException& e)
	 {
	     std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
	     std::cerr << "Resolve " << GROUP_MANAGER_OBJECT_GROUP_NAME << " => CORBA::SystemException raised!" << std::endl;
	 }
	 
	 std::cout << "   -------- Client initialisation ended -----" << std::endl;
	 
	 // Launch the test in a thread
	 m_test_thread = new TestLBInitThread(
	     m_orb.in(), m_rootPOA.in(), m_group_mgr.in(), m_host_name1, m_host_name2, m_host_name3 );
	 
	 m_test_thread->start();
    }

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    virtual void on_stop()
    throw( CORBA::SystemException )
    {
        m_test_thread->join();
    }

private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The root POA.
     */
    PortableServer::POA_var m_rootPOA;

    /**
     * The group manager.
     */
    CdmwLB::LBGroupManager_var m_group_mgr;

    /**
     * The test thread.
     */
    TestLBInitThread* m_test_thread;

    std::string m_host_name1;
    std::string m_host_name2;
    std::string m_host_name3;
  
};


#endif // INCL_MY_PROCESS_BEHAVIOUR_HPP

