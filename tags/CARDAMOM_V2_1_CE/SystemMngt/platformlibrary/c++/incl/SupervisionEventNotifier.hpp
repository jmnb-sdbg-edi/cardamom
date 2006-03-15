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


#ifndef INCL_PLATFORMMNGT_SUPERVISION_EVENTNOTIFIER_HPP 
#define INCL_PLATFORMMNGT_SUPERVISION_EVENTNOTIFIER_HPP 

#include "SystemMngt/platformlibrary/EventNotifier.hpp"
#include "SystemMngt/platformlibrary/DataStoreBaseDefinition.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtSupervisionObserver.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"




namespace Cdmw
{
namespace PlatformMngt
{


/**
* The supervision event deallocator.
*/
class SupervisionEventDeallocator
{

    public:

        /**
        * Purpose:
        * <p>
        * Deallocates the event.
        */ 
        static void deallocate(CdmwPlatformMngt::Event* event)
            throw ()
        {
            CORBA::remove_ref(event);
        }

};



/**
*Purpose:
* The notifier of supervision events.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class SupervisionEventNotifier : virtual public EventNotifier<
    CdmwPlatformMngt::Event, SupervisionEventDeallocator,
    CdmwPlatformMngt::SupervisionObserver_ptr, CdmwPlatformMngt::SupervisionObserver>
{


    public:


        /**
        * Purpose:
        * <p> Constructor.
        */ 
        SupervisionEventNotifier(unsigned long callTimeout)
            throw (InternalErrorException);


        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~SupervisionEventNotifier() throw();


    protected:

        /**
         *Purpose:
         *<p> Returns the string description of the event used for
         * logging.
         */
        virtual std::string eventToString(CdmwPlatformMngt::Event* event);

        /**
         *Purpose:
         *<p> Creates an event notification call corresponding to the event
         * and the observer.
         *
         * @param observer The observer to notify.
         * @param event The event to notify.
         * @return the bound synchronous call.
         *
         * @exception OutOfMemoryException
         */
        virtual BoundSyncCall* createEventNotificationCall(
            CdmwPlatformMngt::SupervisionObserver_ptr observer,
            CdmwPlatformMngt::Event* event)
            throw(OutOfMemoryException);
            
        /**
         *Purpose:
         *<p> Insert the event in DataStore.
         *
         *@param event The event to be placed in the data store.
         *
         */
        virtual void insertEventInDataStore (CdmwPlatformMngt::Event* event);
    
        /**
         *Purpose:
         *<p> remove the event from DataStore.
         *
         *@param event_key The key of event to remove from the data store.
         *
         */
        virtual void removeEventInDataStore (CdmwPlatformMngtBase::EventKey event_key);


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        SupervisionEventNotifier(const SupervisionEventNotifier& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        SupervisionEventNotifier& operator=(const SupervisionEventNotifier& rhs);

    public:

        /**
        * The allowed timeout for each notification call.
        */
        unsigned long m_callTimeout;

        /**
        * The factory of bound synchronous calls.
        */
        BoundSyncCallFactory* m_boundSyncCallFactory;
        
    private:
          
        /**
        * Event report datastore
        */
        EventNotifierDataStore* m_event_notif_ds;



}; // End class SupervisionEventNotifier 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_SUPERVISION_EVENTNOTIFIER_HPP

