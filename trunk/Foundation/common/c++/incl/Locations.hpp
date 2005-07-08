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


#ifndef INCL_COMMON_LOCATIONS_HPP
#define INCL_COMMON_LOCATIONS_HPP

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for common types used by CDMW runtime.
    */

    namespace Common
    {

        /**
        *Purpose:
        *<p>    This class provides standard CDMW paths for CDMW services.
        *
        *Features:
        *<p>    none
        *
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class Locations
        {

            public:

                /**
                 * Name of the CDMW Admin Root Context within the CDMW 
                 * Naming and Repository service.
                 */
                static const char* ADMIN_ROOT_CONTEXT_ID;

                /**
                 * Name of the CDMW NameDomain where CDMW services are registered
                 * within the CDMW Naming and Repository DefaultRootContext.
                 */
                static const char* CDMW_SERVICES_NAME_DOMAIN;

                /**
                 * Name of the CDMW NameDomain where CDMW administration objects
                 * are registered within the CDMW Naming and Repository DefaultRootContext.
                 */
                static const char* CDMW_ADMIN_NAME_DOMAIN;


                /**
                 * NameDomain Location of CDMW Trace Collectors objects within the CDMW 
                 * Services Name Domain.
                 */
                static const char* TRACE_COLLECTORS_NAME_DOMAIN;


                /**
                 * NameDomain Location of CDMW event administration objects 
                 */
                static const char* CDMW_EVENT_SERVICE_NAME_DOMAIN;

                /**
                 * Name of the CDMW NameDomain where CDMW objects related
                 * to the Assembly and the Deployment are registered
                 * within the CDMW Naming and Repository service.
                 */
                static const char* ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN;

            protected:


            private:


        }

        ; // End class Locations

    } // End namespace Common
} // End namespace Cdmw

#endif // INCL_COMMON_LOCATIONS_HPP

