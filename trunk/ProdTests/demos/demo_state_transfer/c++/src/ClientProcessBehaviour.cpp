/* =========================================================================== *
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
 * =========================================================================== */


// Cdmw Files
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

// Demo Files
#include "ClientProcessBehaviour.hpp"
#include "StateTransferConfiguration.hpp"

#include <iostream>


ClientProcessBehaviour::ClientProcessBehaviour(
    CORBA::ORB_ptr orb,
    PortableServer::POA_ptr rootPOA)
{
    m_orb = CORBA::ORB::_duplicate( orb );
    m_client_thread = NULL;
}

    
ClientProcessBehaviour::~ClientProcessBehaviour() throw ()
{
}

    
CORBA::ULong ClientProcessBehaviour::nb_steps()
    throw( CORBA::SystemException )
{
    return 1;
}

    
void ClientProcessBehaviour::on_initialise(
    const CdmwPlatformMngtBase::StartupKind& startup_kind )
throw( CORBA::SystemException )
{
    std::cout << "   -------- Client initialisation -----------" << std::endl;
        
    // get NamingInterface to object_groups (for reading)
    Cdmw::NamingAndRepository::NamingInterface objGroupsNamingInterface =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("demo/object_groups");
        
    // get reference to Database ObjectGroup from NamingInterface
    CORBA::Object_var obj;
    try
    {
        std::cout << "     Client: retrieve group reference of Database " << std::endl;
        obj = objGroupsNamingInterface.resolve (DATABASE_GROUP_NAME);

        m_group =  Demo::Database::_narrow( obj.in() );
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << DATABASE_GROUP_NAME << " => NamingContext::NotFound raised!" << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << DATABASE_GROUP_NAME << " => NamingContext::CannotProceed raised!" << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << DATABASE_GROUP_NAME << " => NamingContext::InvalidName raised!" << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
        std::cerr << "Resolve " << DATABASE_GROUP_NAME << " => CORBA::SystemException raised!" << std::endl;
    }
        
    std::cout << "   -------- Client initialisation ended -----" << std::endl;
}
        
        
void ClientProcessBehaviour::on_next_step()
    throw( CdmwPlatformMngt::Process::InvalidStep,
           CORBA::SystemException )
{
}

    
void ClientProcessBehaviour::on_run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CORBA::SystemException )
{
    std::cout << "   -------- Client is runing ----------------" << std::endl;

    // Launch the test in a thread
    m_client_thread = new ClientThread(m_group.in());

    m_client_thread->start();
}

    
void ClientProcessBehaviour::on_stop()
    throw( CORBA::SystemException )
{
    std::cout << "   -------- Client is stopping --------------" << std::endl;
        
    m_client_thread->stop();
    m_client_thread->join();
        
    delete m_client_thread;

    // Same pb than in PCR-0046 was done again..
    // m_orb->shutdown(false);

    std::cout << "   -------- Client stopped ------------------" << std::endl;
}


