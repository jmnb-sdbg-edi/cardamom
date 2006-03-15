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


#ifndef INCL_FT_CCM_CIF_TEST_NAVIGATION_WITH_FT_HPP
#define INCL_FT_CCM_CIF_TEST_NAVIGATION_WITH_FT_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <CCMContainer/idllib/Components.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>

#include "testftccmcif/ftccmcif_cif.stub.hpp"


namespace TestFtCcmCif
{

class TestNavigationWithFT : public Cdmw::TestUtils::Testable
{

    public:

        TestNavigationWithFT(CORBA::ORB_ptr orb,
                             FT::Location primaryLocation,
                             FT::Location backupLocation,
                             Components::Deployment::Container_ptr containerPrimary,
                             Components::Deployment::Container_ptr containerBackup)
            throw(CORBA::Exception);


    protected:

        virtual void do_tests();

    private:
        TestNavigationWithFT(const TestNavigationWithFT& rhs)
            throw();

        TestNavigationWithFT& operator=(const TestNavigationWithFT& rhs)
            throw();
            
        ComponentsModule::ComponentWithFacet_ptr 
        create_ComponentWithFacet(Components::Deployment::Container_ptr,
                                  ComponentsModule::ComponentWithFacet_ptr)
            throw (CORBA::Exception);
            
        ComponentsModule::ComponentWithConsumer_ptr 
        create_ComponentWithConsumer(Components::Deployment::Container_ptr,
                                     ComponentsModule::ComponentWithConsumer_ptr)
            throw (CORBA::Exception);
            
        void
        call_ConfigurationComplete(Components::CCMObject_ptr component)
            throw (CORBA::Exception);

        ComponentsModule::ComponentWithFacet_ptr
        create_ComponentWithFacetGroup()
            throw (CORBA::Exception);

        CORBA::Object_ptr
        addMembers_ToComponentWithFacetGroup(
                ComponentsModule::ComponentWithFacet_ptr componentWithFacetGroup,
                ComponentsModule::ComponentWithFacet_ptr componentWithFacetOnPrimary,
                ComponentsModule::ComponentWithFacet_ptr componentWithFacetOnBackup)
            throw (CORBA::Exception);

        ComponentsModule::ComponentWithConsumer_ptr
        create_ComponentWithConsumerGroup()
            throw (CORBA::Exception);

        ComponentsModule::ComponentWithConsumer_ptr
        addMembers_ToComponentWithConsumerGroup(
                ComponentsModule::ComponentWithConsumer_ptr componentWithConsumerGroup,
                ComponentsModule::ComponentWithConsumer_ptr componentWithConsumerOnPrimary,
                ComponentsModule::ComponentWithConsumer_ptr componentWithConsumerOnBackup)
            throw (CORBA::Exception);

        bool 
        isGroupReference(CORBA::Object_ptr obj)
            throw (CORBA::Exception);

        void 
        testNavigationInterface(ComponentsModule::ComponentWithFacet_ptr group,
                                ComponentsModule::ComponentWithFacet_ptr primary,
                                ComponentsModule::ComponentWithFacet_ptr backup)
            throw (CORBA::Exception);
    
        void 
        testTypedNavigationInterface(ComponentsModule::ComponentWithFacet_ptr group,
                                     ComponentsModule::ComponentWithFacet_ptr primary,
                                     ComponentsModule::ComponentWithFacet_ptr backup)
            throw (CORBA::Exception);
    
        void 
        testEventsInterface(ComponentsModule::ComponentWithConsumer_ptr group,
                            ComponentsModule::ComponentWithConsumer_ptr primary,
                            ComponentsModule::ComponentWithConsumer_ptr backup)
        throw (CORBA::Exception);
    
        void 
        testTypedEventsInterface(ComponentsModule::ComponentWithConsumer_ptr group,
                                 ComponentsModule::ComponentWithConsumer_ptr primary,
                                 ComponentsModule::ComponentWithConsumer_ptr backup)
        throw (CORBA::Exception);
    

       CORBA::ORB_var m_orb;
       FT::Location m_primaryLocation;
       FT::Location m_backupLocation;
       Components::Deployment::Container_var m_containerPrimary;
       Components::Deployment::Container_var m_containerBackup;
       CdmwFT::ReplicationManager_var m_replicationManager;
       
}; // End class TestNavigationWithFT

}; // End namespace

#endif // INCL_FT_CCM_CIF_TEST_NAVIGATION_WITH_FT_HPP

