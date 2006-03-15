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


#ifndef INCL_CDMW_FT_LOCATION_MEMBERS_ADMIN_HPP
#define INCL_CDMW_FT_LOCATION_MEMBERS_ADMIN_HPP

// Cdmw Files
#include <Foundation/orbsupport/RefCountServantBase.hpp>


#include <idllib/CdmwFTMembersAdmin.skel.hpp>
#include <idllib/CdmwFTGroupRepository.stub.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreBase.hpp>
//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{

namespace FT
{

//
// IDL:thalesgroup.com/CdmwFT/Location:1.0
//
namespace Location
{


// forward declaration
class GroupRepository_impl;

//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin:1.0
//
class MembersAdmin_impl : virtual public POA_CdmwFT::Location::MembersAdmin,
                          virtual public Cdmw::OrbSupport::RefCountServantBase 
{

public:

    MembersAdmin_impl(CORBA::ORB_ptr orb,
                      PortableServer::POA_ptr   poa, 
                      const ::FT::Location&     the_location,
                      Cdmw::FT::Location::GroupRepository_impl* group_repository,
                      bool is_ftmanager);
    ~MembersAdmin_impl();

    virtual PortableServer::POA_ptr _default_POA();

    //
    // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/update_group_view:1.0
    //
    virtual void update_group_view(const CdmwFT::GroupInfo& group_info)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);


	 /* FNO_JE added code
    //
    // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/get_data_store_infos:1.0
    //
    virtual CdmwFT::StateTransfer::DataStoreInfos* get_data_store_infos(const CdmwFT::StateTransfer::DataStoreIDs& dsids)
        throw(CdmwFT::Location::UnknownDataStoreID,
              CORBA::SystemException);              
	 */
              
    //
    // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/the_location:1.0
    //
    virtual ::FT::Location* the_location()
        throw(CORBA::SystemException);
              
    //
    // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/the_slocation:1.0
    //
    virtual char* the_slocation()
        throw(CORBA::SystemException);

protected:
    // this function is use by inherited class to get the location
    std::string get_the_location() { return m_location;}

private:

    MembersAdmin_impl(const MembersAdmin_impl&);
    void operator=(const MembersAdmin_impl&);

    PortableServer::POA_var m_poa;

protected:
    // location where members admin is running
    std::string             m_location;

private:
    // Group Repository reference which is responsible to manage Group information.
    Cdmw::FT::Location::GroupRepository_impl* m_group_repository;
};


} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_LOCATION_MEMBERS_ADMIN_HPP

