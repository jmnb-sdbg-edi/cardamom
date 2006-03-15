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


package cdmw.ccm.configurator.test;

public class HelloWorldImpl
    extends com.thalesgroup.HelloModule.HelloWorldPOA {

    private org.omg.Components.CCM2Context context;
    private boolean isRemoved;

    private int count;
    private com.thalesgroup.HelloModule.Value1 test;

    /**
     * Constructor
     *
     * @param ctx
     */
    public HelloWorldImpl(org.omg.Components.CCM2Context ctx) {
        this.context = ctx;
        isRemoved = false;
    }

    //
    // IDL:thalesgroup.com/HelloModule/Hello/say_hello:1.0
    //
    public void say_hello() {
        System.out.println(this.count + ": Hello !");
    }

    //
    // IDL:thalesgroup.com/HelloModule/Hello/coucou:1.0
    //
    public String coucou() {
        return "Coucou !";
    }

    //
    // IDL:thalesgroup.com/HelloModule/Hello/count:1.0
    //
    public int count() {
        return ++count;
    }

    public void count(int val) {
        this.count = val;
    }

    //
    // IDL:thalesgroup.com/HelloModule/Hello/test:1.0
    //

    public com.thalesgroup.HelloModule.Value1 test() {
        return this.test;
    }

    public void test(com.thalesgroup.HelloModule.Value1 val) {
        this.test = val;
    }

    //
    // IDL:omg.org/Components/CCMObject/get_component_def:1.0
    //
    public org.omg.Components.IRObject get_component_def() {
        return null;
    }

    //
    // IDL:omg.org/Components/CCMObject/get_ccm_home:1.0
    //
    public org.omg.Components.CCMHome get_ccm_home() {
        return context.get_CCM_home();
    }

    //
    // IDL:omg.org/Components/CCMObject/get_primary_key:1.0
    //
    public org.omg.Components.PrimaryKeyBase get_primary_key()
        throws org.omg.Components.NoKeyAvailable {
        throw new org.omg.Components.NoKeyAvailable();
    }

    //
    // IDL:omg.org/Components/CCMObject/configuration_complete:1.0
    //
    public void configuration_complete()
        throws org.omg.Components.InvalidConfiguration {
        // do nothing
    }

    //
    // IDL:omg.org/Components/CCMObject/remove:1.0
    //
    public void remove() throws org.omg.Components.RemoveFailure {

        try {
            if (isRemoved()) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    cdmw.orbsupport.ExceptionMinorCodes.OBJECT_NOT_EXIST,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
            // deactivate servant
            context.req_passivate();

            // register object as removed
            isRemoved = true;
        } catch(org.omg.Components.PolicyMismatch pm) {
            throw new org.omg.Components.RemoveFailure();
        }

    }

    //
    // IDL:omg.org/Components/CCMObject/get_all_ports:1.0
    //
    public org.omg.Components.ComponentPortDescription get_all_ports() {
        return null;
    }


    //
    // IDL:omg.org/Components/Navigation/provide_facet:1.0
    //
    public org.omg.CORBA.Object provide_facet(String name)
        throws org.omg.Components.InvalidName {
        return null;
    }

    //
    // IDL:omg.org/Components/Navigation/get_all_facets:1.0
    //
    public org.omg.Components.FacetDescription[] get_all_facets() {
        return null;
    }

    //
    // IDL:omg.org/Components/Navigation/get_named_facets:1.0
    //
    public org.omg.Components.FacetDescription[]
        get_named_facets(String[] names)
        throws org.omg.Components.InvalidName {
        return null;
    }

    //
    // IDL:omg.org/Components/Navigation/same_component:1.0
    //
    public boolean same_component(org.omg.CORBA.Object object_ref) {

        org.omg.Components.SessionContext session
            = org.omg.Components.SessionContextHelper.narrow(context);
        if (session == null) {
            throw new org.omg.CORBA.INTERNAL();
        }

        try {
            org.omg.CORBA.Object obj = session.get_CCM_object();
            return obj._is_equivalent(object_ref);
        } catch(org.omg.Components.IllegalState is) {
            throw new org.omg.CORBA.INTERNAL();
        }

    }


    //
    // IDL:omg.org/Components/Receptacles/connect:1.0
    //
    public org.omg.Components.Cookie connect(
        String name, org.omg.CORBA.Object connection)
        throws org.omg.Components.InvalidName,
               org.omg.Components.InvalidConnection,
               org.omg.Components.AlreadyConnected,
               org.omg.Components.ExceededConnectionLimit {
        return null;
    }

    //
    // IDL:omg.org/Components/Receptacles/disconnect:1.0
    //
    public void disconnect(String name, org.omg.Components.Cookie ck)
        throws org.omg.Components.InvalidName,
               org.omg.Components.InvalidConnection,
               org.omg.Components.CookieRequired,
               org.omg.Components.NoConnection {
        // do nothing
    }

    //
    // IDL:omg.org/Components/Receptacles/get_connections:1.0
    //
    public org.omg.Components.ConnectionDescription[]
        get_connections(String name) throws org.omg.Components.InvalidName {
        return null;
    }

    //
    // IDL:omg.org/Components/Receptacles/get_all_receptacles:1.0
    //
    public org.omg.Components.ReceptacleDescription[] get_all_receptacles() {
        return null;
    }

    //
    // IDL:omg.org/Components/Receptacles/get_named_receptacles:1.0
    //
    public org.omg.Components.ReceptacleDescription[]
        get_named_receptacles(String[] names)
        throws org.omg.Components.InvalidName {
        return null;
    }


    //
    // IDL:omg.org/Components/Events/get_consumer:1.0
    //
    public org.omg.Components.EventConsumerBase get_consumer(String sink_name)
        throws org.omg.Components.InvalidName {
        return null;
    }

    //
    // IDL:omg.org/Components/Events/subscribe:1.0
    //
    public org.omg.Components.Cookie subscribe(
        String publisher_name, org.omg.Components.EventConsumerBase subscriber)
        throws org.omg.Components.InvalidName,
               org.omg.Components.InvalidConnection,
               org.omg.Components.ExceededConnectionLimit {
        return null;
    }

    //
    // IDL:omg.org/Components/Events/unsubscribe:1.0
    //
    public void unsubscribe(
        String publisher_name, org.omg.Components.Cookie ck)
        throws org.omg.Components.InvalidName,
               org.omg.Components.InvalidConnection {
        // do nothing
    }

    //
    // IDL:omg.org/Components/Events/connect_consumer:1.0
    //
    public void connect_consumer(
        String emitter_name, org.omg.Components.EventConsumerBase consumer)
        throws org.omg.Components.InvalidName,
               org.omg.Components.AlreadyConnected,
               org.omg.Components.InvalidConnection {
        // do nothing
    }

    //
    // IDL:omg.org/Components/Events/disconnect_consumer:1.0
    //
    public org.omg.Components.EventConsumerBase
        disconnect_consumer(String source_name)
        throws org.omg.Components.InvalidName,
               org.omg.Components.NoConnection {
        return null;
    }

    //
    // IDL:omg.org/Components/Events/get_all_consumers:1.0
    //
    public org.omg.Components.ConsumerDescription[] get_all_consumers() {
        return null;
    }

    //
    // IDL:omg.org/Components/Events/get_named_consumers:1.0
    //
    public org.omg.Components.ConsumerDescription[]
        get_named_consumers(String[] names)
        throws org.omg.Components.InvalidName {
        return null;
    }

    //
    // IDL:omg.org/Components/Events/get_all_emitters:1.0
    //
    public org.omg.Components.EmitterDescription[] get_all_emitters() {
        return null;
    }

    //
    // IDL:omg.org/Components/Events/get_named_emitters:1.0
    //
    public org.omg.Components.EmitterDescription[]
        get_named_emitters(String[] names)
        throws org.omg.Components.InvalidName {
            return null;
    }

    //
    // IDL:omg.org/Components/Events/get_all_publishers:1.0
    //
    public org.omg.Components.PublisherDescription[] get_all_publishers() {
        return null;
    }

    //
    // IDL:omg.org/Components/Events/get_named_publishers:1.0
    //
    public org.omg.Components.PublisherDescription[]
        get_named_publishers(String[] names)
        throws org.omg.Components.InvalidName {
        return null;
    }

    /**
     * This checks whether the CORBA object incarnated by the current
     * object (this) has been removed (<I>remove</I> operation has been
     * called).
     *
     * @return <B>true</B> if the object has been removed,
     * <B>false</B> otherwise.
     */
    public boolean isRemoved() {
        return isRemoved == true;
    }

}

