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


#ifndef INCL_COMMON_OPTIONS_HPP
#define INCL_COMMON_OPTIONS_HPP


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
        *<p>    This class provides standard CDMW options for command lines.
        *
        *Features:
        *<p>    none
        *
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class Options
        {

            public:

                /**
                 * Command Line option for CDMW Callback IOR.
                 * Used by Application processes, ApplicationAgent processes
                 * and managed processes to acknowledge their creation.
                 */
                static const char* CALLBACK_IOR_OPTION;

                /**
                 * Command Line option for CDMW Process Name
                 */
                static const char* PROCESS_NAME_OPTION;

                /**
                 * Command Line option for CDMW Process Port
                 */
                static const char* PROCESS_PORT_OPTION;

                /**
                 * Command Line option for CDMW XML file for use during Init of CDMW Processes
                 */
                static const char* PROCESS_XML_FILE_OPTION;

                /**
                 * Command Line option for validating XML files
                 * (By default, XML files are not validated)
                 */
                static const char* VALIDATE_XML_OPTION;

                /**
                 * Command Line option for Cdmw Localisation Service location
                 * (Current option only holds the port number for use for binding
                 * objects for corbaloc keys)
                 */
                static const char* LOCALISATION_SERVICE_OPTION;

                /**
                 * Command Line option for specifying the full path to a log file
                 */
                static const char* LOG_FILE_OPTION;

                /**
                 * Command Line option for specifying the use of DefaultHomeRegistration instead
                 * of the CDMW StandardHomeRegistration.
                 */
                static const char* DEFAULT_HOME_REGISTRATION_OPTION;

                /**
                 * Command Line option for specifying the corbaloc of CdmwPlaformManagementSystem
                 * used to register an observer.
                 */
                static const char* Options::SYSTEM_REGISTRATION_OPTION;


                /**
                 * Command Line option for specifying the FT Domain
                 */
                static const char* Options::FT_DOMAIN_OPTION;

            
                /**
                 * Command Line option for specifying the request duration time
                 */
                static const char* Options::REQUEST_DURATION_TIME;
            
            protected:


            private:


        }

        ; // End class Options

    } // End namespace Common
} // End namespace Cdmw

#endif // INCL_COMMON_OPTIONS_HPP

