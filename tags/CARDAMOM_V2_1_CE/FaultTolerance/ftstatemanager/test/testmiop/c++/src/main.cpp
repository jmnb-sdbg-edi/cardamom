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


// Standard Files 
#include <iostream>

// Cdmw Files
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/CORBASystemExceptionWrapper.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <testmiop/TestMIOP.hpp>


using namespace Cdmw;
using namespace std;


class OrbThread : public Cdmw::OsSupport::Thread 
{

    public:
        
        OrbThread(CORBA::ORB_ptr orb)
            : m_orb(CORBA::ORB::_duplicate(orb))
        {};
         
            
        ~OrbThread() throw()
        {};
    
    
        void shutdown()
        {
            m_orb->shutdown(true);
        };
    
    protected:

        void run() throw()
        {
            m_orb->run();
        };
        
    private:
        CORBA::ORB_var  m_orb;
};



int main( int argc, char* argv[] )
{
    try
    {
        // Initialises the ORB
        OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPool(1);
        strategyList.add_multicast();
        
        CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);
        
        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

        // run ORB in a separate thread
        OrbThread orbThread(orb.in());
        orbThread.start();        
        
        // Do test
        std::cout<<"start test"<<std::endl;
        CdmwFT::Test::TestMIOP test(orb.in());
        test.start();
            
        orbThread.shutdown();
        orbThread.join();
        
        orb->destroy();
    }
    catch( const CORBA::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << std::endl;
        return EXIT_FAILURE;
    }
    catch( const Cdmw::Exception &e )
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr <<__FILE__<<" "<<__LINE__<<" "
                  << "Unexpected exception" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

