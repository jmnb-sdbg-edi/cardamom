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


// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <FaultTolerance/ftinit/FTServiceInit.hpp>
#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include <tao/ORBInitializer_Registry.h>
#endif

#include <Foundation/logging/LogManager.hpp>


// Local Files
#include <ftinit/FTORBInitializer.hpp>

#include <FaultTolerance/ftcommon/FTConfiguration.hpp>

namespace Cdmw
{
    
namespace FT
{

::Cdmw::FT::ReplyRecording* FTServiceInit::M_recorder = 0;

void
FTServiceInit::Init (int& argc, char** argv, bool client_only)
    throw(CORBA::SystemException)
{
    // Init the Logging library
    Cdmw::Logging::LogManager::Init(argc, argv);

    FTConfiguration::Activate_FT_service(argc, argv);

    if (client_only == false)
    {
        // register_interceptors();

        PortableInterceptor::ORBInitializer_var initializer 
            = new Cdmw::FT::FTInit::ORBInitializer();
        
        PortableInterceptor::register_orb_initializer(initializer.in());

        if (M_recorder)
        {
            FTInit::ORBInitializer::Set_reply_recorder(M_recorder);
        }
    }
    else
    {
        // do nothing!
    }
}

void
FTServiceInit::Set_reply_recorder(Cdmw::FT::ReplyRecording* recorder)
{
    M_recorder = recorder;
    FTInit::ORBInitializer::Set_reply_recorder(M_recorder);
}

} // End of namespace FT

} // End of namespace Cdmw



