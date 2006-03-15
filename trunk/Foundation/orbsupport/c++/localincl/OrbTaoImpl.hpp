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


#ifndef INCL_ORBSUPPORT_ORBTAOIMPL_HPP
#define INCL_ORBSUPPORT_ORBTAOIMPL_HPP

#if CDMW_ORB_VDR == tao


#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include <vector>
#include <map>
#include <ace/Task.h>
//#include <tao/RTCORBA/RTCORBA.h>
//#include <tao/RTCORBA/Thread_Pool.h>
#include <tao/Version.h>

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Purpose:
        *<p>    This class allows create threadpool
        *
        *Features:
        *<p> Thread safe
        *
         
        */

        class OrbTaoThreadPool : public ACE_Task_Base
        {

            public:
                OrbTaoThreadPool (CORBA::ORB_ptr orb);

                ~OrbTaoThreadPool() throw();

                /**
                 * Function called by the activate
                 */
                virtual int svc();

                /**
                 * Number of threads launched whith the orb_->run
                 */
                int nbThreads;

            private:

                /**
                 * orb.
                 */

                CORBA::ORB_var orb_;

                /**
                 * Mutex protecting nbThreads modifications
                 */
                Cdmw::OsSupport::Mutex m_nbThreadsMutex;
                
        };




        /**
        *Purpose:
        *<p>    This class implement the OrbSuport required
        *    capability for the TAO ORB
        *
        *Features:
        *<p> Thread safe
        *
        *@see  OrbSupport 
        *
        */

        class OrbTaoImpl
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
                 * <p> Clean up all thread and destroy the orb
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


                //static RTCORBA::RTORB_var rt_orb;
                //static RTCORBA::Current_var current;



                static bool M_init_called;
                static Cdmw::OsSupport::Mutex M_mutex;
                static PortableServer::POAManager_var M_CdmwPOAManager;

                static int M_nbThreads;
                static OrbTaoThreadPool* orbTaoThreadPool;
                static std::map <CORBA::ORB_ptr, OrbTaoThreadPool*> OrbToThreadPool;


        }

        ; // End class OrbTaoImpl

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif //CDMW_ORB_VDR == tao

#endif // INCL_ORBSUPPORT_ORBTAOIMPL_HPP

