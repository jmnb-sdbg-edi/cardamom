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


// Standard Files 
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include <testftinit/TestUtils.hpp>

#include <idllib/FT.stub.hpp>
#include <idllib/CdmwFTReplicationManager.stub.hpp>

//#include <CdmwFTMembersAdmin.stub.hpp>

#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif

namespace
{

    const int NO_PRIMARY = -1;
    const int NB_LOCATION = 3;

} // end anonymous namespace

namespace Cdmw 
{

namespace FT 
{
    int   TestUtils::M_current_primary_index = 0;
    const char* TestUtils::M_locations[NB_LOCATION] = {"A", "B", "C"};
    const char* TestUtils::M_files[NB_LOCATION] = {"hello1", "hello2", "hello3"};
    

CdmwFT::Test::HelloInterface_ptr
TestUtils::get_hello_ref_from_file(CORBA::ORB_ptr orb, std::string file_name)
throw (CORBA::SystemException)
{
    CORBA::Object_var objref;
    CdmwFT::Test::HelloInterface_var hello_ref;

    // import the object reference from the file
    std::ifstream is(file_name.c_str());
    if (is.good())
    {
        std::string objref_str;
        is >> objref_str;
        try
        {
            objref = orb->string_to_object(objref_str.c_str());
            hello_ref = CdmwFT::Test::HelloInterface::_narrow(objref.in());
            
        }
        catch( CORBA::SystemException& e )
        {
            std::cerr << " CORBA System Exception raised: " << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                << " IN FILE: " << __FILE__ << std::endl; 
            throw;
        }
        catch (...)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        }

        is.close();
    }
    else
    {
        std::cerr << "Error opening reference filename: " << file_name << std::endl;
        throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
    }

    return hello_ref._retn();
}

/**********************************************************************************/

::FT::ObjectGroup_ptr
TestUtils::create_group(CORBA::ORB_ptr orb, CdmwFT::ReplicationManager_ptr replication_manager, const char * rep_id)
    throw (CORBA::Exception)
{
    // set default properties
    
    ::FT::Location loc1;
    loc1.length(3);
    loc1[0].id = M_locations[0];
    loc1[0].kind = "hostname";
    
    loc1[1].id = "APPL";
    loc1[1].kind = "application";
    loc1[2].id = "PROC";
    loc1[2].kind = "process";    
    
    ::FT::Location loc2;
    loc2.length(3);
    loc2[0].id = M_locations[1];
    loc2[0].kind = "hostname";
    
    loc2[1].id = "APPL";
    loc2[1].kind = "application";
    loc2[2].id = "PROC";
    loc2[2].kind = "process";    
    
    ::FT::Location loc3;
    loc3.length(3);
    loc3[0].id = M_locations[2];
    loc3[0].kind = "hostname";
    
    loc3[1].id = "APPL";
    loc3[1].kind = "application";
    loc3[2].id = "PROC";
    loc3[2].kind = "process";  
    
    
    std::cout << "set_default_properties" << std::endl;
    
    ::FT::Properties prop;
    
    prop.length(4);
    prop[0].nam.length(1);
    prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort)2;
    
    prop[1].nam.length(1);
    prop[1].nam[0].id="org.omg.ft.ReplicationStyle";
    prop[1].val <<= (CORBA::UShort)2;
    
    prop[2].nam.length(1);
    prop[2].nam[0].id="org.omg.ft.MembershipStyle";
    prop[2].val <<= (CORBA::UShort)0;
    
    prop[3].nam.length(1);
    prop[3].nam[0].id="org.omg.ft.Factories";
    
    ::FT::FactoryInfos factoryInfos;
    factoryInfos.length(3);
    factoryInfos[0].the_factory = ::FT::GenericFactory::_nil();
    factoryInfos[0].the_location = loc1;
    ::FT::Criteria factoryCrit;        
    factoryCrit.length(0);
    factoryInfos[0].the_criteria = factoryCrit;
    
    factoryInfos[1].the_factory = ::FT::GenericFactory::_nil();
    factoryInfos[1].the_location = loc2;
    factoryInfos[1].the_criteria = factoryCrit;
    
    factoryInfos[2].the_factory = ::FT::GenericFactory::_nil();
    factoryInfos[2].the_location = loc3;
    factoryInfos[2].the_criteria = factoryCrit;
    
    prop[3].val <<= factoryInfos;
    
    
    std::string repository_id(rep_id);
    
    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
    // const ::FT::Criteria ft_criteria = prop;
    ::FT::Criteria ft_criteria;       
    ft_criteria.length(1);
    ft_criteria[0].nam.length(1);
    ft_criteria[0].nam[0].id="org.omg.ft.FTProperties";
    ft_criteria[0].val <<= prop;

    ::FT::ObjectGroup_var obj_group;
    
    // create the object group 
    std::cout << "Create an object group" << std::endl;
    try
    {
        obj_group =  replication_manager->create_object(rep_id,
            ft_criteria,
            factory_creation_id.out());
    }    
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        throw;
    }
    
     // add a new menber on the group previously deleted
    try
    {
        
        if (repository_id == "IDL:thalesgroup.com/CdmwFT/Test/HelloInterface:1.0")
        {
            
            CdmwFT::Test::HelloInterface_var hello1 =
            get_hello_ref_from_file(orb, M_files[0]);
            
            std::cout << "Add new member on the group" << std::endl;
            obj_group = replication_manager->add_member(obj_group.in(),
                                                        loc1,
                                                        hello1.in());
            
            CdmwFT::Test::HelloInterface_var hello2 =
            get_hello_ref_from_file(orb, M_files[1]);
            
            std::cout << "Add new member on the group" << std::endl;
            obj_group = replication_manager->add_member(obj_group.in(),
                                                        loc2,
                                                        hello2.in());
            
            CdmwFT::Test::HelloInterface_var hello3 =
            get_hello_ref_from_file(orb, M_files[2]);
            
            std::cout << "Add new member on the group" << std::endl;
            obj_group = replication_manager->add_member(obj_group.in(),
                                                        loc3,
                                                        hello3.in());
        }
        


    }
    catch( ::FT::ObjectGroupNotFound& e )
    {
        std::cerr << " ObjectGroupNotFound Exception raised! " << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
            << " IN FILE: " << __FILE__ << std::endl; 
        throw;
    }   
    catch( ::FT::ObjectNotAdded& e )
    {
        std::cerr << " ObjectNotAdded Exception raised! " << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
            << " IN FILE: " << __FILE__ << std::endl; 
        throw;
    }   
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;;
        throw;
    }

    return obj_group._retn();
}

/**********************************************************************************/

::FT::ObjectGroup_ptr
TestUtils::change_primary(CORBA::ORB_ptr orb, 
                          ::FT::ObjectGroup_ptr object_group,
                          CdmwFT::ReplicationManager_ptr replication_manager,
                          int primary_index)
   throw (CORBA::Exception)
{
    try
    {
//         std::cout << "Set a primary on the group" << std::endl;

//         ::FT::Location loc;
//         loc.length(3);
//         loc[0].id = M_locations[primary_index];
//         loc[0].kind = "hostname";
        
//         loc[1].id = "APPL";
//         loc[1].kind = "application";
//         loc[2].id = "PROC";
//         loc[2].kind = "process";   


//         ::FT::ObjectGroup_var new_obj_group 
//             = replication_manager->set_primary_member(object_group, loc);
//         M_current_primary_index = primary_index;

//         return new_obj_group._retn();

        ::FT::Location new_loc;
        new_loc.length(3);
        new_loc[0].id = M_locations[primary_index];
        new_loc[0].kind = "hostname";
        
        new_loc[1].id = "APPL";
        new_loc[1].kind = "application";
        new_loc[2].id = "PROC";
        new_loc[2].kind = "process";   
        
        ::FT::Location last_loc;
        last_loc.length(3);
        last_loc[0].id = M_locations[M_current_primary_index];
        last_loc[0].kind = "hostname";
        
        last_loc[1].id = "APPL";
        last_loc[1].kind = "application";
        last_loc[2].id = "PROC";
        last_loc[2].kind = "process";   

        try
        {
            std::cout<<"define the first backup location "<<M_locations[primary_index]<<std::endl;
            replication_manager->set_first_backup (new_loc);

            std::cout<<"remove the last primary "<<M_locations[M_current_primary_index]<<std::endl;
            ::FT::ObjectGroup_var new_obj_group  = 
              replication_manager->remove_member(object_group, last_loc);


            CdmwFT::Test::HelloInterface_var hello1 =
            get_hello_ref_from_file(orb, M_files[M_current_primary_index]);
            
            std::cout << "Add last primary on the group "<<M_locations[M_current_primary_index] << std::endl;
            new_obj_group  = replication_manager->add_member(new_obj_group.in(), last_loc, hello1.in());
            M_current_primary_index = primary_index;

            return new_obj_group._retn();
            
        }
        catch(...)
        {
            std::cout<<"****************** FAILED set_first_backup_location ************"<<std::endl;
            throw;
        }


    }
    catch( ::FT::ObjectGroupNotFound& e )
    {
        std::cerr << " ObjectGroupNotFound Exception raised! " << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
            << " IN FILE: " << __FILE__ << std::endl; 
        throw;
    }   
}

/**********************************************************************************/

::FT::ObjectGroup_ptr
TestUtils::update_group_with_primary_outside_cdmw_ft
                         (CORBA::ORB_ptr orb, 
                          ::FT::ObjectGroup_ptr object_group,
                          CdmwFT::ReplicationManager_ptr replication_manager,
                          int primary_index)
   throw (CORBA::Exception)
{
    // get members of the group
    ::CdmwFT::GroupInfo_var group_info 
        = replication_manager->get_object_group_info(object_group); 

    // create object group
    ::FT::ObjectGroup_var new_object_group = ::FT::ObjectGroup::_nil();
    
    using namespace Cdmw::CommonSvcs::Naming;
    ::FT::Location primary_location;
    primary_location.length(3);
    primary_location[0].id = M_locations[primary_index];
    primary_location[0].kind = "hostname";
    
    primary_location[1].id = "APPL";
    primary_location[1].kind = "application";
    primary_location[2].id = "PROC";
    primary_location[2].kind = "process";

    ::CdmwFT::Objects_var no_fallBack = new ::CdmwFT::Objects;     
    no_fallBack->length(0);

    Cdmw::FT::IOGRFactory* m_iogrFactory;
    
#if CDMW_ORB_VDR == tao
    m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(orb,
                                                  std::cout);
#endif

    std::ostringstream ft_domain_id;
    ft_domain_id << "My_" << Cdmw::OsSupport::OS::get_logname();
    ft_domain_id << "FTdomain" << std::ends;
    std::ostringstream multicast_domain;
    multicast_domain << Cdmw::OsSupport::OS::get_logname(); 
    multicast_domain << "ftdomain" << std::ends;
    new_object_group = 
    m_iogrFactory->build_iogr(group_info->members,
                              primary_location,
                              group_info->object_group_id,
                              "type_id",
                              ft_domain_id.str().c_str(),
                              group_info->object_group_ref_version,
                              no_fallBack);



    return new_object_group._retn();
}


/**********************************************************************************/

::CdmwFT::Location::PrimaryBackupAdmin_ptr
TestUtils::get_location_manager_ref_from_file(CORBA::ORB_ptr orb, std::string host_name)
throw (CORBA::SystemException)
{
    CORBA::Object_var objref;
    ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin_ref;

    std::string file_name = host_name + "_primary_backup_admin.ior";

    // import the object reference from the file
    std::cout << "open file " << file_name 
              << " to get PrimaryBackupAdmin ref" << std::endl;
    std::ifstream is(file_name.c_str());
    if (is.good())
    {
        std::string objref_str;
        is >> objref_str;
        try
        {
            objref = orb->string_to_object(objref_str.c_str());
            primary_backup_admin_ref
                = ::CdmwFT::Location::PrimaryBackupAdmin::_narrow(objref.in());
            
        }
        catch( CORBA::SystemException& e )
        {
            std::cerr << " CORBA System Exception raised: " << e << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                << " IN FILE: " << __FILE__ << std::endl; 
            throw;
        }
        catch (...)
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        }

        is.close();

    }
    else
    {
        std::cerr << "Error opening reference filename: " << file_name << std::endl;
        throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
    }
    
    return primary_backup_admin_ref._retn();
}

/**********************************************************************************/

void
TestUtils::location_manager_update_group_view
                         (CORBA::ORB_ptr orb, 
                          ::FT::ObjectGroup_ptr object_group,
                          CdmwFT::ReplicationManager_ptr replication_manager,
                          bool more)
        throw (CORBA::Exception)
{
    // get members of the group
    ::CdmwFT::GroupInfo_var group_info 
        = replication_manager->get_object_group_info(object_group); 

    // get Location Manager Ref for each location of the group
    ::CdmwFT::Location::PrimaryBackupAdmin_var location_managers[group_info->members.length()];
    for (CORBA::ULong i =0; i< (group_info->members).length(); i++)
    {
        ::FT::Location current_location = group_info->members[i].the_location;
        std::string host_name = current_location[0].id.in();
        location_managers[i] 
            = get_location_manager_ref_from_file(orb, host_name);
    }

    // call update_group_view with a group version = current_group_version - 1

    for (CORBA::ULong i =0; i< (group_info->members).length(); i++)
    {       
        try 
        {
            if (more)
                group_info->object_group_ref_version;
            else
                group_info->object_group_ref_version--;
            location_managers[i]->update_group_view(group_info.in());

        } 
        catch (const ::FT::ObjectGroupNotFound&) 
        {
            std::cerr << "location_manager_update_group_view(" 
                << group_info->members[i].the_location[0].id 
                << "): Failed to update_group_view, "
                << "raising ObjectGroupNotFound() exception." << std::endl;
            throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, 
                CORBA::COMPLETED_NO);
        }
        catch (const CORBA::SystemException& e) 
        {
            std::cerr << "location_manager_update_group_view(" 
                << group_info->members[i].the_location[0].id 
                << "): Failed to update_group_view, "
                << "raising CORBA System Exception exception." << std::endl;
            
            throw;
        } 
    }
}

} // end namespace FT

} // end namespace Cdmw

