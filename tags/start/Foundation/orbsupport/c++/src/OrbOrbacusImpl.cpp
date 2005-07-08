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


#include "orbsupport/OrbOrbacusImpl.hpp"

#if CDMW_ORB_VDR == orbacus


#include <sstream>


#include <OB/CORBA.h>
#include <OB/Properties.h>
#include <OB/BootManager.h>

#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "orbsupport/JTCAdoptionToken.hpp"


namespace
{
    /**
     * Name of the CDMW POA Manager
     */
    static const char* CDMW_POA_MANAGER_NAME = "CDMW_POA_MANAGER";

}

; // End anonymous namespace


namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Implementation notes: ORBacus doesn't document error
        *                        or exception which may be thrown
        *<p>
        *Portability issues: none 
        *<p>
        */

        bool OrbOrbacusImpl::M_init_called(false);
        Cdmw::OsSupport::Mutex OrbOrbacusImpl::M_mutex;
        OB::DispatchStrategyFactory_var
        OrbOrbacusImpl::M_dispatchStrategyFactory(OB::DispatchStrategyFactory::_nil());

        PortableServer::POAManager_var
        OrbOrbacusImpl::M_CdmwPOAManager(PortableServer::POAManager::_nil());

        //
        // Initialises the OrbAcus ORB
        //
        CORBA::ORB_ptr
        OrbOrbacusImpl::ORB_init (int& argc, char** argv,
                                  const StrategyList& strategyList,
                                  std::size_t root_poa_port,
                                  std::size_t cdmw_root_poa_port,
                                  const char* orb_identifier)
        throw (CORBA::SystemException)
        {
            CORBA::ORB_var theOrb = CORBA::ORB::_nil();

            try
            {
                // At this time is ORB SHALL be multi-threaded

                if (! strategyList.is_OrbThreaded())
                {
                    throw CORBA::BAD_PARAM(BAD_PARAMMissingORBThreadingStrategy,
                                           CORBA::COMPLETED_NO);
                }

                if (strategyList.is_multicast())
                {
                    // No multicast with Orbacus 
                    throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
                }

                // We create the set of properties required by orbacus
                OB::Properties_var properties = OB::Properties::getDefaultProperties();

                OB::ParseArgs(argc, argv, properties);

                // The ORB is multi-threaded
#if (CDMW_ORB_VER > 410)
# error Check if ORBacus properties have not changed!
#else

                properties->setProperty("ooc.orb.conc_model", "threaded");

#endif


                // The number of thread to use in case of thread pool
                std::size_t nbThreads;


                // Threading strategies
                // --------------------------------------------------------------------------

                // Do we use a thread pool for the POAs ?
                if (strategyList.is_PoaThreadPool (nbThreads))
                {

                    // Must be checked by the StrategyList
                    CDMW_ASSERT(nbThreads > 0);

                    std::stringstream strNbThread;
                    strNbThread << nbThreads << std::ends;

#if (CDMW_ORB_VER > 410)
# error Check if ORBacus properties have not changed!
#else

                    properties->setProperty("ooc.orb.oa.conc_model", "thread_pool");
                    properties->setProperty("ooc.orb.oa.thread_pool",
                                            strNbThread.str().c_str());
#endif

                    // Else we use a multi-threaded POA
                }
                else if (strategyList.is_PoaThreadPerRequest())
                {
#if (CDMW_ORB_VER > 410)
# error Check if ORBacus properties have not changed!
#else
                    properties->setProperty("ooc.orb.oa.conc_model", "thread_per_request");
#endif

                    // Else the user hasn't set a strategy for the POA
                }
                else if (strategyList.is_PoaThreadPerConnection())
                {
#if (CDMW_ORB_VER > 410)
# error Check if ORBacus properties have not changed!
#else
                    properties->setProperty("ooc.orb.oa.conc_model", "thread_per_client");
#endif

                    // Else the user hasn't set a strategy for the POA
                }
                else
                {
                    throw CORBA::BAD_PARAM(BAD_PARAMPOAMutltiThreadStrategyRequired,
                                           CORBA::COMPLETED_NO);
                }

                // IIOP strategies
                // --------------------------------------------------------------------------

                // We want to have IP adress in IOR instead of hostname so we use --numeric
                // Do we use a specific port for the root POA ?
                if (root_poa_port != 0)
                {
                    std::stringstream strPoaPort;
#if (CDMW_ORB_VER < 410)

                    strPoaPort << root_poa_port << std::ends;

                    properties->setProperty("ooc.iiop.port", strPoaPort.str().c_str());
                    properties->setProperty("ooc.iiop.numeric", "true");
#elif (CDMW_ORB_VER == 410)

                    strPoaPort << "iiop --numeric --port " << root_poa_port << std::ends;
                    properties->setProperty("ooc.orb.oa.endpoint", strPoaPort.str().c_str());
#else
# error Check if ORBacus properties have not changed!
#endif

                }
                else
                {
#if (CDMW_ORB_VER < 410)
                    properties->setProperty("ooc.iiop.numeric", "true");
#elif (CDMW_ORB_VER == 410)

                    properties->setProperty("ooc.orb.oa.endpoint", "iiop --numeric");
#else
# error Check if ORBacus properties have not changed!
#endif

                }

                // Do we use a specific port for the Cdmw root POA ?
                if (cdmw_root_poa_port != 0)
                {
                    std::stringstream strPoaPort;
#if (CDMW_ORB_VER < 410)

                    strPoaPort << cdmw_root_poa_port << std::ends;

                    // properties->setProperty( "ooc.iiop.port", strPoaPort.str().c_str() );
                    std::string property_key_base("ooc.orb.poamanager.");
                    property_key_base += CDMW_POA_MANAGER_NAME;

                    std::string property_key(property_key_base);
                    property_key += ".port";
                    properties->setProperty( property_key.c_str(), strPoaPort.str().c_str() );
                    // We want to have IP adress in IOR instead of hostname
                    property_key = property_key_base + ".numeric";
                    properties->setProperty(property_key.c_str(), "true");
#elif (CDMW_ORB_VER == 410)

                    strPoaPort << "iiop --numeric --port " << cdmw_root_poa_port << std::ends;

                    std::string property_key_base("ooc.orb.poamanager.");
                    property_key_base += CDMW_POA_MANAGER_NAME;

                    std::string property_key(property_key_base);
                    property_key += ".endpoint";
                    properties->setProperty( property_key.c_str(), strPoaPort.str().c_str() );
#else
# error Check if ORBacus properties have not changed!
#endif

                }

                //  std::size_t poaPort;
                //  if ( strategyList.is_PoaPortAssigned( poaPort ) ) {

                //      std::stringstream strPoaPort;
                //      strPoaPort << poaPort << std::ends;

                //      properties->setProperty( "ooc.iiop.port", strPoaPort.str().c_str() );

                //  }

                // We want to have IP adress in IOR instead of hostname
                //properties->setProperty("ooc.iiop.numeric", "true");

                // -----------------------------------------------------------------------
                // -----------------------------------------------------------------------


                theOrb = OBCORBA::ORB_init (argc, argv, properties.in(), orb_identifier);

                // Set Dispatch Strategy Factory
                // Use Double-checked locking
                if (!M_init_called)
                {
                    try
                    {
                        CDMW_MUTEX_GUARD(M_mutex);

                        if (!M_init_called)
                        {
                            CORBA::Object_var obj
                            = theOrb->resolve_initial_references("DispatchStrategyFactory");

                            M_dispatchStrategyFactory
                            = OB::DispatchStrategyFactory::_narrow(obj);

                            obj = theOrb->resolve_initial_references("POAManagerFactory");
                            OBPortableServer::POAManagerFactory_var factory =
                                OBPortableServer::POAManagerFactory::_narrow(obj);
                            M_CdmwPOAManager =
                                factory->create_poa_manager(CDMW_POA_MANAGER_NAME);

                            M_init_called = true;
                        }
                    }
                    catch (...)
                    {
                        throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
                    }
                }
            }
            catch (const std::bad_alloc &)
            {
                // Orbcaus operations do not catch this exception
                throw CORBA::NO_MEMORY(NO_MEMORY, CORBA::COMPLETED_NO);
            }
            catch (const CORBA::SystemException & )
            {
                throw;
            }

            return theOrb._retn();

        }

        //
        // Create a new OrbAcus POA
        //
        PortableServer::POA_ptr
        OrbOrbacusImpl::create_POA (PortableServer::POA_ptr parentPOA,
                                    const char* adapter_name,
                                    PortableServer::POAManager_ptr a_POAManager,
                                    const CORBA::PolicyList& policies,
                                    const StrategyList& strategyList)
        throw (PortableServer::POA::AdapterAlreadyExists,
               PortableServer::POA::InvalidPolicy,
               CORBA::SystemException)
        {
            // Check that parentPOA is not a nil object reference

            if (CORBA::is_nil( parentPOA))
            {
                throw CORBA::BAD_PARAM(BAD_PARAMNilObjectReference,
                                       CORBA::COMPLETED_NO);
            }

            // We use the proprietary OrbAcus POA
            OBPortableServer::POA_var obParentPOA =
                OBPortableServer::POA::_narrow(parentPOA);

            // The POA may be not an ORBacus POA
            if (CORBA::is_nil (obParentPOA))
            {
                throw CORBA::BAD_PARAM(BAD_PARAMUnknownPOAType,
                                       CORBA::COMPLETED_NO);
            }

            // We make a copy of the user policies
            CORBA::PolicyList obPolicies (policies);

            // Each Cdmw Strategy is transform into the ORBacus proprietary Policy
            transform_strategy_into_policy (obPolicies, obParentPOA.in(), strategyList);


            PortableServer::POAManager_var the_POAManager =
                PortableServer::POAManager::_duplicate(a_POAManager);

            // Is it the CDMW POA Manager?
            if (CORBA::is_nil(the_POAManager) &&
                    (strcmp(adapter_name, CDMW_ROOT_POA_NAME) == 0))
            {
                the_POAManager = M_CdmwPOAManager;
            }

            // We have inserted our proprietary policies, we create the POA
            return obParentPOA -> create_POA (adapter_name,
                                              the_POAManager,
                                              obPolicies);
        }





        //
        // Attach a servant to a corbaloc reachable object
        //
        void
        OrbOrbacusImpl::bind_object_to_corbaloc (CORBA::ORB_ptr orb,
                const std::string& corbaloc_name,
                CORBA::Object_ptr object)
        throw (CORBA::SystemException)
        {
            // The Id must be defined by us
            PortableServer::ObjectId_var oid =
                PortableServer::string_to_ObjectId (corbaloc_name.c_str());

            CORBA::Object_var bmgrObj =
                orb -> resolve_initial_references ("BootManager");

            OB::BootManager_var bootManager = OB::BootManager::_narrow(bmgrObj);

            // Are we not using a specific version of Orbacus ?
            CDMW_ASSERT(!CORBA::is_nil(bootManager));

            // We register our object

            try
            {
                bootManager->add_binding (oid, object);
            }
            catch (const OB::BootManager::AlreadyExists &)
            {
                throw CORBA::BAD_PARAM(BAD_PARAMCorbalocBindingAlreadyExists,
                                       CORBA::COMPLETED_NO);
            }
            catch (const CORBA::SystemException &)
            {
                throw;
            }
        }

        void
        OrbOrbacusImpl::remove_corbaloc_binding (CORBA::ORB_ptr orb,
                const std::string& corbaloc_name)
        throw (CORBA::SystemException)
        {
            // The Id must be defined by us
            PortableServer::ObjectId_var oid =
                PortableServer::string_to_ObjectId (corbaloc_name.c_str());

            CORBA::Object_var bmgrObj =
                orb -> resolve_initial_references ("BootManager");

            OB::BootManager_var bootManager = OB::BootManager::_narrow(bmgrObj);

            // Are we not using a specific version of Orbacus ?
            CDMW_ASSERT(!CORBA::is_nil(bootManager));

            // We register our object

            try
            {
                bootManager->remove_binding (oid);
            }
            catch (const OB::BootManager::NotFound &)
            {
                throw CORBA::BAD_PARAM(BAD_PARAMCorbalocBindingNotFound,
                                       CORBA::COMPLETED_NO);
            }
            catch (const CORBA::SystemException &)
            {
                throw;
            }
        }


        //
        // Attach a servant to a service reachable object (by resolve_initial_reference)
        //
        void
        OrbOrbacusImpl::register_initial_reference (CORBA::ORB_ptr orb,
                const std::string& service_name,
                CORBA::Object_ptr object)
        throw (CORBA::SystemException)
        {
            try
            {
                orb->register_initial_reference (service_name.c_str(), object);
            }
            catch (const CORBA::ORB::InvalidName&)
            {
                throw CORBA::BAD_PARAM(BAD_PARAMInvalidServiceName,
                                       CORBA::COMPLETED_NO);
            }
            catch (const CORBA::SystemException &)
            {
                throw;
            }
        }

        //
        // Inform the JTC thread system of the creation of a new thread
        //

        ThreadAdoptionToken*
        OrbOrbacusImpl::adopt_thread()
        throw(OutOfMemoryException, InternalErrorException)

        {
            try
            {
                return new JTCAdoptionToken();
            }
            catch (const JTCSystemCallException &)
            {
                CDMW_THROW(InternalErrorException);
            }
            catch (const std::bad_alloc &)
            {
                CDMW_THROW(OutOfMemoryException);
            }
        }




        //
        // Transform an exception into a human readable string
        //
        std::string
        OrbOrbacusImpl::exception_to_string (const CORBA::Exception& ex )
        throw (OutOfMemoryException)
        {
            try
            {
                CORBA::String_var s = ex._to_string();
                return s.in();
            }
            catch ( ... )
            {
                CDMW_THROW(OutOfMemoryException);
            }
        }

        //
        // Transform an exception into a human readable string
        //
        std::string
        OrbOrbacusImpl::exception_name (const CORBA::SystemException& ex )
        throw (OutOfMemoryException)
        {
            try
            {
                return ex._name();
            }
            catch ( ... )
            {
                CDMW_THROW( OutOfMemoryException );
            }

        }





        //
        // Add to the PolicyList the policy to enable thread pool capability
        //
        void
        OrbOrbacusImpl::add_policy_thread_pool (
            CORBA::PolicyList& policies,
            OBPortableServer::POA_ptr obPOA,
            OB::DispatchStrategyFactory_ptr dispatchStrategyFactory,
            std::size_t nbThreads)
        throw (CORBA::INTERNAL,
               CORBA::NO_MEMORY)
        {

            OB::DispatchStrategy_var ds_thread_pool;
            OB::ThreadPoolId thread_pool_id;

            try
            {
                // We create a thread pool of the right size
                thread_pool_id = dispatchStrategyFactory->create_thread_pool (nbThreads);

                // And we create the associated ORBacus strategy
                ds_thread_pool
                = dispatchStrategyFactory->create_thread_pool_strategy(thread_pool_id);

                // We increase it size to insert ours
                std::size_t previousLength = policies.length();
                policies.length (previousLength + 1);
                policies[previousLength] =
                    obPOA->create_dispatch_strategy_policy(ds_thread_pool);

            }
            catch (const std::bad_alloc &)
            {
                throw CORBA::NO_MEMORY(NO_MEMORY, CORBA::COMPLETED_NO);
            }
            catch ( CORBA::NO_IMPLEMENT& ex )
            {
                throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
            }

        }


        //
        // Add to the PolicyList the policy to enable threaded  capability
        //
        void
        OrbOrbacusImpl::add_policy_thread_per_request (
            CORBA::PolicyList& policies,
            OBPortableServer::POA_ptr obPOA,
            OB::DispatchStrategyFactory_ptr dispatchStrategyFactory)

        throw (CORBA::INTERNAL,
               CORBA::NO_MEMORY)
        {

            OB::DispatchStrategy_var ds_threaded;

            try
            {
                // We create the strategy one thread per request
                ds_threaded = dispatchStrategyFactory->create_thread_per_request_strategy();

                // We increase it size to insert ours
                std::size_t previousLength = policies.length();
                policies.length(previousLength + 1);
                policies[previousLength] =
                    obPOA->create_dispatch_strategy_policy( ds_threaded );
            }
            catch (const std::bad_alloc &)
            {
                throw CORBA::NO_MEMORY(NO_MEMORY, CORBA::COMPLETED_NO);
            }
            catch (CORBA::NO_IMPLEMENT& ex)
            {
                throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
            }
        }



        //
        // Analyse all Cdmw Strategy and call the appropriate functions to
        //    initialise ORBacus
        //
        void
        OrbOrbacusImpl::transform_strategy_into_policy (
            CORBA::PolicyList& policies,
            OBPortableServer::POA_ptr obPOA,
            const StrategyList& strategyList
        )
        throw (CORBA::BAD_INV_ORDER,
               CORBA::INTERNAL,
               CORBA::NO_MEMORY )
        {
            if (!M_init_called)
                throw CORBA::BAD_INV_ORDER(BAD_INV_ORDERInitNotCalled,
                                           CORBA::COMPLETED_NO);

            // WARNING: here we do not check for policy consistency, it must have been done
            //  in check_poa_polices_matches_orb_policies

            // Number of threads for the thread pool, if necessary
            std::size_t nbThreads;

            // Do we use a thread pool for the POA ?
            if (strategyList.is_PoaThreadPool (nbThreads))
            {

                add_policy_thread_pool (policies, obPOA,
                                        M_dispatchStrategyFactory.in(), nbThreads);
            }

            // Do we use one thread per request ?
            if (strategyList.is_PoaThreadPerRequest())
            {
                add_policy_thread_per_request (policies, obPOA,
                                               M_dispatchStrategyFactory.in());
            }
        }




        //
        // Check if the policies are consistent with the orb and between them
        //
        // void
        // OrbOrbacusImpl::check_poa_polices_matches_orb_policies( const StrategyList& orb_strategyList,
        //                                                         const StrategyList& poa_strategyList)
        //     throw(  BadParameterException )
        // {

        //     // TODO: currently, we force the ORB to be multi-threaded, but later, we must
        //     //  ensure than if a multi-thread policy ist set for the POA, the ORB is also
        //     //  multi-threaded

        //     // First at least we must have one of the POA policies
        //     std::size_t nbThreads;
        //     if ( ! poa_strategyList.is_PoaThreadPool( nbThreads ) && ! poa_strategyList.is_PoaThreadPerRequest() ) {

        //         CDMW_THROW2( BadParameterException, "strategyList", "No POA strategy defined");

        //     }

        //     if ( poa_strategyList.is_PoaThreadPool( nbThreads ) && poa_strategyList.is_PoaThreadPerRequest() ) {

        //         CDMW_THROW2( BadParameterException, "strategyList", "Incompatible POA strategy set");

        //     }

        // }


        //
        // Clean up
        //
        void
        OrbOrbacusImpl::ORB_cleanup (CORBA::ORB_ptr orb)
        throw (CORBA::BAD_INV_ORDER)
        {
            // do nothing
        }

        //
        // set an endpoint selector factory
        //
        void                
        OrbOrbacusImpl::set_endpoint_selector_factory (const std::string& endpoint_selector_factory)
        {
            // do nothing
        }
           

    } // End namespace OrbSupport
} // End namespace Cdmw


#endif //CDMW_ORB_VDR == orbacus




