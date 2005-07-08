/* =========================================================================== *
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
 * =========================================================================== */


#ifndef INCL_CCM_FT_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 
#define INCL_CCM_FT_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP 

// Standard Files 
#include <string>

// Cdmw Files
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "FaultTolerance/idllib/FT.stub.hpp"


class GroupCreatorProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    // Constructor
    GroupCreatorProcessBehaviour(CORBA::ORB_ptr orb,
                                 std::string host1,
                                 std::string host2,
                                 std::string host3)
		throw (CORBA::SystemException)
		: m_orb(CORBA::ORB::_duplicate( orb )),
          m_host1(host1),
          m_host2(host2),
          m_host3(host3)
    {
    }

    // Destructor
    virtual ~GroupCreatorProcessBehaviour() throw ()
    {
    }

    
    virtual CORBA::ULong nb_steps()
		throw( CORBA::SystemException )
    {
        return 1;
    }

    /**
     * This callback operation is called whenever the CDMW supervision invokes
     * the initialise operation on the process behaviour object.
     */
    virtual void on_initialise(
        const CdmwPlatformMngtBase::StartupKind& startup_kind )
        throw( CORBA::SystemException );
   
    
    virtual void on_run()
    throw( CdmwPlatformMngt::Process::NotReadyToRun,
           CORBA::SystemException )
    {
        std::cout << "   -------- GroupCreator run --------" << std::endl;
        // nothing to do
    }

    
    virtual void on_stop()
    throw( CORBA::SystemException )
    {
        std::cout << "   -------- GroupCreator stop --------" << std::endl;
    }

    
private:

    
    FT::ObjectGroup_ptr create_object_group(const char* repositoryId,
                                            CORBA::UShort minumumNumberReplicas,
                                            bool withdatastore)
         throw (CORBA::SystemException);

         
    void write_ior_file(CORBA::Object_ptr obj,
                        const char* filename);

        
    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;

    std::string m_host1;
    std::string m_host2;
    std::string m_host3;

    /**
     * The replication manager.
     */
    FT::ReplicationManager_var m_replication_mgr;
    
    /**
     * NameDomain for groups registrations
     */
    CdmwNamingAndRepository::NameDomain_var m_ftgroupsDomain;

};


#endif // INCL_CCM_FT_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP


