/* =========================================================================== *
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
 * =========================================================================== */

package cdmw.validation;

public class ProcessAgentFactoryImpl
    extends com.thalesgroup.Validation.ProcessAgentFactoryPOA {

    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA poa;
    private org.omg.PortableServer.POA objectsPOA;
    private String factoryName;

    // method needed by the NamedObjectsFactoryAllocator
    public ProcessAgentFactoryImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        com.thalesgroup.CdmwNamingAndRepository.NameDomain nameDomain,
        String nameDomainName,
        String factoryName,
        cdmw.lifecycle.POAThreadingPolicy threadingPolicy,
        String[] args)
        throws cdmw.common.BadParameterException,
            cdmw.common.InternalErrorException {
        this(orb, parent, factoryName, threadingPolicy);
    }

    // method needed by the AnonymousObjectsFactoryAllocator
    public ProcessAgentFactoryImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        String factoryName,
        cdmw.lifecycle.POAThreadingPolicy threadingPolicy,
        String[] args)
        throws cdmw.common.BadParameterException,
            cdmw.common.InternalErrorException {
        this(orb, parent, factoryName, threadingPolicy);
    }

    public ProcessAgentFactoryImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        String factoryName,
        cdmw.lifecycle.POAThreadingPolicy threadingPolicy)
        throws cdmw.common.BadParameterException,
            cdmw.common.InternalErrorException {

        this.orb = orb;
        this.poa = parent;
        this.objectsPOA = null;
        this.factoryName = factoryName;

        //
        // Create POA for use by the factory
        //
        // Service Life cycle framework use the following POA policies
        //
        // Lifespan policy            = TRANSIENT
        // Id Assignment policy       = SYSTEM_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = NO_IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY;
        // Servant Retention policy   = RETAIN;
        //
        org.omg.PortableServer.POAManager poaManager =
            parent.the_POAManager();
        org.omg.CORBA.Policy[] policies = new org.omg.CORBA.Policy[6];
        policies[0] =
            parent.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.SYSTEM_ID);
        policies[1] =
            parent.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.TRANSIENT);
        policies[2] =
            parent.create_servant_retention_policy(
                org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN);
        policies[3] =
            parent.create_id_uniqueness_policy(
                org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID);
        policies[4] =
            parent.create_request_processing_policy(
                org.omg.PortableServer.RequestProcessingPolicyValue.USE_ACTIVE_OBJECT_MAP_ONLY);
        policies[5] =
            parent.create_implicit_activation_policy(
                org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

        cdmw.orbsupport.StrategyList poaStrategies = new cdmw.orbsupport.StrategyList();
        // Strategies with MULTITHREAD policy will accept Single Thread and
        // Multi Thread strategies. However, SERIALIZE policy will only allow
        // Single Thread policy.
        threadingPolicy.appendToStrategyList(poaStrategies);
        cdmw.orbsupport.PolicyList policyList =
            new cdmw.orbsupport.PolicyList(policies);

        try {
            objectsPOA = cdmw.orbsupport.ORBSupport.createPOA(poa,
                                                            factoryName,
                                                            poaManager,
                                                            policyList,
                                                            poaStrategies);
        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new org.omg.CORBA.INTERNAL(0,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new org.omg.CORBA.INTERNAL(0,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }
    }

    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:thalesgroup.com/Validation/ProcessAgentFactory/create_process_agent:1.0
    //
    public com.thalesgroup.Validation.ProcessAgent create_process_agent() {
        try {
            ProcessAgentImpl proc = new ProcessAgentImpl(orb, objectsPOA);
            byte[] oid = objectsPOA.activate_object(proc);
            return proc._this(orb);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy ex) {
            throw new org.omg.CORBA.INTERNAL(0,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive ex) {
            throw new org.omg.CORBA.INTERNAL(0,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }


    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    public boolean supports(String repositoryId) {
        return repositoryId.equals(
            "IDL:thalesgroup.com/Validation/ProcessAgent:1.0");
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    public String[] get_supported_objects() {
        String[] repIds = new String[1];
        repIds[0] = "IDL:thalesgroup.com/Validation/ProcessAgent:1.0";
        return repIds;
    }

}

