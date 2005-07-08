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


#ifndef INCL_DEMO_FT_CLIENT_PROCESS_BEHAVIOUR_HPP 
#define INCL_DEMO_FT_CLIENT_PROCESS_BEHAVIOUR_HPP 

// Standard Files 
#include <string>

// Cdmw Files
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include "ClientThread.hpp"

/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class ClientProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    ClientProcessBehaviour(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr rootPOA)
    {
        m_orb = CORBA::ORB::_duplicate( orb );
        m_client_thread = NULL;
    }

    
    virtual ~ClientProcessBehaviour() throw ()
    {
    }

    
    virtual CORBA::ULong nb_steps()
    throw( CORBA::SystemException )
    {
        return 1;
    }

    
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CORBA::SystemException )
    {
        std::cout << "   -------- Client on_initialise --------" << std::endl;
        
        // get NamingInterface to hello_servers (for reading)
        Cdmw::NamingAndRepository::NamingInterface helloNamingInterface =
            Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("demo_ft/hello_servers");
        
        // get reference to Hello ObjectGroup from NamingInterface
        CORBA::Object_var obj;
        std::string full_name = "hello_group";
        try
        {
            std::cout << "     Client: retrieve group reference of HelloInterface " << std::endl;
            obj = helloNamingInterface.resolve (full_name.c_str());

            m_group =  DemoFT::HelloInterface::_narrow( obj.in() );
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "Resolve " << full_name << " => NamingContext::NotFound raised!" << std::endl;
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "Resolve " << full_name << " => NamingContext::CannotProceed raised!" << std::endl;
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "Resolve " << full_name << " => NamingContext::InvalidName raised!" << std::endl;
        }
        catch (const CORBA::SystemException& e)
        {
            std::cerr << __FILE__ << "  " << __LINE__ << std::endl;
            std::cerr << "Resolve " << full_name << " => CORBA::SystemException raised!" << std::endl;
        }
        
        std::cout << "   -------- Client on_initialise END --------" << std::endl;
    }
        
        
    void on_next_step()
    throw( CdmwPlatformMngt::Process::InvalidStep,
           CORBA::SystemException )
    {
    }

    
    virtual void on_run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CORBA::SystemException )
    {
        std::cout << "   -------- Client run --------" << std::endl;

        // Launch the test in a thread
        m_client_thread = new ClientThread(m_group.in());

        m_client_thread->start();
    }

    
    virtual void on_stop()
    throw( CORBA::SystemException )
    {
        std::cout << "   -------- Client stop  --------" << std::endl;
        
        m_client_thread->stop();
        m_client_thread->join();
        
        delete m_client_thread;
    }

private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The Hello object group.
     */
    DemoFT::HelloInterface_var m_group;

    /**
     * The test thread.
     */
    ClientThread* m_client_thread;

};


#endif // INCL_DEMO_FT_CLIENT_PROCESS_BEHAVIOUR_HPP


