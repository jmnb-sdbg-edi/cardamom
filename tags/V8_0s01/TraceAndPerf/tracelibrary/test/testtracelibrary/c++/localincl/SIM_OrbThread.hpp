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

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"

#include "Foundation/osthreads/ThreadHandle.hpp"
namespace Cdmw 
{
namespace Trace
{


class OrbThread : public OsSupport::Thread
{

    public:
        
        OrbThread()
		{
            OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPool(10);

	        int argc(0);
            m_orb = OrbSupport::OrbSupport::ORB_init (argc, NULL, strategyList); 
            
            // And activate the root POA
            CORBA::Object_var obj = m_orb->resolve_initial_references("RootPOA");
            m_poa = PortableServer::POA::_narrow(obj.in());
            PortableServer::POAManager_var poaMgr = m_poa->the_POAManager();
            poaMgr->activate();
        }

		virtual
        ~OrbThread()
		   throw()
		{
		}

        void shutdown() 
		{
            m_orb->shutdown(false);
	    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(m_orb.in());
	    m_orb->destroy();
        }


        
		CORBA::ORB_ptr get_orb()
        {
            return CORBA::ORB::_duplicate (m_orb.in());
        }

        
        PortableServer::POA_ptr get_poa()
        {
            return PortableServer::POA::_duplicate (m_poa.in());
        }

    protected:

        void run() throw() 
		{
            m_orb->run();
        }

    private:

        //OrbSupport::OrbSupport m_orbSupport;
        
		CORBA::ORB_var  m_orb;

        PortableServer::POA_var m_poa;

};

}; // End namespace Cdmw 

}; // End namespace Trace

#endif // INCL_TRACE_ORBTHREAD_HPP 

