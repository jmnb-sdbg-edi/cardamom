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


#ifndef INCL_TESTPROCESSBEHAVIOUR_HPP 
#define INCL_TESTPROCESSBEHAVIOUR_HPP 

#include "test1/TestNamingContext.hpp"
#include "test1/TestNameDomain.hpp"

#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"
#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "Foundation/common/Locations.hpp"
#include "Foundation/testutils/Testable.hpp"

#include <fstream>
#include <iostream>
#include <string>


using namespace Cdmw;
using namespace Cdmw::PlatformMngt;
using namespace Cdmw::Trace;
using namespace Cdmw::OsSupport;


/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class TestProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    TestProcessBehaviour(CORBA::ORB_ptr orb,
            bool platformManaged,
            const std::string& nsPort,
            bool launchRepository,
            bool deactivateTracesForNameDomain)
        : m_platformManaged(platformManaged),
          m_nsPort(nsPort),
          m_launchRepository(launchRepository),
          m_deactivateTracesForNameDomain(deactivateTracesForNameDomain)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~TestProcessBehaviour() throw ()
    {
    }


    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
	* operation
	*/
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException)
    {
        std::cout <<"Init requested" << std::endl;

        try
        {

            std::cout << "Launch the naming service" << std::endl;

        #if (CDMW_ORB_VDR == orbacus )
            std::string namingservice="nameserv";
            m_idNS = OS::create_process( namingservice, "-OAport " + m_nsPort );
        #elif (CDMW_ORB_VDR == tao )
            std::string namingservice="Naming_Service";
            m_idNS = OS::create_process( namingservice, "-ORBEndpoint iiop://localhost:" + m_nsPort );
        #endif

			int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OS::sleep(timescale*3000);

            if (m_launchRepository)
            {
                std::cout << "Launch the repository" << std::endl;

                m_idRepository = OS::create_process("cdmw_naming_and_repository",
                    "--CdmwLocalisationService=4997 --id=GlobalRepository --conf=CdmwDefaultNamingAndRepository.xml");

                OS::sleep(timescale*10000);


                std::cout << "Launch the repository proxy" << std::endl;

                m_idRepositoryProxy = OS::create_process("cdmw_naming_and_repository_proxy",
                    "-ORBInitRef RepositoryExt=file://RepositoryExt.IOR");

                OS::sleep(timescale*10000);
            }

        }
        catch(const BadParameterException &e)
        {
            notifyFatalError("TEST_NAMREP", e.what());
        }

        CORBA::Object_var repository_temp;

        if (m_platformManaged)
        {
            std::cout << "Getting repository from the Platform Management" << std::endl;
            repository_temp = PlatformInterface::Get_service(ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
        }
        else
        {
            std::cout << "Getting repository from IOR file" << std::endl;

            std::string repositoryIOR;
            std::ifstream iorFile("Repository.IOR");
            iorFile >> repositoryIOR;
            iorFile.close();

            repository_temp = m_orb->string_to_object(repositoryIOR.c_str());
        }

        m_repository = CdmwNamingAndRepository::Repository::_narrow(repository_temp.in());

        if (CORBA::is_nil(m_repository.in()))
        {
            notifyFatalError("TEST_NAMREP", "Invalid repository");
        }

    }


    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
	* operation
	*/
    virtual void run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
              CORBA::SystemException)
    {
        std::cout <<"Run requested" << std::endl;

        TestNamingContext test1("Naming Context", m_orb.in(), m_repository.in());

        test1.start();

        TestNameDomain test2("Name Domain", m_orb.in(), m_repository.in());

        if (m_deactivateTracesForNameDomain)
        {
            CosNaming::NamingContext_var adminRootContext_temp =
                m_repository->resolve_root_context(Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID);
            CosNaming::NamingContextExt_var adminRootContext =
                CosNaming::NamingContextExt::_narrow(adminRootContext_temp.in());

            if (CORBA::is_nil(adminRootContext.in()))
            {
                notifyFatalError("TEST_NAMREP", "Admin root context not found");
            }

            CORBA::Object_var traceProducer_temp = adminRootContext->resolve_str("RepositoryTraceProducer/TRACE/TraceProducer");
            CdmwTrace::TraceProducer_var traceProducer = CdmwTrace::TraceProducer::_narrow(traceProducer_temp.in());
            if (CORBA::is_nil(traceProducer.in()))
            {
                notifyFatalError("TEST_NAMREP", "Trace producer not found");
            }

            traceProducer->deactivate_level("CDMW_NREP", 0 );
            traceProducer->deactivate_level("CDMW_NREP", 1 );

        }

        test2.start();
    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
	* operation
	*/
    virtual void stop() throw(CORBA::SystemException)
    {
        std::cout <<"Stop requested" << std::endl;

        // kill the ORB naming service
        OS::kill_process(m_idNS);

        if (m_launchRepository)
        {
            // kill the repository
            OS::kill_process(m_idRepository);
            OS::kill_process(m_idRepositoryProxy);
        }

        m_orb->shutdown(false);
    }


private:

    /**
    * Notifies the fatal error via the platform interface or
    * output in cerr.
    */ 
    void notifyFatalError(const char* issuer, const char* message)
        throw(CORBA::SystemException)
    {

        if (m_platformManaged)
        {
            PlatformInterface::Notify_fatal_error(issuer, message);
        }
        else
        {
            std::cerr << "FAILURE : " << message << std::endl;
        }
        
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);

    }

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;

    /**
    * Indicates whether this test executable is managed by the platform management.
    */
    bool m_platformManaged;

    /**
    * The port of the naming service used for the test.
    */
    std::string m_nsPort;

    /**
    * Indicates whether the repository must be launched.
    * (Used for unit tests of the repository)
    */
    bool m_launchRepository;

    /**
    * Indicates whether the traces will be deactivated
    * for the tests of name domains.
    */
    bool m_deactivateTracesForNameDomain;

    /**
    * The process id of the ORB naming service.
    */
    OS::ProcessId m_idNS;

    /**
    * The process id of the launched repository.
    */
    OS::ProcessId m_idRepository;

    /**
    * The process id of the launched repository proxy.
    */
    OS::ProcessId m_idRepositoryProxy;

    /**
    * The naming and repository object;
    */
    CdmwNamingAndRepository::Repository_var m_repository;

};


#endif // INCL_TESTPROCESSBEHAVIOUR_HPP

