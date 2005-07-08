/* ========================================================================== *
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
 * ========================================================================= */
#ifndef INCL_MY_PROCESS_HELLO_SERVER_BEHAVIOUR_HPP
#define INCL_MY_PROCESS_HELLO_SERVER_BEHAVIOUR_HPP 

#include <fstream>

// Cdmw Files
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include <Foundation/common/Options.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
//#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <loadbalancingserver/TestHello_impl.hpp>

#include <idllib/PortableGroup.stub.hpp>
#include <idllib/CdmwLBGroupManager.stub.hpp>
#include <string>
#include <sstream>
#include <iostream>


/**
 * Purpose:
 * <p>
 * The abstract process behaviour.
 */
//class MyProcessHelloServerBehaviour: public Cdmw::PlatformMngt::ProcessBehaviour
class MyProcessHelloServerBehaviour: public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
     * Purpose:
     * <p> The constructor.
     */
    MyProcessHelloServerBehaviour (CORBA::ORB_ptr orb, std::string the_host, std::string rep_name)
        : m_rep_name(rep_name),
           m_hostname(the_host)
         
        {
            m_orb = CORBA::ORB::_duplicate(orb);
        }


    /**
     * Purpose:
     * <p> The destructor.
     */
    virtual ~MyProcessHelloServerBehaviour() throw ()
        {

        }


    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
     * attribute
     */
    virtual CORBA::ULong nb_steps() throw(CORBA::SystemException)
        {
            return 1;
        }


    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
     * operation
     */
    virtual CORBA::Object_ptr get_service() throw(CORBA::SystemException)
        {
            return CORBA::Object::_nil();
        }


    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    virtual void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw( CORBA::SystemException)
        {


        Cdmw::HelloInterface_impl * hello_interface
            = new Cdmw::HelloInterface_impl(m_hostname.c_str());
        CdmwLBInit::HelloInterface_var hello_interface_ref
            =  hello_interface->_this();

        // Get the Naming Context reference
        CdmwNamingAndRepository::Repository_var repository
            = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

        // ======================================================
        // Get HELLO name domain from repository
        // ======================================================
        CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("dom1/dom2/dom3");

        // reserve the name "HelloInterface" in the domain
        CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (m_rep_name.c_str());
        // register the object
        helloDomain->register_object (regId.in(), hello_interface_ref.in ());
        
        
            std::cerr<< m_hostname << " test_server: orb run"<<std::endl;
            Cdmw::OsSupport::OS::sleep(500);
        }




    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    virtual void on_run()
        throw(CORBA::SystemException,
              CdmwPlatformMngt::Process::NotReadyToRun)
        {

        }
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    virtual void on_stop() throw(CORBA::SystemException)
        {
            // Get the Naming Context reference
            CdmwNamingAndRepository::Repository_var repository
                = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

            // ======================================================
            // Get HELLO name domain from repository
            // ======================================================
            CdmwNamingAndRepository::NameDomain_var helloDomain =
                repository->resolve_name_domain ("dom1/dom2/dom3");

            // release the object
            helloDomain->release_name(m_rep_name.c_str());
            std::cerr<< m_hostname << " test_server: stopped"<<std::endl;
        }

    
private:

    std::string m_rep_name;
    std::string m_hostname;
    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;
};

#endif // INCL_MY_PROCESS_HELLO_SERVER_BEHAVIOUR_HPP
