/* =========================================================================== *
 * Copyright (c) 2003-2005 THALES All rights reserved.
 * Software commonly developed by THALES and AMS.
 *
 * This file is part of CARDAMOM.
 *
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with CARDAMOM; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


#include "starttime/FTManagerStarter.hpp"

#include <iostream>
#include <iomanip>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>




namespace Cdmw
{

static const std::string FT_MANAGER_ARGS = 
" --CdmwXMLFile=CdmwFaultToleranceManager_conf.xml  --groupConf=CdmwFTSystemMngtGroupCreator_conf.xml";
    
const std::string FTManagerStarter::FT_MANAGER_EXE = 
    "cdmw_ft_manager";    
const std::string FTManagerStarter::FT_MANAGER_CORBALOC = 
    "corbaloc::localhost:4555/fault_manager";
            
            
FTManagerStarter::FTManagerStarter(const std::string& args,
                                   CORBA::ORB_ptr orb)
    : ProcessStarter(FT_MANAGER_EXE, args),
      m_orb(CORBA::ORB::_duplicate(orb))
{
}
    
    
    
bool FTManagerStarter::check_if_started() 
    throw (ProcessError)
{
    try {
        CORBA::Object_var obj = m_orb->string_to_object(FT_MANAGER_CORBALOC.c_str());
        
        CdmwFT::FTManager_var ft_manager = 
            CdmwFT::FTManager::_narrow(obj.in());
        
        CdmwFT::ReplicationManager_var rep_manager = 
            ft_manager->get_replication_manager();
        
        set_as_started();
        
    } catch (const FT::ObjectGroupNotFound& e) {
        std::cout << "FT::ObjectGroupNotFound exception received while get_replication_manager() !"
                  << std::endl;
    } catch (const CdmwFT::FTManager::NoAlreadyActivated& e) {
        // not yet started
    } catch (const CORBA::SystemException& e) {
        // not yet started        
    }
    
    return m_is_started;
}


}  // namespace

