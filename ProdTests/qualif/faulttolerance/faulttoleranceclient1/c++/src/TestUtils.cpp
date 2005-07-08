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

// Standard Files 
#include <string>
#include <iostream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <faulttoleranceclient1/TestUtils.hpp>

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

