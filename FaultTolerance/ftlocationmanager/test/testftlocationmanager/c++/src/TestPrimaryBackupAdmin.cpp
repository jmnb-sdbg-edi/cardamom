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


#include <exception>
#include <iostream>

#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include "testftlocationmanager/TestPrimaryBackupAdmin.hpp"



namespace Cdmw {
    namespace FT {
        
        // constructor
        TestPrimaryBackupAdmin::TestPrimaryBackupAdmin(PortableServer::POA_ptr   poa, 
                                         const ::FT::Location&     the_location )
            : m_update_group_view_called(false),
              m_update_primary_called(false),
              m_poa (PortableServer::POA::_duplicate(poa)),
              m_location(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location))
        {
        }
        
        // destructor
        TestPrimaryBackupAdmin::~TestPrimaryBackupAdmin()
        {
        }
    
        PortableServer::POA_ptr
        TestPrimaryBackupAdmin::_default_POA()
        {
            return PortableServer::POA::_duplicate(m_poa.in());
        }
        


        //
        // IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupAdmin/update_primary:1.0
        //
        void 
        TestPrimaryBackupAdmin::update_primary(const CdmwFT::GroupInfo& group_info,
                                const ::FT::Location& primary_location)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException)
        {
            m_update_primary_called = true;
        }       

         //
        // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/update_group_view:1.0
        //
        void
        TestPrimaryBackupAdmin::update_group_view(const CdmwFT::GroupInfo& group_info)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException)
        {
            m_update_group_view_called = true;
        }
        
		  /* FNO_JE added code
        //
        // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/get_data_store_infos:1.0
        //
        CdmwFT::StateTransfer::DataStoreInfos* 
        TestPrimaryBackupAdmin::get_data_store_infos(const CdmwFT::StateTransfer::DataStoreIDs& dsids)
            throw(CdmwFT::Location::UnknownDataStoreID,
                  CORBA::SystemException)
        {
            if (dsids.length() == 0) {
                // if dsids is emtpy: multicast state transfer
                // return empty sequence
                return new CdmwFT::StateTransfer::DataStoreInfos();
            }
            
            // not implemented for point-to-point state transfer
            throw CORBA::NO_IMPLEMENT();
        }
		  */
        
              
        //
        // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/the_location:1.0
        //
        ::FT::Location*
        TestPrimaryBackupAdmin::the_location()
            throw(CORBA::SystemException)
        {
            ::FT::Location* location = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(m_location);
            return location;
        }
        
        
    }; // namespace FT
}; // namespace Cdmw

