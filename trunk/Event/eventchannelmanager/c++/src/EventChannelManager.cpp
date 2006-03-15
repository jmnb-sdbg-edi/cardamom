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


// C++ include
#include <iostream>
#include <cstdlib>


// CDMW include
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>

#include "SystemMngt/platforminterface/PlatformInterface.hpp"

// CDMW idl include

// Application include
#include "eventchannelmanager/InitUtils.hpp"
#include "eventchannelmanager/EventChannelProfileManager_impl.hpp"

using namespace Cdmw::EventAdmin;
using namespace Cdmw::Common;
using namespace Cdmw::OrbSupport;
using namespace Cdmw::NamingAndRepository;
using namespace Cdmw::PlatformMngt;

namespace
{
    
    const int POA_THREAD_POOL_SIZE = 5;    
    
    
    void exceptionHandler(void)
    {
        std::cout << "Event Channel Manager => Unexpected exception raised, file: " << __FILE__ << ", line: " << __LINE__ << std::endl;
    }
    
    
    void usage()
    {
        using Cdmw::Common;
        
        std::cout << "usage: EventChannelProfileManager " 
            << Options::CALLBACK_IOR_OPTION << "=<callbackIOR> "
            << Options::PROCESS_XML_FILE_OPTION << "=<xmlFileName> " 
            << InitUtils::PROFILE_XML_FILE_OPTION << "=<xmlFileName> " 
            << "[" << Options::VALIDATE_XML_OPTION << "]" << std::endl;
    }


    class ManagerProcessControl : public Cdmw::CdmwInit::ProcessControl
    {
    public:
        
        ManagerProcessControl(EventChannelProfileManager_impl* mng)
            throw(CORBA::SystemException):
            m_eventDomain(CdmwNamingAndRepository::NameDomain::_nil()),
            m_profileManager(mng),
            m_profileManagerServant_var(mng),
            m_is_registered(false)
        {
            m_profileManagerServant_var->_add_ref();
            
        }
               
        ~ManagerProcessControl() throw()
        {
            std::cout << "Destructor of ManagerProcessControl called" << std::endl;
            
            if (m_is_registered)
            {              
                try
                {                    
                    m_eventDomain->release_name(EventChannelProfileManager_impl::m_name.c_str());
                }
                catch (const CORBA::Exception& ex)
                {
                }
            }
        }
            
        /**
        * Purpose:
        * <p>
        * the behaviour for the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
        * operation
        */
        virtual 
            void 
            on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_event_channel_manager init" << std::endl;
                        
            // getting the default root context from the repository
            CdmwNamingAndRepository::Repository_var repository 
                = RepositoryInterface::get_repository();
            CosNaming::NamingContext_var temp_rootCtx 
                = repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
            
            // getting the "CDMW/SERVICES/EVENT" name domain from the repository
            m_eventDomain 
                = repository->resolve_name_domain("CDMW/SERVICES/EVENT");
            
            // registering profileManager in domain EVENT
            try
            {
                
                CdmwEventAdmin::EventChannelProfileManager_var ref = m_profileManager->_this();
                m_eventDomain->register_new_object(EventChannelProfileManager_impl::m_name.c_str(),
                                                   ref.in());
                m_is_registered = true;
            }
            catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists&)
            {
                std::cerr << "EventChannelProfileManager already registred! Continue..."  << std::endl;
                // Persistent Server so its not a pb!
                // continue...
            }       
        }
        
        
        /**
        * Purpose:
        * <p>
        * the behaviour for the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
        * operation
        */
        virtual 
            void 
            on_run()
            throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
            CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_event_channel_manager run" << std::endl;

            if (m_profileManager == NULL)
            {
                std::cerr << "ERROR: EventChannelProfileManager object not intanciated" << std::endl;
                throw CdmwPlatformMngt::ProcessDelegate::NotReadyToRun();
            }     
        }
        
        /**
        * Purpose:
        * <p>
        * the behaviour for the
        * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
        * operation
        */
        virtual 
            void 
            on_stop()
            throw(CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_event_channel_manager stop" << std::endl;

            try
            {
                m_eventDomain->release_name(EventChannelProfileManager_impl::m_name.c_str());
                m_is_registered = false;
            }
            catch (const CORBA::Exception& ex)
            {
                std::cerr << ex << std::endl;
                throw;
            }
        }
        
    private:
        //CORBA::ORB_var                           m_orb;
        CdmwNamingAndRepository::NameDomain_var m_eventDomain;
        EventChannelProfileManager_impl*         m_profileManager;
        PortableServer::ServantBase_var          m_profileManagerServant_var;
        bool                                     m_is_registered;
    };
    
};  // end anonymous namespace

//----------------------------------------------------------------
int main(int argc, char** argv)
{
#ifndef _MSC_VER    

    std::set_unexpected(&exceptionHandler);

    // VC++ contrary to the clause 18.6 of the C++ standard, 
    //  set_unexpected, is not inside the std:: namespace
#else

    set_unexpected(&exceptionHandler);

#endif  
    
    CORBA::ORB_var          orb;
    PortableServer::POA_var rootPOA;
    std::string             XMLFilePath;
    bool                    validate = false;
       
    
    if (argc < 2)
    {
        usage();
        return EXIT_FAILURE;
    }
    
    // Get the XML file Name : mandatory argument
    try
    { 
        XMLFilePath = InitUtils::get_xml_initialisation_file (argc, argv);
    }
    catch (const CORBA::NO_MEMORY& e)
    {
        std::cerr << e << std::endl;
        return EXIT_FAILURE;
    }
    catch (const CORBA::INTERNAL& e)
    {
        std::cerr << e << std::endl;
        return EXIT_FAILURE;
    }
    catch (const CORBA::BAD_PARAM& e)
    {
        // Cannot Proceed with no file name
        usage();
        return EXIT_FAILURE;
    }
    
    try
    {
        // Get validation option for xml file
        validate = InitUtils::get_xml_validation_option(argc, argv);
        
        // Init ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
        
        if (CORBA::is_nil(orb.in()))
        {
            std::cerr << "ERROR: No ORB" << std::endl;
            return EXIT_FAILURE;
        }
        
         // Resolve Root POA.
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        rootPOA = PortableServer::POA::_narrow(obj.in());
        
        if (CORBA::is_nil(rootPOA.in()))
        {
            std::cerr << "ERROR: No RootPOA" << std::endl;
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
            return EXIT_FAILURE;
        }
        
        // Get a reference to the POA manager.
        PortableServer::POAManager_var manager = rootPOA->the_POAManager();
        
        if (CORBA::is_nil(manager.in()))
        {
            std::cerr << "ERROR: No POAmanager" << std::endl;
        Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
        orb->destroy();
            return EXIT_FAILURE;
        }

        // Activate POA manager
        manager->activate();
        
        // Create the single instance of the profile manager
        EventChannelProfileManager_impl* profileManager 
            = EventChannelProfileManager_impl::init(orb.in(),
                                                    rootPOA.in(),
                                                    XMLFilePath, 
                                                    validate);
        
        if (profileManager == NULL)
        {
            std::string msg = "ERROR: Impossible to instantiate the EventChannelProfileManager object";
            std::cerr << msg.c_str() << std::endl;
            //PlatformInterface::Notify_fatal_error("cdmw_event_channel_manager", msg.c_str());
        }  
        
        Cdmw::CdmwInit::ProcessControl_var mng_ctrl 
            = new ManagerProcessControl(profileManager);

        // init Cdmw
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv, mng_ctrl.in());
                
        // wait for event
        orb->run();
    }   
    catch(const CORBA::SystemException& e)
    {    
        std::cerr << e << std::endl;
    }
    catch(const CORBA::Exception& e)
    {
        std::cerr << e._name() << std::endl;
    }
    catch(const Cdmw::Exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    if (!CORBA::is_nil(orb.in()))
    {
        try
        {
            Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
        }
    }        
    
    return EXIT_SUCCESS;
}

