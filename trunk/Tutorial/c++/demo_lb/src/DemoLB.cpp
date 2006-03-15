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
 

#include <Foundation/orbsupport/CORBA.hpp>

#include <DemoLB.hpp>
#include <DemoUtils.hpp>

#include <DemoHello.stub.hpp>
#include "Foundation/ossupport/OS.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"


#define ECHO_HEADER() \
    "[Debug] "

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)
#define ENABLE_LB_DEBUG_DUMP 

#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment ;\
} while(0)

#endif


namespace Cdmw 
{

namespace LB 
{

// constructor
DemoLB::DemoLB(CORBA::ORB_ptr                  orb,
	       PortableServer::POA_ptr         rootPOA,
	       CdmwLB::LBGroupManager_ptr group_manager,
	       const std::string&              name ,
	       std::string host_name1,
	       std::string host_name2,
	       std::string host_name3)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_rootPOA(PortableServer::POA::_duplicate(rootPOA)),
      m_group_manager(CdmwLB::LBGroupManager::_duplicate(group_manager)),
      m_host_name1(host_name1),
      m_host_name2(host_name2),
      m_host_name3(host_name3)
{    
    
}

// destructor
DemoLB::~DemoLB()  throw()
{
}

// do_tests
void DemoLB::run() throw()
{

    const char * rep_id_HelloInterface = "IDL:CdmwLBDemo/HelloInterface:1.0";

    DEBUG_DUMP("***DEMO STARTED***");  

    DEBUG_DUMP("define load balancing properties");
    
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

    
    

    //
    // Create group HelloInterface
    //
    DEBUG_DUMP("Create group for HelloInterface");
    ::PortableGroup::ObjectGroup_var obj_group_HelloInterface;

    try
    {
        obj_group_HelloInterface =  m_group_manager->create_object(rep_id_HelloInterface,
                                                                   crit,
                                                                   factory_creation_id1.out());
        DEBUG_DUMP("...OK");
    }   
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        DEBUG_DUMP("...NOT OK");
        throw;
    }
    

    CdmwLBDemo::HelloInterface_var hello_group_HelloInterface = CdmwLBDemo::HelloInterface::_narrow(obj_group_HelloInterface.in());
    

    Cdmw::OsSupport::OS::sleep( 1000 );
    
    // call the method on an empty object group
    try
    {
        DEBUG_DUMP("Calling hello on HelloInterface on an empty object group...");
        hello_group_HelloInterface->hello();
        DEBUG_DUMP("...NOT OK");
    }
    catch (const CORBA::TRANSIENT& e)
    {
        DEBUG_DUMP("Expected CORBA::TRANSIENT Exception raised" << e);
        if (e.completed () == CORBA::COMPLETED_NO)
            DEBUG_DUMP("...OK");  
        else
            DEBUG_DUMP("...NOT OK");
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << e._name() << std::endl;;
        DEBUG_DUMP("...NOT OK");  
    }

    Cdmw::OsSupport::OS::sleep( 1000 );
 
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
 
    Cdmw::NamingAndRepository::RepositoryInterface::init ("CDMW",
                                                           repository.in());

    Cdmw::CommonSvcs::Naming::NamingInterface ni =
    Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");


    ::PortableGroup::Location loc;
    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P11";
    loc[2].kind = "processname";

    std::string s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);

    std::string full_name = "dom3/P11HelloInterface";

    CORBA::Object_var obj;
    
    try
    {
        obj = ni.resolve (full_name);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }


    CdmwLBDemo::HelloInterface_var hello_APPL1_PROC11_INTER1 =  CdmwLBDemo::HelloInterface::_narrow(obj.in());


    // add a new member in the object group
    DEBUG_DUMP("Add new member on the group");


    try
    {
        
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL1_PROC11_INTER1.in());
    }
    catch (const CORBA::SystemException& e)
    {
        std::cerr << e._name() << std::endl;;
    }

    hello_group_HelloInterface = CdmwLBDemo::HelloInterface::_narrow(obj_group_HelloInterface.in());
    Cdmw::OsSupport::OS::sleep( 1500 );

    // add new member

    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P21";
    loc[2].kind = "processname";

    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P21HelloInterface";

    // recover the reference of the APPL1/PROC11/HelloInterface object
    try
    {
        obj = ni.resolve (full_name);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
     }

    CdmwLBDemo::HelloInterface_var hello_APPL2_PROC21_INTER1 =  CdmwLBDemo::HelloInterface::_narrow(obj.in());
 
    // add a new member in the object group
 
    DEBUG_DUMP("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL2_PROC21_INTER1.in());
    hello_group_HelloInterface = CdmwLBDemo::HelloInterface::_narrow(obj_group_HelloInterface.in());
    Cdmw::OsSupport::OS::sleep( 1500 );

    DEBUG_DUMP("get client information");
    Cdmw::Demo::process_info_var hello_APPL1_PROC11_INTER1_info = hello_APPL1_PROC11_INTER1->who_is_called();
    Cdmw::Demo::process_info_var hello_APPL2_PROC21_INTER1_info = hello_APPL2_PROC21_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    std::string dummy;

    // call the method 8 times on an object group with default=LoadBalancing Policy
    // and get the group version
    //(REQ 257 - 258)
    int version_HelloInterface = 0;
    
    try 
    {
        CORBA::Long id = 0;
        DEBUG_DUMP("Calling hello on HelloInterface...");
        for(int i = 0; i < 8; i++)
        {
            DEBUG_DUMP ("Request..." << i);
            Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
        
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            DEBUG_DUMP ("Checking Round Robin policy");
            if ((temp_info->processID == id) && (!(i==5)))
                DEBUG_DUMP("...NOT OK");
            else
            {
                id = temp_info->processID;
                DEBUG_DUMP("...OK");
            }

            
        }
        
    }
    catch (const CORBA::SystemException& e)
    {
	std::cout<< e << std::endl;
    }

    version_HelloInterface = DemoUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in());

    // add new member

    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P12";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P12HelloInterface";


    // recover the reference of the APPL1/PROC12/HelloInterface object
    try
    {
        obj = ni.resolve (full_name);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }

    CdmwLBDemo::HelloInterface_var hello_APPL1_PROC12_INTER1 =  CdmwLBDemo::HelloInterface::_narrow(obj.in());
    Cdmw::Demo::process_info_var hello_APPL1_PROC12_INTER1_info = hello_APPL1_PROC12_INTER1->who_is_called();

    // add a new member in the object group
    // (REQ 261)
    DEBUG_DUMP("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL1_PROC12_INTER1.in());
    hello_group_HelloInterface = CdmwLBDemo::HelloInterface::_narrow(obj_group_HelloInterface.in());
    Cdmw::OsSupport::OS::sleep( 1500 );
    // add new member

    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P31";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P31HelloInterface";


    // recover the reference of the APPL3/PROC31/HelloInterface object
    try
    {
        obj = ni.resolve (full_name);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }

    CdmwLBDemo::HelloInterface_var hello_APPL3_PROC31_INTER1 =  CdmwLBDemo::HelloInterface::_narrow(obj.in());

    DEBUG_DUMP("get client information");
    Cdmw::Demo::process_info_var hello_APPL3_PROC31_INTER1_info = hello_APPL3_PROC31_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P12------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;



    // add a new member in the object group
    // (REQ 261)
    DEBUG_DUMP("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL3_PROC31_INTER1.in());
    hello_group_HelloInterface = CdmwLBDemo::HelloInterface::_narrow(obj_group_HelloInterface.in());
    Cdmw::OsSupport::OS::sleep( 1500 );

    // call the method on an object group with the new member
    // and get the group version
    // the group version must be higher than the previous group version stored
    //(REQ 259)
    try
    {
        CORBA::Long id = 0;
        DEBUG_DUMP("Calling hello on HelloInterface...");
        for(int i = 0; i < 5; i++)
        {
            DEBUG_DUMP ("Request..." << i);

            Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            DEBUG_DUMP ("Checking Round Robin policy");
            if ((temp_info->processID == id) && (!(i==5)))
		DEBUG_DUMP("...NOT OK");
            else
            {
		id = temp_info->processID;
		DEBUG_DUMP("...OK");
            }
	    DEBUG_DUMP("check the version");
        }
        if ( version_HelloInterface < DemoUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in()))
        {
            version_HelloInterface = DemoUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in());
        }
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }
    

    // get_object_group_id
   
    ::PortableGroup::ObjectGroupId object_group_id = 0;
    CdmwLBDemo::HelloInterface_var temp;
    std::cout<<" Calling the get_object_group_id on the group manager"<<std::endl;
    try
    {
        object_group_id = m_group_manager->get_object_group_id(hello_group_HelloInterface.in());
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
    }
    
    // use group manager method to recover the most recent object group reference
    DEBUG_DUMP("Calling the hello interface from the reference coming from the group manager (get_object_group_ref_from_gid)");
    
    try
    {
        ::PortableGroup::ObjectGroup_var object_group = m_group_manager->get_object_group_ref_from_id(object_group_id);
        temp = CdmwLBDemo::HelloInterface::_narrow(object_group.in());
        CORBA::Long id = 0;
        DEBUG_DUMP("Calling hello on HelloInterface...");
        for(int i = 0; i < 5; i++)
        {
            DEBUG_DUMP ("Request..." << i);
          
            Cdmw::Demo::process_info_var temp_info = temp->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            DEBUG_DUMP ("Checking Round Robin policy");

	    if ((temp_info->processID == id) && (!(i==5)))
		DEBUG_DUMP("...NOT OK");
            else
            {
                id = temp_info->processID;
                DEBUG_DUMP("...OK");
            }
        }
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
    }

    DEBUG_DUMP("Calling the hello interface from the reference coming from the group manager (get_object_group_ref)");

    try
    {
        ::PortableGroup::ObjectGroup_var object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());
        temp = CdmwLBDemo::HelloInterface::_narrow(object_group.in());
    CORBA::Long id = 0;
        DEBUG_DUMP("Calling hello on HelloInterface...");
        for(int i = 0; i < 5; i++)
        {
            DEBUG_DUMP ("Request..." << i);
          
            Cdmw::Demo::process_info_var temp_info = temp->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            DEBUG_DUMP ("Checking Round Robin policy");

            if ((temp_info->processID == id) && (!(i==5)))
		DEBUG_DUMP("...NOT OK");
            else
            {
                id = temp_info->processID;
                DEBUG_DUMP("...OK");
            }
        }    
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
    }

     // add new member

    loc.length(3);
    loc[0].id =m_host_name2.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL2";
    loc[1].kind = "applicationname";
    loc[2].id = "P22";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P22HelloInterface";


    // recover the reference of the APPL2/PROC22/HelloInterface object
    try
    {
        obj = ni.resolve (full_name);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }

    CdmwLBDemo::HelloInterface_var hello_APPL2_PROC22_INTER1 =  CdmwLBDemo::HelloInterface::_narrow(obj.in());

    Cdmw::Demo::process_info_var hello_APPL2_PROC22_INTER1_info = hello_APPL2_PROC22_INTER1->who_is_called();
    // add a new member in the object group
    // (REQ 261)
    DEBUG_DUMP("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL2_PROC22_INTER1.in());
    hello_group_HelloInterface = CdmwLBDemo::HelloInterface::_narrow(obj_group_HelloInterface.in());
    Cdmw::OsSupport::OS::sleep( 1500 );

  // add new member

    loc.length(3);
    loc[0].id =m_host_name3.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P32";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P32HelloInterface";


    // recover the reference of the APPL2/PROC22/HelloInterface object
    try
    {
        obj = ni.resolve (full_name);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }

    CdmwLBDemo::HelloInterface_var hello_APPL3_PROC32_INTER1 =  CdmwLBDemo::HelloInterface::_narrow(obj.in());

    DEBUG_DUMP("get client information");
    Cdmw::Demo::process_info_var hello_APPL3_PROC32_INTER1_info = hello_APPL3_PROC32_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |        |                                          PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |        ----------P12------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 5 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P32------HelloInterface------Replica 6 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    


    // add a new member in the object group
    // (REQ 261)
    DEBUG_DUMP("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL3_PROC32_INTER1.in());
    hello_group_HelloInterface = CdmwLBDemo::HelloInterface::_narrow(obj_group_HelloInterface.in());
    Cdmw::OsSupport::OS::sleep( 1500 );


    // use replication manager method to recover the next object group reference
    // (REQ 259 - 261)
    DEBUG_DUMP("use group manager method to recover the next object group reference");
    try
    {
        DEBUG_DUMP("Check the IOGR version...");
        DEBUG_DUMP("NEW OBJECT GROUP VERSION: " << Cdmw::LB::DemoUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in()));
	DEBUG_DUMP("OLD OBJECT GROUP VERSION: " << Cdmw::LB::DemoUtils::get_object_group_version(m_orb.in(), temp.in()));

	if ( Cdmw::LB::DemoUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in()) >
             Cdmw::LB::DemoUtils::get_object_group_version(m_orb.in(), temp.in()))
	    DEBUG_DUMP("...OK");
	
        else
            DEBUG_DUMP("...NOT OK");
        
	DEBUG_DUMP("Recover the next object group reference");
        ::PortableGroup::ObjectGroup_var object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());
        temp = CdmwLBDemo::HelloInterface::_narrow(object_group.in());

	DEBUG_DUMP("Check the IOGR version...");
        if ( Cdmw::LB::DemoUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in()) ==
             Cdmw::LB::DemoUtils::get_object_group_version(m_orb.in(), temp.in()))
            DEBUG_DUMP("...OK");
        else
            DEBUG_DUMP("...NOT OK");
        CORBA::Long id = 0;
        DEBUG_DUMP("Calling hello on HelloInterface...");
        for(int i = 0; i < 8; i++)
        {
            DEBUG_DUMP ("Request..." << i);
            Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();

            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            DEBUG_DUMP ("Checking Round Robin policy");

            if ((temp_info->processID == id) && (!(i==5)))
		DEBUG_DUMP("...NOT OK");
            else
            {
                id = temp_info->processID;
                DEBUG_DUMP("...OK");
            }
        }
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
    }

    int helloInterface_version = Cdmw::LB::DemoUtils::get_object_group_version(m_orb.in(), hello_group_HelloInterface.in());

    
    //In case of server crash after a request dispatching,
    //CARDAMOM shall be able to send again the same request
    //to one of the remaining available servers (REQ 262)
    
    DEBUG_DUMP("Test the crash of a replica...");
    DEBUG_DUMP("kill process P11 ==> Start the script command_proc1_stop.sh");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;
    
    DEBUG_DUMP("Calling hello on HelloInterface...");
    try
    {   
        
        for (int i = 0; i<10; i++)
        {
            DEBUG_DUMP ("Request..." << i);
            Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            DEBUG_DUMP ("Checking Round Robin policy");
            if (temp_info->processID == hello_APPL1_PROC11_INTER1_info->processID)
                    DEBUG_DUMP("...NOT OK");
            else
                    DEBUG_DUMP("...OK");
        }
        
    }catch (const CORBA::SystemException& e)
    {
	std::cout<< e << std::endl;
    }
    
    
    Cdmw::OsSupport::OS::sleep( 1500 );
    DEBUG_DUMP("Check the good version of the object group");
    DEBUG_DUMP("OLD OBJECT GROUP VERSION: " << helloInterface_version);
    int object_group_version = m_group_manager->get_object_group_version_from_ref(hello_group_HelloInterface.in());
    DEBUG_DUMP("NEW OBJECT GROUP VERSION: " << object_group_version);
    if ( helloInterface_version <  object_group_version)
    {
        helloInterface_version = object_group_version;
        DEBUG_DUMP("...OK");
    }
    else 
        DEBUG_DUMP("...NOT OK");

    DEBUG_DUMP("calling all object without the object group ref");
    Cdmw::Demo::process_info_var hello_APPL1_PROC12_INTER2_info = hello_APPL1_PROC12_INTER1->who_is_called();
    Cdmw::Demo::process_info_var hello_APPL2_PROC21_INTER2_info = hello_APPL2_PROC21_INTER1->who_is_called();
    Cdmw::Demo::process_info_var hello_APPL2_PROC22_INTER2_info = hello_APPL2_PROC22_INTER1->who_is_called();
    Cdmw::Demo::process_info_var hello_APPL3_PROC31_INTER2_info = hello_APPL3_PROC31_INTER1->who_is_called();
    Cdmw::Demo::process_info_var hello_APPL3_PROC32_INTER2_info = hello_APPL3_PROC32_INTER1->who_is_called();
    

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P12------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL3_PROC31_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P32------HelloInterface------Replica 5 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER2_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;

    DEBUG_DUMP("Check the good number of member in the object group (should be 5)");
    ::PortableGroup::ObjectGroup_var object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());    
    std::cout<<"get_number_of_member "<< Cdmw::LB::DemoUtils::get_number_of_member(m_orb.in(), object_group.in())<<std::endl;
    if ( Cdmw::LB::DemoUtils::get_number_of_member(m_orb.in(), object_group.in()) == 5)
        DEBUG_DUMP("...OK");
    else
        DEBUG_DUMP("...NOT OK");


    DEBUG_DUMP("Test the recovering of a replica...");
    DEBUG_DUMP("re-start process P11 ==> Start the script command_proc1_run.sh");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

  // add new member

    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL1";
    loc[1].kind = "applicationname";
    loc[2].id = "P11";
    loc[2].kind = "processname";


    s_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
    
    full_name = "dom3/P11HelloInterface";


    // recover the reference of the APPL1/PROC11/HelloInterface object
    try
    {
        obj = ni.resolve (full_name);
    }
    catch(const CosNaming::NamingContext::NotFound& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::CannotProceed& e)
    {
        std::cout<< e << std::endl;
    }
    catch(const CosNaming::NamingContext::InvalidName& e)
    {
        std::cout<< e << std::endl;
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout<< e << std::endl;
    }

    hello_APPL1_PROC11_INTER1 =  CdmwLBDemo::HelloInterface::_narrow(obj.in());

    DEBUG_DUMP("get client information");
    hello_APPL1_PROC11_INTER1_info = hello_APPL1_PROC11_INTER1->who_is_called();

    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL1------P11------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |        |                                          PID : "<<hello_APPL1_PROC11_INTER1_info->processID<<std::endl;
    std::cout<<"     |        ----------P12------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL1_PROC12_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL2------P21------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER1_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 5 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL3_PROC31_INTER1_info->processID<<std::endl;
    std::cout<<"     |         ---------P32------HelloInterface------Replica 6 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER1_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    
    

    // add a new member in the object group
    // (REQ 261)
    DEBUG_DUMP("Add new member on the group");
    obj_group_HelloInterface = m_group_manager->add_member(hello_group_HelloInterface.in(),
                                                           loc,
                                                           hello_APPL1_PROC11_INTER1.in());
    Cdmw::OsSupport::OS::sleep( 1500 );


    
    DEBUG_DUMP("Calling hello on HelloInterface...the LBGroupManager should update the IOGR with the re-started replica");
    try
    {   
      CORBA::Long id = 0;
        for (int i = 0; i<10; i++)
        {
            DEBUG_DUMP ("Request..." << i);
            Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            if (temp_info->processID == hello_APPL1_PROC11_INTER1_info->processID)
	      std::cout<< "[RE-STARTED REPLICA]==>" << "the process called is the "<<temp_info->name<<std::endl;
	    else
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            DEBUG_DUMP ("Checking Round Robin policy");
            if ((temp_info->processID == id) && (!(i==5)))
		DEBUG_DUMP("...NOT OK");
            else
            {
                id = temp_info->processID;
                DEBUG_DUMP("...OK");
            }
        }
        
    }catch (const CORBA::SystemException& e)
    {
	std::cout<< e << std::endl;
    }
    
    Cdmw::OsSupport::OS::sleep( 1500 );
    DEBUG_DUMP("Check the good version of the object group");
    DEBUG_DUMP("OLD OBJECT GROUP VERSION: " << helloInterface_version);
    object_group_version = m_group_manager->get_object_group_version_from_ref(hello_group_HelloInterface.in());
    DEBUG_DUMP("NEW OBJECT GROUP VERSION: " << object_group_version);
    if ( helloInterface_version <  object_group_version)
    {
        helloInterface_version = object_group_version;
        DEBUG_DUMP("...OK");
    }
    else 
        DEBUG_DUMP("...NOT OK");

    DEBUG_DUMP("Check the good number of member in the object group (should be 6)");
    object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());    
    std::cout<<"get_number_of_member "<< Cdmw::LB::DemoUtils::get_number_of_member(m_orb.in(), object_group.in())<<std::endl;
    if ( Cdmw::LB::DemoUtils::get_number_of_member(m_orb.in(), object_group.in()) == 6)
        DEBUG_DUMP("...OK");
    else
        DEBUG_DUMP("...NOT OK");
    
    Cdmw::OsSupport::OS::sleep( 2000 );
    DEBUG_DUMP("kill the application APPL1==> Start the script command_app_stop.sh");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin >> dummy;

    Cdmw::OsSupport::OS::sleep( 2000 );

    std::cout<<"NEW SYSTEM"<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL2------P21------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"     |                                                   PID : "<<hello_APPL2_PROC22_INTER2_info->processID<<std::endl;
    std::cout<<"     |-------APPL3------P31------HelloInterface------Replica 3 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL3_PROC31_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P32------HelloInterface------Replica 4 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL3_PROC32_INTER2_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    
    try
    {
        CORBA::Long id = 0;
        DEBUG_DUMP("Calling hello on HelloInterface...");
        for(int i = 0; i < 8; i++)
        {
            DEBUG_DUMP ("Request..." << i);
            
            Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
            DEBUG_DUMP ("Checking Round Robin policy");
            if (temp_info->processID == hello_APPL1_PROC12_INTER2_info->processID)
                DEBUG_DUMP("...NOT OK");
            else
            {
                id = temp_info->processID;
                DEBUG_DUMP("...OK");
            }
        }
    }
    catch (const CORBA::SystemException& e)
        {
	    std::cout<< e << std::endl;
        }
        
    object_group_version = m_group_manager->get_object_group_version_from_ref(hello_group_HelloInterface.in());
    
    DEBUG_DUMP("Check the good version of each object group");
        
    DEBUG_DUMP("OLD OBJECT GROUP VERSION :"<<helloInterface_version);
    DEBUG_DUMP(" NEW object_group_version :" << object_group_version);
        
    if ( helloInterface_version < object_group_version )
        {
            helloInterface_version = object_group_version;
            DEBUG_DUMP("...OK");
        }
    else 
        DEBUG_DUMP("...NOT OK");
    
    DEBUG_DUMP("Check the good number of member in the object group (should be 4)");
    object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());
    std::cout<<"get_number_of_member "<< Cdmw::LB::DemoUtils::get_number_of_member(m_orb.in(), object_group.in())<<std::endl;
    if ( Cdmw::LB::DemoUtils::get_number_of_member(m_orb.in(), object_group.in()) == 4)
        DEBUG_DUMP("...OK");
    else
        DEBUG_DUMP("...NOT OK");

    Cdmw::OsSupport::OS::sleep( 2000 );

    
    DEBUG_DUMP("kill the daemon on the host3 ==> type ^C in the daemon term on HOST3");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin>>dummy;
    Cdmw::OsSupport::OS::sleep( 2000 );
    
    std::cout<<"NEW SYSTEM"<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    std::cout<<"----SYS------APPL2------P21------HelloInterface------Replica 1 "<<std::endl;
    std::cout<<"     |         |                                         PID : "<<hello_APPL2_PROC21_INTER2_info->processID<<std::endl;
    std::cout<<"     |         ---------P22------HelloInterface------Replica 2 "<<std::endl;
    std::cout<<"                                                         PID : "<<hello_APPL2_PROC22_INTER2_info->processID<<std::endl;
    std::cout<<"----------------------------------------------------------------"<<std::endl;
    DEBUG_DUMP("Check the good version of each object group");
    DEBUG_DUMP("helloInterface_version :"<<helloInterface_version);
    object_group_version = m_group_manager->get_object_group_version_from_ref(hello_group_HelloInterface.in());
    DEBUG_DUMP(" NEW object_group_version :"<< object_group_version);
    
    if ( helloInterface_version < object_group_version )
        DEBUG_DUMP("...OK");
    
    else 
        DEBUG_DUMP("...NOT OK");
    DEBUG_DUMP("Check the good number of member in the object group (shoulld be 2)");

    object_group = m_group_manager->get_object_group_ref(hello_group_HelloInterface.in());
    std::cout<<"get_number_of_member "<< Cdmw::LB::DemoUtils::get_number_of_member(m_orb.in(), object_group.in())<<std::endl;
    if ( Cdmw::LB::DemoUtils::get_number_of_member(m_orb.in(), object_group.in()) == 2)
        DEBUG_DUMP("...OK");
    else
        DEBUG_DUMP("...NOT OK");
    
    DEBUG_DUMP("Calling hello on HelloInterface...");
     try{

       for (int i = 0; i<10; i++)
	
       {
	    Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
	    DEBUG_DUMP ("Request..." << i);
	    
	    std::cout<<"the process called is the "<<temp_info->name<<std::endl;
	    DEBUG_DUMP ("Checking Round Robin policy");
	    if ((temp_info->processID == hello_APPL3_PROC31_INTER2_info->processID) ||
		(temp_info->processID == hello_APPL3_PROC32_INTER2_info->processID))
		DEBUG_DUMP("...NOT OK");
	    else
	    {
		DEBUG_DUMP("...OK");
	    }
       }
	}
	catch (const CORBA::SystemException& e)
	{
	    std::cout<< e << std::endl;
	}
   
    
    // remove the menber previously deleted

    DEBUG_DUMP("Remove a member P32 already killed from the helloInterface (Member Not Found Exception should be raised)");
    // set_primary on the first member
    loc.length(3);
    loc[0].id =m_host_name1.c_str();
    loc[0].kind = "hostname";
    loc[1].id = "APPL3";
    loc[1].kind = "applicationname";
    loc[2].id = "P32";
    loc[2].kind = "processname";
    try
    {
        obj_group_HelloInterface = m_group_manager->remove_member(hello_group_HelloInterface.in(),
                                                                  loc);
    }
    catch( const PortableGroup::MemberNotFound& e )
    {
        DEBUG_DUMP("Exception MemberNotFound Raised");
    }

    
    DEBUG_DUMP("kill the Process P21 ==> Start the script command_proc2_stop.sh");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin>>dummy;
    Cdmw::OsSupport::OS::sleep( 2000 );
    
    try
    {
      DEBUG_DUMP("Calling hello on HelloInterface...");
      for(int i = 0; i < 8; i++)
        {
            DEBUG_DUMP ("Request..." << i);
	    Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
	    std::cout<<"the process called is the "<<temp_info->name<<std::endl;
	    DEBUG_DUMP ("Checking Round Robin policy...there is only P22 alive in the object group");
	    if (temp_info->processID ==  hello_APPL2_PROC22_INTER2_info->processID) 
	      DEBUG_DUMP("...OK");
	    else
	      DEBUG_DUMP("...NOT OK");
	}
    }
    catch (const CORBA::SystemException& e)
    {
      std::cout << e._name() << std::endl;
    }


    // remove the object group
    DEBUG_DUMP("Delete the object group helloInterface");
    try
    {
        m_group_manager->delete_object(factory_creation_id1.in());    
    }
    
    catch( const CORBA::Exception& e )
    {
	std::cout<< e << std::endl;
    } 


    DEBUG_DUMP("kill the Group Manager Supervision Observer ==> type ^C in the LBGroupManagerSupervisionObserver term");
    std::cout<<"Press any key and Enter"<<std::endl;
    std::cin>>dummy;
    Cdmw::OsSupport::OS::sleep( 2000 );
    try
    {
        
        DEBUG_DUMP("Calling hello on HelloInterface...");
        for(int i = 0; i < 8; i++)
        {
            DEBUG_DUMP ("Request..." << i);
            
            Cdmw::Demo::process_info_var temp_info = hello_group_HelloInterface->who_is_called();
            std::cout<<"the process called is the "<<temp_info->name<<std::endl;
	    DEBUG_DUMP ("Checking Round Robin policy...there is only P22 alive in the object group");
	    if (temp_info->processID ==  hello_APPL2_PROC22_INTER2_info->processID) 
	      DEBUG_DUMP("...OK");
	    else
	      DEBUG_DUMP("...NOT OK");   
	}
    }
    catch (const CORBA::SystemException& e)
        {
	    std::cout<< e << std::endl;
        }

DEBUG_DUMP("***DEMO STOPPED***");  
}


} // end namespace LB

} // end namespace Cdmw

