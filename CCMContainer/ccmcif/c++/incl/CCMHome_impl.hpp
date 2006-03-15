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


#ifndef INCL_CCM_CIF_CCMHOME_IMPL_HPP
#define INCL_CCM_CIF_CCMHOME_IMPL_HPP

#include <LifeCycle/lifecycle/LifeCycleSingleObject.hpp>
#include <LifeCycle/lifecycle/ObjectDeactivator.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>

#include <CCMContainer/ccmcontainer/ComponentServantProvider.hpp>
#include <CCMContainer/idllib/CdmwCcmCif.skel.hpp>
#include <CCMContainer/idllib/CdmwCcmContainer.stub.hpp>
#include <CCMContainer/ccmcif/Context.hpp>

#include <string>
#include <map>

namespace Cdmw {

namespace CCM {

namespace CIF {


//
// IDL:thalesgroup.com/CdmwCcmCif/CdmwCCMHome:1.0
// This class implements the Components::CCMHome interface. This is a the base
// class for all generated well-typed component home.
//
class CCMHome_impl : virtual public POA_CdmwCcmCif::CCMHome,
                     public Cdmw::LifeCycle::LifeCycleSingleObject,
                     virtual public Container::ComponentServantProvider,
                     virtual public OrbSupport::RefCountServantBase
{

public:

   /**
    * Purpose:
    * <p>
    *   Constructor.
    *
    *@param rep_id       The home repository ID
    *@param comp_rep_id  The component repository ID
    *@param deactivator  The Object Deactivator responsible to 
    *                    deactivate home at its removal.
    *@param config       The config values used to specify if
    *                    the home is FT or not
    *                    
    *@exception Any CORBA Exception.
    */ 
    CCMHome_impl(const std::string&                         rep_id, // home repository ID
                 const std::string&                         comp_rep_id,
                 const Cdmw::LifeCycle::ObjectDeactivator&  deactivator,
                 const Components::ConfigValues&            config,
                 const bool                                 is_a_base=false)
       throw(CORBA::SystemException);

    virtual ~CCMHome_impl() throw();

    /**
    * Purpose:
    * <p>
    *   This operation returns an object reference that supports the 
    *   CORBA::ComponentIR::ComponentDef interface, describing the component 
    *   type associated with the home object.
    * 
    *@return CORBA::ComponentIR::ComponentDef described the component type 
    * associated with the home object.
    *@exception Any CORBA Exception.
    *
    */
    //
    // IDL:omg.org/Components/CCMHome/get_component_def:1.0
    //
    virtual CORBA::IRObject_ptr get_component_def()
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   This returns an object reference that supports the 
    *   CORBA::ComponentIR::HomeDef interface describing the home type.
    * 
    *@return CORBA::ComponentIR::HomeDef interface describing the home type.
    *@exception Any CORBA Exception.
    *
    */
    //
    // IDL:omg.org/Components/CCMHome/get_home_def:1.0
    //
    virtual CORBA::IRObject_ptr get_home_def()
        throw(CORBA::SystemException);


    /**
    * Purpose:
    * <p>
    *   This causes the component denoted by the reference to cease to exist.
    *   Subsequent invocations on the reference will cause an OBJECT_NOT_EXIST
    *   system exception to be raised.
    * 
    *@return void
    *@exception BAD_PARAM system exception raised if the component denoted by
    *   the parameter does not exist in the container associated with target 
    *   home object.
    *@exception Components::RemoveFailure exception raised if application errors
    *@exception Any other CORBA Exception.
    *
    */
    //
    // IDL:omg.org/Components/CCMHome/remove_component:1.0
    //
    virtual void remove_component(Components::CCMObject_ptr comp)
        throw(Components::RemoveFailure,
              CORBA::SystemException);

    /**
    * Purpose:
    * <p>Remove instructs the object to cease to exist. The object reference for
    * the target is no longer valid after remove successfully completes.
    *
    * 
    * <B>NOTA:</B> Any further call to <I>Remove</I> for this object will raise
    *  a CORBA::OBJECT_NOT_EXIST system exception.
    *
    *@exception CosLifeCycle::NotRemovable An implementation that refuses to 
    * remove itself should raise this exception
    *@exception CORBA::SystemException Any CORBA system exception
    */ 
    //
    // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
    //
    virtual void remove()
        throw(CosLifeCycle::NotRemovable,
              CORBA::SystemException);

    /**
    * This operation returns whether the CDMW factory supports the 
    * creation of objects supporting the specified repository Id.
    *
    * @param repository_id The Repository Id of the desired interface.
    *
    * @return <B>true</B> if the factory supports/creates objects of 
    * <I>repository_id</I> type or of derived types. Otherwise it returns
    * <B>false</B>.
    */ 
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    virtual CORBA::Boolean supports(const char* repository_id)
        throw(CORBA::SystemException);

    /**
    * This operation returns a sequence of CORBA Repository IDs of CDMW 
    * objects that can be created by the factory.
    *
    * @param repository_id The Repository Id of the desired interface.
    *
    * @return A sequence of CORBA Repository IDs of CDMW objects that 
    * can be created by the factory.
    */
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    virtual CdmwLifeCycle::RepositoryIdSeq* get_supported_objects()
        throw(CORBA::SystemException);
    
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
    virtual std::string
        get_supported_interface(const PortableServer::ObjectId& oid) const
        throw (CORBA::OBJECT_NOT_EXIST);    
    
       
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
    virtual PortableServer::Servant 
        lookup_servant(const PortableServer::ObjectId& oid) 
        throw(PortableServer::ForwardRequest,
              CORBA::SystemException); 
    
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
    virtual void 
        release_servant(const PortableServer::ObjectId& oid,
                        PortableServer::POA_ptr         poa,
                        PortableServer::Servant         servant) 
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>This method is called by Container after Home creation.
    * 
    *@param ctxt Component context given by container.
    *
    *@exception Components::CCMException Any Component exception
    *@exception CORBA::SystemException Any CORBA system exception
    */
    virtual void 
        set_context(CdmwCcmContainer::CCM2Context_ptr ctxt)
        throw (Components::CCMException, 
               CORBA::SystemException);

   /**
    * Purpose:
    * <p>This checks whether the CORBA object incarnated by the current
    * object (this) has been removed (<I>remove</I> operation has been 
    * called).
    *
    *@return <B>true</B> if the object has been removed, <B>false</B> otherwise.
    */         
    inline void check_is_removed(const PortableServer::ObjectId& oid) const
       throw (CORBA::OBJECT_NOT_EXIST)
    {
       if (is_removed(oid))
       {
           throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXISTObjectRemoved,
                                         CORBA::COMPLETED_NO);
       }
    }


protected:
    
    /**
    * Purpose:
    * <p>
    *   This creates new component reference.
    * 
    *@return A reference on the new component.
    *@exception Components::CreateFailure if application errors.
    *@exception Any other CORBA Exception.
    *
    */ 
    virtual CORBA::Object_ptr create_component_ref(
          const Components::ConfigValues& config, 
          Components::EnterpriseComponent_ptr  comp_exec = 
             Components::EnterpriseComponent::_nil())
        throw (Components::CreateFailure,
               CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   This creates new component reference. This is called by 
    *   create_component_ref
    *@param comp_oid Out parameter containing the oid of the new created 
    * component.
    *@return A reference on the new component.
    *@exception Components::CreateFailure if application errors.
    *@exception Any other CORBA Exception.
    *
    */ 
    virtual CORBA::Object_ptr create_component_ref_i(
          std::string&                         comp_oid, 
          Components::EnterpriseComponent_ptr  comp_exec = 
             Components::EnterpriseComponent::_nil())
        throw (Components::CreateFailure,
               CORBA::SystemException);
    /**
    * Purpose:
    * <p>
    *   This delete all stored information concerning a component.
    *@param comp_oid parameter containing the oid of the removed 
    * component.
    */
    virtual void delete_component_info(const std::string& comp_oid);



    // Component map with key = stringified oid.
    struct ComponentInfo {
        CORBA::Object_var                    comp_ref;    // component reference
        PortableServer::ServantBase_var      comp_servant;// component servant
        Components::EnterpriseComponent_var  comp_exec;   // component executor
        ComponentInfo() 
            : comp_ref(CORBA::Object::_nil()),
              comp_servant(0),
              comp_exec(Components::EnterpriseComponent::_nil()) 
        {
        };
    };
    typedef std::map<std::string, 
                     ComponentInfo, 
                     std::less<std::string> > ComponentMap;

    /**
    * Purpose:
    * <p>
    *   This creates new component servant.
    *   This shall be implemented by the well typed home servant.
    *
    *@param ctx      The component context
    *@param executor The component executor
    * 
    *@return The new component servant.
    *
    */ 
    virtual PortableServer::Servant 
        create_component_servant(const std::string&  comp_oid,
                                 Context*            ctx,
                                 ComponentInfo&      comp_info)
        throw(Components::CreateFailure, 
              CORBA::SystemException) = 0;

    /**
     * Purpose:
     * <p>Read a config value
     *
     *@param name Name of the configuration data
     *@param config A sequence of all configuration values
     *@param value Out CORBA::Any parameter for holding the configuration value
     *
     *@return <B>true</B> if the configration value is found, <B>false</B> 
     * otherwise.
     */
     bool read_config_value(const std::string & name,
                           const Components::ConfigValues & config,
                           CORBA::Any_out value)
        throw();


    Context*                       m_context;     // home Context
    PortableServer::ObjectId_var   m_oid;         // home oid 
    const std::string              m_rep_id;      // home repository id
    const std::string              m_comp_rep_id; // component rep id
    ComponentMap                   m_components;  // list of created component 
    
private:
    CCMHome_impl(const CCMHome_impl&);
    void operator=(const CCMHome_impl&);

    bool m_is_a_base;  // true if this class has inherited class.
};


} // End of namespace CIF

} // End of namespace CCM

} // End of namespace Cdmw

#endif  // INCL_CCM_CIF_CCMHOME_IMPL_HPP

