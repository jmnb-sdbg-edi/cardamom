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

#ifndef INCL_CDMW_FT_REPLICATION_MANAGER_REPOSITORY_IMPL_HPP
#define INCL_CDMW_FT_REPLICATION_MANAGER_REPOSITORY_IMPL_HPP

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

#include <ftreplicationmanager/CdmwFTReplicationManagerRepository.skel.hpp>
#include <idllib/CdmwFTReplicationManager.stub.hpp>



namespace Cdmw
{
    
namespace FT
{

class ReplicationManagerRepository_impl : public CdmwFT::ReplicationManagerRepository,
                                          public Cdmw::OrbSupport::RefCountLocalObject
{
public:
    /**
    * Purpose:
    * <p>  
    *  Return the single instance of the GroupRepository_impl 
    *
    *@param  none
    *@exception none
    */ 
    static ReplicationManagerRepository_impl*  Get_instance()
        throw();

    ~ReplicationManagerRepository_impl();


    void init(CdmwFT::ReplicationManager_ptr the_replication_manager)
        throw (CORBA::SystemException);

    //
    // IDL to C++ Mapping
    //

    virtual CdmwFT::ReplicationManager_ptr
    get_replication_manager()
        throw (CORBA::SystemException);

protected:
    ReplicationManagerRepository_impl();

    // singleton instance
    static ReplicationManagerRepository_impl* mp_instance;  

private:
    ReplicationManagerRepository_impl(const ReplicationManagerRepository_impl&);
    void operator=(const ReplicationManagerRepository_impl&);


    // set to true when init operation has been called with success
    bool          m_is_initialised;


    CdmwFT::ReplicationManager_var m_replication_manager;


};

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_REPLICATION_MANAGER_REPOSITORY_IMPL_HPP

