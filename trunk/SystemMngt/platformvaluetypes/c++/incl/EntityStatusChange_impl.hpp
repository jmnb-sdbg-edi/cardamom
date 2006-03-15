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


#ifndef INCL_PLATFORMMNGT_ENTITY_STATUS_CHANGE_IMPL_HPP
#define INCL_PLATFORMMNGT_ENTITY_STATUS_CHANGE_IMPL_HPP

#include <string>
#include "idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/platformvaluetypes/Event_impl.hpp"

namespace Cdmw
{        // Begin namespace Cdmw

    namespace PlatformMngt
    { // Begin namespace PlatformMngt


        /**
         *Purpose:
         *<p> A factory class for creating CdmwPlatformMngt::SystemEntityStatusChange events.
         */

        class SystemEntityStatusChangeFactory : public virtual CdmwPlatformMngt::SystemEntityStatusChange_init
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
                 * <p> Creates a CdmwPlatformMngt::SystemEntityStatusChange with the given properties
                 * provided as parameters. Use this method with a process's entity event.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param application_name The application's name.
                 *@param process_name The name of the process.
                 *@param entity_name The entity's name.
                 *@param entity_status Its status .
                 *@param info Some additional information.
                 *
                 *@return the newly created CdmwPlatformMngt::SystemEntityStatusChange object.
                 */
                virtual CdmwPlatformMngt::SystemEntityStatusChange* create(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemNname,
                    const char* entityName,
                    const char* entityStatus,
                    const char* theInfo );
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

        class SystemEntityStatusChange_impl :
                    public virtual OBV_CdmwPlatformMngt::SystemEntityStatusChange,
                    public virtual Event_impl
        {

                friend class SystemEntityStatusChangeFactory;

            protected:
                /**
                 * Purpose:
                 * <p> The default constructor.
                 */
                SystemEntityStatusChange_impl();

                /**
                 * Purpose:
                 * <p> Creates a CdmwPlatformMngt::SystemEntityStatusChange_impl with the given properties
                 * provided as parameters. Use this method with a process's entity event.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param entity_name The entity's name.
                 *@param entity_status Its status .
                 *@param info Some additional information.
                 */
                SystemEntityStatusChange_impl(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* entityName,
                    const char* entityStatus,
                    const char* theInfo );


                /**
                 * Purpose:
                 * <p> Returns the end of the description string containing :
                 * "E:'entity_name' entity_status : info"
                 */
                std::string endString();

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



        /**
         *Purpose:
         *<p> A factory class for creating CdmwPlatformMngt::HostEntityStatusChange events.
         */

        class HostEntityStatusChangeFactory : public virtual CdmwPlatformMngt::HostEntityStatusChange_init
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
                 * <p> Creates a CdmwPlatformMngt::HostEntityStatusChange with the given properties
                 * provided as parameters. Use this method with a process's entity event.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param host_name Name of the host issuing this status change event.
                 *@param entity_name The entity's name.
                 *@param entity_status Its status .
                 *@param info Some additional information.
                 *
                 *@return the newly created CdmwPlatformMngt::SystemEntityStatusChange object.
                 */
                virtual CdmwPlatformMngt::HostEntityStatusChange* create(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* hostName,
                    const char* entityName,
                    const char* entityStatus,
                    const char* theInfo );
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

        class HostEntityStatusChange_impl :
                    public virtual OBV_CdmwPlatformMngt::HostEntityStatusChange,
                    public virtual SystemEntityStatusChange_impl
        {

            protected:
                /**
                 * Purpose:
                 * <p> The default constructor.
                 */
                HostEntityStatusChange_impl();

                /**
                 * Purpose:
                 * <p> Creates a CdmwPlatformMngt::HostEntityStatusChange_impl with the given properties
                 * provided as parameters. Use this method with a process's entity event.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param host_name The host's name.
                 *@param entity_name The entity's name.
                 *@param entity_status Its status .
                 *@param info Some additional information.
                 */
                HostEntityStatusChange_impl(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* hostName,
                    const char* entityName,
                    const char* entityStatus,
                    const char* theInfo );
                    
                friend class HostEntityStatusChangeFactory;


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

        /**
         *Purpose:
         *<p> A factory class for creating CdmwPlatformMngt::ApplicationEntityStatusChange events.
         */

        class ApplicationEntityStatusChangeFactory : public virtual CdmwPlatformMngt::ApplicationEntityStatusChange_init
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
                 * <p> Creates a CdmwPlatformMngt::ApplicationEntityStatusChange with the given properties
                 * provided as parameters. Use this method with a process's entity event.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param application_name The application's name.
                 *@param process_name The name of the process.
                 *@param entity_name The entity's name.
                 *@param entity_status Its status .
                 *@param info Some additional information.
                 *
                 *@return the newly created CdmwPlatformMngt::ApplicationEntityStatusChange object.
                 */
                virtual CdmwPlatformMngt::ApplicationEntityStatusChange* create(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* applicationName,
                    const char* entityName,
                    const char* entityStatus,
                    const char* theInfo );
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

        class ApplicationEntityStatusChange_impl :
                    public virtual OBV_CdmwPlatformMngt::ApplicationEntityStatusChange,
                    public virtual SystemEntityStatusChange_impl
        {

            protected:
                /**
                 * Purpose:
                 * <p> The default constructor.
                 */
                ApplicationEntityStatusChange_impl();

                /**
                 * Purpose:
                 * <p> Creates a CdmwPlatformMngt::ApplicationEntityStatusChange_impl with the given properties
                 * provided as parameters. Use this method with a process's entity event.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param application_name The application's name.
                 *@param process_name The name of the process.
                 *@param entity_name The entity's name.
                 *@param entity_status Its status .
                 *@param info Some additional information.
                 */
                ApplicationEntityStatusChange_impl(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* applicationName,
                    const char* entityName,
                    const char* entity_status,
                    const char* theInfo );

                friend class ApplicationEntityStatusChangeFactory;

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




        /**
         *Purpose:
         *<p> A factory class for creating CdmwPlatformMngt::ProcessEntityStatusChange events.
         */

        class ProcessEntityStatusChangeFactory : public virtual CdmwPlatformMngt::ProcessEntityStatusChange_init
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
                 * <p> Creates a CdmwPlatformMngt::ProcessEntityStatusChange with the given properties
                 * provided as parameters. Use this method with a process's entity event.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param application_name The application's name.
                 *@param process_name The name of the process.
                 *@param entity_name The entity's name.
                 *@param entity_status Its status .
                 *@param info Some additional information.
                 *
                 *@return the newly created CdmwPlatformMngt::ProcessEntityStatusChange object.
                 */
                virtual CdmwPlatformMngt::ProcessEntityStatusChange* create(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* applicationName,
                    const char* processName,
                    const char* hostName,
                    const char* entityName,
                    const char* entityStatus,
                    const char* theInfo );
        };

        /**
         *Purpose:
         */

        class ProcessEntityStatusChange_impl :
                    public virtual OBV_CdmwPlatformMngt::ProcessEntityStatusChange,
                    public virtual ApplicationEntityStatusChange_impl
        {

            protected:
                /**
                 * Purpose:
                 * <p> The default constructor.
                 */
                ProcessEntityStatusChange_impl();

                /**
                 * Purpose:
                 * <p> Creates a CdmwPlatformMngt::ProcessEntityStatusChange_impl with the given properties
                 * provided as parameters. Use this method with a process's entity event.
                 * 
                 *@param system_name Name of the system issuing this status change event.
                 *@param application_name The application's name.
                 *@param process_name The name of the process.
                 *@param entity_name The entity's name.
                 *@param entity_status Its status .
                 *@param info Some additional information.
                 */
                ProcessEntityStatusChange_impl(
                    const CdmwPlatformMngtBase::EventHeader& aHeader,
                    const char* systemName,
                    const char* applicationName,
                    const char* processName,
                    const char* hostName,
                    const char* entityName,
                    const char* entityStatus,
                    const char* theInfo );

                friend class ProcessEntityStatusChangeFactory;

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

#endif //INCL_PLATFORMMNGT_ENTITY_STATUS_CHANGE_IMPL_HPP

