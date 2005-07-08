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


#ifndef INCL_CCM_CIF_CCMOBJECT_IMPL_HPP
#define INCL_CCM_CIF_CCMOBJECT_IMPL_HPP

#include <string>
#include <map>

#include <CCMContainer/idllib/Components.skel.hpp>
#include <CCMContainer/idllib/CdmwCcmContainer.stub.hpp>
#include <Event/idllib/CosEventComm.skel.hpp>

// Cdmw Files
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <Event/eventinterface/EventChannelInterface.hpp>

// Cdmw CCM Files
#include <CCMContainer/ccmcommon/Cookie_impl.hpp>

#ifdef CDMW_USE_FAULTTOLERANCE  
// Cdmw FT Files
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTCurrent.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>
#endif

namespace std 
{
     template <>
         bool less<Components::Cookie_var>::operator() (const Components::Cookie_var& ck1_var, 
                                                        const Components::Cookie_var& ck2_var) const
     {
         Cdmw::CCM::Common::Cookie_impl* ck1 
             = dynamic_cast<Cdmw::CCM::Common::Cookie_impl *>(ck1_var.in());
         Cdmw::CCM::Common::Cookie_impl*  ck2 
             = dynamic_cast<Cdmw::CCM::Common::Cookie_impl *>(ck2_var.in());
         
         return ((*ck1) < (*ck2));
     }
}

namespace {


} // End anonymous space

namespace Cdmw {

namespace CCM {

namespace CIF {


/**
*Purpose:
*<p>    [class description]
*    This class implement the Components::CCMObject interface.
*    It will be inherited by all components servant implementation.
*
*/

//
// IDL:omg.org/Components/CCMObject:1.0
//
class CCMObject_impl : virtual public POA_Components::CCMObject,
                       virtual public OrbSupport::RefCountServantBase
{
public:

    CCMObject_impl(const std::string comp_oid,
                   CdmwCcmContainer::CCM2Context_ptr ctx, 
                   const std::string rep_id,
                   CORBA::Object_ptr comp_ref,
                   bool              is_monolithic=false);
    ~CCMObject_impl();

    virtual CORBA::Object_ptr _get_component();

    //
    // IDL:omg.org/Components/CCMObject/get_component_def:1.0
    //
    virtual CORBA::IRObject_ptr get_component_def()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMObject/get_ccm_home:1.0
    //
    virtual Components::CCMHome_ptr get_ccm_home()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMObject/get_primary_key:1.0
    //
    virtual Components::PrimaryKeyBase* get_primary_key()
        throw(Components::NoKeyAvailable,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMObject/configuration_complete:1.0
    //
    virtual void configuration_complete()
        throw(Components::InvalidConfiguration,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMObject/remove:1.0
    //
    virtual void remove()
        throw(Components::RemoveFailure,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/CCMObject/get_all_ports:1.0
    //
    virtual Components::ComponentPortDescription* get_all_ports()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Navigation/provide_facet:1.0
    //
    virtual CORBA::Object_ptr provide_facet(const char* name)
        throw(Components::InvalidName,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Navigation/get_all_facets:1.0
    //
    virtual Components::FacetDescriptions* get_all_facets()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Navigation/get_named_facets:1.0
    //
    virtual Components::FacetDescriptions* get_named_facets(const Components::NameList& names)
        throw(Components::InvalidName,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Navigation/same_component:1.0
    //
    virtual CORBA::Boolean same_component(CORBA::Object_ptr object_ref)
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Receptacles/connect:1.0
    //
    /**
    * Purpose:
    * <p>
    *   connects the object reference specified by the connection parameter to
    *   the receptacle specified by the name parameter on the component.
    * 
    *@param name        The receptacle name.
    *@param connection  The connection object reference.
    *
    *@return a cookie if receptacle is multiplex, nil otherwise.
    *@exception Components::InvalidName The name parameter does not specify a valid receptacle name.
    *@exception Components::InvalidConnection The object reference in the connection parameter does
    *           not support the interface declared in the receptacle�s uses statement.
    *@exception Components::AlreadyConnected The receptacle is a simplex receptacle and it is already connected.
    *@exception Components::ExceededConnectionLimit The receptacle is a multiplex receptacle and the 
                implementation-defined limit to the number of connections is exceeded.
    *@exception CORBA::SystemException  Any Corba system exception.
    */ 
    virtual Components::Cookie* connect(const char* name,
                                        CORBA::Object_ptr connection)
        throw(Components::InvalidName,
              Components::InvalidConnection,
              Components::AlreadyConnected,
              Components::ExceededConnectionLimit,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Receptacles/disconnect:1.0
    //
    /**
    * Purpose:
    * <p>
    *   disconnects the object reference specified by ck parameter to
    *   the receptacle specified by the name parameter on the component.
    * 
    *@param name   The receptacle name.
    *@param ck     The connection cookie value.
    *
    *@return void.
    *@exception Components::InvalidName The name parameter does not specify a valid receptacle name.
    *@exception Components::InvalidConnection The receptacle is a multiplex receptacle and the cookie
    *           value in the ck parameter does not denote an existing connection on the receptacle.
    *@exception Components::CookieRequired The receptacle is a multiplex receptacle and a null value
    *           is specified in the ck parameter.
    *@exception Components::NoConnection The receptacle is a simplex receptacle there is no current connection.
    *@exception CORBA::SystemException  Any Corba system exception.
    *
    */ 
    virtual void disconnect(const char* name,
                            Components::Cookie* ck)
        throw(Components::InvalidName,
              Components::InvalidConnection,
              Components::CookieRequired,
              Components::NoConnection,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Receptacles/get_connections:1.0
    //
    virtual Components::ConnectionDescriptions* get_connections(const char* name)
        throw(Components::InvalidName,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Receptacles/get_all_receptacles:1.0
    //
    virtual Components::ReceptacleDescriptions* get_all_receptacles()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Receptacles/get_named_receptacles:1.0
    //
    virtual Components::ReceptacleDescriptions* get_named_receptacles(const Components::NameList& names)
        throw(Components::InvalidName,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/get_consumer:1.0
    //
    virtual Components::EventConsumerBase_ptr get_consumer(const char* sink_name)
        throw(Components::InvalidName,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/subscribe:1.0
    //
    virtual Components::Cookie* subscribe(const char* publisher_name,
                                          Components::EventConsumerBase_ptr subscriber)
        throw(Components::InvalidName,
              Components::InvalidConnection,
              Components::ExceededConnectionLimit,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/unsubscribe:1.0
    //
    virtual void unsubscribe(const char* publisher_name,
                             Components::Cookie* ck)
        throw(Components::InvalidName,
              Components::InvalidConnection,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/connect_consumer:1.0
    //
    virtual void connect_consumer(const char* emitter_name,
                                  Components::EventConsumerBase_ptr consumer)
        throw(Components::InvalidName,
              Components::AlreadyConnected,
              Components::InvalidConnection,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/disconnect_consumer:1.0
    //
    virtual Components::EventConsumerBase_ptr disconnect_consumer(const char* source_name)
        throw(Components::InvalidName,
              Components::NoConnection,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/get_all_consumers:1.0
    //
    virtual Components::ConsumerDescriptions* get_all_consumers()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/get_named_consumers:1.0
    //
    virtual Components::ConsumerDescriptions* get_named_consumers(const Components::NameList& names)
        throw(Components::InvalidName,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/get_all_emitters:1.0
    //
    virtual Components::EmitterDescriptions* get_all_emitters()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/get_named_emitters:1.0
    //
    virtual Components::EmitterDescriptions* get_named_emitters(const Components::NameList& names)
        throw(Components::InvalidName,
              CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/get_all_publishers:1.0
    //
    virtual Components::PublisherDescriptions* get_all_publishers()
        throw(CORBA::SystemException);

    //
    // IDL:omg.org/Components/Events/get_named_publishers:1.0
    //
    virtual Components::PublisherDescriptions* get_named_publishers(const Components::NameList& names)
        throw(Components::InvalidName,
              CORBA::SystemException);


    /**
    * Purpose:
    * <p>This checks whether the CORBA object incarnated by the current
    * object (this) has been removed (<I>remove</I> operation has been 
    * called).
    *
    *@return <B>true</B> if the object has been removed, <B>false</B> otherwise.
    */ 
    bool is_removed() const 
        throw();

    /**
    * Purpose:
    * <p>This returns the proxy consumer for the event source specified by
    * source_name. This operation has been created for internal use.
    *
    * @param source_name The event source name (publisher/emitter)
    *
    * @return the proxy consumer of the event service.
    */
    CosEventComm::PushConsumer_ptr get_proxy_consumer(const char* source_name)
        throw (CORBA::SystemException);

    /**
    * Purpose:
    * <p>This returns the servant for the local push consumer specified by source_name
    * and cookie value.
    *
    * @param source_name The source name
    * @param ck_value    The cookie value
    *
    * @return the local push consumer servant.
    */
    PortableServer::Servant get_local_push_consumer_servant(const char* source_name, 
                                                            const char* ck_value) 
        throw (CORBA::SystemException);

    /**
    * Purpose:
    * <p>This returns the servant for the facet specified by facet_name.
    * This operation is implemented by the well typed component servant.
    *
    *@param facet_name The facet name
    *
    *@return the facet servant.
    */ 
    virtual PortableServer::Servant get_facet_servant(const char* facet_name) 
        throw (CORBA::SystemException) = 0;


protected:
    //////////////////////////
    //                      //
    // Internal Operations  //
    //                      //
    //////////////////////////

    /**
    * Purpose:
    * <p>
    *   add a facet in FacetMap
    * 
    *@param comp_oid       The component oid used to create the facet reference.
    *@param facet_name     The facet name.
    *@param rep_id         The facet repository id.
    *
    *@return  The reference to be exported to clients
    *@exception InternalErrorException
    *@exception AlreadyDoneException
    */ 

    CORBA::Object_ptr declare_facet(const char*  comp_oid,
                                    const char*  facet_name,
                                    const char*  rep_id)
        throw (AlreadyDoneException, InternalErrorException);

    /**
    * Purpose:
    * <p>
    *   add a receptacle in ReceptacleMap
    * 
    *@param receptacle_name  The receptacle name.
    *@param rep_id           The receptacle repository id.
    *@param is_multiplex     This is set to true if receptacle is multiple
    *
    *@return void
    *@exception none
    */ 
    void declare_receptacle(const char*  receptacle_name,
                            const char*  rep_id,
                            bool         is_multiplex = false)
        throw (AlreadyDoneException);

    /**
    * Purpose:
    * <p>
    *   add a consumer in ConsumerMap
    * 
    *@param comp_oid        The component oid used to create the consumer reference.
    *@param consumer_name   The consumer name.
    *@param rep_id          The consumer repository id.
    *
    *@return void
    *@exception AlreadyDoneException
    */ 
    CORBA::Object_ptr declare_consumer(const char*  comp_oid,
                                       const char*  consumer_name,
                                       const char*  rep_id)
        throw (AlreadyDoneException, InternalErrorException);
    /**
    * Purpose:
    * <p>
    *   add an emitter in EmitterMap
    * 
    *@param emitter_name   The emitter name.
    *@param rep_id         The emitter repository id.
    *
    *@return void
    *@exception AlreadyDoneException
    */ 
    void declare_emitter(const char*  emitter_name,
                         const char*  rep_id)
        throw (AlreadyDoneException, CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   add a publisher in PublisherMap
    * 
    *@param publisher_name   The publisher name.
    *@param rep_id           The publisher repository id.
    *
    *@return void
    *@exception AlreadyDoneException if the publisher has already been declared.
    */ 
    void declare_publisher(const char*  publisher_name,
                           const char*  rep_id)
        throw (AlreadyDoneException, CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   return an event consumer stored in PublisherMap at ck
    * 
    *@param publisher_name   The publisher name.
    *@param ck               The cookie return at consumer subscription.
    *
    *@return a reference on the target consumer
    *@exception Components::InvalidConnection if the connection does'nt exist
    */ 
    Components::EventConsumerBase_ptr get_consumer(const char*  publisher_name,
                                                   Components::Cookie* ck)
       throw(Components::InvalidName, Components::InvalidConnection, CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   call ccm_passivate and ccm_remove on executor. This function shall be implemented
    *   by the well typed class.
    * 
    *@return void
    *@exception Any CORBA Exception.
    */ 
    virtual void advise_executor_of_remove_component()
        throw (CORBA::SystemException) = 0;

    /**
    * Purpose:
    * <p>
    *   return a group reference if the current request is invoked on a FT group reference.
    *   Otherwise return the same reference.
    * 
    *@param obj_ref       The facet/consumer/component reference.
    *
    *@return  The reference to be exported to clients
    */ 
    CORBA::Object_ptr get_group_object_ref_if_FTrequest(const CORBA::Object_ptr obj_ref)
        throw (CORBA::SystemException);



    ////////////////////////
    //                    //
    //    Attributes      //
    //                    //
    ////////////////////////

    CdmwCcmContainer::CCM2Context_var   m_context;   // Component Context for CDMW component
    std::string                         m_rep_id;       // component rep id
    CORBA::Object_var                   m_comp_ref;     // component reference to be return by _get_component
    bool                                m_configured;   // set to true when configuration completed
    std::string                         m_component_oid;  // object Id of the current component

private:
    CCMObject_impl(const CCMObject_impl&);
    void operator=(const CCMObject_impl&);

    //
    // Map typedef
    //

    // Facet Map: find facet info by name
    struct FacetInfo
    {
        std::string        rep_id;   // facet repository id
        CORBA::Object_var  obj_ref;  // facet reference
    };
    typedef std::map<std::string, FacetInfo, std::less<std::string> > FacetMap;

    // Receptacle Connection Map: find connection by cookie
    typedef std::map<Components::Cookie_var, CORBA::Object_var, std::less<Components::Cookie_var> > ConnectionMap;

    struct ReceptacleInfo
    {
        std::string   rep_id;         // receptacle repository id
        bool          is_multiplex;   // true if receptacle is multiplex
        ConnectionMap connections;    // connection info
    };
    // find receptacle by its name
    typedef std::map<std::string, ReceptacleInfo, std::less<std::string> > ReceptacleMap;

    // Consumer Map
    struct ConsumerInfo
    {
        std::string        rep_id;    // consumer repository id
        CORBA::Object_var  consumer;  // consumer reference
    };
    // find consumer by its name
    typedef std::map<std::string, ConsumerInfo, std::less<std::string> > ConsumerMap;

    struct EmitterInfo
    {
        std::string                       rep_id;         // consumer repository id
        Event::EventChannelInterface*     channel;        // event channel
        CosEventComm::PushConsumer_var    proxy_consumer; // proxy push consumer used to push event in event channel
        CosEventComm::PushSupplier_var    proxy_supplier; // proxy push supplier used to connect consumer
        Components::EventConsumerBase_var ccm_consumer;   // component consumer reference
        CosEventComm::PushConsumer_var    push_consumer;  // consumer reference implemented CosEventComm::PushConsumer
        PortableServer::ServantBase_var   push_consumer_servant;  // local consumer servant implementing CosEventComm::PushConsumer
    };
    // find emitter by its name
    typedef std::map<std::string, EmitterInfo, std::less<std::string> > EmitterMap;

    struct SubscriberInfo
    {
        CosEventComm::PushSupplier_var    proxy_supplier; // proxy push supplier used to connect consumer
        Components::EventConsumerBase_var ccm_consumer;   // component consumer reference
        CosEventComm::PushConsumer_var    push_consumer;  // consumer reference implementing CosEventComm::PushConsumer
        PortableServer::ServantBase_var   push_consumer_servant;  // local consumer servant implementing CosEventComm::PushConsumer
    };
    // Subscriber Connection Map: find subscriber by cookie
    typedef std::map<Components::Cookie_var, SubscriberInfo, std::less<Components::Cookie_var> > SubscriberMap;

    struct PublisherInfo
    {
        std::string                    rep_id;         // consumer repository id
        Event::EventChannelInterface*  channel;        // event channel
        CosEventComm::PushConsumer_var proxy_consumer; // proxy push consumer used to push event in event channel
        SubscriberMap                  subscriptions;  // connection info
    };
    // find publisher by its name
    typedef std::map<std::string, PublisherInfo, std::less<std::string> > PublisherMap;



    FacetMap        m_facets;       // map of component facet
    ReceptacleMap   m_receptacles;  // map of component receptacle
    ConsumerMap     m_consumers;    // map of component event sink
    EmitterMap      m_emitters;     // map of component source (emitter)
    PublisherMap    m_publishers;   // map of component source (publisher)

    bool            m_is_removed;   // true if remove operation has been called

    bool            m_is_monolithic; // true if component has only one servant for
                                     // it and its facets and consumers.

#ifdef CDMW_USE_FAULTTOLERANCE
    CdmwFT::Current_var                   m_ft_current;
    CdmwFT::Location::GroupRepository_var m_ft_group_rep;
    CdmwFT::ReplicationManager_var        m_ft_rep_mng;
#endif


};  // End CCMObject_impl class

} // End namespace CIF

} // End namespace CCM

} // End namespce Cdmw


#endif  // INCL_CCM_CCMOBJECT_IMPL_HPP

