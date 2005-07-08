/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_CCM_CONTAINER_INIT_UTILS_HPP
#define INCL_CCM_CONTAINER_INIT_UTILS_HPP

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/CORBASystemExceptionWrapper.hpp>


namespace Cdmw {

namespace CCM {

namespace Container {

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
         * <p> 
         * 
         *@param orb  A reference to the ORB pseudo-object
         *
         *@exception CORBA::SystemException
         */ 
        static 
        void 
        register_valuetype_factories(CORBA::ORB_ptr orb)
            throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);
    }; // End class InitUtils


} // End namespace Container

} // End namespace CCM

} // End namespce Cdmw


#endif  // INCL_CCM_CONTAINER_INIT_UTILS_HPP

