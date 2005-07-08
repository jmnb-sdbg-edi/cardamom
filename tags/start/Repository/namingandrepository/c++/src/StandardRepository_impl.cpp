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


#include "namingandrepository/StandardRepository_impl.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Assert.hpp"

#include "namingandrepository/BindingIterator_impl.hpp"
#include "namingandrepository/NamingContext_impl.hpp"
#include "namingandrepository/NamingContextActivator_impl.hpp"
#include "namingandrepository/NameDomainContext_impl.hpp"
#include "namingandrepository/NameDomainContextActivator_impl.hpp"
#include "namingandrepository/NameDomain_impl.hpp"
#include "namingandrepository/NameDomainActivator_impl.hpp"


namespace
{

    const int POA_THREAD_POOL_SIZE = 5;

}


namespace Cdmw
{
namespace NamingAndRepository
{


bool StandardRepository_impl::initialize(CORBA::ORB_ptr orb)
{

    bool result = false;

    CDMW_ASSERT(!CORBA::is_nil(orb));

    try
    {

//        CORBA::ORB_var orb = orbSupport.get_ORB();

        // Get the root POA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        // Get the POA manager
        PortableServer::POAManager_var POAMgr = rootPOA->the_POAManager();

        // Create the policies and the strategies
        CORBA::PolicyList policies(5);
        policies.length(5);
        policies[0] = rootPOA->create_thread_policy(PortableServer::ORB_CTRL_MODEL);
        policies[1] = rootPOA->create_lifespan_policy(PortableServer::PERSISTENT);
        policies[2] = rootPOA->create_id_assignment_policy(PortableServer::USER_ID);
        policies[3] = rootPOA->create_servant_retention_policy(PortableServer::RETAIN);
        policies[4] = rootPOA->create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
    
        Cdmw::OrbSupport::StrategyList poaStrategies;

        // the PERSISTENT policy is incompatible with the THREADPOOL policy for TAO
    #if CDMW_ORB_VDR == tao
        poaStrategies.add_PoaThreadPerConnection();
    #else
        poaStrategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
    #endif

		
        // -- Initialize BindingIterator --
        BindingIterator_impl::initialize(rootPOA.in());
	

        using Cdmw::OrbSupport::OrbSupport;
        // -- Create the POA for NamingContext_impl and its derived classes --
        PortableServer::POA_var ncPOA = OrbSupport::create_POA(rootPOA.in(), 
                                                               "NamingContextPOA", 
                                                               POAMgr.in(), policies, poaStrategies);

        PortableServer::ServantActivator_var ncActivator = new NamingContextActivator_impl;
        ncPOA->set_servant_manager(ncActivator.in());

        // -- Initialize NamingContext_impl --
        NamingContext_impl::initialize(ncPOA.in());

		
        // -- Create the POA for NameDomainContext_impl and its derived classes --
        PortableServer::POA_var ndcPOA 
            = OrbSupport::create_POA(rootPOA.in(), "NameDomainContextPOA", 
                                     POAMgr.in(), policies, poaStrategies);

        PortableServer::ServantActivator_var ndcActivator = new NameDomainContextActivator_impl;
        ndcPOA->set_servant_manager(ndcActivator.in());
        
        // -- Initialize NameDomainContext_impl --
        NameDomainContext_impl::initialize(ndcPOA.in());

		
        // -- Create the POA for NameDomain_impl and its derived classes --
        PortableServer::POA_var ndPOA 
            = OrbSupport::create_POA(rootPOA.in(), "NameDomainPOA", POAMgr.in(), policies, poaStrategies);

        PortableServer::ServantActivator_var ndActivator = new NameDomainActivator_impl;
        ndPOA->set_servant_manager(ndActivator.in());

        // -- Initialize NameDomain_impl --
        NameDomain_impl::initialize(ndPOA.in());
	

        // release the policies
        for (unsigned long i = 0; i < policies.length(); i++)
            policies[i]->destroy();

        result = true;
    
    }
    catch(...)
    {
    
    }
    
    return result;
												 			  			  
}	  



StandardRepository_impl::StandardRepository_impl()
{

}



} // End of namespace NamingAndRepository
} // End of namespace Cdmw

