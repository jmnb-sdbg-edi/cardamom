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


#ifndef INCL_CDMW_CCM_CONTAINER_VALUE_TYPE_REGISTRATION_IMPL_HPP
#define INCL_CDMW_CCM_CONTAINER_VALUE_TYPE_REGISTRATION_IMPL_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include "CCMContainer/idllib/CdmwCcmContainer.skel.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"


namespace Cdmw
{

namespace CCM
{
namespace Container
{
    /**
     * Implementation of the Session2Context internal interface for use with
     * Session container type.
     */
    class ValueTypeRegistration_impl :
        public CdmwCcmContainer::ValueTypeRegistration,
        public Cdmw::OrbSupport::RefCountLocalObject
    {
    public:
        /**
        * Purpose:
        * <p> Constructor.
        *
        *@param orb A reference to the ORB pseudo-object
        *
        *@exception CORBA::SystemException TODO:
        */
        ValueTypeRegistration_impl(CdmwCcmContainer::ValueTypeInstaller_ptr installer,
                                   const std::string &                       repid)
            throw (CORBA::SystemException);
        
        /**
        * Purpose:
        * <p> Destructor
        *
        */
        ~ValueTypeRegistration_impl()
            throw ();
        
         //
        // IDL:thalesgroup.com/CdmwCcmContainer/ValueTypeFactoryRegistration/unregister:1.0
        //
        virtual void unregister()
           throw (CORBA::SystemException);
 
       
    private:

        /**
         *[Attribute description]
         */
        CdmwCcmContainer::ValueTypeInstaller_var m_installer;
        const std::string                         m_repid;

        
        ValueTypeRegistration_impl();
        ValueTypeRegistration_impl(const ValueTypeRegistration_impl&);
    };
}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_VALUE_TYPE_REGISTRATION_IMPL_HPP

