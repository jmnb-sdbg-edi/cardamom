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


#ifndef INCL_ORBSUPPORT_ORBORBACUSIMPL_HPP
#define INCL_ORBSUPPORT_ORBORBACUSIMPL_HPP

#if CDMW_ORB_VDR == orbacus


#include <OB/CORBA.h>

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/ThreadAdoptionToken.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Purpose:
        *<p>    This class implement the OrbSuport required
        *    capability for the OrBacus ORB
        *
        *Features:
        *<p> Thread safe
        *
        *@see  OrbSupport 
        *
        */

        class OrbOrbacusImpl
        {

            public:


                /**
                 * Purpose:
                 * <p> initialises the ORB
                 * 
                 *@see OrbSupport::ORB_init
                 *
                 */
                static
                CORBA::ORB_ptr
                ORB_init (int& argc, char** argv,
                          const StrategyList& strategylist,
                          std::size_t root_poa_port = 0,
                          std::size_t cdmw_root_poa_port = 0,
                          const char* orb_identifier = "")
                throw (CORBA::SystemException);

                /**
                 * Purpose:
                 * <p> Creates a new POA
                 * 
                 *@see OrbSupport::create_POA
                 *
                 */
                static
                PortableServer::POA_ptr
                create_POA (PortableServer::POA_ptr parentPOA,
                            const char* adapter_name,
                            PortableServer::POAManager_ptr a_POAManager,
                            const CORBA::PolicyList& policies,
                            const StrategyList& strategyList)
                throw (PortableServer::POA::AdapterAlreadyExists,
                       PortableServer::POA::InvalidPolicy,
                       CORBA::SystemException);

                /**
                 * Purpose:
                 * <p> Inform the thread system of a newly created thread
                 * 
                 *@see OrbSupport::adopt_thread
                 *
                 */
                static
                ThreadAdoptionToken*
                adopt_thread()
                throw(OutOfMemoryException, InternalErrorException);

                /**
                 * Purpose:
                 * <p> Attach a CORBA object to a corbaloc reachable reference
                 * 
                 *@see OrbSupport::bind_object_to_corbaloc
                 *
                 */
                static
                void
                bind_object_to_corbaloc (CORBA::ORB_ptr orb,
                                         const std::string& corbaloc_name,
                                         CORBA::Object_ptr object)
                throw (CORBA::SystemException);

                /**
                 * Purpose:
                 * <p> Remove a corbaloc binding
                 * 
                 *@see OrbSupport::remove_corbaloc_binding
                 *
                 */
                static
                void
                remove_corbaloc_binding (CORBA::ORB_ptr orb,
                                         const std::string& corbaloc_name)
                throw (CORBA::SystemException);


                /**
                 * Purpose:
                * <p> Register the object reference as specified service 
                *     to be reachable with "resolve_initial_reference" api.
                *
                 * 
                 *@see OrbSupport::register_initial_reference
                 *
                 */
                static
                void
                register_initial_reference (CORBA::ORB_ptr orb,
                                            const std::string& service_name,
                                            CORBA::Object_ptr object)
                throw (CORBA::SystemException);


                /**
                 * Purpose:
                 * <p> transform an exception into a human readable format
                 * 
                 *@see OrbSupport::exception_to_string
                 *
                 */
                static
                std::string
                exception_to_string (const CORBA::Exception& ex )
                throw (OutOfMemoryException);


                /**
                 * Purpose:
                 * <p> Return the name of a CORBA system exception
                 * 
                 *@param ex A CORBA system exception
                 *
                 *@return Name of the name of a CORBA system exception
                 *
                 *@exception OutOfMemoryException Failed to allocate memory
                 *
                 */
                static
                std::string
                exception_name (const CORBA::SystemException& ex)
                throw (OutOfMemoryException);


                /**
                 * Purpose:
                 * <p> Clean up (do nothing with Orbacus)
                 * 
                 *@param 
                 *
                 *@return 
                 *
                 *@exception 
                 *
                 */
                static
                void
                ORB_cleanup (CORBA::ORB_ptr orb)
                throw (CORBA::BAD_INV_ORDER);

                /**
                 * Purpose:
                 * <p> set an endpoint selector factory
                 * 
                 *@param name of the endpoint selector factory
                 *
                 *@return
                 *
                 *@exception
                 */
                static
                void                
                set_endpoint_selector_factory (const std::string& endpoint_selector_factory);

            protected:

            private:

                static bool M_init_called;
                static Cdmw::OsSupport::Mutex M_mutex;
                static OB::DispatchStrategyFactory_var M_dispatchStrategyFactory;
                static PortableServer::POAManager_var M_CdmwPOAManager;


                /**
                 * Purpose:
                 * <p> Add to the PolicyList the Policy required
                 * to create a POA with a thread pool
                 * 
                 *@param policies List of policies to be updated
                 *    to create a POA with thread pool
                 *@param obPOA The Orbacus Parent POA
                 *@param dispatchStrategyFactory A reference to the Dispatch Strategy factory
                 *@param nbThreads The size of the thread pool
                 *
                 *@exception CORBA::INTERNAL Failure within OrbSupport library
                 *@exception CORBA::NO_MEMORY Could not allocate memory
                 *
                 */
                static
                void
                add_policy_thread_pool (
                    CORBA::PolicyList& policies,
                    OBPortableServer::POA_ptr obPOA,
                    OB::DispatchStrategyFactory_ptr dispatchStrategyFactory,
                    size_t nbThreads)
                throw (CORBA::INTERNAL,
                       CORBA::NO_MEMORY);
                /**
                 * Purpose:
                 * <p> Add to the PolicyList the Policy required
                 * to create a POA with a thread per request
                 * 
                 *@param policies List of policies to be updated
                 *    to create a POA with thread pool
                 *@param obPOA The Orbacus Parent POA
                 *@param dispatchStrategyFactory A reference to the Dispatch Strategy factory
                 *
                 *@exception CORBA::INTERNAL Failure within OrbSupport library
                 *@exception CORBA::NO_MEMORY Could not allocate memory
                 *
                 */
                static
                void
                add_policy_thread_per_request (
                    CORBA::PolicyList& policies,
                    OBPortableServer::POA_ptr obPOA,
                    OB::DispatchStrategyFactory_ptr dispatchStrategyFactory)
                throw (CORBA::INTERNAL,
                       CORBA::NO_MEMORY);

                /**
                 * Purpose:
                 * <p> Add to the PolicyList Policies required
                 * to create a POA matching the CDMW StrategyList
                 * 
                 *@param policies List of policy to be updated
                 *@param obPOA The Orbacus Parent POA
                 *@param strategyList CDMW StrategyList specifying the POA behavior
                 *
                 *@exception CORBA::BAD_INV_ORDER The ORB_init operation was never called.
                 *@exception CORBA::INTERNAL Failure within OrbSupport library
                 *@exception CORBA::NO_MEMORY Could not allocate memory
                 *
                 */
                static
                void
                transform_strategy_into_policy (
                    CORBA::PolicyList& policies,
                    OBPortableServer::POA_ptr obPOA,
                    const StrategyList& strategyList
                )
                throw (CORBA::BAD_INV_ORDER,
                       CORBA::INTERNAL,
                       CORBA::NO_MEMORY) ;

                //     /**
                //      * Purpose:
                //      * <p> check if a set of policies defining for the ORB
                //      * matches policies the user wants to use to create a POA.
                //      * This function also checks if policy are consistent.
                //      *
                //      *@param orb_strategyList policies for the ORB
                //      *@param poa_strategyList policies for the POA
                //      *
                //      *@exception BadParameterException
                //      *
                //      */
                //     static
                //     void
                //     check_poa_polices_matches_orb_policies(
                //                                            const StrategyList& orb_strategyList,
                //                                            const StrategyList& poa_strategyList)
                //         throw(  BadParameterException );



        }

        ; // End class OrbOrbacusImpl

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif //CDMW_ORB_VDR == orbacus

#endif // INCL_ORBSUPPORT_ORBORBACUSIMPL_HPP

