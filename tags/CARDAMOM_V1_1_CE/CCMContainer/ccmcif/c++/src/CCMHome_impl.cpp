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


#include <iostream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>

// Cdmw CCM Files
#include <CCMContainer/ccmcif/CCMHome_impl.hpp>
#include <CCMContainer/ccmcif/CCMObject_impl.hpp>
#include <CCMContainer/ccmcif/CCMUtils.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>

#include <CCMContainer/idllib/CdmwDeployment.stub.hpp>
#ifdef CDMW_USE_FAULTTOLERANCE
#include <FaultTolerance/idllib/FT.stub.hpp>
#endif

namespace {

    /**
     * Purpose:
     * <p>Read a config value
     *
     *@param name Name of the configuration data
     *@param config A sequence of all configuration values
     *@param value Out CORBA::Any parameter for holding the configuration value
     *
     *@return <B>true</B> if the configration value is found, <B>false</B> otherwise.
     */
    bool read_config_value(const std::string & name,
                           const Components::ConfigValues & config,
                           CORBA::Any_out value)
        throw()
    {
        bool found = false;
        CORBA::ULong len = config.length();
        for (CORBA::ULong count = 0; count < len; ++count)
            if (name.compare(config[count]->name()) == 0) 
            {
                value = new CORBA::Any(config[count]->value());
                found = true;
                break;
            }
        return found;
    }


} // anonymous namespace

namespace Cdmw {

namespace CCM {

namespace CIF {


//
// IDL:thalesgroup.com/CdmwCcmCif/CdmwCCMHome:1.0
//
CCMHome_impl::CCMHome_impl(const std::string&                         rep_id,
                           const std::string&                         comp_rep_id,
                           const Cdmw::LifeCycle::ObjectDeactivator&  deactivator,
                           const Components::ConfigValues&            config)
  throw(CORBA::SystemException)
  : LifeCycleSingleObject(deactivator),
    m_context(CdmwCcmContainer::CCM2Context::_nil()), 
    m_oid(deactivator.get_object_id()),
    m_rep_id(rep_id),
    m_comp_rep_id(comp_rep_id),
    m_create_ft_component(false)
{
   CORBA::Any_var ft_rep_style_any;
   bool ft_config_value_found = 
      read_config_value(CdmwDeployment::FAULT_TOLERANCE_REPLICATION_STYLE, config, ft_rep_style_any); 

#ifdef CDMW_USE_FAULTTOLERANCE
   if (ft_config_value_found) {
     CdmwDeployment::FTReplicationStyleValue ft_rep_style;
     if (ft_rep_style_any.in() >>= ft_rep_style) {       
       if (ft_rep_style == CdmwDeployment::WARM_PASSIVE)  {
	 m_create_ft_component = true;
       } else {
	 PRINT_ERROR("Unsupported FT Configuration values for FT home creation");
	 throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidConfigValues, 
				CORBA::COMPLETED_NO);
       }
     } else {
       PRINT_ERROR("Cannot read FT Configuration values for FT home creation");
       throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidConfigValues, 
                                  CORBA::COMPLETED_NO);
     }
   }
#else
   if (ft_config_value_found)
   {
       PRINT_ERROR("Unexpected FT Configuration values for non FT home creation");
       throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidConfigValues, 
                                  CORBA::COMPLETED_NO);
   }
#endif
   
}

CCMHome_impl::~CCMHome_impl()
  throw()
{
    PRINT_INFO("CCMHome_impl destructor called!");
}

//
// IDL:omg.org/Components/CCMHome/get_component_def:1.0
//
CORBA::IRObject_ptr
CCMHome_impl::get_component_def()
    throw(CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context.in()));

    // TODO: Implementation
    CORBA::IRObject_ptr _r = CORBA::IRObject::_nil();
    return _r;
}

//
// IDL:omg.org/Components/CCMHome/get_home_def:1.0
//
CORBA::IRObject_ptr
CCMHome_impl::get_home_def()
    throw(CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context.in()));

    // TODO: Implementation
    CORBA::IRObject_ptr _r = CORBA::IRObject::_nil();
    return _r;
}


//
// IDL:omg.org/Components/CCMHome/remove_component:1.0
//
void
CCMHome_impl::remove_component(Components::CCMObject_ptr comp)
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context.in()));

    if (CORBA::is_nil(comp))
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
    
    // check component context is a Session2Context one
    Components::Session2Context_var s2ctx 
        = Components::Session2Context::_narrow(m_context.in());
    
    if (!CORBA::is_nil(s2ctx.in()))
    {
        try
        {
            CORBA::OctetSeq_var  comp_oid = s2ctx->get_oid_from_ref(comp);
            std::string comp_oid_str = Cdmw::CCM::Common::OctetSeq_to_string(comp_oid.in());
            
            ComponentMap::iterator comp_iter = m_components.find(comp_oid_str);
            
            if (comp_iter != m_components.end())
            {
                try
                {
                    // ask to the component its removal
                    comp->remove();
                }
                catch (const Components::RemoveFailure&)
                {
                    PRINT_ERROR("Exception raised when removing component");
                    throw;
                }
                catch (const CORBA::OBJECT_NOT_EXIST&)
                {
                    PRINT_ERROR("This component is being removed!");
                    throw Components::RemoveFailure(CdmwDeployment::BAD_COMPONENT_REFERENCE);
                }
                catch (const CORBA::SystemException&)
                {
                    PRINT_ERROR("System exception raised when removing component");
                    throw;
                }
            }
            else
            {
                // this component has not been created by this home
                PRINT_ERROR("This component has not been created by this home or has already been removed!");
                throw Components::RemoveFailure(CdmwDeployment::BAD_COMPONENT_REFERENCE);
            }
        }
        catch (const Components::IllegalState& )
        {
            PRINT_ERROR("IllegalState exception raised!");
            throw Components::RemoveFailure(CdmwDeployment::ILLEGAL_STATE);
        }
        catch (const Components::BadComponentReference& )
        {
            PRINT_ERROR("BadComponentReference exception raised!");
            throw Components::RemoveFailure(CdmwDeployment::BAD_COMPONENT_REFERENCE);
        }
        catch (const CORBA::SystemException& ex)
        {
            PRINT_ERROR(ex); 
            throw;
        }
    }
    else
    {
        PRINT_ERROR("Context is not a Session2Context!");
        throw Components::RemoveFailure(CdmwDeployment::WRONG_CONTEXT_KIND);
    }
}

//
// IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
//
void 
CCMHome_impl::remove()
throw(CosLifeCycle::NotRemovable,
      CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context.in()));

    try 
    {
        ComponentMap::iterator comp_iter = m_components.begin();
            
        while (comp_iter != m_components.end())
        {
            ComponentInfo comp_info = comp_iter->second;
                
            try
            {
                Components::CCMObject_var ccm_obj 
                   = Components::CCMObject::_narrow(comp_info.comp_ref.in());
                    
                ccm_obj->remove();
            } 
            catch (const Components::RemoveFailure&)
            {
                // Ignore
                PRINT_ERROR("CCMHome_impl remove: remove failure when trying to remove a component! Ignore it");
            }
            catch (const CORBA::OBJECT_NOT_EXIST&)
            {
                // Ignore
                PRINT_ERROR("CCMHome_impl remove: component is being removed! Ignore it");
            }
            catch (const CORBA::SystemException&)
            {
                // Ignore
                PRINT_ERROR("CCMHome_impl remove: System exception raised when removing component! Ignore it");
            }

                
            comp_iter++;
        }

        // clean component list
        m_components.clear();
    } 
    catch (const PortableServer::Current::NoContext & ) 
    {
        // Not called within the context of an operation invocation
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                              CORBA::COMPLETED_NO);
    } 
    catch (const CORBA::SystemException & ) 
    {
        // rethrow
        throw;
    }

    LifeCycle::LifeCycleSingleObject::remove();

    // release context
    m_context = CdmwCcmContainer::CCM2Context::_nil();
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
//
CORBA::Boolean
CCMHome_impl::supports(const char* repository_id)
        throw(CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context.in()));

    return CORBA::Boolean(LifeCycle::ServantProviderBase::supports(repository_id));
}


//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
//
CdmwLifeCycle::RepositoryIdSeq* 
CCMHome_impl::get_supported_objects()
    throw(CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context.in()));

    return LifeCycle::ServantProviderBase::get_supported_objects();
}


//
// ServantProvider abstract operation implementation
//
 
/**
* Purpose:
* <p>This method provides a servant for the CORBA object with
* the specified ObjectId. It is called by the Container
* whenever a servant is needed to incarnate an object.
* 
*@param oid ObjectId of the object to be incarnated
*
*@return A servant incarnating the object 
*
*@exception PortableServer::ForwardRequest Indicates to the Life Cycle
* Framework ORB that the SevantProvider cannot handle this object 
* and requests can be handled to the returned IOR. If this exception 
* is translated/forwarded to the client-side ORB, the request invocation 
* will be transparently re-transmitted to the returnded object.
*@exception CORBA::SystemException Any CORBA system exception
*/
PortableServer::Servant        
CCMHome_impl::lookup_servant(const PortableServer::ObjectId& oid) 
    throw(PortableServer::ForwardRequest,
          CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context.in()));

    PRINT_INFO("Entering in lookup_servant...");

    PortableServer::ServantBase_var servant = 0;

    try  
    {
        // check if it is a component oid or a facet/consumer oid or 
        // a local push consumer oid.
        CORBA::String_var oid_str = PortableServer::ObjectId_to_string(oid);
        std::string object_id(oid_str.in());
        NameVector names = read_oid(object_id);

        std::string comp_oid = names[0];
        PRINT_INFO("oid= " << object_id);
        PRINT_INFO("comp_oid= " << comp_oid);
        
        // check a component with this oid has been created in this home
        ComponentMap::iterator comp_pos = m_components.find(comp_oid);
        
        if (comp_pos  == m_components.end()) 
        {
            throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                CORBA::COMPLETED_NO); 
        }

        ComponentMap::value_type& comp_value = *comp_pos;
        
        if (comp_value.second.comp_servant.in())
        {
            // because the call to release servant comes from POA, it may occurs that
            // the object is deactivated but release_servant not called and so the 
            // component has not been deleted from the map
            CCMObject_impl*  component 
                = dynamic_cast<CCMObject_impl*>(comp_value.second.comp_servant.in());

            if (component->is_removed()) 
            {
                throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                              CORBA::COMPLETED_NO); 
            }
        }

        switch (names.size())
        {
        case 1: // oid is a component oid
            {
                // Now we can create the component servant and store servant
                // while facet servant are part of component servant
                comp_value.second.comp_servant = create_component_servant(comp_oid, m_context.in(), comp_value.second);
                PRINT_INFO("component servant created");
                CDMW_ASSERT(comp_value.second.comp_servant.in());
                PRINT_INFO("assert on component servant done");
                
                // NOTA: comp executor is not null if an executor has been created at ref 
                // creation that is a factory operation has been used to create ref.
                
                servant = comp_value.second.comp_servant;
                PRINT_INFO("servant copied");
            }
            break;

        case 2: // oid is a facet/consumer oid
            {        
                std::string name = names[1];
                PRINT_INFO("name= " << name);

                CDMW_ASSERT(comp_value.second.comp_servant.in());
                CCMObject_impl* comp = dynamic_cast<CCMObject_impl*>(comp_value.second.comp_servant.in());
                
                servant = comp->get_facet_servant(name.c_str());
            }
            break;
            
        case 3:  // oid is a local push consumer
            {
                CDMW_ASSERT(comp_value.second.comp_servant.in());
                CCMObject_impl* comp = dynamic_cast<CCMObject_impl*>(comp_value.second.comp_servant.in());
                
                std::string name = names[1];
                std::string ck_value = names[2];
                PRINT_INFO("name= " << name);
                PRINT_INFO("ck_value= " << ck_value);
                
                servant = comp->get_local_push_consumer_servant(name.c_str(), ck_value.c_str());
            }
            break;

        default:
            {
                PRINT_ERROR("internal exception raised : deafult case encoutered!!!!");
                throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                    CORBA::COMPLETED_NO); 
            }
            break;
        }
    } 
    catch (const std::bad_alloc & ) 
    { 
        // Failed to allocate memory
        PRINT_ERROR("lookup_servant: Failed to allocate memory!");
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
            CORBA::COMPLETED_NO); 
    } 
    catch (const CORBA::BAD_PARAM & ) 
    { // Pb with ObjectId
        PRINT_ERROR("lookup_servant: Pb with ObjectId!");
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
            CORBA::COMPLETED_NO); 
    } 
    catch (const Components::CreateFailure& )
    {
        PRINT_ERROR("lookup_servant: Executor create failure!");
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
            CORBA::COMPLETED_NO); 
    }
    catch (const CORBA::OBJECT_NOT_EXIST &)
    {
        PRINT_ERROR("lookup_servant: Object not exist!");
        throw;
    }

    PRINT_INFO("End of lookup_servant...");
    return servant._retn();
}
  
  
/**
* Purpose:
* <p>This method is called by Container whenever it is done
* with the specified servant. The ServantProvider is free to handle it at
* will.
* 
*@param oid ObjectId of the object incarnated by the servant
*@param poa The POA where the object is activated
*@param servant The servant in question
*
*@exception CORBA::SystemException Any CORBA system exception
*/
void 
CCMHome_impl::release_servant(const PortableServer::ObjectId& oid,
                                   PortableServer::POA_ptr         poa,
                                   PortableServer::Servant         servant) 
    throw(CORBA::SystemException)
{
    // CDMW_ASSERT(!CORBA::is_nil(m_context.in()));
    // Do not check if context is not nil while release_servant can be called afteri
    // remove operation on the home to release each component.

    PRINT_INFO("Entering in release_servant...");

    try  
    {
        // check if it is a component oid or a facet oid
        CORBA::String_var oid_str = PortableServer::ObjectId_to_string(oid);
        std::string object_id(oid_str.in());
        NameVector names = read_oid(object_id);

        std::string comp_oid = names[0];
        PRINT_INFO("oid= " << object_id);
        PRINT_INFO("comp_oid= " << comp_oid);

        // check a component with this oid has been created in this home
        ComponentMap::const_iterator comp_pos = m_components.find(comp_oid);
        
        if (comp_pos  == m_components.end()) 
        {
            throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                          CORBA::COMPLETED_NO); 
        }

        ComponentMap::value_type comp_value = *comp_pos;

        switch (names.size())
        {
        case 1: // oid is a component oid
            {
                // check if operation removed has been requested on component
                //CDMW_ASSERT(comp_value.second.comp_servant.in());
                //CCMObject_impl*  component = dynamic_cast<CCMObject_impl*>(comp_value.second.comp_servant.in());
                CDMW_ASSERT(servant);
                CCMObject_impl*  component = dynamic_cast<CCMObject_impl*>(servant);
                
                CDMW_ASSERT(component);
                if (component->is_removed()) 
                {
                    PRINT_INFO("Component is removed, delete it from map");
                    // if so remove component from the map
                    m_components.erase(comp_oid);            
                }
                
                PRINT_INFO("Release servant");
                servant->_remove_ref();
            }
            break;

        case 2: // oid is a facet/consumer oid
            {
                std::string name = names[1];
                PRINT_INFO("name= " << name);       
                // release servant
                servant->_remove_ref();
            }
            break;
            
        case 3:  // oid is a local push consumer
            {
                std::string name = names[1];
                std::string ck_value = names[2];
                PRINT_INFO("name= " << name);
                PRINT_INFO("ck_value= " << ck_value);
                // release servant
                servant->_remove_ref();
            }
            break;

        default:
            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                CORBA::COMPLETED_NO); 
        }
    } 
    catch (const std::bad_alloc & ) 
    { 
        // Failed to allocate memory
        PRINT_ERROR("release_servant: Failed to allocate memory!");
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                      CORBA::COMPLETED_NO); 
    } 
    catch (const CORBA::BAD_PARAM & ) 
    { 
        // Pb with ObjectId
        PRINT_ERROR("release_servant: Pb with ObjectId!");
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                      CORBA::COMPLETED_NO); 
    }
}

/**
* Purpose:
* <p>This method is called by Container after Home creation.
* 
*@param ctxt Component context given by container.
*
*@exception Components::CCMException Any Component exception
*@exception CORBA::SystemException Any CORBA system exception
*/
void 
CCMHome_impl::set_context(CdmwCcmContainer::CCM2Context_ptr ctxt)
throw (Components::CCMException,
       CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    m_context = CdmwCcmContainer::CCM2Context::_duplicate(ctxt);
    
#ifdef CDMW_USE_FAULTTOLERANCE
    if (m_create_ft_component)
    {
        try
        {
            CORBA::Object_var obj_ref = m_context->resolve_initial_references("FTCurrent");
        }
        catch (const CdmwCcmContainer::CCM2Context::InvalidName& )
        {
          // Home is FT, process should be FT
          PRINT_ERROR("set_context: FT Home is waiting for a FT context from a FT process");
          throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMWrongContextType,
                                 CORBA::COMPLETED_NO);
        }
    }
#endif
}


/**
* Purpose:
* <p>This method returns the RepositoryID of the object associated to the
* <I>oid</I>.
* 
*@param oid ObjectId of the object
*
*@return The RepositoryId of the object
*
*@exception CORBA::OBJECT_NOT_EXIST The object does not exist any more.
*/
std::string
CCMHome_impl::get_supported_interface(const PortableServer::ObjectId& oid) const
   throw (CORBA::OBJECT_NOT_EXIST)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context));

    try  
    {
        CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
        std::string name(s.in());
        ComponentMap::const_iterator serv_pos = m_components.find(name);

        if (serv_pos  == m_components.end()) {
            throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                          CORBA::COMPLETED_NO); 
        }
        else 
        {
            std::string result(m_comp_rep_id);
            return result;
        }
    } 
    catch (const std::bad_alloc & ) 
    { // Failed to allocate memory
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                      CORBA::COMPLETED_NO); 
    } 
    catch (const CORBA::BAD_PARAM & ) 
    { // Pb with ObjectId
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                      CORBA::COMPLETED_NO); 
    }  
}



CORBA::Object_ptr 
CCMHome_impl::create_component_ref(const Components::ConfigValues& config, Components::EnterpriseComponent_ptr  comp_exec)
    throw(Components::CreateFailure,
          CORBA::SystemException)
{
    if (is_removed(m_oid.in()))
    {
       throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                     CORBA::COMPLETED_NO); 
    }
    CDMW_ASSERT(!CORBA::is_nil(m_context.in()));

    CORBA::Any_var ft_group_ref_any;
    bool ft_config_value_found = 
       read_config_value(CdmwDeployment::FAULT_TOLERANCE_GROUP_REF, config, ft_group_ref_any); 
#ifdef CDMW_USE_FAULTTOLERANCE
    ::FT::ObjectGroup_var ft_group_ref = ::FT::ObjectGroup::_nil();

    if (m_create_ft_component)
    {
        if (ft_config_value_found)
        {
            if (ft_group_ref_any >>= CORBA::Any::to_object(ft_group_ref))
            {
               if (CORBA::is_nil(ft_group_ref.in()))
               {
                   PRINT_ERROR("Wrong FT Configuration values for FT component creation: group_ref is nil!");
                   throw Components::CreateFailure(CdmwDeployment::WRONG_FT_CONFIG_VALUES);
               }
               // all is ok ft_group_ref will be saved below in comp_info
            }
            else
            {
                PRINT_ERROR("Wrong FT Configuration values for FT component creation");
                throw Components::CreateFailure(CdmwDeployment::WRONG_FT_CONFIG_VALUES);
            }
        }
        else
        {
            PRINT_ERROR("Missing FT Configuration values for FT component creation");
            throw Components::CreateFailure(CdmwDeployment::MISSING_FT_CONFIG_VALUES);
        }
    }
    else
    {
        if (ft_config_value_found)
        {
            PRINT_ERROR("Unexpected FT Configuration values for non FT component creation");
            throw Components::CreateFailure(CdmwDeployment::UNEXPECTED_FT_CONFIG_VALUES);
        }
    }
#else
    if (ft_config_value_found)
    {
        PRINT_ERROR("Unexpected FT Configuration values for non FT component creation");
        throw Components::CreateFailure(CdmwDeployment::UNEXPECTED_FT_CONFIG_VALUES);
    }
#endif

    // check component context is a Session2Context one
    Components::Session2Context_var s2ctx 
        = Components::Session2Context::_narrow(m_context.in());
    ComponentInfo comp_info;
    
    if (!CORBA::is_nil(s2ctx.in()))
    {                
        try
        {
            comp_info.comp_ref = s2ctx->create_ref(m_comp_rep_id.c_str());
    
            if (CORBA::is_nil(comp_info.comp_ref.in()))
            {
                PRINT_ERROR("Create_ref has failed!");
                throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            }
    
            // set executor => executor is not nil if factory operation is used!
            comp_info.comp_exec = Components::EnterpriseComponent::_duplicate(comp_exec);
#ifdef CDMW_USE_FAULTTOLERANCE
            comp_info.comp_group_ref = ft_group_ref;
#endif
    
            CORBA::OctetSeq_var comp_oid = s2ctx->get_oid_from_ref(comp_info.comp_ref.in());
            std::string comp_oid_str = Cdmw::CCM::Common::OctetSeq_to_string(comp_oid.in());
    
            m_components.insert(ComponentMap::value_type(comp_oid_str, comp_info));
        }
        catch (const Components::IllegalState& )
        {
            PRINT_ERROR("IllegalState exception raised!");
            throw Components::CreateFailure(CdmwDeployment::ILLEGAL_STATE);
        }
        catch (const Components::BadComponentReference& )
        {
            PRINT_ERROR("BadComponentReference exception raised!");
            throw Components::CreateFailure(CdmwDeployment::BAD_COMPONENT_REFERENCE);
        }
        catch (const CORBA::SystemException& ex)
        {
            PRINT_ERROR(ex);
            throw;
        }
    }
    else
    {
        PRINT_ERROR("Context is not a Session2Context!");
        throw Components::CreateFailure(CdmwDeployment::WRONG_CONTEXT_KIND);
    }
    
    return comp_info.comp_ref._retn();
}


} // End of namespace CIF

} // End of namespace CCM

} // End of namespace Cdmw

