/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <loadbalancingclient/TestUtils.hpp>

#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif

#include <iostream>
using namespace std;

namespace Cdmw 
{

namespace LB 
{
    const char* TestUtils::M_locations[2] = {"cardamom01", "cardamom02"};


::PortableGroup::ObjectGroup_ptr
TestUtils::create_group(CORBA::ORB_ptr orb, CdmwLB::LBGroupManager_ptr group_manager, const char * rep_id)
    throw (CORBA::Exception)
{
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
    
    
    ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id;
    ::PortableGroup::ObjectGroup_var obj_group;
    
    // create the object group 
    std::cout << "Create an object group" << std::endl;
    try
    {
        obj_group =  group_manager->create_object(rep_id,
                                                  crit,
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
TestUtils::get_object_group_version(CORBA::ORB_ptr orb, ::PortableGroup::ObjectGroup_ptr object_group)
    throw (CORBA::Exception)
{

    Cdmw::LB::IOGRFactory* m_iogrFactory;
    
#if CDMW_ORB_VDR == tao
    m_iogrFactory = new Cdmw::LB::TAO_LB_IOGRFactory(std::cout,
                                                     orb);
#endif

    ::CdmwLB::TagLBGroupTaggedComponent *tag = m_iogrFactory->decode_profile_with_group_tag (object_group,0);
      return tag->object_group_ref_version;
             
}


int 
TestUtils::get_number_of_member(CORBA::ORB_ptr orb, ::PortableGroup::ObjectGroup_ptr object_group)
    throw (CORBA::Exception)
{
    Cdmw::LB::IOGRFactory* m_iogrFactory;
    
#if CDMW_ORB_VDR == tao
    m_iogrFactory = new Cdmw::LB::TAO_LB_IOGRFactory(std::cout,
                                                     orb);
#endif

    CORBA::Long  nb_profile = m_iogrFactory->profile_count(object_group);
    return (nb_profile - 1) ;
    

}

} // end namespace LB

} // end namespace Cdmw

