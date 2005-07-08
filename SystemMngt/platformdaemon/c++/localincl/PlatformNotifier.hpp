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


#ifndef INCL_PLATFORMMNGT_PLATFORMNOTIFIER_HPP 
#define INCL_PLATFORMMNGT_PLATFORMNOTIFIER_HPP 

#include "SystemMngt/platformlibrary/EventNotifier.hpp"
#include "idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "idllib/CdmwPlatformMngtPlatformObserver.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
* The PlatformInfo deallocator.
*/
class PlatformInfoDeallocator
{

    public:

        /**
        * Purpose:
        * <p>
        * Deallocates the event.
        */ 
        static void deallocate(CdmwPlatformMngt::PlatformInfo* event)
            throw ()
        {
            delete event;
        }

};



/**
*Purpose:
* The actual notifier of PlatformInfo events.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class PlatformNotifier : virtual public EventNotifier<
    CdmwPlatformMngt::PlatformInfo, PlatformInfoDeallocator,
    CdmwPlatformMngt::PlatformObserver_ptr, CdmwPlatformMngt::PlatformObserver>
{


    public:


        /**
        * Purpose:
        * <p> Constructor.
        */ 
        PlatformNotifier(unsigned long callTimeout)
            throw (InternalErrorException);


        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~PlatformNotifier() throw();


    protected:

        /**
         *Purpose:
         *<p> Returns the string description of the event used for
         * logging.
         */
        virtual std::string eventToString(CdmwPlatformMngt::PlatformInfo* event);

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
            CdmwPlatformMngt::PlatformObserver_ptr observer,
            CdmwPlatformMngt::PlatformInfo* event)
            throw(OutOfMemoryException);


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        PlatformNotifier(const PlatformNotifier& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        PlatformNotifier& operator=(const PlatformNotifier& rhs);

    public:

        /**
        * The allowed timeout for each notification call.
        */
        unsigned long m_callTimeout;

        /**
        * The factory of bound synchronous calls.
        */
        BoundSyncCallFactory* m_boundSyncCallFactory;


}; // End class PlatformNotifier 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PLATFORMNOTIFIER_HPP

