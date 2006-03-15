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


#ifndef INCL_CDMW_CCM_CONTAINER_CONTAINER_IMPL_HPP
#define INCL_CDMW_CCM_CONTAINER_CONTAINER_IMPL_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <CCMContainer/idllib/CdmwCcmContainer.skel.hpp>
#include <CCMContainer/idllib/CdmwDeployment.stub.hpp>
#include <Event/idllib/CdmwEvent.stub.hpp>
#include <LifeCycle/lifecycle/RetainObjectDeactivator.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <list>
#include <utility> // for std::pair


namespace Cdmw
{
namespace CCM
{
namespace Container
{



/**
* Purpose:
* <p>
* Implementation of a CDMW Generic Container.
*/
class Container_impl : virtual public POA_CdmwCcmContainer::Container,
                       virtual public OrbSupport::RefCountServantBase
{

public:

    /**
    * Purpose:
    * <p> Constructor.
    *
    *@param orb A reference to the ORB pseudo-object
    *@param poa A parent POA used for the container activation. Should have a RETAIN servant
    * retention policy
    *@param m_homes_serialize_poa A POA used for single thread component homes. Single thread
    * homes will be activated on this POA. (Use of HomesServantActivator needed with the POA)
    *@param m_homes_multithread_poa A POA used for thread-safe component homes. Multi-thread
    * (thread-safe) homes will be activated on this POA. (Use of HomesServantActivator needed
    * with the POA)
    *@param component_installation A reference to the ComponentInstallation object
    *@param component_server A reference to the ComponentServer object
    *@param home_registration A reference to a HomeRegistration for use to register component
    * homes so they may be located by the HomeFinder.
    *@param process_name The name of the CDMW Process.
    *@param application_name The name of the CDMW Application.
    *@param config A sequence of ConfigValue objects passed in to the
    * <I>ComponentServer::create_container</I> operation at the Container creation.
    *
    *@exception CORBA::SystemException TODO:
    */ 
    Container_impl(CORBA::ORB_ptr          orb,
                   PortableServer::POA_ptr poa, // With RETAIN servant retention policy
                   PortableServer::POA_ptr homes_serialize_poa,
                   PortableServer::POA_ptr homes_multithread_poa,
                   CdmwDeployment::ComponentInstallation_ptr        component_installation,
                   Components::Deployment::ComponentServer_ptr       component_server,
                   Components::HomeRegistration_ptr                  home_registration,
                   CdmwEvent::EventChannelFactory_ptr               event_channel_factory,
                   const std::string &                               process_name,
                   const std::string &                               application_name,
                   const Components::ConfigValues& config)
    throw(CORBA::SystemException);
    

    /**
    * Purpose:
    * <p> Destructor.
    */ 
    virtual 
    ~Container_impl() throw ();

    virtual PortableServer::POA_ptr _default_POA() {
        return PortableServer::POA::_duplicate(m_poa.in());
    }

      

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    /**
     * This operation returns whether the Container supports the 
     * creation of component homes with the specified repository Id.
     *
     * @param repository_id The Repository Id of the desired home interface.
     *
     * @return <B>true</B> if the container supports/creates homes of 
     * <I>repository_id</I> type or of derived types. Otherwise it returns
     * <B>false</B>.
     */
    virtual CORBA::Boolean supports(const char* repository_id)
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //

    /**
     * This operation returns a sequence of CORBA Repository IDs of Components' 
     * homes that can be created by the container.
     *
     * @return A sequence of CORBA Repository IDs of components' homes that 
     * can be created by the container.
     */ 
    virtual CdmwLifeCycle::RepositoryIdSeq* get_supported_objects()
        throw(CORBA::SystemException);
    
    //
    // IDL:omg.org/Components/Deployment/Container/configuration:1.0
    //
    /**
    * Purpose:
    * <p>Gives access to the ConfigValue objects passed in to the create_container operation
    * at the Container creation.
    *@return A sequence of ConfigValue objects passed in to the <I>create_container</I> operation
    * at the Container creation.
    */
    virtual Components::ConfigValues* configuration()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Deployment/Container/get_component_server:1.0
    //
    /**
    * Purpose:
    * <p>Gives access to the ComponentServer object that created this Container.
    *
    *@return Object reference to the ComponentServer object that created this Container.
    */
    virtual Components::Deployment::ComponentServer_ptr get_component_server()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Deployment/Container/install_home:1.0
    //
    /**
    * Purpose:
    * <p>The install_home operation installs and returns a new CCMHome object. The id and
    * entrypt parameters are used by the container to locate an implementation file and
    * instantiate a new home object.
    *
    *@param id Refers to a particular implementation within a component package.
    *@param entrypt Entry point to create a component home within the [shared] library.
    *@param config Represents a sequence of ConfigValue objects that provide name value pairs
    * used to configure the installation of the new home instance, e.g. provide persistency source,
    * transaction and security policies which must be applied to the home and its components.
    *
    * <p> <B>NOTA:</B> These are the configuration values understood by the container
    * for component home installation.
    * <TABLE>
    *   <TR>
    *      <TD><U><B>Config Name</B></U></TD><TD><U><B>Type</B></U></TD>
    *   </TR>
    *   <TR>
    *      <TD>COMPONENT_KIND (*)</TD><TD>ComponentKindValue</TD>
    *   </TR>
    *   <TR>
    *      <TD>SERVANT_LIFETIME (**)</TD><TD>ServantLifetimeValue</TD>
    *   </TR>
    *   <TR>
    *      <TD>POA_POLICIES_FOR_UNCLASSIFIED (***)</TD><TD>sequence< PoaPolicy ></TD>
    *   </TR>
    *   <TR>
    *      <TD>THREADING_POLICY (*)</TD><TD>ThreadingPolicyValue</TD>
    *   </TR>
    *   <TR>
    *      <TD>EXTENDED_POA_POLICIES</TD><TD>sequence< PoaPolicy ></TD>
    *   </TR>
    *   <TR>
    *      <TD>HOME_REPOSITORY_ID (*)</TD><TD>CORBA::RepositoryId</TD>
    *   </TR>
    *   <TR>
    *      <TD>HOME_SERVANT_CLASSNAME (*)</TD><TD>string</TD>
    *   </TR>
    *   <TR>
    *      <TD>VALUETYPE_FACTORY_DEPENDENCIES</TD><TD>sequence< ValuetypeFactoryDescription ></TD>
    *   </TR>
    * </TABLE>
    * <p>
    * <TABLE>
    *   <TR>
    *      <TD>(*)</TD><TD>mandatory</TD>
    *   </TR>
    *   <TR>
    *      <TD>(**)</TD><TD>mandatory for session, process, and entity and NOT for service component kind</TD>
    *   </TR>
    *   <TR>
    *      <TD>(***)</TD><TD>mandatory for unclassified component kind</TD>
    *   </TR>
    * </TABLE>
    *
    *
    *@return Object reference to the new CCMHome object.
    *
    *@exception Components::Deployment::UnknownImplId Raised if the <I>id</I> parameter does not
    * correspond to any component packages installed using the ComponentInstallation object.
    *@exception Components::Deployment::ImplEntryPointNotFound Raised if the <I>entrypt</I> parameter
    * cannot be found in the implementation returned from ComponentInstallation.
    *@exception Components::Deployment::InstallationFailure Raised if the home could not be installed
    * in the container for internal reasons such as insufficient resources or inadequate implementation
    * for this container .
    *@exception Components::Deployment::InvalidConfiguration Raised if the <I>config</I> parameter
    * does not contain valid configuration name value pairs.
    *@exception CORBA::SystemException TODO:
    */
    virtual Components::CCMHome_ptr install_home(const char* id,
                                                 const char* entrypt,
                                                 const Components::ConfigValues& config)
        throw(Components::Deployment::UnknownImplId,
              Components::Deployment::ImplEntryPointNotFound,
              Components::Deployment::InstallationFailure,
              Components::Deployment::InvalidConfiguration,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Deployment/Container/remove_home:1.0
    //
    /**
    * Purpose:
    * <p>The remove_home operation removes a home from the container and all components
    * that were created by this home.
    *
    *@param href Object reference of the component home to remove.
    *
    *@exception Components::RemoveFailure Raised if the home or any of its components could not be
    * removed from the container for any internal reasons.
    *@exception CORBA::BAD_PARAM Raised if <i>href</I> parameter was not created by this container.
    *@exception CORBA::SystemException TODO:
    */
    virtual void remove_home(Components::CCMHome_ptr href)
        throw(Components::RemoveFailure,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Deployment/Container/get_homes:1.0
    //
    /**
    * Purpose:
    * <p>The get_homes operation returns a sequence of all CCMHome objects created 
    * by this Container.
    *
    *@exception CORBA::SystemException TODO:
    */
    virtual Components::CCMHomes* get_homes()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Deployment/Container/remove:1.0
    //
    /**
    * Purpose:
    * <p>The remove operation removes the container and all homes and components 
    * created in the container
    *
    *@exception Components::RemoveFailure Raised if the container or any of its homes could
    * not be removed for any internal reasons.
    *@exception CORBA::SystemException TODO:
    */
    virtual void remove()
        throw(Components::RemoveFailure,
              CORBA::SystemException);        
    
private:
    /**
     * Purpose:
     * <p> [Constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    Container_impl()
    throw();
    
    // Hide copy constructor/assignment operator
    Container_impl(const Container_impl& rhs)
    throw();
        
    Container_impl&
    operator=(const Container_impl& rhs)
    throw();

    /**
     * Data type that holds any home instance-specific information needed by the container.
     */
    struct CCMHomeData {
        
        typedef std::list<CdmwCcmContainer::ValueTypeRegistration_var> DependencyList;
        
        CCMHomeData(Cdmw::OsSupport::OS::SharedObjectHandle *     handle,
                    const std::string &                        repid,
                    const DependencyList &                     dependencies)
            : shlib_handle(handle),
              repository_id(repid),
              valuetype_dependencies(dependencies)
        {}
        CCMHomeData(const CCMHomeData & hd)
            : shlib_handle(hd.shlib_handle),
              repository_id(hd.repository_id),
              valuetype_dependencies(hd.valuetype_dependencies)
        {}
        CCMHomeData & operator=(const CCMHomeData & rhs)
        {
            if(&rhs != this)
            {
                shlib_handle           = rhs.shlib_handle;
                repository_id          = rhs.repository_id;
                valuetype_dependencies = rhs.valuetype_dependencies;
            }
            return *this;
        }

        /**
         * Handle to the [shared|dynamic] library hosting the home's executor.
         */
        Cdmw::OsSupport::OS::SharedObjectHandle *shlib_handle;
        /**
         * The homes's repository Id.
         */
        std::string                        repository_id;
        /**
         * Value type factories dependencies.
         */
        DependencyList                     valuetype_dependencies;
    };



public :

    /**
     * A container type for all homes and their associated data created by the container.
     */ 
    typedef std::list<std::pair<Components::CCMHome_var,CCMHomeData> > CCMHomesList;

private :

    /**
     * A Functor class responsible for removing a home.
     */
    class HomeRemover {
    public:
        HomeRemover() {}
        void operator()(const CCMHomesList::value_type & home) const
            throw(Components::RemoveFailure,
                  CORBA::SystemException);
    };
    
    /**
     * A Functor class responsible for closing a loaded library .
     */
    class DlCloser {
    public:
        DlCloser() {}
        void operator()(const CCMHomesList::value_type & home) const
            throw();
    };

    
    /**
    * Purpose:
    * <p>Removes all homes created by the container and all the components they've created.
    *
    *@exception Components::RemoveFailure Raised if any of the homes could not be removed
    * for any internal reasons.
    *@exception CORBA::SystemException TODO:
    */
    void remove_all_homes()
        throw(Components::RemoveFailure,
              CORBA::SystemException);
    
    /**
    * Purpose:
    * <p>Closes all shared libraries opened by the container by the container.
    *
    */
    void dlclose_all()
        throw();
    
    /**
    * Purpose:
    * <p>Creates a home Identifier for use for home registration within the home CDMW
    * naming and repository service.
    *
    *@param uuid The Home's UUID as received from Container::install_home()
    *
    *@return A process wide unique id.
    *
    */
    std::string create_home_identifier(const std::string& uuid)
        throw(std::bad_alloc);
    //
    // Attributes
    //
    
    /**
     * A reference to the ORB pseudo-object
     */
    CORBA::ORB_var          m_orb;
    /**
     * A parent POA used for the container activation
     */
    PortableServer::POA_var m_poa;
    /**
     * A POA used for single thread component homes
     */
    PortableServer::POA_var m_homes_serialize_poa;
    /**
     * A POA used for thread-safe component homes
     */
    PortableServer::POA_var m_homes_multithread_poa;
    /**
     * A reference to the ComponentInstallation object
     */
    CdmwDeployment::ComponentInstallation_var        m_component_installation;
    /**
     * A reference to the ComponentServer object
     */
    Components::Deployment::ComponentServer_var       m_component_server;
    /**
     * A sequence of ConfigValue objects passed in to the <I>ComponentServer::create_container</I>
     * operation at the Container creation.
     */
    Components::ConfigValues                          m_config_values;
    /**
     * Object Deactivator for use by homes for their deactivation.
     */
    Cdmw::LifeCycle::RetainObjectDeactivator         m_object_deactivator;
    /**
     * A reference to a HomeRegistration for use to register component homes so they may be
     * located by the HomeFinder.
     */
    Components::HomeRegistration_var                  m_home_registration;
    /**
     * The CDMW Event Channel Factory.
     */
    CdmwEvent::EventChannelFactory_var               m_event_channel_factory;
    /**
     * The CDMW Process name.
     */
    std::string                                       m_process_name;
    /**
     * The CDMW Application name.
     */
    std::string                                       m_application_name;
    /**
     * A list of all the homes created by the container and their associated data.
     */
    CCMHomesList            m_ccm_homes;
    /**
     * Indicates whether the remove operation has been called or not.
     */ 
    bool m_is_removed;
    /**
     * A mutex to protect from concurrent access to local data.
     */ 
    Cdmw::OsSupport::Mutex m_container_mutex;

};


}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_CONTAINER_IMPL_HPP

