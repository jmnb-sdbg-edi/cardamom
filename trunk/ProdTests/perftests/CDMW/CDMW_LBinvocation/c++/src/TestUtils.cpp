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

// Standard Files
#include <string>
#include <iostream>
#include <fstream>

// Cdmw Files
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

// Local Files
#include "TestUtils.hpp"

using namespace std;

namespace
  {

  const int NB_LOCATION = 3;

} // end anonymous namespace

namespace Cdmw
  {

  namespace LB
    {
    const char* TestUtils::M_locations[NB_LOCATION] =
      {"host1", "host2", "host3"
      };
    const char* TestUtils::M_files[NB_LOCATION] =
      {"server1", "server2", "server3"
      };

    CdmwLBPerf::PingableInterface_ptr
    TestUtils::Get_pingable_ref_from_file(CORBA::ORB_ptr orb, std::string file_name)
    throw (CORBA::SystemException)
    {
      CORBA::Object_var objref;
      CdmwLBPerf::PingableInterface_var pingable_ref;

      // import the object reference from the file
      std::ifstream is(file_name.c_str());
      if (is.good())
        {
          std::string objref_str;
          is >> objref_str;
          try
            {
              objref = orb->string_to_object(objref_str.c_str());
              pingable_ref = CdmwLBPerf::PingableInterface::_narrow(objref.in());

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
              throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);
            }

          is.close();
        }
      else
        {
          std::cerr << "Error opening reference filename: " << file_name << std::endl;
          throw CORBA::INTERNAL(OrbSupport::INTERNALLoadBalancingError, CORBA::COMPLETED_NO);
        }

      return pingable_ref._retn();
    }

    CORBA::Object_ptr
    TestUtils::Create_group(CORBA::ORB_ptr orb_tmp,
                            CdmwLB::LBGroupManager_ptr group_manager,
                            const char * rep_id,
                            const char * policy)
    throw (CORBA::Exception)
    {
      // set default properties
      std::cerr << "set_lb_policy_property (default = RoundRobin)" << std::endl;
      ::PortableGroup::Properties prop;
      CORBA::ORB_var orb = CORBA::ORB::_duplicate(orb_tmp);
      prop.length(1);
      prop[0].nam.length(1);
      prop[0].nam[0].id="cdmw.lb.LoadBalancingPolicy";
      std::string lb_policy(policy);
      prop[0].val <<=  lb_policy.c_str();

      std::string repository_id(rep_id);

      ::PortableGroup::GenericFactory::FactoryCreationId_var factory_creation_id;
      ::PortableGroup::Criteria crit;
      crit.length(1);
      crit[0].nam.length(1);
      crit[0].nam[0].id="cdmw.lb.LBProperties";
      crit[0].val <<= prop;

      CORBA::Object_var obj_group;


      // create the object group
      std::cerr << "Create an object group" << std::endl;
      try
        {
          obj_group =  group_manager->create_object(rep_id,
                       crit,
                       factory_creation_id.out());
        }
      catch( CORBA::Exception& e )
        {
          std::cerr << e._name() << std::endl;
          ;
          throw;
        }

      ::PortableGroup::Location loc;
      loc.length(3);
      loc[0].id = M_locations[0];
      loc[0].kind = "hostname";

      loc[1].id = "APPL";
      loc[1].kind = "application";
      loc[2].id = "PROC";
      loc[2].kind = "process";

      // add a new menber
      try
        {
          if (repository_id == "IDL:CdmwLBPerf/PingableInterface:1.0")
            {
              CORBA::Object_var pingable =
                Get_pingable_ref_from_file(orb.in(), M_files[0]);

              std::cerr << "Add new member on the group" << std::endl;
              obj_group = group_manager->add_member(obj_group.in(),
                                                    loc,
                                                    pingable.in());


              loc[0].id = M_locations[1];
              pingable =
                Get_pingable_ref_from_file(orb.in(), M_files[1]);


              std::cerr << "Add new member on the group" << std::endl;
              obj_group = group_manager->add_member(obj_group.in(),
                                                    loc,
                                                    pingable.in());



              loc[0].id = M_locations[2];
              pingable =
                Get_pingable_ref_from_file(orb.in(), M_files[2]);


              std::cerr << "Add new member on the group" << std::endl;

              obj_group = group_manager->add_member(obj_group.in(),
                                                    loc,
                                                    pingable.in());



            }
        }
      catch( ::PortableGroup::ObjectGroupNotFound& e )
        {
          std::cerr << " ObjectGroupNotFound Exception raised! " << std::endl;
          std::cerr << " **** TEST FAILED AT LINE " << __LINE__
          << " IN FILE: " << __FILE__ << std::endl;
          throw;
        }
      catch( ::PortableGroup::ObjectNotAdded& e )
        {
          std::cerr << " ObjectNotAdded Exception raised! " << std::endl;
          std::cerr << " **** TEST FAILED AT LINE " << __LINE__
          << " IN FILE: " << __FILE__ << std::endl;
          throw;
        }
      catch( CORBA::Exception& e )
        {
          std::cerr << e._name() << std::endl;
          ;
          throw;
        }
      return obj_group._retn();
    }

  }
  ; // end namespace LB

}
; //end namespace Cdmw

