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


#ifndef INCL_PRIMARY_ACTIVATION_HANDLER_HPP 
#define INCL_PRIMARY_ACTIVATION_HANDLER_HPP 

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include <FaultTolerance/idllib/CdmwFTActivation.skel.hpp>

#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include "SystemMngt/platformlibrary/DataStoreBaseDefinition.hpp"

#include "SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp"

#include <string>
#include <list>

namespace Cdmw 
{
namespace PlatformMngt 
{
    
class CentralisedSupervisionSystem_impl;
class SynchroProcessing;
class PrimaryActivationHandler_impl;



/**
 *Purpose: Dumper of the event notification from datastore
 *<p>
 */
class EventNotifDumper : 
   public std::unary_function <std::pair <
              OrbSupport::type_traits <CdmwPlatformMngtBase::EventKey>::_var_type, 
              OrbSupport::type_traits <CdmwPlatformMngt::Event>::_var_type> , void>
{
    
public:

    typedef std::pair <
              OrbSupport::type_traits <CdmwPlatformMngtBase::EventKey>::_var_type, 
              OrbSupport::type_traits <CdmwPlatformMngt::Event>::_var_type> 
                     PairType;
                     
    /**
     *Purpose:
     *<p> constructor.
     *
     *@param synchro_proc         reference of synchro processing.
     */
    EventNotifDumper(SynchroProcessing* synchro_proc) : m_synchro_proc(synchro_proc)
    { 
    }

    /**
     *Purpose:
     *<p> operator () called by datastore interface.
     *
     *@param pair of <oid, record>
     */
    void operator()(const PairType &elt);

private:

    SynchroProcessing* m_synchro_proc;
};

/**
 *Purpose: Dumper of the event report from datastore
 *<p>
 */
class EventReportDumper : 
   public std::unary_function <std::pair <
              OrbSupport::type_traits <CdmwPlatformMngtBase::EventKey>::_var_type, 
              OrbSupport::type_traits <CdmwPlatformMngt::EventReportRecord>::_var_type> , void>
{
    
public:

    typedef std::pair <
              OrbSupport::type_traits <CdmwPlatformMngtBase::EventKey>::_var_type, 
              OrbSupport::type_traits <CdmwPlatformMngt::EventReportRecord>::_var_type> 
                     PairType;

    /**
     *Purpose:
     *<p> constructor.
     *
     *@param synchro_proc         reference of synchro processing.
     */
    EventReportDumper(SynchroProcessing* synchro_proc) : m_synchro_proc (synchro_proc)
    { 
    }

    /**
     *Purpose:
     *<p> operator () called by datastore interface.
     *
     *@param pair of <oid, record>
     */
    void operator()(const PairType &elt);

private:

    SynchroProcessing* m_synchro_proc;
};


/**
*Purpose:
*<p>    This class implement the synchro processing of supervision
*       after the primary switch
*
*/
class SynchroProcessing : public OsSupport::Thread
{

public:

   /**
    * Purpose:
    * <p> Constructor
    * 
    *@param orb the orb reference
    *@param system_accessor_ref the reference of the system accessor member
    *@param system the reference of system. 
    *@param event_notif_ds the event notification datastore
    *@param event_report_ds the event report datastore
    *@param handler the primary activation handler
    */ 
    SynchroProcessing (CORBA::ORB_ptr orb,
                       CdmwFTSystMngt::SystemAccessor_ptr system_accessor_ref,
                       CentralisedSupervisionSystem_impl* system,
                       EventNotifierDataStore* event_notif_ds,
                       EventReportDataStore* event_report_ds,
                       PrimaryActivationHandler_impl* handler);

   /**
    * Purpose:
    * <p> Destructor
    * 
    */ 
    virtual 
    ~SynchroProcessing()
            throw();
   
   
   /**
    * Purpose:
    * <p> Add Event notif still in datastore
    * 
    *@param event_notif the event notification
    */
   void add_event_notif(CdmwPlatformMngt::Event * event_notif);
   
            
   /**
    * Purpose:
    * <p> Add Event report still in datastore
    * 
    *@param event_key    the event key
    *@param event_report the event report record
    */
   void add_event_report(CdmwPlatformMngtBase::EventKey event_key,
                         CdmwPlatformMngt::EventReportRecord * event_report);




protected:


    /**
    * Purpose:
    * <p> Call by the underlying thread system in a new thread
    * 
    */ 
    virtual
    void run()
        throw();

private:

    /**
    * ORB reference
    */ 
    CORBA::ORB_var m_orb;
    
    /**
    * the reference of the system accessor member
    */
    CdmwFTSystMngt::SystemAccessor_var m_system_accessor_ref;

    /**
    * The centralised system
    */
    CentralisedSupervisionSystem_impl* m_system;
    
    /**
    * Event report datastore
    */
    EventNotifierDataStore* m_event_notif_ds;
    
    /**
    * Event report datastore
    */
    EventReportDataStore* m_event_report_ds;
    
    /**
    * Primary activation handler
    */
    PrimaryActivationHandler_impl* m_handler;
    
    /**
    * Event notif list
    */
    typedef std::list<CdmwPlatformMngt::Event_var>  EventNotifList;
    
    EventNotifList m_event_notif_list;
    
    /**
    * Event key list
    */
    typedef std::list<CdmwPlatformMngtBase::EventKey>  EventKeyList;
    
    EventKeyList m_event_key_list;
        
    /**
    * Event report list
    */
    typedef std::list<CdmwPlatformMngt::EventReportRecord *>  EventReportList;
    
    EventReportList m_event_report_list;
        

}; // End class SynchroProcessing 


//
// IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler:1.0
//

class PrimaryActivationHandler_impl : virtual public ::CdmwFT::Location::ActivationHandler,
                                      virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    PrimaryActivationHandler_impl(const PrimaryActivationHandler_impl&);
    
    void operator=(const PrimaryActivationHandler_impl&);

public:

    /**
    * Purpose:
    * <p> Constructor
    * 
    *@param orb the orb reference
    *@param system_accessor_ref the reference of the system accessor member
    *@param system the reference of system. 
    */ 
    PrimaryActivationHandler_impl(CORBA::ORB_ptr orb,
                                  CdmwFTSystMngt::SystemAccessor_ptr system_accessor_ref,
                                  CentralisedSupervisionSystem_impl* system);
    
    /**
    * Purpose:
    * <p> Destructor
    * 
    */
    ~PrimaryActivationHandler_impl();

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/activate:1.0
	* operation
	*/
    virtual void activate()
        throw(CORBA::SystemException);

    /**
	* Purpose:
	* <p>
	* implements the
	* IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/deactivate:1.0
	* operation
	*/
    virtual void deactivate()
        throw(CORBA::SystemException);
    
    /**
	* Purpose:
	* <p>
	* set the synchronisation ready to start
    * the activation handler can return to its caller
	*/   
    void set_synchro_ready();

private:

    /**
     * The centralised system
     */
    CentralisedSupervisionSystem_impl* m_system;
    
    /**
     * Event report datastore
     */
    EventNotifierDataStore* m_event_notif_ds;
    
    /**
     * Event report datastore
     */
    EventReportDataStore* m_event_report_ds;
    
    /**
     * Synchro processing thread
     */
    SynchroProcessing* m_synchro_processing;
    
    /**
    * The synchro ready flag.
    */
    bool m_sync_ready;
    
    /**
    * The lock protecting concurrent access to synchro ready flag.
    */
    OsSupport::Mutex m_sync_ready_mutex;
    
    /**
    * The condition on the ready flag.
    */
    OsSupport::Condition m_sync_ready_condition;
    
};

} // End of namespace PlatformMngt
} // End of namespace Cdmw

#endif // INCL_PRIMARY_ACTIVATION_HANDLER_HPP 

