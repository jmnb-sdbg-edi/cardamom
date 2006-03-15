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


#ifndef INCL_TRACE_ORBTHREAD_HPP 
#define INCL_TRACE_ORBTHREAD_HPP 

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "Foundation/osthreads/Thread.hpp"

using namespace Cdmw;
using namespace Cdmw::OsSupport;

class OrbThread : public OsSupport::Thread
{
    public:
        
        OrbThread(int argc=0, char* argv[]=NULL)
        {

            OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPool( 5 );
            strategyList.add_multicast();

            m_orb = OrbSupport::OrbSupport::ORB_init(
                argc, argv, strategyList ); 
            
            if (CORBA::is_nil(m_orb.in()))
            {
                std::cerr << "ORB init failed" << std::endl;
            }

            // Activate the POA manager
            CORBA::Object_var obj
                = m_orb->resolve_initial_references( "RootPOA" );
            m_poa = PortableServer::POA::_narrow( obj.in() );
            m_poaMgr = m_poa->the_POAManager();
            m_poaMgr->activate();
        }

        ~OrbThread() throw()
        {
        };


        void shutdown()
        {
            std::cout<<"shutdown started"<<std::endl;
            m_poa->destroy(true, true);
            m_orb->shutdown( true );
            OrbSupport::OrbSupport::ORB_cleanup( m_orb.in() );
            m_orb->destroy();
            std::cout<<"shutdown completed"<<std::endl;            
        }

        CORBA::ORB_ptr orb()
        {
            return CORBA::ORB::_duplicate( m_orb.in() );
        }

        
        PortableServer::POA_ptr poa()
        {
            return PortableServer::POA::_duplicate( m_poa.in() );
        }


    
    
    protected:

        void run() throw()
        {
            m_orb->run();
        }

    private:

        CORBA::ORB_var  m_orb;

        PortableServer::POA_var m_poa;
        PortableServer::POAManager_var m_poaMgr;
    
};


#endif // INCL_TRACE_ORBTHREAD_HPP 

