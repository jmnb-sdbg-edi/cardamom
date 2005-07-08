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


package cdmw.basicdemo;

import cdmw.common.Assert;
import cdmw.testutils.Testable;

//
// IDL:thalesgroup.com/BasicDemo/CCM_Client:1.0
//
public class CCMClientImpl extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.UserBasicDemo.User_CCM_Client {

    /**
     * This class provides an object to test the ccm_activate.
     *
     */
    private class CCMClientActivateTest extends Testable {

        private CCMClientImpl client;
        
        public CCMClientActivateTest(CCMClientImpl client) {
            this.name = "Test ccm_activate on Client";
            this.out = System.out;
            this.client = client;
        }
    
        public void doTests() {
            Assert.check(this.client != null);
            setNbOfRequestedTestOK(5);
            
            com.acme.BasicDemo.Registration registerServer = null;
            com.acme.BasicDemo.CCM_Client_Context clientContext = 
                this.client.getContext();

            println("Trying get_connection_to_register...");
            boolean testOk = false;
            try {
                registerServer = clientContext.get_connection_to_register();

                if (registerServer != null) {
                    testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);
    
            println("Registering client...");
            testOk = false;
            try {
                if (registerServer != null) {
                    String clientName = registerServer.register();
                    this.client.setName(clientName);
                    testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Connecting to the display facet...");
            com.acme.BasicDemo.Display display = null;
            testOk = false;
            try {
                display = clientContext.get_connection_to_server();
                if (display != null) {
                    testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Printing 'hello' via the server...");
            testOk = false;
            try {
                if (registerServer != null) {
                    display.print(this.client.getText());
                    testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("Printing ValueType hello via the server...");
            testOk = false;
            try {
                if (registerServer != null) {
                    com.acme.BasicDemo.Event event = new EventImpl();
                    event.text = "Hello ValueType!";
         
                    display.print_event(event);
                    testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);
        }

    }

    private String name;
    
    private String text;
    
    private com.acme.BasicDemo.CCM_Client_Context sessionContext;

    public CCMClientImpl() {
        this.name = "";
        this.sessionContext = null;
    }

    //
    // IDL:thalesgroup.com/BasicDemo/CCM_Client_Executor/name:1.0
    //
    public String name() {
        return this.name;
    }
     
    //
    // IDL:thalesgroup.com/BasicDemo/CCM_Client_Executor/event:1.0
    //
    //public com.acme.BasicDemo.Event event() {
    //    com.acme.BasicDemo.Event event = new EventImpl();
    public com.acme.BasicDemo.Event2 event() {
        com.acme.BasicDemo.Event2 event = new com.acme.BasicDemo.Event2();
        event.text = this.text;
        return event;
    }

    public void event(com.acme.BasicDemo.Event2 event) {
        this.text = event.text;
    }

    public void set_session_context(org.omg.Components.SessionContext ctx) {
        System.out.println("setSessionContext called!");
        this.sessionContext = 
            com.acme.BasicDemo.CCM_Client_ContextHelper.narrow(ctx);
    }

    public void ccm_activate() {
        CCMClientActivateTest test = new CCMClientActivateTest(this);
        test.start();
    }
    
    public void ccm_passivate() {
        System.out.println("ccm_passivate called!");
    }
    
    public void ccm_remove() {
        System.out.println("ccm_remove called!");
    }

     // additional operation
     
    public void setName(String name) {
        this.name = name;
    }
    
    public com.acme.BasicDemo.CCM_Client_Context getContext() {
        return this.sessionContext;
    }

    public String getText() {
        return this.text;
    }

}

