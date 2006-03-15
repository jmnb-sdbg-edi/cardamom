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

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <faulttoleranceclient2/TestUtils.hpp>

#include <FaultTolerance/idllib/FT.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>
#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif

namespace
{

    const int NO_PRIMARY = -1;

} // end anonymous namespace

namespace Cdmw 
{

namespace FT 
{
    int   TestUtils::M_current_primary_index = NO_PRIMARY;
    const char* TestUtils::M_locations[3] = {"amidala", "r2d2", "leia"};


::FT::ObjectGroup_ptr
TestUtils::create_group(CORBA::ORB_ptr orb, CdmwFT::ReplicationManager_ptr replication_manager, const char * rep_id)
    throw (CORBA::Exception)
{
    // set default properties
    std::cout << "set_default_properties" << std::endl;
    
    ::FT::Properties prop;
    
    prop.length(4);
    prop[0].nam.length(1);
    prop[0].nam[0].id="MinimumNumberReplicas";
    prop[0].val <<= (CORBA::UShort)2;
    
    prop[1].nam.length(1);
    prop[1].nam[0].id="ReplicationStyle";
    prop[1].val <<= (CORBA::UShort)2;
    
    prop[2].nam.length(1);
    prop[2].nam[0].id="MemberShipStyle";
    prop[2].val <<= (CORBA::UShort)0;
    
    prop[3].nam.length(1);
    prop[3].nam[0].id="FaultMonitoringStyle";
    prop[3].val <<= (CORBA::UShort)0;
    

    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
    const ::FT::Criteria ft_criteria = prop;
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

    return obj_group._retn();
}

int
TestUtils::get_object_group_version(CORBA::ORB_ptr orb, ::FT::ObjectGroup_ptr object_group)
    throw (CORBA::Exception)
{

    Cdmw::FT::IOGRFactory* m_iogrFactory;
    
#if CDMW_ORB_VDR == tao
    m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(orb,
                                                  std::cout);
#endif

    ::FT::TagFTGroupTaggedComponent *tag = m_iogrFactory->decode_profile_with_group_tag (object_group,0);
      return tag->object_group_ref_version;
             
}


int 
TestUtils::get_number_of_member(CORBA::ORB_ptr orb, ::FT::ObjectGroup_ptr object_group)
    throw (CORBA::Exception)
{

    Cdmw::FT::IOGRFactory* m_iogrFactory;
    
#if CDMW_ORB_VDR == tao
    m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(orb,
                                                  std::cout);
#endif
    
    CORBA::Long  nb_profile = m_iogrFactory->profile_count(object_group);
    
    return (nb_profile - 1) ;
    

}



::FT::ObjectGroup_ptr
TestUtils::update_group_with_primary_outside_cdmw_ft
                         (CORBA::ORB_ptr orb, 
                          ::FT::ObjectGroup_ptr object_group,
                          CdmwFT::ReplicationManager_ptr replication_manager,
                          ::FT::Location & primary_location)
   throw (CORBA::Exception)
{
    // get members of the group
    ::CdmwFT::GroupInfo_var group_info 
        = replication_manager->get_object_group_info(object_group); 

    // create object group
    ::FT::ObjectGroup_var new_object_group = ::FT::ObjectGroup::_nil();

    ::CdmwFT::Objects_var no_fallBack = new ::CdmwFT::Objects;     
    no_fallBack->length(0);

    Cdmw::FT::IOGRFactory* m_iogrFactory;
    
#if CDMW_ORB_VDR == tao
    m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(orb,
                                                  std::cout);
#endif

    new_object_group = 
    m_iogrFactory->build_iogr(group_info->members,
                              primary_location,
                              group_info->object_group_id,
                              "type_id",
                              "My_FTdomain",
                              group_info->object_group_ref_version,
                              no_fallBack);


    return new_object_group._retn();
}


} // end namespace FT

} // end namespace Cdmw

