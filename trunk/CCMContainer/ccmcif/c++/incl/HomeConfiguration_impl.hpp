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


#ifndef INCL_CCM_CIF_HOMECONFIGURATION_IMPL_HPP
#define INCL_CCM_CIF_HOMECONFIGURATION_IMPL_HPP

#include <Foundation/orbsupport/RefCountServantBase.hpp>

#include <CCMContainer/idllib/Components.skel.hpp>



namespace Cdmw {

namespace CCM {

namespace CIF {


//
// IDL:omg.org/Components/HomeConfiguration:1.0
//
class HomeConfiguration_impl : virtual public POA_Components::HomeConfiguration,
                               virtual public OrbSupport::RefCountServantBase
{
    HomeConfiguration_impl(const HomeConfiguration_impl&);
    void operator=(const HomeConfiguration_impl&);


public:

    HomeConfiguration_impl();
    ~HomeConfiguration_impl();

    /**
    * Purpose:
    * <p>
    *   establishes a configurator object for the target home object.
    *   Factory operations on the home object will apply this configurator to 
    *   newly-created instances.
    * 
    *@param cfg  The configurator.
    *
    *@return void
    *@exception BAD_INV_ORDER Exception raised if a client attempts to invoke this
    *   operation after disable_home_configuration operation has been requested.
    *@exception Any other CORBA Exception.
    *
    */ 
    //
    // IDL:omg.org/Components/HomeConfiguration/set_configurator:1.0
    //
    virtual void set_configurator(Components::Configurator_ptr cfg)
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   establishes an attribute configuration for the target home object, 
    *   as an instance of Components::ConfigValues.
    *   Factory operations on the home object will apply this configurator to 
    *   newly-created instances.
    * 
    *@param config  The attribute configuration.
    *
    *@return void
    *@exception BAD_INV_ORDER Exception raised if a client attempts to invoke this
    *   operation after disable_home_configuration operation has been requested.
    *@exception Any other CORBA Exception.
    *
    */ 
    //
    // IDL:omg.org/Components/HomeConfiguration/set_configuration_values:1.0
    //
    virtual void set_configuration_values(const Components::ConfigValues& config)
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   This operation determines whether factory operations on the target home
    *   object will invoke configuration_complete on newly-created instances. 
    * 
    *@param b  If the value of the boolean is TRUE, factory operations will invoke
    *   configuration_complete on component instances after applying any required
    *   configurator or configuration values to the instance. If the parameter is
    *   FALSE, configuration_complete will not be invoked.
    *
    *@return void
    *@exception BAD_INV_ORDER Exception raised if a client attempts to invoke this
    *   operation after disable_home_configuration operation has been requested.
    *@exception Any other CORBA Exception.
    *
    */ 
    //
    // IDL:omg.org/Components/HomeConfiguration/complete_component_configuration:1.0
    //
    virtual void complete_component_configuration(CORBA::Boolean b)
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    *   This operation disables further use of operations on the HomeConfiguration
    *   interface of the target home object. If a client attempts to invoke 
    *   HomeConfiguration operations, the request will raise a BAD_INV_ORDER system
    *   exception.
    * 
    *@return void
    *@exception Any CORBA Exception.
    *
    */ 
    //
    // IDL:omg.org/Components/HomeConfiguration/disable_home_configuration:1.0
    //
    virtual void disable_home_configuration()
        throw(CORBA::SystemException);


    /*
    * Non CORBA operation
    *
    * Accessor operations
    */
    Components::Configurator* get_configurator()
    {
        return m_configurator._retn();
    };

    Components::ConfigValues* get_config_values()
    {
        return m_config_values._retn();
    };

    bool is_complete_comp_conf_enable()
    {
        return m_enable_comp_config;
    };


protected:
    Components::Configurator_var  m_configurator;  // component configurator


private:
    Components::ConfigValues_var  m_config_values; // component configuration values
    bool                          m_enable_comp_config; // false to disable call
                                                        // to configuration_complete on component
    bool                          m_enable_home_config; // false if no more configuration
                                                        // operation available
};


} // End of namespace CIF

} // End of namespace CCM

} // End of namespace Cdmw

#endif  // INCL_CCM_CIF_HOMECONFIGURATION_IMPL_HPP

