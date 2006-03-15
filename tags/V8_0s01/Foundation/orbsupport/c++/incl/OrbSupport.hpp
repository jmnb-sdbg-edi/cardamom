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


#ifndef INCL_ORBSUPPORT_ORBSUPPORT_HPP
#define INCL_ORBSUPPORT_ORBSUPPORT_HPP

#include <memory>
#include <string>

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/ThreadAdoptionToken.hpp"
#include "Foundation/common/Exception.hpp"

#include <iostream>


namespace Cdmw
{

    namespace OrbSupport
    {

        /**
         * Name of the CDMW POA root of all CDMW-specific POAs
         */
        extern const char* CDMW_ROOT_POA_NAME;

        /**
        *Purpose:
        *<p>    This class provide a wrapping of the ORB dependent
        * capability.
        *
        *Features:
        *<p> Thread safe
        *
        */

        class OrbSupport
        {
                // TODO: For tests only...

                OrbSupport()
                {}

                ~OrbSupport()
                {}

            public:

                /**
                 * Purpose:
                 * <p> Creates a new ORB instance, initialised with the
                 * specified CDMW StrategyList. This method calls the
                 * underlying ORB specific ORB_init, so the semantic is as
                 * defined by CORBA.
                 * 
                 *@param argc The number of parameter received of the command line
                 *@param argv The list of parameter received of the command line
                 *@param strategyList A list of CDMW strategies 
                 *@param orb_identifier Optional ORB identifier
                 *
                 *@return A valid reference to an ORB pseudo-object
                 *
                 *@exception CORBA::BAD_PARAM Invalid parameter. The following CDMW minor codes 
                 * detail the reason: 
                 * <UL>
                 * <LI><B>BAD_PARAMMissingORBThreadingStrategy</B> when the multithread strategy
                 * is not set for the ORB, or
                 * <LI><B>BAD_PARAMPOAMutltiThreadStrategyRequired</B> when the required POA
                 * multithread is not set.
                 * </UL>
                 *@exception CORBA::NO_MEMORY The CDMW <B>NO_MEMORY</B> minor code indicates
                 * that the underlying ORB failed to allocate memory.
                 *@exception CORBA::INTERNAL  The CDMW <B>INTERNAL</B> minor code indicates an
                 * internal error occured within OrbSupport library.
                 *@exception CORBA::SystemException Any CORBA system exception that
                 * may be raised by the underlying ORB.
                 */
                static
                CORBA::ORB_ptr
                ORB_init (int& argc, char** argv,
                          const StrategyList& strategylist,
                          const char* orb_identifier = "")
                throw(CORBA::SystemException);

                /**
                 * Purpose:
                 * <p> Creates a POA child of the specified parentPOA. The
                 *  created POA is initialised with the specified policies
                 *  and with others Policy, created according to the
                 *  specified CDMW StrategyList.
                 * 
                 *@param parentPOA The parent POA of the POA to be created
                 *@param adapter_name Name of the new POA
                 *@param a_POAManager Manager of the POA
                 *@param policies List of policies to use to create the POA
                 *@param strategyList List of CDMW Strategies
                 *
                 *@return A reference to the created POA
                 *
                 *@exception PortableServer::POA::AdapterAlreadyExists A POA with the same
                 * name is already created by parentPOA.
                 *@exception PortableServer::POA::InvalidPolicy Supplied policies are not
                 * compatible.
                 *@exception CORBA::BAD_PARAM Invalid parameter. The following CDMW minor codes 
                 * detail the reason: 
                 * <UL>
                 * <LI><B>BAD_PARAMNilObjectReference</B> when a nil object reference is supplied
                 * for as parentPOA, or
                 * <LI><B>BAD_PARAMUnknownPOAType</B> when the POA is not created by the
                 * underlying ORB.
                 * </UL>
                 *@exception CORBA::BAD_INV_ORDER The CDMW <B>BAD_INV_ORDERInitNotCalled</B>
                 * indicates that ORB_init operation was never called.
                 *@exception CORBA::NO_MEMORY The CDMW <B>NO_MEMORY</B> minor code indicates
                 * that the underlying ORB failed to allocate memory.
                 *@exception CORBA::INTERNAL  The CDMW <B>INTERNAL</B> minor code indicates an
                 * internal error occured within OrbSupport library.
                 *@exception CORBA::SystemException Any CORBA system exception that
                 * may be raised by the underlying ORB.
                 *
                 *@see StrategyList
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
                 * <p> Binds the specified object reference to a name reachable by corbaloc URLs.
                 * 
                 *@param orb The ORB pseudo-object
                 *@param corbaloc_name Name to be assigned to reach the object
                 *@param object Object reference to bind to the corbaloc_name
                 *
                 *@exception CORBA::BAD_PARAM Invalid parameter. The CDMW
                 * <B>BAD_PARAMCorbalocBindingAlreadyExists</B> minor code indicates that
                 * an object reference has already been bound to the specified name.
                 *@exception CORBA::SystemException Any CORBA system exception that
                 * may be raised by the underlying ORB.
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
                 * <p> Removes a corbaloc binding.
                 * 
                 *@param orb The ORB pseudo-object
                 *@param corbaloc_name Name of the binding to be removed.
                 *
                 *@exception CORBA::BAD_PARAM Invalid parameter. The CDMW
                 * <B>BAD_PARAMCorbalocBindingNotFound</B> minor code indicates that
                 * there is no object reference bound to the specified name.
                 *@exception CORBA::SystemException Any CORBA system exception that
                 * may be raised by the underlying ORB.
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
                 *@param orb The ORB pseudo-object
                 *@param service_name Name to be assigned as service to reach the object
                 *@param object Object reference to associate to service name
                 *
                 *@exception CORBA::BAD_PARAM Invalid parameter. The CDMW
                 * <B>BAD_PARAMCorbalocBindingAlreadyExists</B> minor code indicates that
                 * an object reference has already been bound to the specified name.
                 *@exception CORBA::SystemException Any CORBA system exception that
                 * may be raised by the underlying ORB.
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
                 * <p> Informs the underlying thread management system of the
                 *    creation of a new thread. This method must be called 
                 *    from any new thread created by third-party library.
                 *    This method must not be called from the main thread.
                 *
                 *@return A token which destructor is responsible for unregistering this thread.
                 * 
                 *@exception Cdmw::OutOfMemoryException Could not allocate memory.
                 *@exception Cdmw::InternalErrorException Failure within the ORB thread library
                 * (failure of system calls ... etc.)
                 */
                static
                ThreadAdoptionToken*
                adopt_thread()
                throw(OutOfMemoryException, InternalErrorException);

                /**
                 * Purpose:
                 * <p> Translate a CORBA::Exception into a string in
                 *  a human readable format.
                 * 
                 *@param ex The CORBA exception in question
                 *
                 *@return A string in a human readable format
                 *
                 *@exception OutOfMemoryException Could not allocate memory.
                 */
                static
                std::string
                exception_to_string(const CORBA::Exception& ex)
                throw(OutOfMemoryException);


                /**
                 * Purpose:
                 * <p> Clean up the orb 
                 * 
                 *@param orb The ORB pseudo-object
                 *
                 *@return
                 *
                 *@exception
                 */
                static
                void
                ORB_cleanup(CORBA::ORB_ptr orb)
                throw(CORBA::BAD_INV_ORDER);


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

               /**
                * Purpose:
                * <p> get the repository_id from the object reference
                * 
                *@param object the object
                *
                *@return the repository_id
                *
                *@exception
                */           
                static
                std::string
                get_repository_id_from_ior(CORBA::ORB_ptr orb, CORBA::Object_ptr object)
                    throw( CORBA::SystemException );

            protected:

            private:
                /**
                 * Purpose:
                 * <p> Translate a CORBA::SystemException into a string in
                 *  a human readable format.
                 * 
                 *@param ex The CORBA system exception in question
                 *
                 *@return A string in a human readable format
                 *
                 *@exception OutOfMemoryException Could not allocate memory.
                 */
                static
                std::string
                system_exception_to_string(const CORBA::SystemException& ex)
                throw(OutOfMemoryException);


        }

        ; // End class OrbSupport

    } // End namespace OrbSupport
} // End namespace Cdmw

#if HAS_CORBA_EXCEPTION_OSTREAM_OPERATOR_DECLARATION == 0

//
// Exception output on std::ostream
//
std::ostream &operator<<(std::ostream&, const CORBA::Exception&);

#endif

#endif // INCL_ORBSUPPORT_ORBSUPPORT_HPP

