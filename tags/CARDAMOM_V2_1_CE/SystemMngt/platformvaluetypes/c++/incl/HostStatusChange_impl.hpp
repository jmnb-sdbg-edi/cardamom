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


#ifndef INCL_PLATFORMMNGT_HOST_STATUS_CHANGE_IMPL_HPP
#define INCL_PLATFORMMNGT_HOST_STATUS_CHANGE_IMPL_HPP

#include "idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/platformvaluetypes/Event_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt

        /**
         *Purpose:
         *<p> A factory class for creating CdmwPlatformMngt::HostStatusChange events.
         *
         *Features:
         *<p> [Thread safety, exception safety, performance]
         *    [if no pertinent write none ]
         *
         *@see  [if necessary] 
         *@link [if necessary] 
         *
         */

        class HostStatusChangeFactory : public virtual CdmwPlatformMngt::HostStatusChange_init
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
                 * <p> Creates a CdmwPlatformMngt::HostStatusChange with the given properties
                 * provided as parameters.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param host_name The name of the host.
                 *@param host_status Its status .
                 *
                 *@return the newly created CdmwPlatformMngt::HostStatusChange object.
                 */
                virtual CdmwPlatformMngt::HostStatusChange* create(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* hostName,
                    CdmwPlatformMngt::HostStatus hostStatus );
        };

        /**
         *Purpose:
         *<p> An implementation class.
         *
         *Features:
         *<p> [Thread safety, exception safety, performance]
         *    [if no pertinent write none ]
         *
         *@see  [if necessary] 
         *@link [if necessary] 
         *
         */

        class HostStatusChange_impl : public virtual OBV_CdmwPlatformMngt::HostStatusChange,
                    public virtual Event_impl
        {

            protected:
                /**
                 * Purpose:
                 * <p> The default constructor.
                 */
                HostStatusChange_impl();

                /**
                 * Purpose:
                 * <p> Creates a CdmwPlatformMngt::HostStatusChange_impl with the given properties
                 * provided as parameters.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param host_name The name of the host.
                 *@param host_status Its status .
                 */
                HostStatusChange_impl(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* hostName,
                    CdmwPlatformMngt::HostStatus hostStatus );

                friend class HostStatusChangeFactory;

            public:
                /**
                 * Inherited from CORBA::ValueBase
                 */
                virtual CORBA::ValueBase* _copy_value();

                /**
                 * Purpose:
                 * <p> Provides the description of this event.
                 * 
                 *@return The description string of this event.
                 */
                virtual char* to_string();
        };

    } // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_HOST_STATUS_CHANGE_IMPL_HPP

