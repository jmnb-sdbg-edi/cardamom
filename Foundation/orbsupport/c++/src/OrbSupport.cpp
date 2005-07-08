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


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "orbsupport/ExceptionStrings.hpp"

#if CDMW_ORB_VDR == orbacus

#include "orbsupport/OrbOrbacusImpl.hpp"

#elif CDMW_ORB_VDR == tao

#include "orbsupport/OrbTaoImpl.hpp"

#endif

#include <sstream>
#include <string>

#define MAKE_EXCEPTION_TO_STRING(T) \
    std::string T##_to_string(const CORBA::##T & ex) \
    throw (Cdmw::OutOfMemoryException)  \
    { \
        return system_exception_string(#T, \
                                       ex.minor(), \
                                       ex.completed(), \
                                       Cdmw::OrbSupport::ExceptionStrings::##T##_reasons); \
    }

/*
#define CHECK_AND_RETURN_SYSTEM_EXCEPTION(T) \
            (ex_name == #T) \
            do { \
                const CORBA::##T * sys_ex \
                    = CORBA::##T::_downcast(&ex); \
                return T##_to_string(*sys_ex); \
            } while(0)
*/ 
// -----
#define CHECK_AND_RETURN_SYSTEM_EXCEPTION(T) \
    (ex_name == #T) \
    do { \
        CORBA::##T * sys_ex \
        = CORBA::##T::_downcast( (CORBA::Exception*)&ex ); \
        return T##_to_string(*sys_ex); \
    } while(0)
// -----



#if HAS_CORBA_EXCEPTION_OSTREAM_OPERATOR_DEFINITION == 0

//
// Exception output on std::ostream
//
std::ostream &operator<<(std::ostream& os, const CORBA::Exception& ex)
{
    try
    {
        os << Cdmw::OrbSupport::OrbSupport::exception_to_string(ex);
    }
    catch (...)
    {
        // ignore ...
    }

    return os;
}

#endif

namespace
{
    // VMCID (Vendor Minor Codeset ID) mask
    const CORBA::ULong VMCID_MASK = 0xfffff000;
    const CORBA::ULong MINOR_CODE_MASK = 0x00000fff;

    const long MIN_PORT_NUMBER = 0;
    const long MAX_PORT_NUMBER = 65535;

    std::string system_exception_string(const std::string & exception_name,
                                        CORBA::ULong minor_code,
                                        CORBA::CompletionStatus completion_status,
                                        const char* messages[])
    throw(Cdmw::OutOfMemoryException)
    {
        try
        {
            std::ostringstream s;
            s << "System exception `" << exception_name << "'" << "\n";
            s << "Completed: ";

            switch (completion_status)
            {

                    case CORBA::COMPLETED_YES:
                    s << "yes";
                    break;

                    case CORBA::COMPLETED_NO:
                    s << "no";
                    break;

                    case CORBA::COMPLETED_MAYBE:
                    s << "maybe";
                    break;
            }

            s << "\n";
            s << "Minor code: " << std::hex
            << (unsigned long)minor_code << std::dec << "\n";

            s << "(" << messages[minor_code & MINOR_CODE_MASK] << ")";

            return s.str();
        }
        catch (const std::bad_alloc & )
        {
            CDMW_THROW(Cdmw::OutOfMemoryException);
        }
    }

    using namespace CORBA;

    std::string BAD_INV_ORDER_to_string(const CORBA::BAD_INV_ORDER & ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("BAD_INV_ORDER",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::BAD_INV_ORDER_reasons);
    }

    std::string BAD_PARAM_to_string(const CORBA::BAD_PARAM & ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("BAD_PARAM",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::BAD_PARAM_reasons);
    }

    std::string INTERNAL_to_string(const CORBA::INTERNAL & ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("INTERNAL",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::INTERNAL_reasons);
    }

    std::string NO_IMPLEMENT_to_string(const CORBA::NO_IMPLEMENT & ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("NO_IMPLEMENT",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::NO_IMPLEMENT_reasons);
    }

    std::string NO_MEMORY_to_string(const CORBA::NO_MEMORY & ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("NO_MEMORY",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::NO_MEMORY_reasons);
    }

    std::string NO_PERMISSION_to_string(const CORBA::NO_PERMISSION & ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("NO_PERMISSION",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::NO_PERMISSION_reasons);
    }

    std::string NO_RESOURCES_to_string(const CORBA:: NO_RESOURCES& ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("NO_RESOURCES",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::NO_RESOURCES_reasons);
    }

    std::string OBJECT_NOT_EXIST_to_string(const CORBA:: OBJECT_NOT_EXIST& ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("OBJECT_NOT_EXIST",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::OBJECT_NOT_EXIST_reasons);
    }

    std::string TRANSIENT_to_string(const CORBA::TRANSIENT & ex)
    throw (Cdmw::OutOfMemoryException)
    {
        return system_exception_string("TRANSIENT",
                                       ex.minor(),
                                       ex.completed(),
                                       Cdmw::OrbSupport::ExceptionStrings::TRANSIENT_reasons);
    }



    //   MAKE_EXCEPTION_TO_STRING(BAD_INV_ORDER)
    //     MAKE_EXCEPTION_TO_STRING(BAD_PARAM)
    //     MAKE_EXCEPTION_TO_STRING(INTERNAL)
    //     MAKE_EXCEPTION_TO_STRING(NO_IMPLEMENT)
    //     MAKE_EXCEPTION_TO_STRING(NO_MEMORY)
    //     MAKE_EXCEPTION_TO_STRING(NO_PERMISSION)
    //     MAKE_EXCEPTION_TO_STRING(NO_RESOURCES)
    //     MAKE_EXCEPTION_TO_STRING(OBJECT_NOT_EXIST)
    //     MAKE_EXCEPTION_TO_STRING(TRANSIENT)
}

; // End anonymous namespace

namespace Cdmw
{

    namespace OrbSupport
    {

        const char* CDMW_ROOT_POA_NAME = "CDMW_RootPOA";

#if CDMW_ORB_VDR == orbacus

        typedef OrbOrbacusImpl UnderlyingORB;

#elif CDMW_ORB_VDR == tao

        typedef OrbTaoImpl UnderlyingORB;

#endif

        /**
        *Implementation notes: none 
        *<p>
        *Portability issues: none
        *<p>
        */

        //
        // Returns an initiliased ORB
        //
        CORBA::ORB_ptr
        OrbSupport::ORB_init (int& argc, char** argv,
                              const StrategyList& strategylist,
                              const char* orb_identifier)
        throw (CORBA::SystemException)
        {
            // Is CDMW Process port number specified?
            // Check Options::PROCESS_PORT_OPTION in argv for the value of the port number

            using namespace Cdmw::OsSupport;

            using namespace Cdmw::Common;
            std::string root_poa_port_option;

            try
            {
                root_poa_port_option = OS::get_option_value(argc, argv,
                                       Options::LOCALISATION_SERVICE_OPTION);
            }
            catch (const OutOfMemoryException &)
            {
                throw CORBA::NO_MEMORY(NO_MEMORY, CORBA::COMPLETED_NO);
            }
            catch (const InternalErrorException &)
            {
                throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
            }


            std::size_t root_poa_port_number(0);

            if (root_poa_port_option != "no")
            {
                if (root_poa_port_option != "yes")
                {
                    long port;
                    std::istringstream strPoaPort(root_poa_port_option);
                    strPoaPort >> port;

                    if (strPoaPort.fail() ||
                            !(port > MIN_PORT_NUMBER && port < MAX_PORT_NUMBER))
                    {
                        throw CORBA::BAD_PARAM(BAD_PARAMInvalidPortNumber, CORBA::COMPLETED_NO);
                    }
                    else
                        root_poa_port_number = port;
                }
                else
                {
                    throw CORBA::BAD_PARAM(BAD_PARAMInvalidPortNumber, CORBA::COMPLETED_NO);
                }
            } // else ignore.

            std::string cdmw_root_poa_port_option;

            try
            {
                cdmw_root_poa_port_option = OS::get_option_value (argc, argv,
                                             Options::PROCESS_PORT_OPTION);
            }
            catch (const OutOfMemoryException &)
            {
                throw CORBA::NO_MEMORY(NO_MEMORY, CORBA::COMPLETED_NO);
            }
            catch (const InternalErrorException &)
            {
                throw CORBA::INTERNAL(INTERNAL, CORBA::COMPLETED_NO);
            }


            std::size_t cdmw_root_poa_port_number(0);

            if (cdmw_root_poa_port_option != "no")
            {
                if (cdmw_root_poa_port_option != "yes")
                {
                    long port;
                    std::istringstream strPoaPort(cdmw_root_poa_port_option);
                    strPoaPort >> port;

                    if (strPoaPort.fail() ||
                            !(port > MIN_PORT_NUMBER && port < MAX_PORT_NUMBER))
                    {
                        throw CORBA::BAD_PARAM(BAD_PARAMInvalidPortNumber, CORBA::COMPLETED_NO);
                    }
                    else
                        cdmw_root_poa_port_number = port;
                }
                else
                {
                    throw CORBA::BAD_PARAM(BAD_PARAMInvalidPortNumber, CORBA::COMPLETED_NO);
                }
            } // else ignore.




            return UnderlyingORB::ORB_init (argc, argv, strategylist,
                                            root_poa_port_number,
                                            cdmw_root_poa_port_number,
                                            orb_identifier);
        }


        //
        // Create a POA, child of the specified POA, with the right
        //    policies to respect the Cdmw Strategies.
        //
        PortableServer::POA_ptr
        OrbSupport::create_POA (PortableServer::POA_ptr parentPOA,
                                const char* adapter_name,
                                PortableServer::POAManager_ptr a_POAManager,
                                const CORBA::PolicyList& policies,
                                const StrategyList& strategyList)
        throw (PortableServer::POA::AdapterAlreadyExists,
               PortableServer::POA::InvalidPolicy,
               CORBA::SystemException)
        {
            return UnderlyingORB::create_POA(parentPOA, adapter_name, a_POAManager, policies,
                                             strategyList);
        }



        //
        // Make the current thread adopted by the underlying thread
        //    system
        //
        ThreadAdoptionToken*
        OrbSupport::adopt_thread()
        throw(OutOfMemoryException, InternalErrorException)
        {
#if CDMW_ORB_VDR == orbacus

            return UnderlyingORB::adopt_thread();

#else

            return NULL;

#endif
        }



        //
        // Transform a CORBA excpetion in a human readable format
        //
        std::string
        OrbSupport::exception_to_string(const CORBA::Exception& ex )
        throw(OutOfMemoryException)
        {
            /*
            // Is it a system exception?
            const CORBA::SystemException* sys 
            = CORBA::SystemException::_downcast(&ex);
            */

            // -----
            // Is it a system exception?
            const CORBA::SystemException* sys
            = CORBA::SystemException::_downcast( (CORBA::Exception*) & ex );
            // -----

            if (sys)
                return system_exception_to_string(*sys);
            else
                return UnderlyingORB::exception_to_string( ex );
        }

        std::string
        OrbSupport::system_exception_to_string(const CORBA::SystemException& ex)
        throw(OutOfMemoryException)
        {
            try
            {
                CORBA::ULong minor_code = ex.minor();

                if ((minor_code & VMCID_MASK) == CDMW_VMCID)
                {
                    // This is a CDMW system exception.
                    std::string ex_name = UnderlyingORB::exception_name(ex);
                    //             if CHECK_AND_RETURN_SYSTEM_EXCEPTION(BAD_INV_ORDER);
                    //             else if CHECK_AND_RETURN_SYSTEM_EXCEPTION(BAD_PARAM);
                    //             else if CHECK_AND_RETURN_SYSTEM_EXCEPTION(INTERNAL);
                    //             else if CHECK_AND_RETURN_SYSTEM_EXCEPTION(NO_IMPLEMENT);
                    //             else if CHECK_AND_RETURN_SYSTEM_EXCEPTION(NO_MEMORY);
                    //             else if CHECK_AND_RETURN_SYSTEM_EXCEPTION(NO_PERMISSION);
                    //             else if CHECK_AND_RETURN_SYSTEM_EXCEPTION(NO_RESOURCES);
                    //             else if CHECK_AND_RETURN_SYSTEM_EXCEPTION(OBJECT_NOT_EXIST);
                    //             else if CHECK_AND_RETURN_SYSTEM_EXCEPTION(TRANSIENT);
                    //             else
                    //                 return "";


                    if (ex_name == "BAD_INV_ORDER")
                        do
                        {
                            CORBA::BAD_INV_ORDER * sys_ex \
                            = CORBA::BAD_INV_ORDER::_downcast( (CORBA::Exception*) & ex );
                            return BAD_INV_ORDER_to_string(*sys_ex);
                        }
                        while (0);
                    else if (ex_name == "BAD_PARAM")
                        do
                        {
                            CORBA::BAD_PARAM * sys_ex
                            = CORBA::BAD_PARAM::_downcast( (CORBA::Exception*) & ex );
                            return BAD_PARAM_to_string(*sys_ex);
                        }
                        while (0);
                    else if (ex_name == "INTERNAL")
                        do
                        {
                            CORBA::INTERNAL * sys_ex
                            = CORBA::INTERNAL::_downcast( (CORBA::Exception*) & ex );
                            return INTERNAL_to_string(*sys_ex);
                        }
                        while (0);
                    else if (ex_name == "NO_IMPLEMENT")
                        do
                        {
                            CORBA::NO_IMPLEMENT * sys_ex
                            = CORBA::NO_IMPLEMENT::_downcast( (CORBA::Exception*) & ex );
                            return NO_IMPLEMENT_to_string(*sys_ex);
                        }
                        while (0);
                    else if (ex_name == "NO_MEMORY")
                        do
                        {
                            CORBA::NO_MEMORY * sys_ex
                            = CORBA::NO_MEMORY::_downcast( (CORBA::Exception*) & ex );
                            return NO_MEMORY_to_string(*sys_ex);
                        }
                        while (0);
                    else if (ex_name == "NO_PERMISSION")
                        do
                        {
                            CORBA::NO_PERMISSION * sys_ex
                            = CORBA::NO_PERMISSION::_downcast( (CORBA::Exception*) & ex );
                            return NO_PERMISSION_to_string(*sys_ex);
                        }
                        while (0);
                    else if (ex_name == "NO_RESOURCES")
                        do
                        {
                            CORBA::NO_RESOURCES * sys_ex
                            = CORBA::NO_RESOURCES::_downcast( (CORBA::Exception*) & ex );
                            return NO_RESOURCES_to_string(*sys_ex);
                        }
                        while (0);
                    else if (ex_name == "OBJECT_NOT_EXIST")
                        do
                        {
                            CORBA::OBJECT_NOT_EXIST * sys_ex
                            = CORBA::OBJECT_NOT_EXIST::_downcast( (CORBA::Exception*) & ex );
                            return OBJECT_NOT_EXIST_to_string(*sys_ex);
                        }
                        while (0);
                    else if (ex_name == "TRANSIENT")
                        do
                        {
                            CORBA::TRANSIENT * sys_ex
                            = CORBA::TRANSIENT::_downcast( (CORBA::Exception*) & ex );
                            return TRANSIENT_to_string(*sys_ex);
                        }
                        while (0);
                    else
                        return "";


                }
                else
                    return UnderlyingORB::exception_to_string( ex );
            }
            catch (const OutOfMemoryException & )
            {
                throw;
            }
            catch (const std::bad_alloc & )
            {
                CDMW_THROW(OutOfMemoryException);
            }
        }



        void
        OrbSupport::bind_object_to_corbaloc (CORBA::ORB_ptr orb,
                                             const std::string& corbaloc_name,
                                             CORBA::Object_ptr object)
        throw (CORBA::SystemException)
        {
            UnderlyingORB::bind_object_to_corbaloc (orb, corbaloc_name, object);
        }

        void
        OrbSupport::remove_corbaloc_binding (CORBA::ORB_ptr orb,
                                             const std::string& corbaloc_name)
        throw (CORBA::SystemException)
        {
            UnderlyingORB::remove_corbaloc_binding (orb, corbaloc_name);
        }


        void
        OrbSupport::register_initial_reference (CORBA::ORB_ptr orb,
                                                const std::string& service_name,
                                                CORBA::Object_ptr object)
        throw (CORBA::SystemException)
        {
            UnderlyingORB::register_initial_reference (orb, service_name, object);
        }


        void
        OrbSupport::ORB_cleanup(CORBA::ORB_ptr orb)
        throw(CORBA::BAD_INV_ORDER)
        {
            UnderlyingORB::ORB_cleanup(orb);
        }
        
        void                
        OrbSupport::set_endpoint_selector_factory (const std::string& endpoint_selector_factory)
        {
            UnderlyingORB::set_endpoint_selector_factory(endpoint_selector_factory);
        }
        
        
        std::string
        OrbSupport::get_repository_id_from_ior(CORBA::ORB_ptr orb, CORBA::Object_ptr object)
            throw( CORBA::SystemException )
        {
            return UnderlyingORB::get_repository_id_from_ior(orb, object);
        }
        
    } // End namespace OrbSupport
} // End namespace Cdmw

