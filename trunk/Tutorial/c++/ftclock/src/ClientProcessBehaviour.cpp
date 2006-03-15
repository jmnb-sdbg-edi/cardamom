/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * =========================================================================== */

#include <iostream>

// Cdmw Files
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include "ClientProcessBehaviour.hpp"
#include "Time/clockservice/StateTransferConfig.hpp"

#include <iostream>


Cdmw::clock::valid::ClientProcessBehaviour::ClientProcessBehaviour(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_client_thread = NULL;
}

    
Cdmw::clock::valid::ClientProcessBehaviour::~ClientProcessBehaviour() throw ()
{
}

void
Cdmw::clock::valid::ClientProcessBehaviour::on_initialise(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CORBA::SystemException )
{
    std::cout << "   -------- Client initialisation -----------" << std::endl;
        
    // get NamingInterface to object_groups (for reading)
    Cdmw::CommonSvcs::Naming::NamingInterface objGroupsNamingInterface =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(REPOSITORY_CLOCK_GROUP_NAME.c_str());
        
    // get reference to Database ObjectGroup from NamingInterface
    CORBA::Object_var obj;
    try
    {
        std::cout << "     Client: retrieve group reference of Database " << std::endl;
        obj =
            objGroupsNamingInterface.resolve (CLOCK_GROUP_NAME);

        m_group =
            CosClockService::PeriodicExecution::Executor::_narrow( obj.in() );
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => NamingContext::NotFound raised!" << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => NamingContext::CannotProceed raised!" << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => NamingContext::InvalidName raised!" << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << CLOCK_GROUP_NAME << " => CORBA::SystemException raised!" << std::endl;
    }
        
    std::cout << "   -------- Client initialisation ended -----" << std::endl;
}
        
        
void
Cdmw::clock::valid::ClientProcessBehaviour::on_run()
    throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
           CORBA::SystemException )
{
    std::cout << "   -------- Client is runing ----------------" << std::endl;

    // Launch the test in a thread
    m_client_thread = new ClientThread(m_group.in());

    m_client_thread->start(); 
}

    
void
Cdmw::clock::valid::ClientProcessBehaviour::on_stop()
    throw( CORBA::SystemException )
{
    std::cout << "   -------- Client is stopping --------------" << std::endl;
        
    m_client_thread->stop();
    m_client_thread->join();
        
    delete m_client_thread;

    m_orb->shutdown(false);

    std::cout << "   -------- Client stopped ------------------" << std::endl;
}


