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


#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <fstream>
#include <sys/time.h>

#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"

#include "SupervisionObserverPerf_impl.hpp"

/**
 * @author Antonello Cedrone <acedrone@vitrociset.it>
 * @author Raffaele Mele <rmele@progesi.it>
 */

namespace Cdmw
  {

  namespace PlatformMngt
    { //

    void
    SupervisionObserverPerf_impl::notify( CdmwPlatformMngt::Event* an_event )
    throw( CORBA::SystemException )
    {


      try
        {
          CdmwPlatformMngt::ProcessStatusChange* prc_chg = dynamic_cast<CdmwPlatformMngt::ProcessStatusChange*> (an_event);

          if (((prc_chg!=0))&&(prc_chg->process_status()==8))
            {
              struct timeval tv;
              struct timezone tz;
              gettimeofday(&tv,&tz);
              std::ofstream crash_obs("time2observer.dat",std::ios::out|std::ios::app);
              if (!crash_obs) 
				std::cerr << "*** ERROR IN WRITING FILE time2observer.dat ***" << std::endl;
			  else
				std::cerr << "*** WRITING FILE time2observer.dat ***" << std::endl;

              crash_obs <<"Sec " << tv.tv_sec <<" Usec " << tv.tv_usec <<std::endl;
              crash_obs.close();
            }
        }
      catch (const std::exception& ex)
        {
          std::cerr << " Perf observer exception  " << ex.what();
        }

    }

  } // PlatformMngt
} // Cdmw

