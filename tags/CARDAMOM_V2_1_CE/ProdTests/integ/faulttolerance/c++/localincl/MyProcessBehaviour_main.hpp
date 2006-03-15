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
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "faulttolerance/TestFTInitThread.hpp"
#include "FaultTolerance/idllib/FT.stub.hpp"

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
        PortableServer::POA_ptr rootPOA)
        : m_nb_init_step( 2 ),
          m_current_step( 0 )
    {
        m_orb = CORBA::ORB::_duplicate( orb );
        m_test_thread = NULL;
    }

    /**
     * Purpose:
     * <p> The destructor.
     */
    virtual ~MyProcessBehaviour() throw ()
    {
    }

    virtual CORBA::ULong nb_steps()
    throw( CORBA::SystemException )
    {
        return m_nb_init_step;
    }

    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
    throw( CORBA::SystemException )
    {
        ++m_current_step;

        // Get the Replication Manager
        std::cout << "Obtain the ReplicationManager " << std::endl;

        CORBA::Object_var replication_mgr_obj
            = m_orb->resolve_initial_references("ReplicationManager");

        m_replication_mgr = CdmwFT::ReplicationManager::_narrow(
            replication_mgr_obj.in() );

        // Create the object group
        std::cout << "Create group" << std::endl;
        const char* rep_id_HelloInterface
            = "IDL:thalesgroup.com/Cdmw/Test/HelloInterface:1.0";

        ::FT::ObjectGroup_var obj_group
            = Cdmw::FT::TestUtils::create_group(
                m_orb.in(),
                m_replication_mgr.in(),
                rep_id_HelloInterface );

        m_group = Cdmw::Test::HelloInterface::_narrow( obj_group.in() );
    }

    void on_next_step()
    throw( CdmwPlatformMngt::ProcessDelegate::InvalidStep,
           CORBA::SystemException )
    {
        if( m_current_step < m_nb_init_step )
            ++m_current_step;
        else
            throw CdmwPlatformMngt::ProcessDelegate::InvalidStep();

        std::cout << "Adding new member to the group..." << std::endl;
        {
            // Get the reference of an object from the NamingAndRepository
            ::FT::Location loc;
            loc.length( 3 );
            loc[0].id = 
                CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
            loc[0].kind = "hostname";
            loc[1].id = "FTApplication";
            loc[1].kind = "applicationname";
            loc[2].id = "hello1";
            loc[2].kind = "processname";

			
            Cdmw::CommonSvcs::Naming::NamingInterface ni =
                Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");
            
            
            std::string full_name = "dom3/hello1HelloInterface";

            CORBA::Object_var obj;
            
            try
            {
                obj = ni.resolve (full_name);
                std::cout<<__FILE__<<"  "<<__LINE__<<std::endl;

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
            
            
            Cdmw::Test::HelloInterface_var hello1
                =  Cdmw::Test::HelloInterface::_narrow( obj.in() );
            
            // add this reference as a new member of the object group
            ::FT::ObjectGroup_var obj_group
                = m_replication_mgr->add_member(m_group.in(), loc, hello1.in() );
            
            m_group = Cdmw::Test::HelloInterface::_narrow(
                obj_group.in() );            
        }

        std::cout << "Adding new member to the group..." << std::endl;
        {
            // Get the reference of an object from NameServ
            ::FT::Location loc;
                loc.length( 3 );
                loc[0].id = 
						 CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
                loc[0].kind = "hostname";
                loc[1].id = "FTApplication";
                loc[1].kind = "applicationname";
                loc[2].id = "hello2";
                loc[2].kind = "processname";

            Cdmw::CommonSvcs::Naming::NamingInterface ni =
                Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");
            
            std::string full_name = "dom3/hello2HelloInterface";

            CORBA::Object_var obj;
            
            try
            {
                obj = ni.resolve (full_name);
                std::cout<<__FILE__<<"  "<<__LINE__<<std::endl;

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
            
            Cdmw::Test::HelloInterface_var hello2
                =  Cdmw::Test::HelloInterface::_narrow( obj.in() );
            
            // add this reference as a new member of the object group
            ::FT::ObjectGroup_var obj_group
                = m_replication_mgr->add_member(
                    m_group.in(), loc, hello2.in() );

            m_group = Cdmw::Test::HelloInterface::_narrow(
                obj_group.in() );
        }

        std::cout << "Adding new member to the group..." << std::endl;
        {
            // Get the reference of an object from NamingAndRepository
            ::FT::Location loc;
                loc.length( 3 );
                loc[0].id = 
						 CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
                loc[0].kind = "hostname";
                loc[1].id = "FTApplication";
                loc[1].kind = "applicationname";
                loc[2].id = "hello3";
                loc[2].kind = "processname";

            Cdmw::CommonSvcs::Naming::NamingInterface ni =
                Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");
            
            std::string full_name = "dom3/hello3HelloInterface";

            CORBA::Object_var obj;
            
            try
            {
                obj = ni.resolve (full_name);
                std::cout<<__FILE__<<"  "<<__LINE__<<std::endl;

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
            
            Cdmw::Test::HelloInterface_var hello3
                =  Cdmw::Test::HelloInterface::_narrow( obj.in() );
            
            // add this reference as a new member of the object group
            ::FT::ObjectGroup_var obj_group
                = m_replication_mgr->add_member(
                    m_group.in(), loc, hello3.in() );

            m_group = Cdmw::Test::HelloInterface::_narrow(
                obj_group.in() );
        }

        ++m_current_step;
    }

    virtual void on_run()
    throw( CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
           CORBA::SystemException )
    {
        // Launch the test in a thread
        m_test_thread = new TestFTInitThread(
            m_orb.in(), m_group.in(), m_replication_mgr.in());

        m_test_thread->start();
    }

    virtual void on_stop()
    throw( CORBA::SystemException )
    {
        m_test_thread->shutdown();
        m_test_thread->join();

        Cdmw::OsSupport::OS::unlink("hello.stt");
    }

private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    /**
     * The object group.
     */
    Cdmw::Test::HelloInterface_var m_group;

    /**
     * The replication manager.
     */
    CdmwFT::ReplicationManager_var m_replication_mgr;

    /**
     * The test thread.
     */
    TestFTInitThread* m_test_thread;

    /**
     * The number of init step.
     */
    unsigned int m_nb_init_step;

    /**
     * The current init step.
     */
    unsigned int m_current_step;

};


#endif // INCL_MY_PROCESS_BEHAVIOUR_HPP

