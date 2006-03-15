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

#ifndef CARDAMOM_LB_ADMIN_H_
#define CARDAMOM_LB_ADMIN_H_

/**
 * @brief A Class Admin for object group management
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */
#include <LoadBalancing/idllib/CdmwLBGroupManager.stub.hpp>
#include <Foundation/orbsupport/CosNaming.stub.hpp>


namespace Cdmw
{

namespace LB
{
 
    class LBGroupAdmin {
    public:

        // Constructor
        LBGroupAdmin(CdmwLB::LBGroupManager_ptr gm, CosNaming::NamingContext_ptr ns, CORBA::ORB_ptr orb);

        // Destructor
        ~LBGroupAdmin();
        
    public:
        void run();
        static const int ERROR                = 0;
        static const int SUCCESS              = 1;
        
        
    private:
        static const int ADD_GROUP_OPT        = 1;
        static const int ADD_REPLICA_OPT      = 2;
        static const int REMOVE_GROUP_OPT     = 4;
        static const int REMOVE_MEMBER_OPT    = 3;
        static const int SHUT_DOWN_LB_OPT     = 5;
        static const int EXIT_OPT             = 6;
        
        
        CdmwLB::LBGroupManager_var m_gm;
        CosNaming::NamingContext_var m_ns;
        CORBA::ORB_var m_orb;
    };
    
} /* LB */
    
} /* Cdmw */

#endif /* AC_LB_ADMIN_H_ */
