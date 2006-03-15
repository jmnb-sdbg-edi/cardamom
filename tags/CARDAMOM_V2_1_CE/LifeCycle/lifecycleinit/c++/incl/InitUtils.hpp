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


#ifndef INCL_LIFECYCLE_INIT_UTIL_HPP
#define INCL_LIFECYCLE_INIT_UTIL_HPP

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "LifeCycle/lifecycle/Dictionary.hpp"
#include "LifeCycle/lifecycle/FactoryAllocator.hpp"
#include "Foundation/orbsupport/POAThreadingPolicy.hpp"
#include "LifeCycle/lifecycle/ErrorMessages.hpp"
#include "LifeCycle/lifecycle/Dictionary.hpp"
#include "LifeCycle/lifecycleinit/FactoryAllocationGuard.hpp"
#include "LifeCycle/lifecycleinit/StandardObjectRegistration.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <string>
#include <typeinfo>

namespace
{    

    /**
     * Name of the POA root of CDMW LifeCycle framework
     */
    static const char*  FACTORIES_POA_NAME = "FactoriesPOA";   

    /**
     * Default Thread pool size of the CDMW factories POA (above)
     */
    const size_t FACTORIES_POA_THREAD_POOL_SIZE = 5;

    /**
     * CDMW Factories Servant Activator
     */
    class CDMW_init_ServantActivator 
        : public virtual PortableServer::ServantActivator,
          public virtual Cdmw::OrbSupport::RefCountLocalObject
    {
    public:
        CDMW_init_ServantActivator () 
        {
//                std::cerr << "CDMW_init_ServantActivator ()" << std::endl;
        }
        ~CDMW_init_ServantActivator () 
        {
//                std::cerr << "~CDMW_init_ServantActivator ()" << std::endl;
        }
        virtual PortableServer::Servant 
        incarnate(const PortableServer::ObjectId& oid, 
                  PortableServer::POA_ptr poa)
            throw(PortableServer::ForwardRequest, CORBA::SystemException)
        {
            // Should not happen (at this stage!)
            CDMW_ASSERT(0); 
            return 0; // TODO: Maybe raise CORBA::TRANSIENT()
        }

        /**
         * The main purpose of this operation is to provide a hook
         * that will be called by the ORB during its shutdown phase
         * to etherealize CDMW factories objects.
         */ 
        virtual void 
        etherealize(const PortableServer::ObjectId& oid,
                    PortableServer::POA_ptr poa,
                    PortableServer::Servant servant,
                    CORBA::Boolean cleanup_in_progress,
                    CORBA::Boolean remaining_activations)
            throw(CORBA::SystemException)
        {
            try {
                CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                using namespace Cdmw::LifeCycle;
                typedef Dictionary<FactoryAllocatorBase*> FDictionary;
                std::cout << /*ECHO <<*/ "CLEANUP of " << s.in() 
                          << " <" << typeid(*FDictionary::Get()[s.in()]).name() << ">" 
                          << std::endl;
                FDictionary::Get()[s.in()]->dispose(servant);                    
            } catch (...) {
                CDMW_ASSERT(0); // Shouldn't happen
            }
        }
    };

}; // End anonymous namespace

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW LifeCycle Frameworks types and data.
*/
namespace LifeCycle
{

/**
*Purpose:
*<p> This function format a string to a string suitable for use
* as an ObjectId.
*@param s Input string
*
*@return A properly formatted string
*
*/
inline static
std::string format_name_for_ObjectId(const std::string & s)
    throw ()
{
    std::string result(s);
    std::string::iterator it = result.begin();
    while (it != result.end()) {
        if (*it == '/') *it = '_';
        ++it;
    }
    return result;
}


/**
*Purpose:
*<p> This class provides functions needed during CDMW process initialisation
* to create and register CDMW named objects in the CDMW Naming and Repository.
*
*Features:
*
*/
class NamedObjectsFactory_initUtil
{
    // Should not be instantiated!
    NamedObjectsFactory_initUtil();
public:

    /**
     * This function creates/activates a named objects CDMW factory.
     * INPUT DATA:
     *   [I]    - A reference to the ORB pseudo-object
     *   [II]   - Parent POA for the factory
     *   [III]  - Reference of the CDMW Repository
     *   [IV]   - Default Root Context of the CDMW Repository
     *   [V]    - Fully qualified name of the name domain where the factory
     *            will register objects it creates
     *   [VI]   - Fully qualified name of the CDMW factory
     *   [VII]  - Repository ID of the factory
     *   [VIII] - argc, argv
     *   [IX]   - Thrading issues : POA threading policy + thread pool size ... etc.
     */
    static void 
    setup_named_object_factory(NamedObjectsFactoryAllocatorBase &        allocator,
                               CORBA::ORB_ptr                            orb, 
                               PortableServer::POA_ptr                   parent,
                               CdmwNamingAndRepository::Repository_ptr  repository,
                               Cdmw::CommonSvcs::Naming::NamingInterface & root_context,
                               const std::string &                       fully_qualified_target_domain_name,
                               const std::string &                       fully_qualified_factory_name,
                               const std::string &                       factory_repository_id,
                               Cdmw::OrbSupport::POAThreadingPolicy &                      threading_policy,
                               int & argc, char** argv
                               )
        throw (Cdmw::OutOfMemoryException,
               Cdmw::BadParameterException,
               Cdmw::InternalErrorException,
               CORBA::SystemException)
    {
        CDMW_ASSERT(!CORBA::is_nil(parent)); 
        CDMW_ASSERT(!CORBA::is_nil(repository)); 
        
        // What do we need?
        // 1) NameDomain where the factory is/will be registered
        // 2) NameDomain where the factory will register objects it creates
        // 3) Name of the factory
        // 4) Parent POA for the factory
        // 5) A reference to the ORB pseudo-object
        // 6) argc, argv
        // 7) Add thrading issues : POA threading policy + thread pool size ... etc.
        using namespace Cdmw::LifeCycle;
        typedef Dictionary<FactoryAllocatorBase*> FDictionary;
//        typedef NamedObjectsFactoryAllocator<T>   Allocator;
        
        // TODO: ?
        // Use default name domain if 
        // CdmwNamingAndRepository::NoNameDomain, 
        // CdmwNamingAndRepository::InvalidName & CORBA::SystemException
        CdmwNamingAndRepository::NameDomain_var target_name_domain 
            = CdmwNamingAndRepository::NameDomain::_nil();
        try {
            target_name_domain
                = repository->resolve_name_domain(fully_qualified_target_domain_name.c_str());
        } catch (const CdmwNamingAndRepository::NoNameDomain & ) {
            std::string parameter(fully_qualified_target_domain_name);
            CDMW_THROW2( BadParameterException, parameter, ErrorMessages::NoNameDomainMessage);
        } catch (const CdmwNamingAndRepository::InvalidName & ) {
            std::string parameter(fully_qualified_target_domain_name);
            CDMW_THROW2( BadParameterException, parameter, ErrorMessages::InvalidNameMessage);
        } catch (const CORBA::SystemException & ) {
            throw;
        }
        
        // Register the name of the factory within its NameDomain. This
        // will check validity of the name and whether it is already in use.
        std::string factory_name; 
        std::string factory_name_domain_name;
        try {
            factory_name 
                = Cdmw::CommonSvcs::Naming::NamingInterface::basename
                (fully_qualified_factory_name);
            factory_name_domain_name
                = Cdmw::CommonSvcs::Naming::NamingInterface::dirname
                (fully_qualified_factory_name);
        } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException & ) {
            std::string parameter(fully_qualified_factory_name);
            CDMW_THROW2( BadParameterException, parameter, ErrorMessages::InvalidNameMessage);
        }
        
        // CDMW names should be within a name domain
        CDMW_ASSERT(!factory_name_domain_name.empty());
        
        CdmwNamingAndRepository::NameDomain_var factory_name_domain 
            = CdmwNamingAndRepository::NameDomain::_nil();
        try {
            factory_name_domain
                = repository->resolve_name_domain(factory_name_domain_name.c_str());
        } catch (const CdmwNamingAndRepository::NoNameDomain & ) {
            std::string parameter(factory_name_domain_name);
            CDMW_THROW2( BadParameterException, parameter, ErrorMessages::NoNameDomainMessage);
        } catch (const CdmwNamingAndRepository::InvalidName & ) {
            std::string parameter(factory_name_domain_name);
            CDMW_THROW2( BadParameterException, parameter, ErrorMessages::InvalidNameMessage);
        } catch (const CORBA::SystemException & ) {
            throw;
        }
        
        CdmwNamingAndRepository::NameDomain::RegistrationId_var reg_id;
        bool do_create_factory_reference = true;
        try {
            reg_id = factory_name_domain->new_name(factory_name.c_str()); 
        } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists &) {
            do_create_factory_reference = false;
        } catch (const CdmwNamingAndRepository::InvalidName &) {
            // The name should have been valid
            CDMW_ASSERT(false);
        } catch (const CORBA::OBJECT_NOT_EXIST &) {
            // Oops! CDMW Repository has been destroyed
            CDMW_THROW( InternalErrorException );
//            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryNotExists,
//                                  CORBA::COMPLETED_NO);
        } catch (const CORBA::SystemException &) {
            // Cannot reach the CDMW Repository at the moment, retry later!
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }

        // Create the object registration
        CdmwLifeCycle::ObjectRegistration_var object_registration =
          CdmwLifeCycle::ObjectRegistration::_nil();
        object_registration = new StandardObjectRegistration(target_name_domain.in());

        // TODO: If factory's name has been registered successfuly. Do not forget
        // to unregister it if an exception occurs. WORKAROUND: Use 
    // register_new_object() instead of new_name() and register_object().
        //Cdmw::LifeCycle::ObjectRegistrationGuard unreg(factory_name_domain.in(),
        //                                                factory_name);
	
        PortableServer::Servant fac = 
          allocator.allocate (orb,                               // ORB
                              parent,                            // PARENT POA
                              object_registration.in(),          // NameDomain reference
                              fully_qualified_target_domain_name,// Fully qualified name domain
                              fully_qualified_factory_name,      // Fully qualified factory name
                              threading_policy,                  // Threading Policy
                              argc, argv);

        // We add a guard object to deallocate the factory if an exception is raised.
        FactoryAllocationGuard allocGuard(allocator,fac);
        
        // Obtain object Id from fully_qualified_factory_name
        std::string oidstring = format_name_for_ObjectId(fully_qualified_factory_name);
        PortableServer::ObjectId_var oid 
            = PortableServer::string_to_ObjectId(oidstring.c_str());
        
        try {
            parent->activate_object_with_id(oid.in(),fac);
        } catch (const PortableServer::POA::ObjectAlreadyActive & ) {
            std::string parameter(fully_qualified_factory_name);
            CDMW_THROW2( BadParameterException, parameter, 
                ErrorMessages::FactoryExistsMessage);
        } catch (const PortableServer::POA::ServantAlreadyActive & ) {
            std::string parameter(fully_qualified_factory_name);
            CDMW_THROW2( BadParameterException, parameter, 
                ErrorMessages::FactoryInUseMessage);
        } catch (const PortableServer::POA::WrongPolicy & ) {
            CDMW_THROW( InternalErrorException );
        }

        // Register the factory instance within the local dictionary
        FDictionary::Get()[oidstring]= &allocator;
        
        if (do_create_factory_reference) { 
            // Register the object reference within the CDMW repository            
            try {
                CORBA::Object_var obj = parent->id_to_reference(oid.in());
            
                // Register Factory's object reference
                CdmwLifeCycle::FactoryBase_var fac_ref 
                    = CdmwLifeCycle::FactoryBase::_narrow(obj.in());
                CDMW_ASSERT(!CORBA::is_nil(fac_ref.in()));

                factory_name_domain->register_factory(reg_id.in(),fac_ref.in());
            } catch (const PortableServer::POA::ObjectNotActive & ) {
                // From id_to_reference
                CDMW_THROW( InternalErrorException );
            } catch (const PortableServer::POA::WrongPolicy & ) {
                // From id_to_reference
                CDMW_THROW( InternalErrorException );
            } catch (const CdmwNamingAndRepository::NameDomain::InvalidRegistration & ) {
                // From register_factory
                CDMW_THROW( InternalErrorException );
            } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists & ) {
                // From register_factory
                CDMW_THROW( InternalErrorException );
            } catch (const CORBA::SystemException & ) {
                throw;
            }
        }
        // Do not deallocate!
        allocGuard.cancel();
        //       We need de release a servant's reference at this stage since
        //       the POA holds one; but we need to see if the the factory allocation cannot
        //       deal with this.
        fac->_remove_ref();
    }

}; // End template NamedObjectsFactory_initUtil

/**
*Purpose:
*<p> This class provides functions needed during CDMW process initialisation
* to create CDMW anonymous objects.
*
*Features:
*
*/
class AnonymousObjectsFactory_initUtil
{
    // Should not be instantiated!
    AnonymousObjectsFactory_initUtil();
public:

    //---------------------------------------------------------------------
    /**
     * This function creates/activates a named objects CDMW factory.
     * INPUT DATA:
     *   [I]    - A reference to the ORB pseudo-object
     *   [II]   - Parent POA for the factory
     *   [III]  - Reference of the CDMW Repository
     *   [IV]   - Default Root Context of the CDMW Repository
     *   [V]    - Fully qualified name of the CDMW factory
     *   [VI]   - Repository ID of the factory
     *   [VII]  - argc, argv
     *   [VIII] - Thrading issues : POA threading policy + thread pool size ... etc.
     */
    static void 
    setup_anonymous_object_factory(AnonymousObjectsFactoryAllocatorBase &    allocator,
                                   CORBA::ORB_ptr                            orb, 
                                   PortableServer::POA_ptr                   parent,
                                   CdmwNamingAndRepository::Repository_ptr  repository,
                                   Cdmw::CommonSvcs::Naming::NamingInterface & root_context,
                                   const std::string &                       fully_qualified_factory_name,
                                   const std::string &                       factory_repository_id,
                                   Cdmw::OrbSupport::POAThreadingPolicy &                      threading_policy,
                                   int & argc, char** argv
                                   )
        throw (Cdmw::OutOfMemoryException,
               Cdmw::BadParameterException,
               Cdmw::InternalErrorException,
               CORBA::SystemException)
    {
        CDMW_ASSERT(!CORBA::is_nil(parent)); 
        CDMW_ASSERT(!CORBA::is_nil(repository)); 
        
        // What do we need?
        // 1) NameDomain where the factory is/will be registered
        // 2) Name of the factory
        // 3) Parent POA for the factory
        // 4) A reference to the ORB pseudo-object
        // 5) argc, argv
        // 6) Add thrading issues : POA threading policy + thread pool size ... etc.
        using namespace Cdmw::LifeCycle;
        typedef Dictionary<Cdmw::LifeCycle::FactoryAllocatorBase*> FDictionary;
//        typedef AnonymousObjectsFactoryAllocator<T>                 Allocator;
        
        // Use default name domain if no name domain is specified within the factory name
        
        // Register the name of the factory within its NameDomain. This
        // will check validity of the name and whether it is already in use.
        std::string factory_name; 
        std::string factory_name_domain_name;
        try {
            factory_name 
                = Cdmw::CommonSvcs::Naming::NamingInterface::basename
                (fully_qualified_factory_name);
            factory_name_domain_name
                = Cdmw::CommonSvcs::Naming::NamingInterface::dirname
                (fully_qualified_factory_name);
        } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException & ) {
            std::string parameter(fully_qualified_factory_name);
            CDMW_THROW2( BadParameterException, parameter, ErrorMessages::InvalidNameMessage);
        }
        
        // CDMW names should be within a name domain
        CDMW_ASSERT(!factory_name_domain_name.empty());
        
        CdmwNamingAndRepository::NameDomain_var factory_name_domain 
            = CdmwNamingAndRepository::NameDomain::_nil();
        try {
            factory_name_domain
                = repository->resolve_name_domain(factory_name_domain_name.c_str());
        } catch (const CdmwNamingAndRepository::NoNameDomain & ) {
            std::string parameter(factory_name_domain_name);
            CDMW_THROW2( BadParameterException, parameter, ErrorMessages::NoNameDomainMessage);
        } catch (const CdmwNamingAndRepository::InvalidName & ) {
            std::string parameter(factory_name_domain_name);
            CDMW_THROW2( BadParameterException, parameter, ErrorMessages::InvalidNameMessage);
        } catch (const CORBA::SystemException & ) {
            throw;
        }

        
        CdmwNamingAndRepository::NameDomain::RegistrationId_var reg_id;
        bool do_create_factory_reference = true;
        try {
            reg_id = factory_name_domain->new_name(factory_name.c_str()); 
        } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists &) {
            do_create_factory_reference = false;
        } catch (const CdmwNamingAndRepository::InvalidName &) {
            // The name should have been valid
            CDMW_ASSERT(false);
        } catch (const CORBA::OBJECT_NOT_EXIST &) {
            // Oops! CDMW Repository has been destroyed
            CDMW_THROW( InternalErrorException );
//            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryNotExists,
//                                  CORBA::COMPLETED_NO);
        } catch (const CORBA::SystemException &) {
            // Cannot reach the CDMW Repository at the moment, retry later!
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }
        
        // TODO: If factory's name has been registered successfuly. Do not forget
        // to unregister it if an exception occurs
        //Cdmw::LifeCycle::ObjectRegistrationGuard unreg(factory_name_domain.in(),
        //                                                factory_name);
        PortableServer::Servant fac = 
            allocator.allocate(orb,                         // ORB
                               parent,                      // PARENT POA
                               fully_qualified_factory_name,// Fully qualified factory name
                               threading_policy,            // Threading policy
                               argc, argv);

        // We add a guard object to deallocate the factory if an exception is raised.
        FactoryAllocationGuard allocGuard(allocator,fac);
    
        // Obtain object Id from fully_qualified_factory_name
        std::string oidstring = format_name_for_ObjectId(fully_qualified_factory_name);
        PortableServer::ObjectId_var oid 
            = PortableServer::string_to_ObjectId(oidstring.c_str());
        
        try {
            parent->activate_object_with_id(oid.in(),fac);
        } catch (const PortableServer::POA::ObjectAlreadyActive & ) {
            std::string parameter(fully_qualified_factory_name);
            CDMW_THROW2( BadParameterException, parameter, 
                ErrorMessages::FactoryExistsMessage);
        } catch (const PortableServer::POA::ServantAlreadyActive & ) {
            std::string parameter(fully_qualified_factory_name);
            CDMW_THROW2( BadParameterException, parameter, 
                ErrorMessages::FactoryInUseMessage);
        } catch (const PortableServer::POA::WrongPolicy & ) {
            CDMW_THROW( InternalErrorException );
        }
        
        // Register the factory instance within the local dictionary
        FDictionary::Get()[oidstring]= &allocator;
        
        if (do_create_factory_reference) { 
            // Register the object reference within the CDMW repository          
            try {
                CORBA::Object_var obj = parent->id_to_reference(oid.in());
            
                // Register Factory's object reference
                CdmwLifeCycle::FactoryBase_var fac_ref 
                    = CdmwLifeCycle::FactoryBase::_narrow(obj.in());
                CDMW_ASSERT(!CORBA::is_nil(fac_ref.in()));
                
                factory_name_domain->register_factory(reg_id.in(),fac_ref.in());
            } catch (const PortableServer::POA::ObjectNotActive & ) {
                // From id_to_reference
                CDMW_THROW( InternalErrorException );
            } catch (const PortableServer::POA::WrongPolicy & ) {
                // From id_to_reference
                CDMW_THROW( InternalErrorException );
            } catch (const CdmwNamingAndRepository::NameDomain::InvalidRegistration & ) {
                // From register_factory
                CDMW_THROW( InternalErrorException );
            } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists & ) {
                // From register_factory
                CDMW_THROW( InternalErrorException );
            } catch (const CORBA::SystemException & ) {
                throw;
            }      
        }
        // Do not deallocate!
        allocGuard.cancel();
        //       We need de release a servant's reference at this stage since
        //       the POA holds one; but we need to see if the the factory allocation cannot
        //       deal with this.
        fac->_remove_ref();
    }
}; // End template AnonymousObjectsFactory_initUtil

/**
 * Purpose:
 * <p> This class provides helper functions to be used by the CDMW generated
 * CdmwInterface.cpp file.
 *
 *@see CDMW_init
 *@see CDMW_cleanup
 */
class InitUtils
{
public:
static
PortableServer::POA_ptr
InitUtils::create_cdmw_LifeCycle_POA(PortableServer::POA_ptr parent)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
    CDMW_ASSERT(!CORBA::is_nil(parent));
    
    PortableServer::POA_var factories_poa = PortableServer::POA::_nil();
    
    // Create CDMW Factories POA (persistent POA)            
    using namespace PortableServer;
    try {  

        PortableServer::POAManager_var poaManager = parent -> the_POAManager();
        
        CORBA::PolicyList policies;
        policies.length(6);
        policies[0] = parent->create_id_assignment_policy(USER_ID);
        policies[1] = parent->create_lifespan_policy(PERSISTENT);
        policies[2] = parent->create_servant_retention_policy(RETAIN);
        policies[3] = parent->create_id_uniqueness_policy(UNIQUE_ID);
        policies[4] = parent->create_request_processing_policy(USE_SERVANT_MANAGER);
        policies[5] = parent->create_implicit_activation_policy(NO_IMPLICIT_ACTIVATION);
        
        Cdmw::OrbSupport::StrategyList poaStrategies;
#if CDMW_ORB_VDR == tao
        Cdmw::OrbSupport::ThreadPerConnectionPolicy threading;
#else
        Cdmw::OrbSupport::ThreadPoolPolicy threading(FACTORIES_POA_THREAD_POOL_SIZE);
#endif
        threading.append_to_StrategyList(poaStrategies);
        
        factories_poa = Cdmw::OrbSupport::OrbSupport::create_POA(parent,
                                                                  FACTORIES_POA_NAME,
                                                                  poaManager.in(), 
                                                                  policies,
                                                                  poaStrategies);
        //
        // Create and install servant activator
        //
        PortableServer::ServantActivator_var the_activator 
            = new CDMW_init_ServantActivator();
        
        CDMW_ASSERT(!CORBA::is_nil(the_activator.in()));
        
        factories_poa -> set_servant_manager(the_activator.in());
        
    } catch (const POA::AdapterAlreadyExists & ) {
        // Raised by create_POA
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMLifeCycleFrameworkPOAExists,
                                       CORBA::COMPLETED_NO) );
    } catch (const POA::InvalidPolicy & ) {
        // Raised by create_POA
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                      CORBA::COMPLETED_NO) );
    } catch (const POA::WrongPolicy & ) {
        // Raised by set_servant_manager
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                      CORBA::COMPLETED_NO) );
    } catch (const std::bad_alloc & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                      CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                       CORBA::COMPLETED_NO),
                      ex.what() );           
    } catch (const CORBA::SystemException & ex) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
        // TODO
    }
    return factories_poa._retn();
}
}; // End class InitUtils

}; // End namespace LifeCycle
}; // End namespace Cdmw

#endif // INCL_LIFECYCLE_CDMW_INIT_UTIL_HPP

