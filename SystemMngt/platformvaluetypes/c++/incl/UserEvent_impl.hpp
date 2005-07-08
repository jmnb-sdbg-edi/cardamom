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


#ifndef INCL_USER_EVENT_IMPL_HPP
#define INCL_USER_EVENT_IMPL_HPP

#include "idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/platformvaluetypes/Event_impl.hpp"


namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         *Purpose:
         *<p> A factory class for creating UserEvent events.
         *
         *Features:
         *<p> [Thread safety, exception safety, performance]
         *    [if no pertinent write none ]
         *
         *@see  [if necessary] 
         *@link [if necessary] 
         *
         */

        class UserEventFactory : public virtual CdmwPlatformMngt::UserEvent_init
        {

            private:
                /**
                 * Purpose:
                 * <p> Inherited method used for unmarshalling factory object.
                 * 
                 *@return The unmarshalled factory.
                 */
                virtual CORBA::ValueBase* create_for_unmarshal();

            public:
                /**
                 * Purpose:
                 * <p> Creates a UserEvent.
                 * 
                 *@param info The information about the event.
                 *
                 *@return the newly created UserEvent object.
                 */
                virtual CdmwPlatformMngt::UserEvent* create(
                    const CdmwPlatformMngtBase::EventHeader& header,
                    const char* system_name,
                    const char* application_name,
                    const char* process_name,
                    const char* issuer,
                    const CORBA::Any& data );

        };

        /**
         *Purpose:
         *<p> The implementation of the UserEvent value type.
         *
         *Features:
         *<p> [Thread safety, exception safety, performance]
         *    [if no pertinent write none ]
         *
         *@see  [if necessary] 
         *@link [if necessary] 
         *
         */

        class UserEvent_impl : public virtual OBV_CdmwPlatformMngt::UserEvent,
                    public virtual Event_impl
        {

            private:
                /**
                 * The issuer of the event.
                 */
                std::string m_issuer;

            protected:
                /**
                 * Purpose:
                 * <p> The default constructor.
                 */
                UserEvent_impl();

                /**
                 * Purpose:
                 * <p> Creates a UserEvent_impl.
                 */
                UserEvent_impl(
                    const CdmwPlatformMngtBase::EventHeader& theHeader,
                    const char* systemName,
                    const char* applicationName,
                    const char* processName,
                    const char* issuer,
                    const CORBA::Any& theData );

                friend class UserEventFactory;

            public:
                /**
                 * Inherited from CORBA::ValueBase
                 */
                virtual CORBA::ValueBase* _copy_value();

                /**
                 * IDL:thalesgroup.com/CdmwPlatformMngt/Event/get_issuer:1.0
                 */
                virtual char* get_issuer();

                /**
                 * IDL:thalesgroup.com/CdmwPlatformMngt/Event/to_string:1.0
                 */
                virtual char* to_string();
        };

    } // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_USER_EVENT_IMPL_HPP

