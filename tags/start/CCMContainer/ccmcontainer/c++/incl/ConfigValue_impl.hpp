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


#ifndef INCL_CDMW_CCM_CONTAINER_CONFIG_VALUE_IMPL_HPP
#define INCL_CDMW_CCM_CONTAINER_CONFIG_VALUE_IMPL_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include "CCMContainer/idllib/Components.skel.hpp"

namespace Cdmw
{

namespace CCM
{
namespace Container
{
    
    /**
    * Purpose:
    * <p> Implementation of a ConfigValue valuetype
    *
    */
    class ConfigValue_impl : public OBV_Components::ConfigValue,
                             public virtual CORBA::DefaultValueRefCountBase
    {
    public:
        /**
        * Purpose:
        * <p> Constructor
        */
        ConfigValue_impl();
        
        /**
        * Purpose:
        * <p> Constructor
        *
        *@param the_name Name of the ConfigValue
        *@param the_value Value of the ConfigValue
        */
        ConfigValue_impl(const char* the_name, const CORBA::Any& the_value);
        
        /**
        * Purpose:
        * <p> Destructor
        *
        */
        ~ConfigValue_impl() throw();

        
        /**
        * Purpose:
        * <p> Clone current ConfigValue
        *
        *@exception A pointer to a clone of self
        */
        virtual CORBA::ValueBase* _copy_value();

    };

    /**
     * Implementation of a ConfigValue valuetype factory.
     */
    class ConfigValueFactory : virtual public Components::ConfigValue_init
    {
    public:
        CORBA::ValueBase * create_for_unmarshal ();
    };
}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_CONFIG_VALUE_IMPL_HPP

