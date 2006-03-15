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


#ifndef INCL_CCM_LB_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP
#define INCL_CCM_LB_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP


#include <string>

#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <LoadBalancing/idllib/CdmwLBGroupManager.stub.hpp>


class GroupCreatorProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{
    public:
        // Constructor
        GroupCreatorProcessBehaviour(CORBA::ORB_ptr orb)
            throw (CORBA::SystemException)
                : m_orb(CORBA::ORB::_duplicate(orb))
        {
        }

        // Destructor
        virtual ~GroupCreatorProcessBehaviour() throw ()
        {
        }

        virtual CORBA::ULong nb_steps()
            throw (CORBA::SystemException)
        {
            return 1;
        }

        /**
         * This callback operation is called whenever the CDMW supervision
         * invokes the initialise operation on the process behaviour object.
         */
        virtual void
        on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw (CORBA::SystemException);

        virtual void
        on_run()
            throw (CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
                   CORBA::SystemException)
        {
            std::cout << "   -------- GroupCreator run --------" << std::endl;
            // nothing to do
        }

        virtual void
        on_stop()
            throw (CORBA::SystemException)
        {
            std::cout << "   -------- GroupCreator stop --------" << std::endl;
        }

    private:
        ::PortableGroup::ObjectGroup_ptr
        create_object_group(const char* repositoryId)
            throw (CORBA::SystemException);

        void write_ior_file(CORBA::Object_ptr obj,
                            const char* filename);

        // The ORB.
        CORBA::ORB_var m_orb;

        CdmwLB::LBGroupManager_var m_lbgroup_mgr;
};


#endif // INCL_CCM_LB_GROUP_CREATOR_PROCESS_BEHAVIOUR_HPP
