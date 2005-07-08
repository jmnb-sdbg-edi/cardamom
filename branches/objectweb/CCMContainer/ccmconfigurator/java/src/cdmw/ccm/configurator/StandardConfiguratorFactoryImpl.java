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


package cdmw.ccm.configurator;

import cdmw.orbsupport.PolicyList;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBSupport;

import com.thalesgroup.CdmwDeployment.StandardHomeConfigurator;

import org.omg.Components.StandardConfigurator;
import org.omg.Components.WrongComponentType;

public class StandardConfiguratorFactoryImpl
    extends com.thalesgroup.CdmwDeployment.StandardConfiguratorFactoryPOA {

    /**
     * Reference to the current ORB
     */
    private org.omg.CORBA.ORB orb;

    /**
     * StandardConfiguratorFactory POA
     */
    private org.omg.PortableServer.POA factoryPoa;

    /**
     * Name of the StandardConfiguratorFactory POA
     */
    public static final String CONFIGURATOR_POA = "ConfiguratorPOA";

    /**
     * Constructor
     *
     * @param orb
     * @param parent
     */
    public StandardConfiguratorFactoryImpl(
        org.omg.CORBA.ORB orb, org.omg.PortableServer.POA parent) {

        this.orb = orb;
        try {
            // Create policy list for simple persistence
            org.omg.CORBA.Policy[] pl = new org.omg.CORBA.Policy[6];

            pl[0] = parent.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.SYSTEM_ID);
            pl[1] = parent.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.TRANSIENT);
            pl[2] = parent.create_servant_retention_policy(
                org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN);
            pl[3] = parent.create_id_uniqueness_policy(
                org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID);
            pl[4] = parent.create_request_processing_policy(
                org.omg.PortableServer.RequestProcessingPolicyValue.USE_ACTIVE_OBJECT_MAP_ONLY);
            pl[5] = parent.create_implicit_activation_policy(
                org.omg.PortableServer.ImplicitActivationPolicyValue.IMPLICIT_ACTIVATION);

            // Create new POA
            StrategyList poaStrategies = new StrategyList();
            // Strategies with MULTITHREAD policy will accept Single Thread and
            // Multi Thread strategies. However, SERIALIZE policy will only allow
            // Single Thread policy.
            this.factoryPoa = ORBSupport.createPOA(parent, CONFIGURATOR_POA,
                parent.the_POAManager(), new PolicyList(pl), poaStrategies);

        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new org.omg.CORBA.INTERNAL(aae.toString());
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            // should never happen
            throw new org.omg.CORBA.INTERNAL(ip.toString());
        }

    }


    public org.omg.PortableServer.POA _default_POA() {
        return factoryPoa;
    }



    //
    // IDL:thalesgroup.com/CdmwDeployment/StandardConfiguratorFactory/create_configurator:1.0
    //
    /**
     * This operation creates a StandardConfigurator for a
     * specified component type.
     *
     * @param type_id The RepositoryId of the component type for which a
     * StandardConfigurator is required.
     *
     * @exception WrongComponentType If the StandardConfiguratorFactory
     * doesn't know the component type.
     */
    public StandardConfigurator create_configurator(String type_id)
        throws WrongComponentType {

        GenericConfiguratorImpl configuratorImpl
            = new GenericConfiguratorImpl(this.orb, this.factoryPoa, type_id);
        return configuratorImpl._this(orb);

    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/StandardConfiguratorFactory/create_home_configurator:1.0
    //
    /**
     * This operation creates a StandardHomeConfigurator for a
     * specified home type.
     *
     * @param type_id The RepositoryId of the home type for which a
     * StandardHomeConfigurator is required.
     *
     * @exception WrongComponentType If the StandardConfiguratorFactory
     * doesn't know the home type.
     */
    public StandardHomeConfigurator create_home_configurator(String type_id)
        throws WrongComponentType {

        GenericConfiguratorImpl configuratorImpl
            = new GenericConfiguratorImpl(this.orb, this.factoryPoa, type_id);
        return configuratorImpl._this(orb);

    }

}

