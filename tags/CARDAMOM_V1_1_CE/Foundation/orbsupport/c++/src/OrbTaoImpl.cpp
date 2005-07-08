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


#include "orbsupport/OrbTaoImpl.hpp"

#if CDMW_ORB_VDR == tao


#include <sstream>
#include <ace/Service_Config.h>

#include <tao/TAO_Internal.h>
#include <tao/Client_Strategy_Factory.h>
#include <tao/Server_Strategy_Factory.h>
#include <tao/IORTable/IORTable.h>
#include "tao/ORB_Core.h"

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/ossupport/OS.hpp"



namespace Cdmw
{

    namespace OrbSupport
    {


        /**
        *Implementation notes: 
        *<p>
        *Portability issues: none 
        *<p>
        */



        RTCORBA::RTORB_var OrbTaoImpl::rt_orb(RTCORBA::RTORB::_nil());
        RTCORBA::Current_var OrbTaoImpl::current(RTCORBA::Current::_nil());

        std::map<CORBA::ORB_ptr, OrbTaoThreadPool*> OrbTaoImpl::OrbToThreadPool;

        int OrbTaoImpl::M_nbThreads = 0;

        OrbTaoThreadPool::OrbTaoThreadPool (CORBA::ORB_ptr orb)
                : nbThreads(0), orb_(CORBA::ORB::_duplicate(orb))
        {}

        ;

        OrbTaoThreadPool::~OrbTaoThreadPool() throw()
        {}

        ;

        int OrbTaoThreadPool::svc()
        {
            // increase the number of threads
            ++nbThreads;
            orb_->run();
            // decrease the number of threads
            --nbThreads;
            return 0;
        };


        OrbTaoThreadPool* OrbTaoImpl::orbTaoThreadPool = NULL;

        //
        // Initialises the Tao ORB
        //
        CORBA::ORB_ptr
        OrbTaoImpl::ORB_init( int& argc, char** argv,
                              const StrategyList& strategyList,
                              std::size_t root_poa_port,
                              std::size_t cdmw_root_poa_port,
                              const char* orb_identifier )
        throw( CORBA::SystemException )
        {

            // Initialise service repository with signal set to SIGINT instead of default SIGHUP
            // to allow the process terminates when its father terminates
            ACE_Service_Config service_config (1,
                                               ACE_Service_Config::MAX_SERVICES,
                                               SIGUSR1);




            CORBA::ORB_var theOrb = CORBA::ORB::_nil();
            size_t nbThreads = 0;

            try
            {
                // At this time is ORB SHALL be multi-threaded

                if ( !strategyList.is_OrbThreaded() )
                {
                    throw CORBA::BAD_PARAM(
                        BAD_PARAMMissingORBThreadingStrategy,
                        CORBA::COMPLETED_NO );
                }

                // Initialisation of the 3 catogories of definitions to put
                // in default_svc_conf_entries 
                std::string resourceFactoryDefs = "";
                std::string serverStrategyFactoryDefs = "";
                std::string clientStrategyFactoryDefs = "";

                if (strategyList.is_multicast())
                {
                  resourceFactoryDefs = "dynamic UIPMC_Factory Service_Object * TAO_PortableGroup:_make_TAO_UIPMC_Protocol_Factory() \"\"\nstatic Resource_Factory \"-ORBProtocolFactory IIOP_Factory -ORBProtocolFactory UIPMC_Factory\"";
                  // REM1: i am not sure in which category PortableGroup_Loader must be
                  // but in fact the 3 categories are processed the same way
                  // inside ORB_init : via a ACE_Service_Config::process_directive
                  // REM2: we could also replace the 2 "dynamic.."  by a process_directive
                  // because these initialisations can be done immediatly. However,
                  // "static Resource_Factory" line can be processed only inside ORB_init
                  serverStrategyFactoryDefs = "dynamic PortableGroup_Loader Service_Object * TAO_PortableGroup:_make_TAO_PortableGroup_Loader() \"\"";
                }


                // Do we use a thread pool for the POAs ?
                if (strategyList.is_PoaThreadPool (nbThreads))
                {
                    serverStrategyFactoryDefs = serverStrategyFactoryDefs +
                        "\nstatic Server_Strategy_Factory \""
                        "-ORBConcurrency                  reactive "
                        "-ORBPOALock                      thread "
                        "-ORBAllowReactivationOfSystemids 1\"";

                    clientStrategyFactoryDefs = // clientStrategyFactoryDefs + "\n"
                        "static Client_Strategy_Factory \""
                        "-ORBProfileLock                  thread "
                        "-ORBClientConnectionHandler      MT\"";                    
                }
                else if (strategyList.is_PoaThreadPerRequest ())
                {
                    serverStrategyFactoryDefs = serverStrategyFactoryDefs +
                        "\nstatic Server_Strategy_Factory \""
                        "-ORBConcurrency                  reactive "
                        "-ORBPOALock                      thread "
                        "-ORBAllowReactivationOfSystemids 1\"";

                    clientStrategyFactoryDefs = // clientStrategyFactoryDefs + "\n"
                        "static Client_Strategy_Factory \"" 
                        "-ORBProfileLock                  thread "
                        "-ORBClientConnectionHandler      MT\"";
                }
                else if (strategyList.is_PoaThreadPerConnection ())
                {
                    serverStrategyFactoryDefs = serverStrategyFactoryDefs +
                        "\nstatic Server_Strategy_Factory \""
                        "-ORBConcurrency                  thread-per-connection "
                        "-ORBPOALock                      thread "
                        "-ORBAllowReactivationOfSystemids 1\"";

                    clientStrategyFactoryDefs = // clientStrategyFactoryDefs + "\n"
                        "static Client_Strategy_Factory \""
                        "-ORBProfileLock                  thread "
                        "-ORBClientConnectionHandler      MT\"";
                }

                // This call set definitions that will be processed only inside ORB_init
                // If this method has been called previously, the definitions are overidden
                TAO_Internal::default_svc_conf_entries(
                    resourceFactoryDefs.c_str(),
                    serverStrategyFactoryDefs.c_str(),
                    clientStrategyFactoryDefs.c_str() );


                // We must ensure to be listening on a host and a port known to the client
                std::string str_opt( "" );

                char** argv_tmp = NULL;

                int argc_tmp = argc;

                for (int i = 0; i < argc; i++)
                    str_opt += std::string( argv[i] ) + " ";

                str_opt += std::string( "-ORBDottedDecimalAddresses 1 " );

                argc_tmp += 2;

                // We add both root_poa_port and cdmw_root_poa_port, while TAO is able to
                // create as many POAManager listening on two ports defined at ORB init with
                // the ORBEndpoint option.
                // The pb with Orbaccus was to define an other POA manager with a well_known
                // port!
                if (root_poa_port != 0)
                {
                    std::ostringstream root_poa_endpoint;
                    root_poa_endpoint << "iiop://:" << root_poa_port << std::ends;

                    str_opt += std::string( "-ORBEndpoint ") + root_poa_endpoint.str() + " ";

                    argc_tmp += 2;
                }

                if (cdmw_root_poa_port != 0)
                {
                    std::ostringstream cdmw_root_poa_endpoint;
                    cdmw_root_poa_endpoint << "iiop://:" << cdmw_root_poa_port << std::ends;

                    str_opt += std::string( "-ORBEndpoint ")
                               + cdmw_root_poa_endpoint.str() + " ";
                    argc_tmp += 2;
                }

                argv_tmp = Cdmw::Common::String::to_char_array(
                               Cdmw::Common::String::to_strings( str_opt ) );

                // call the TAO ORB_init

                theOrb = CORBA::ORB_init( argc_tmp, argv_tmp, orb_identifier );


                for ( int i = 0; i < argc_tmp; ++i )
                    delete[] argv_tmp[i];

                delete[] argv_tmp;

                // We don't need to create a specific CdmwPOAManager while the same port
                // specified with ORBEndpoint option is used for every POA manager.
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

            // 4.6 ORB Initialisation : The OrbId

            // The identifier for the ORB will be a name of type CORBA::ORBid. All ORBid
            // strings other than the empty string are allocated by ORB administrators and are not
            // managed by the OMG. ORBid strings other than the empty string are intended to be
            // used to uniquely identify each ORB used within the same address space in a multi-
            // ORB application. These special ORBid strings are specific to each ORB
            // implementation and the ORB administrator is responsible for ensuring that the names
            // are unambiguous.
            // If an empty ORBid string is passed to ORB_init, then the arg_list arguments shall be
            // examined to determine if they indicate an ORB reference that should be returned. This
            // is achieved by searching the arg_list parameters for one preceded by -ORBid for
            // example, -ORBid example_orb (the white space after the -ORBid tag is
            // ignored) or -ORBidMyFavoriteORB (with no white space following the -ORBid
            // tag). Alternatively, two sequential parameters with the first being the string -ORBid
            // indicates that the second is to be treated as an ORBid parameter. If an empty string is
            // passed and no arg_list parameters indicate the ORB reference to be returned, the
            // default ORB for the environment will be returned.


            // if the ord reference return by the ORB_init is presented in the list, we return the reference whitout do anything
            std::map <CORBA::ORB_ptr, OrbTaoThreadPool*>::iterator i = OrbToThreadPool.find(theOrb.in());

            if (i != OrbToThreadPool.end())
            {
                return (theOrb._retn());
            }

            //else we register the reference in the list with a new OrbTaoThreadPool object (the OrbTaoThreadPool allows the creation of threadpool
            orbTaoThreadPool = new OrbTaoThreadPool(theOrb.in());

            OrbToThreadPool[theOrb.in()] = orbTaoThreadPool;



            // Do we use a thread pool for the ORBs ?
            if (strategyList.is_PoaThreadPool (nbThreads))
            {
                // Must be checked by the StrategyList
                CDMW_ASSERT(nbThreads > 0);
                // we decrease the number of thread because the user invoke an orb->run in his man program
                M_nbThreads = nbThreads - 1;

                if (nbThreads > 0)
                {
                    // creation and activation of the threads for the orb->run method

                    if (orbTaoThreadPool->activate(THR_NEW_LWP , M_nbThreads) != 0)
                        CDMW_THROW(InternalErrorException);
                }

                CORBA::Object_var object_rt_orb
                = theOrb->resolve_initial_references("RTORB");
                rt_orb = RTCORBA::RTORB::_narrow(object_rt_orb.in());

                CORBA::Object_var object_current
                = theOrb->resolve_initial_references("RTCurrent");
                current = RTCORBA::Current::_narrow(object_current.in());
            }
            else if (strategyList.is_PoaThreadPerRequest ())
            {

                // Must be checked by the StrategyList
                M_nbThreads = 10;

                // creation and activation of the threads for the orb->run method

                if (orbTaoThreadPool->activate(THR_NEW_LWP , M_nbThreads) != 0)
                    CDMW_THROW(InternalErrorException);

                CORBA::Object_var object_rt_orb
                = theOrb->resolve_initial_references("RTORB");

                rt_orb = RTCORBA::RTORB::_narrow(object_rt_orb.in());

                CORBA::Object_var object_current
                = theOrb->resolve_initial_references("RTCurrent");

                current = RTCORBA::Current::_narrow(object_current.in());
            }
            else if (strategyList.is_PoaThreadPerConnection ())
            {

                CORBA::Object_var object_rt_orb
                = theOrb->resolve_initial_references("RTORB");
                rt_orb = RTCORBA::RTORB::_narrow(object_rt_orb.in());

                CORBA::Object_var object_current
                = theOrb->resolve_initial_references("RTCurrent");
                current = RTCORBA::Current::_narrow(object_current.in());
            }

            return theOrb._retn();
        }

        //
        // Create a new TAO POA
        //
        PortableServer::POA_ptr
        OrbTaoImpl::create_POA( PortableServer::POA_ptr parentPOA,
                                const char* adapter_name,
                                PortableServer::POAManager_ptr a_POAManager,
                                const CORBA::PolicyList& policies,
                                const StrategyList& strategyList )
        throw( PortableServer::POA::AdapterAlreadyExists,
               PortableServer::POA::InvalidPolicy,
               CORBA::SystemException )
        {
            // Check that parentPOA is not a nil object reference

            if ( CORBA::is_nil( parentPOA ) )
            {
                throw CORBA::BAD_PARAM( BAD_PARAMNilObjectReference,
                                        CORBA::COMPLETED_NO );
            }


            // We make a copy of the user policies

            PortableServer::POAManager_var the_POAManager
            = PortableServer::POAManager::_duplicate(a_POAManager);

            CORBA::PolicyList TAOpolicies(policies);

            size_t nbThreads = 0;

            // Do we use a thread pool for the POAs ?
            if (strategyList.is_PoaThreadPool( nbThreads ))
            {
                // configure the threadpool parameter
                CORBA::ULong stacksize = 30000;
                CORBA::Boolean allow_request_buffering = 0;
                CORBA::ULong max_buffered_requests = 0;
                CORBA::ULong max_request_buffer_size = 0;

                // obtain the priority in the current thread
                RTCORBA::Priority default_thread_priority = current->the_priority();

                RTCORBA::ThreadpoolId threadpool_id
                = rt_orb->create_threadpool(stacksize,
                                            nbThreads,
                                            0,
                                            default_thread_priority,
                                            allow_request_buffering,
                                            max_buffered_requests,
                                            max_request_buffer_size);

                CORBA::Policy_var threadpool_policy_1
                = rt_orb->create_threadpool_policy(threadpool_id);

                TAOpolicies.length (TAOpolicies.length () + 2);


                TAOpolicies[TAOpolicies.length() - 2]
                = rt_orb->create_priority_model_policy(RTCORBA::CLIENT_PROPAGATED,
                                                       default_thread_priority);
                TAOpolicies[TAOpolicies.length() - 1]
                = CORBA::Policy::_duplicate(threadpool_policy_1.in());
            }
            else if (strategyList.is_PoaThreadPerRequest())
            {

                nbThreads = 10;
                CORBA::ULong stacksize = 30000;
                CORBA::Boolean allow_request_buffering = 0;
                CORBA::ULong max_buffered_requests = 0;
                CORBA::ULong max_request_buffer_size = 0;
                RTCORBA::Priority default_thread_priority = current->the_priority();

                RTCORBA::ThreadpoolId threadpool_id
                = rt_orb->create_threadpool (stacksize,
                                             nbThreads,
                                             0,
                                             default_thread_priority,
                                             allow_request_buffering,
                                             max_buffered_requests,
                                             max_request_buffer_size);

                CORBA::Policy_var threadpool_policy_1
                = rt_orb->create_threadpool_policy(threadpool_id);

                TAOpolicies.length(TAOpolicies.length () + 2);

                TAOpolicies[TAOpolicies.length() - 1]
                = rt_orb->create_priority_model_policy(RTCORBA::CLIENT_PROPAGATED,
                                                       default_thread_priority);
                TAOpolicies[TAOpolicies.length() - 2]
                = CORBA::Policy::_duplicate(threadpool_policy_1.in());
            }


            // We have inserted our proprietary policies, we create the POA
            return parentPOA->create_POA(adapter_name, the_POAManager.in(), TAOpolicies);

        }




        //
        // Attach a servant to a corbaloc reachable object
        //
        void
        OrbTaoImpl::bind_object_to_corbaloc( CORBA::ORB_ptr orb,
                                             const std::string& corbaloc_name,
                                             CORBA::Object_ptr object )
        throw( CORBA::SystemException )
        {
            CORBA::Object_var iorTableObj =
                orb->resolve_initial_references( "IORTable" );

            IORTable::Table_var iorTable
            = IORTable::Table::_narrow( iorTableObj.in() );

            // Are we not using a specific version of Tao ?
            CDMW_ASSERT( !CORBA::is_nil( iorTable.in() ) );

            // We register our object

            try
            {
                CORBA::String_var ior_string = orb->object_to_string( object );

                iorTable->bind( corbaloc_name.c_str(), ior_string.in() );
            }
            catch ( const IORTable::AlreadyBound & )
            {
                throw CORBA::BAD_PARAM(
                    BAD_PARAMCorbalocBindingAlreadyExists,
                    CORBA::COMPLETED_NO );
            }
            catch ( const CORBA::SystemException & )
            {
                throw;
            }
        }

        void
        OrbTaoImpl::remove_corbaloc_binding( CORBA::ORB_ptr orb,
                                             const std::string& corbaloc_name )
        throw( CORBA::SystemException )
        {
            // The Id must be defined by us
            CORBA::Object_var obj =
                orb->string_to_object( corbaloc_name.c_str() );

            CORBA::Object_var iorTableObj =
                orb->resolve_initial_references( "IORTable" );

            IORTable::Table_var iorTable
            = IORTable::Table::_narrow( iorTableObj.in() );

            // Are we not using a specific version of Tao ?
            CDMW_ASSERT( !CORBA::is_nil( iorTable.in() ) );

            // We register our object

            try
            {
                CORBA::String_var ior_string
                = orb->object_to_string( obj.in() );

                iorTable->unbind( ior_string.in() );
            }
            catch ( const IORTable::NotFound & )
            {
                throw CORBA::BAD_PARAM(
                    BAD_PARAMCorbalocBindingNotFound,
                    CORBA::COMPLETED_NO );
            }
            catch ( const CORBA::SystemException & )
            {
                throw;
            }
        }


        //
        // Attach a servant to a service reachable object (by resolve_initial_reference)
        //
        void
        OrbTaoImpl::register_initial_reference (CORBA::ORB_ptr orb,
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
        // Transform an exception into a human readable string
        // WARNING: This function should return a maximum of information on the exception!
        //
        std::string
        OrbTaoImpl::exception_to_string (const CORBA::Exception& ex )
        throw (OutOfMemoryException)
        {
            try
            {
                return ex._info().c_str();
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
        OrbTaoImpl::exception_name (const CORBA::SystemException& ex )
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
        // Clean up the threadpool create for the threadpool policies
        //
        void
        OrbTaoImpl::ORB_cleanup (CORBA::ORB_ptr orb)
        throw (CORBA::BAD_INV_ORDER)
        {
            // shutdown the orb to wait the threads completions
            // BAD_INV_ORDER minor code     explanation
            //               1              Dependency exists in IFR preventing destruction of this object
            //             2              Attempt to destroy indestructible objects in IFR
            //             3              Operation would deadlock
            //             4              ORB has shutdown

            try
            {
                orb->shutdown(false);
            }
            catch (const CORBA::BAD_INV_ORDER & ex)
            {
#if (TAO_MINOR_VERSION <3)

                if (ex.minor() != (TAO_OMG_VMCID | 4))
                    throw ;

#else

                if (ex.minor() != (CORBA::OMGVMCID | 4))
                    throw ;

#endif

            }
            catch ( ... )
            {}

            // wait the threads completion
            while (OrbToThreadPool[orb]->nbThreads != 0)
            {}

            // delete the OrbToThreadPool
            delete OrbToThreadPool[orb];

            //  erase the orb reference on the list
            OrbToThreadPool.erase(orb);

        }


        //
        // set an endpoint selector factory
        //
        void                
        OrbTaoImpl::set_endpoint_selector_factory (const std::string& endpoint_selector_factory)
        {
            TAO_ORB_Core::set_endpoint_selector_factory (endpoint_selector_factory.c_str());
        }

        //
        // get the repository_id from the object reference
        //
        std::string
        OrbTaoImpl::get_repository_id_from_ior(CORBA::ORB_ptr orb, CORBA::Object_ptr object)
            throw( CORBA::SystemException )
        {
            char* str;
            // Strip the IOR: off the string.
            ACE_CString aString = orb->object_to_string(object);
            ACE_CString prefix = "IOR:";
            short prefixLength = prefix.length ();
            
            ACE_CString subString =
            aString.substring (prefixLength,
                               aString.length () - prefixLength);
            subString[subString.length ()] = '\0';
            str = subString.rep ();
            
            ACE_Message_Block mb (ACE_OS::strlen ((char *) str)  / 2 + 1
                                  + ACE_CDR::MAX_ALIGNMENT);
            ACE_CDR::mb_align (&mb);
            char *buffer = mb.rd_ptr ();
            char *tmp = (char *) str;
            size_t len = 0;
            
            
            // The prefix of the IOR must be removed, and the string must start
            // with the encapsulation byte
            
            while (tmp [0] && tmp [1])
            {
                u_char byte;
                
                if (! (isxdigit (tmp [0]) && isxdigit (tmp [1])))
                    break;
                
                byte = (u_char) (ACE::hex2byte (tmp [0]) << 4);
                byte |= ACE::hex2byte (tmp [1]);
                
                buffer [len++] = byte;
                tmp += 2;
            }
            
            // Create deencapsulation stream ... then unmarshal objref from that
            // stream.
            int byteOrder = *(mb.rd_ptr ());
            mb.rd_ptr (1);
            mb.wr_ptr (2 * len - 1);
            TAO_InputCDR stream (&mb, ACE_static_cast(int,byteOrder));
            char* type_hint;
            
            if (!(stream >> type_hint))
            {
                type_hint = "";   
            }  
            return type_hint;
            
        }
        

    } // End namespace OrbSupport
} // End namespace Cdmw


#endif //CDMW_ORB_VDR == tao


