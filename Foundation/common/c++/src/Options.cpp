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


#include "Foundation/common/Options.hpp"


namespace Cdmw
{

    namespace Common
    {

        /**
         *Implementation notes:  none 
         *<p>
         *Portability issues: none
         *<p>
         */
        const char* Options::CALLBACK_IOR_OPTION = "--CdmwCallbackIor";
        const char* Options::PROCESS_NAME_OPTION = "--CdmwProcessName";
        const char* Options::PROCESS_PORT_OPTION = "--CdmwProcessPort";
        const char* Options::PROCESS_XML_FILE_OPTION = "--CdmwXMLFile";
        const char* Options::VALIDATE_XML_OPTION = "--validateXML";
        const char* Options::LOCALISATION_SERVICE_OPTION = "--CdmwLocalisationService";
        const char* Options::LOG_FILE_OPTION = "--CdmwLogFile";
        const char* Options::DEFAULT_HOME_REGISTRATION_OPTION = "--DefaultHomeRegistration";
        const char* Options::SYSTEM_REGISTRATION_OPTION = "--CdmwSystemRegistration";
        const char* Options::FT_DOMAIN_OPTION = "--FTDomain";
        const char* Options::REQUEST_DURATION_TIME = "--RequestDurationTime";
        


    } // End namespace Common
} // End namespace Cdmw

