/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */
#ifndef INCL_MY_PROCESS_GROUP_CREATOR_BEHAVIOUR_HPP
#define INCL_MY_PROCESS_GROUP_CREATOR_BEHAVIOUR_HPP 

#include <fstream>

// Cdmw Files
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include <Foundation/common/Options.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
//#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <loadbalancingserver/TestHello_impl.hpp>


#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>

#include <loadbalancingserver/TestUtils.hpp>
#include <idllib/PortableGroup.stub.hpp>
#include <idllib/CdmwLBGroupManager.stub.hpp>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

/**
 * Purpose:
 * <p>
 * The abstract process behaviour.
 */
//class MyProcessGroupCreatorBehaviour: public Cdmw::PlatformMngt::ProcessBehaviour
class MyProcessGroupCreatorBehaviour: public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
     * Purpose:
     * <p> The constructor.
     */
    MyProcessGroupCreatorBehaviour (CORBA::ORB_ptr orb)
         
        {
            m_orb = CORBA::ORB::_duplicate(orb);
        }


    /**
     * Purpose:
     * <p> The destructor.
     */
    virtual ~MyProcessGroupCreatorBehaviour() throw ()
        {

        }


    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
     * attribute
     */
    virtual CORBA::ULong nb_steps() throw(CORBA::SystemException)
        {
            return 1;
        }


    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
     * operation
     */
    virtual CORBA::Object_ptr get_service() throw(CORBA::SystemException)
        {
            return CORBA::Object::_nil();
        }


    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    virtual void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw( CORBA::SystemException)
        {
            try
            {
                CORBA::Object_var obj = m_orb->resolve_initial_references("LBGroupManager");
                CdmwLB::LBGroupManager_var group_manager
                    = CdmwLB::LBGroupManager::_narrow(obj.in());
                Cdmw::OsSupport::OS::sleep(500);
                const char * rep_id_hello = "IDL:CdmwLBInit/HelloInterface:1.0";
                std::string lb_policy = "default";
                //
                // Create a group with default load balancing policy (RoundRobin)
                //
                ::PortableGroup::Properties prop;
                
                prop.length(1);
                prop[0].nam.length(1);
                prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
                prop[0].val <<=  lb_policy.c_str();
                
                ::PortableGroup::Criteria crit;
                crit.length(1);
                crit[0].nam.length(1);
                crit[0].nam[0].id="cdmw.lb.LBProperties";
                crit[0].val <<= prop;
                ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id1;
                
                obj = group_manager->create_object(rep_id_hello,
                                                   crit,
                                                   factory_creation_id1.out());
                
                // Add replicas
                CORBA::Object_var hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_repository(m_orb.in(), "hello1");
                ::PortableGroup::Location loc;
                loc.length(3);
                loc[0].id="HOST";
                loc[0].kind="hostname1";
                loc[1].id="APPL";
                loc[1].kind="applicationname1";
                loc[2].id="PROC";
                loc[2].kind="hello1";
                
                
                obj = group_manager->add_member(obj.in(),
                                                loc,
                                                hello_object.in());
                
                hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_repository(m_orb.in(), "hello2");
                loc[2].kind = "hello2";
                obj = group_manager->add_member(obj.in(),
                                                loc,
                                        hello_object.in());
                
                hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_repository(m_orb.in(), "hello3");
                loc[2].kind = "hello3";
                obj = group_manager->add_member(obj.in(),
                                                loc,
                                                hello_object.in());
                
                hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_repository(m_orb.in(), "hello4");
                loc[2].kind = "hello4";
                obj = group_manager->add_member(obj.in(),
                                                loc,
                                                hello_object.in());
                
                // export the object reference to a file
                std::string rep_name = "hello_group_default";
                // Get the Naming Context reference
                CdmwNamingAndRepository::Repository_var repository
                    = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
                // ======================================================
                // Get HELLO name domain from repository
                // ======================================================
                CdmwNamingAndRepository::NameDomain_var helloDomain =
                    repository->resolve_name_domain ("dom1/dom2/dom3");
                
                
                // ========================================================
                // Register the HelloInterface object in the HELLO name domain
                // ========================================================
                // reserve the name "HelloInterface" in the domain
                CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (rep_name.c_str());
                // register the object
                helloDomain->register_object (regId.in(), obj.in ());
                
                
                lb_policy = "Random";
                
                //
                // Create a group with Random policy
                //
                ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id2;
                
                prop[0].val <<=  lb_policy.c_str();
                
                crit[0].val <<= prop;
                
                obj = group_manager->create_object(rep_id_hello,
                                                   crit,
                                                   factory_creation_id2.out());
                
                // Add replicas
                hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_repository(m_orb.in(), "hello1");
                loc[2].kind = "hello1";
                obj = group_manager->add_member(obj.in(),
                                                loc,
                                                hello_object.in());
                
                hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_repository(m_orb.in(), "hello2");
                loc[2].kind = "hello2";
                obj = group_manager->add_member(obj.in(),
                                                loc,
                                                hello_object.in());
                
                hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_repository(m_orb.in(), "hello3");
                loc[2].kind = "hello3";
                obj = group_manager->add_member(obj.in(),
                                                loc,
                                                hello_object.in());
                
                hello_object = Cdmw::LB::TestUtils::Get_hello_ref_from_repository(m_orb.in(), "hello4");
                loc[2].kind = "hello4";
                obj = group_manager->add_member(obj.in(),
                                                loc,
                                                hello_object.in());
                
                // export the object reference to a file
                rep_name = "hello_group_random";
                /*  ref_string = orb->object_to_string(obj.in());
                    std::ofstream os2(file_name.c_str());
                    os2 << ref_string.in();
                    os2.close();
                */
                // ========================================================
                // Register the HelloInterface object in the HELLO name domain
                // ========================================================
                // reserve the name "HelloInterface" in the domain
                regId = helloDomain->new_name (rep_name.c_str());
                // register the object
                helloDomain->register_object (regId.in(), obj.in ());

                Cdmw::OsSupport::OS::sleep(2000);
                
            }
            catch( const CORBA::Exception &e )
            {
                cerr <<__FILE__<<" "<<__LINE__<<" "<< e._name() << endl;
            }
            catch( const Cdmw::Exception &e )
            {
                cerr <<__FILE__<<" "<<__LINE__<<" "<< e.what() << endl;
            }
            catch(...)
            {
                cerr << "Unexpected exception" << endl;
            }
        }  
            
            
            
            
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    virtual void on_run()
        throw(CORBA::SystemException,
              CdmwPlatformMngt::Process::NotReadyToRun)
        {

        }
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    virtual void on_stop() throw(CORBA::SystemException)
        {

        }

    
private:

    /**
     * The ORB.
     */
    CORBA::ORB_var m_orb;
};

#endif // INCL_MY_PROCESS_GROUP_CREATOR_BEHAVIOUR_HPP
