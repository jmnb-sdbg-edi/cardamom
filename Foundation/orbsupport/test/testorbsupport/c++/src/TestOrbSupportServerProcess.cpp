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


#include "testorbsupport/TestOrbSupportServerProcess.hpp"

#include <fstream>
#include <sstream>

#include "Foundation/common/String.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "testorbsupport/TestOrbSupportServer_impl.hpp"


namespace Cdmw
{

    namespace OrbSupport
    {

        const char* TestOrbSupportServerProcess::SERVER_MT_CORBALOC_NAME = "ServerMT";
        const char* TestOrbSupportServerProcess::SERVER_ST_CORBALOC_NAME = "ServerST";

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        TestOrbSupportServerProcess::TestOrbSupportServerProcess()
        {
        }


        TestOrbSupportServerProcess::~TestOrbSupportServerProcess()
        throw()
        {
        }


        void
        TestOrbSupportServerProcess::start(int& argc, char** argv)
        {

            using namespace CdmwTest;

            // We initialise the ORB, with a thread pool of 5
            //    OrbSupport orbSupport;
            StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPool(5);

            // force CdmwProcessPort and CdmwLocalisationService
            int argc2 = argc + 2;
            char** argv2 = NULL;
            std::string str_opt( "" );
            // copy initial args

            for (int i = 0; i < argc; i++)
                str_opt += std::string( argv[i] ) + " ";

            // add args
            std::stringstream str_stream;

            str_stream << "--CdmwProcessPort=" << CDMW_PROCESS_PORT
            << " --CdmwLocalisationService=" << CDMW_LOCALISATION_PORT;

            str_opt += str_stream.str();

            argv2 = Cdmw::Common::String::to_char_array(
                        Cdmw::Common::String::to_strings( str_opt ) );

            CORBA::ORB_var orb = OrbSupport::ORB_init( argc2, argv2, strategyList);


            // And activate the root POA
            CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");

            PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

            PortableServer::POAManager_var poaMgr = poa->the_POAManager();



            // We create the servant server on the RootPOA
            TestOrbSupportServer_impl testOrbSupportServer_impl;

            TestOrbSupportServer_var testOrbSupportServer = testOrbSupportServer_impl._this();

            // bind multi-threaded object to corbaloc
            OrbSupport::bind_object_to_corbaloc(orb.in(), SERVER_MT_CORBALOC_NAME, testOrbSupportServer.in());

            // We write its ior in a file
            std::ofstream iorFile("SupportServerMT.ior");

            CORBA::String_var strIor = orb->object_to_string( testOrbSupportServer.in() );

            iorFile << strIor.in() << std::endl;

            iorFile.close();

            // Now we create a POA mono-threaded ( with a thread pool of 1)
            StrategyList strategyListSingleThread;

            strategyListSingleThread.add_PoaThreadPool(1);

            CORBA::PolicyList policies;

            // Give it the CDMW_ROOT_POA_NAME. thus, its POAManager
            // will use the CdmwProcessPort
            PortableServer::POA_var poaSingleThread = OrbSupport::create_POA(
                        poa.in(),
                        CDMW_ROOT_POA_NAME,
                        NULL,
                        policies,
                        strategyListSingleThread);

            TestOrbSupportServer_impl* ptestOrbSupportServer_impl_SingleThread =
                new TestOrbSupportServer_impl;


            PortableServer::ObjectId_var id_ptestOrbSupportServer =
                poaSingleThread->activate_object(ptestOrbSupportServer_impl_SingleThread);

            CORBA::Object_var obj_id_ptestOrbSupportServer =
                poaSingleThread->id_to_reference( id_ptestOrbSupportServer.in() );

            // bind single-threaded object to corbaloc
            OrbSupport::bind_object_to_corbaloc(orb.in(), SERVER_ST_CORBALOC_NAME, obj_id_ptestOrbSupportServer.in());

            // We write its ior in a file
            std::ofstream iorFile2("SupportServerST.ior");

            CORBA::String_var strIor2 = orb->object_to_string( obj_id_ptestOrbSupportServer.in() );

            iorFile2 << strIor2.in() << std::endl;

            iorFile2.close();



            // Ok, we can start the ORB
            poaMgr->activate();

            poaSingleThread->the_POAManager()->activate();

            orb->run();

        }




    } // End namespace OrbSupport
} // End namespace Cdmw

