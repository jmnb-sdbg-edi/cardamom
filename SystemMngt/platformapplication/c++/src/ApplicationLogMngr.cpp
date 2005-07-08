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


#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/CommonLogMessageId.hpp"
#include "SystemMngt/platformapplicationlibrary/ApplicationLogMessageId.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

const char* LogMngr::getService()
{
    return "PFMAPP";
}

const char* LogMngr::getMessage( long messageId )
{
    if( (messageId >= 0)  && (messageId < MSG_ID_APPLICATION_END) )
        return ApplicationLogMessages::M_applicationMessages[ messageId ];
    else if( (messageId >= MSG_ID_COMMON_START) && (messageId < MSG_ID_COMMON_END) )
        return CommonLogMessages::M_commonMessages[ messageId - MSG_ID_COMMON_START ];
    else
        return NULL;
}

} // End namespace PlatformMngt
} // End namespace Cdmw

