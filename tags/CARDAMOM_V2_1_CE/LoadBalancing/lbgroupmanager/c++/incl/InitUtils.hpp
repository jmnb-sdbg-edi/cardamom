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


#ifndef _CDMW_LB_GROUP_MANAGER_INIT_UTILS_HPP_
#define _CDMW_LB_GROUP_MANAGER_INIT_UTILS_HPP_

#include "idllib/CdmwLBGroupManager.stub.hpp"
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <string>
#include <ostream>

namespace Cdmw {
namespace LB {
namespace GroupManager {


    /**
     * Purpose:
     * <p> This class provides helper functions to be used by the CDMW generated 
     * CdmwInterface.cpp file.
     *
     *@see CDMW_init
     *@see CDMW_cleanup
     */
    class InitUtils
    {
    public:

        /**
         * Purpose:
         * <p> pre_init the ORB where a CDMW ReplicationManager object will be created.
         * 
         *@exception CORBA::SystemException
         */        
        static 
        void
        pre_init(int& argc, char** argv)
            throw(CORBA::SystemException);
        

        /** Purpose:
         * <p> post_init the context where a CDMW ReplicationManager object has been created.
         * 
         *@exception CORBA::SystemException
         */    
        static
        void
        post_init (CdmwFT::ReplicationManager_ptr the_replication_manager)
            throw(CORBA::SystemException);


        /**
         * Purpose:
         * <p> Creates a CDMW ReplicationManager object.
         * <b>NOTA</b>: The current version of the CDMW ReplicationManager
         * serialises processing of all request invocations.
         * 
         *@param orb  A reference to the ORB pseudo-object
         *@param parent  A reference to the CdmwRootPOA pseudo-object
         *@param ft_domain_id  The Fault Tolerance domain identifier
         *@param echo_stream An output stream for use by the ReplicationManager
         * whenever there is a need to output a debug/warning/info message.
         *
         *@return A reference to a CDMW ReplicationManager object
         *
         *@exception CORBA::SystemException
         */        
        static 
        CdmwFT::ReplicationManager_ptr
        create_replication_manager(int                     argc, 
                                   char*                   argv[],
                                   CORBA::ORB_ptr          orb,
                                   PortableServer::POA_ptr parent,
                                   const char*             ft_domain_id,
                                   const char*             dataStorePort,
                                   const char*             domain,
                                   const char*             ip_address,
                                   int                     max_transaction_in_progress,
                                   int                     max_transaction_done,
                                   OsSupport::OS::Timeval  cohort_timeout,
                                   OsSupport::OS::Timeval  coordinator_timeout,
                                   const char*             security_level,
                                   const char*             replication_level,
                                   std::ostream &          echo_stream)
            throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);


    }; // End class InitUtils
    
}; // namespace ReplicationManager    
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_REPLICATION_MANAGER_INIT_UTILS_HPP_

