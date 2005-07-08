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


package cdmw.ccm.cif;

import cdmw.orbsupport.ExceptionMinorCodes;

//
// IDL:omg.org/Components/HomeConfiguration:1.0
//
public abstract class HomeConfigurationImpl 
    extends org.omg.Components.HomeConfigurationPOA {

    /**
     * component configurator
     */
    protected org.omg.Components.Configurator configurator = null;

    /**
     * component configuration values
     */
    private org.omg.Components.ConfigValue[] configValues;
    
    /**
     * false to disable call to configuration_complete on component
     */
    private boolean enableCompConfig = false;

    /**
     * false if no more configuration operation available
     */
    private boolean enableHomeConfig = true;

    /**
     * Establishes a configurator object for the target home object.
     * Factory operations on the home object will apply this configurator to 
     * newly-created instances.
     * 
     * @param cfg  The configurator.
     *
     * @exception BAD_INV_ORDER Exception raised if a client attempts 
     * to invoke this operation after disable_home_configuration operation 
     * has been requested.
     */ 
    //
    // IDL:omg.org/Components/HomeConfiguration/set_configurator:1.0
    //
    public void set_configurator(org.omg.Components.Configurator cfg) {
        if (!this.enableHomeConfig) {
            throw new org.omg.CORBA.BAD_INV_ORDER(
                ExceptionMinorCodes.BAD_INV_ORDER, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        if (cfg == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        this.configurator = cfg;
    }

    /**
     * Establishes an attribute configuration for the target home object, 
     * as an instance of Components::ConfigValues.
     * Factory operations on the home object will apply this configurator to 
     * newly-created instances.
     * 
     * @param config  The attribute configuration.
     *
     * @exception BAD_INV_ORDER Exception raised if a client attempts 
     * to invoke this operation after disable_home_configuration operation 
     * has been requested.
     */ 
    //
    // IDL:omg.org/Components/HomeConfiguration/set_configuration_values:1.0
    //
    public void 
        set_configuration_values(org.omg.Components.ConfigValue[] config) {
        if (!this.enableHomeConfig) {
            throw new org.omg.CORBA.BAD_INV_ORDER(
                ExceptionMinorCodes.BAD_INV_ORDER, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    
        this.configValues = config;
    }

    /**
     * This operation determines whether factory operations on the target home
     * object will invoke configuration_complete on newly-created instances. 
     * 
     * @param b If the value of the boolean is TRUE, factory operations will 
     * invoke configuration_complete on component instances after applying any 
     * required configurator or configuration values to the instance. 
     * If the parameter is FALSE, configuration_complete will not be invoked.
     *
     * @exception BAD_INV_ORDER Exception raised if a client attempts 
     * to invoke this operation after disable_home_configuration operation 
     * has been requested.
     */ 
    //
    // IDL:omg.org/Components/HomeConfiguration/complete_component_configuration:1.0
    //
    public void complete_component_configuration(boolean b) {
        if (!this.enableHomeConfig) {
            throw new org.omg.CORBA.BAD_INV_ORDER(
                ExceptionMinorCodes.BAD_INV_ORDER, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        this.enableCompConfig = b;
    }

    /**
     * This operation disables further use of operations 
     * on the HomeConfiguration interface of the target home object. 
     * If a client attempts to invoke HomeConfiguration operations, the request 
     * will raise a BAD_INV_ORDER system exception.
     */ 
    //
    // IDL:omg.org/Components/HomeConfiguration/disable_home_configuration:1.0
    //
    public void disable_home_configuration() {
        this.enableHomeConfig = false;
    }

    /**
     * Returns the component configurator.
     * @return The component configurator
     */
    public org.omg.Components.Configurator getConfigurator() {
        return this.configurator;
    }

    /**
     * Returns component configuration values
     * @return The component configuration values
     */
    public org.omg.Components.ConfigValue[] getConfigValues() {
        return this.configValues;
    }

    /**
     * Returns the possibility to call configuration_complete on component
     * @return false if call to configuration_complete on component is disabled
     * true otherwise
     */
    public boolean isCompleteCompConfEnable() {
        return this.enableCompConfig;
    }

}

