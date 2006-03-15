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


package cdmw.ccm.cif.test;

import cdmw.testutils.Testable;
import cdmw.common.Assert;
import org.omg.CORBA.ShortHolder;
import org.omg.CORBA.LongHolder;
import org.omg.CORBA.IntHolder;
import org.omg.CORBA.FloatHolder;
import org.omg.CORBA.BooleanHolder;
import org.omg.CORBA.CharHolder;
import org.omg.CORBA.DoubleHolder;
import org.omg.CORBA.StringHolder;
import org.omg.CORBA.ByteHolder;
import org.omg.CORBA.AnyHolder;
import org.omg.CORBA.Any;

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
        };
    
        public void doTests() {
        
            // set number of requested successfull tests
            setNbOfRequestedTestOK(29);
        
            Assert.check(this.client != null);
            
            com.acme.CommonDemo.Registration registerServer = null;
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
                    event.text = new String("Hello ValueType!");
         
                    display.print_event(event);
                    testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!" + e.toString());
            }
            check(testOk);


            println("test_short ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    short in_ = 1;
                    ShortHolder inout_ = new ShortHolder();
                    ShortHolder out_ = new ShortHolder();
                    short ret_;
         
                    ret_ = display.test_short(in_, inout_, out_);
                    if ((inout_.value == (in_*2))
                        && (out_.value == (in_*3))
                        && (ret_ == (in_*4)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);


            println("test_long ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    int in_ = 1;
                    IntHolder inout_ = new IntHolder();
                    IntHolder out_ = new IntHolder();
                    int ret_;
         
                    ret_ = display.test_long(in_, inout_, out_);
                    if ((inout_.value == (in_*2))
                        && (out_.value == (in_*3))
                        && (ret_ == (in_*4)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_long_long ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    long in_ = 1;
                    LongHolder inout_ = new LongHolder();
                    LongHolder out_ = new LongHolder();
                    long ret_;
         
                    ret_ = display.test_long_long(in_, inout_, out_);
                    if ((inout_.value == (in_*2))
                        && (out_.value == (in_*3))
                        && (ret_ == (in_*4)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_unsigned_short ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    short in_ = 1;
                    ShortHolder inout_ = new ShortHolder();
                    ShortHolder out_ = new ShortHolder();
                    short ret_;
         
                    ret_ = display.test_unsigned_short(in_, inout_, out_);
                    if ((inout_.value == (in_*2))
                        && (out_.value == (in_*3))
                        && (ret_ == (in_*4)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);


            println("test_unsigned_long ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    int in_ = 1;
                    IntHolder inout_ = new IntHolder();
                    IntHolder out_ = new IntHolder();
                    int ret_;
         
                    ret_ = display.test_unsigned_long(in_, inout_, out_);
                    if ((inout_.value == (in_*2))
                        && (out_.value == (in_*3))
                        && (ret_ == (in_*4)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_unsigned_long_long ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    long in_ = 1;
                    LongHolder inout_ = new LongHolder();
                    LongHolder out_ = new LongHolder();
                    long ret_;
         
                    ret_ = display.test_unsigned_long_long(in_, inout_, out_);
                    if ((inout_.value == (in_*2))
                        && (out_.value == (in_*3))
                        && (ret_ == (in_*4)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_float ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    float in_ = 1;
                    FloatHolder inout_ = new FloatHolder();
                    FloatHolder out_ = new FloatHolder();
                    float ret_;
         
                    ret_ = display.test_float(in_, inout_, out_);
                    if ((inout_.value == (in_*2))
                        && (out_.value == (in_*3))
                        && (ret_ == (in_*4)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_double ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    double in_ = 1;
                    DoubleHolder inout_ = new DoubleHolder();
                    DoubleHolder out_ = new DoubleHolder();
                    double ret_;
         
                    ret_ = display.test_double(in_, inout_, out_);
                    if ((inout_.value == (in_*2))
                        && (out_.value == (in_*3))
                        && (ret_ == (in_*4)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_boolean ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    boolean in_ = true;
                    BooleanHolder inout_ = new BooleanHolder();
                    BooleanHolder out_ = new BooleanHolder();
                    boolean ret_;
         
                    ret_ = display.test_boolean(in_, inout_, out_);
                    if ((inout_.value == in_)
                        && (out_.value == in_)
                        && (ret_ == in_))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_char ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    char in_ = '1';
                    CharHolder inout_ = new CharHolder();
                    CharHolder out_ = new CharHolder();
                    char ret_;
         
                    ret_ = display.test_char(in_, inout_, out_);
                    if ((inout_.value == in_)
                        && (out_.value == in_)
                        && (ret_ == in_))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_wchar ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    char in_ = '1';
                    CharHolder inout_ = new CharHolder();
                    CharHolder out_ = new CharHolder();
                    char ret_;
         
                    ret_ = display.test_wchar(in_, inout_, out_);
                    if ((inout_.value == in_)
                        && (out_.value == in_)
                        && (ret_ == in_))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_octet ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    byte in_ = 1;
                    ByteHolder inout_ = new ByteHolder();
                    ByteHolder out_ = new ByteHolder();
                    byte ret_;
         
                    ret_ = display.test_octet(in_, inout_, out_);
                    if ((inout_.value == in_)
                        && (out_.value == in_)
                        && (ret_ == in_))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_interface ...");
            testOk = false;
            try {
                if (registerServer != null) {

                    display.set(1);
                    com.acme.BasicDemo.DisplayHolder inout_ = new com.acme.BasicDemo.DisplayHolder((com.acme.BasicDemo.Display)(display._duplicate()));
                    inout_.value.set(2);
                    com.acme.BasicDemo.DisplayHolder out_ = new com.acme.BasicDemo.DisplayHolder();
                    com.acme.BasicDemo.Display ret_;
         
                    ret_ = display.test_function(display, inout_, out_);
                    if ((inout_.value.get() == display.get())
                        && (out_.value.get() == display.get())
                        && (ret_.get() == display.get()))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_fixed_struct ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    com.acme.BasicDemo.s1 in_ = new com.acme.BasicDemo.s1();
                    in_.l=1;
                    com.acme.BasicDemo.s1Holder inout_ = new com.acme.BasicDemo.s1Holder(new com.acme.BasicDemo.s1()); 
                    inout_.value.l=0;
                    com.acme.BasicDemo.s1Holder out_ = new com.acme.BasicDemo.s1Holder();
                    com.acme.BasicDemo.s1 ret_;                   

                    ret_ = display.test_fixed_struct(in_, inout_, out_);
                    if ((inout_.value.l == in_.l)
                        && (out_.value.l == in_.l)
                        && (ret_.l == in_.l))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!" + e.toString());
            }
            check(testOk);

            println("test_var_struct ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    com.acme.BasicDemo.s2 in_ = new com.acme.BasicDemo.s2();
                    in_.s="1";
                    com.acme.BasicDemo.s2Holder inout_ = new com.acme.BasicDemo.s2Holder( new com.acme.BasicDemo.s2()); 
                    com.acme.BasicDemo.s2Holder out_ = new com.acme.BasicDemo.s2Holder();
                    com.acme.BasicDemo.s2 ret_;                   
         
                    ret_ = display.test_var_struct(in_, inout_, out_);
                    if ((inout_.value.s.equals(in_.s))
                        && (out_.value.s.equals(in_.s))
                        && (ret_.s.equals(in_.s)))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_fixed_union ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    com.acme.BasicDemo.DisplayPackage.union1 in_ = new com.acme.BasicDemo.DisplayPackage.union1();
                    in_.l(1);
                    com.acme.BasicDemo.DisplayPackage.union1Holder inout_ = new com.acme.BasicDemo.DisplayPackage.union1Holder();
                    inout_.value = new com.acme.BasicDemo.DisplayPackage.union1();
                    inout_.value.l(0);
                    com.acme.BasicDemo.DisplayPackage.union1Holder out_ = new com.acme.BasicDemo.DisplayPackage.union1Holder();
                    com.acme.BasicDemo.DisplayPackage.union1 ret_;                   
         
                    ret_ = display.test_fixed_union(in_, inout_, out_);
                    if ((inout_.value.l() == in_.l())
                        && (out_.value.l() == in_.l())
                        && (ret_.l() == in_.l()))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_var_union ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    com.acme.BasicDemo.DisplayPackage.union2 in_ = new com.acme.BasicDemo.DisplayPackage.union2();
                    in_.s1("1");
                    com.acme.BasicDemo.DisplayPackage.union2Holder inout_ = new com.acme.BasicDemo.DisplayPackage.union2Holder();
                    inout_.value = new com.acme.BasicDemo.DisplayPackage.union2();
                    inout_.value.s1("0");
                    com.acme.BasicDemo.DisplayPackage.union2Holder out_ = new com.acme.BasicDemo.DisplayPackage.union2Holder();
                    com.acme.BasicDemo.DisplayPackage.union2 ret_;                   
         
                    ret_ = display.test_var_union(in_, inout_, out_);
                    if ((inout_.value.s1().equals(in_.s1()))
                        && (out_.value.s1().equals(in_.s1()))
                        && (ret_.s1().equals(in_.s1())))
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_seq_struct ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    com.acme.BasicDemo.s1[] in_ = new com.acme.BasicDemo.s1[1];
                    com.acme.BasicDemo.s1 s = new com.acme.BasicDemo.s1();
                    s.l = 1;
                    in_[0] = s;
                    com.acme.BasicDemo.s1sHolder inout_ = new com.acme.BasicDemo.s1sHolder(new com.acme.BasicDemo.s1[0]); 
                    com.acme.BasicDemo.s1sHolder out_ = new com.acme.BasicDemo.s1sHolder();
                    com.acme.BasicDemo.s1[] ret_;   
         
                    ret_ = display.test_seq_struct(in_, inout_, out_);
                    if (inout_.value[0].l == in_[0].l  
                        && out_.value[0].l == in_[0].l
                        && ret_[0].l == in_[0].l)                        
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!" + e.toString());               
            }
            check(testOk);

            println("test_string ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    String in_  = new String("Hello");
                    StringHolder inout_ = new StringHolder("hella"); 
                    StringHolder out_ = new StringHolder();
                    String ret_;   
         
                    ret_ = display.test_string(in_, inout_, out_);
                    if (inout_.value.equals(in_)  
                        && out_.value.equals(in_)
                        && ret_.equals(in_))                        
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_wstring ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    String in_  = new String("Hello");
                    StringHolder inout_ = new StringHolder("hella"); 
                    StringHolder out_ = new StringHolder();
                    String ret_;   
         
                    ret_ = display.test_wstring(in_, inout_, out_);
                    if (inout_.value.equals(in_)  
                        && out_.value.equals(in_)
                        && ret_.equals(in_))                        
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_fixed_array ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    int[] in_ = {1,2,3,4,5};
                    com.acme.BasicDemo.DisplayPackage.array1Holder inout_ = new com.acme.BasicDemo.DisplayPackage.array1Holder(new int[5]);              
                    com.acme.BasicDemo.DisplayPackage.array1Holder out_ = new com.acme.BasicDemo.DisplayPackage.array1Holder();
                    int[] ret_;
         
                    ret_ = display.test_fixed_array(in_, inout_, out_);
                    if (inout_.value[0] == in_[0]  
                        && out_.value[0] == in_[0]
                        && ret_[0] == in_[0])                        
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_var_array ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    java.lang.String[] in_ = {"hello","2","3","4","5"};
                    com.acme.BasicDemo.DisplayPackage.array2Holder inout_ = new com.acme.BasicDemo.DisplayPackage.array2Holder(new String[5]);   
                    inout_.value[0] = "1";
                    inout_.value[1] = "2";
                    inout_.value[2] = "3";
                    inout_.value[3] = "4";
                    inout_.value[4] = "5";
                    com.acme.BasicDemo.DisplayPackage.array2Holder out_ = new com.acme.BasicDemo.DisplayPackage.array2Holder();
                    java.lang.String[] ret_;
         
                    ret_ = display.test_var_array(in_, inout_, out_);
                    if (inout_.value[0].equals(in_[0])  
                        && out_.value[0].equals(in_[0])
                        && ret_[0].equals(in_[0]))                        
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);


            println("test_any ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    Any in_ = org.omg.CORBA.ORB.init().create_any();
                    in_.insert_string("hello");
                    AnyHolder inout_ = new AnyHolder(org.omg.CORBA.ORB.init().create_any());
                    AnyHolder out_ = new AnyHolder();
                    Any ret_;
                    String in_string, inout_string, out_string, ret_string;               
                    ret_ = display.test_any(in_, inout_, out_);
                    
                    in_string = in_.extract_string();
                    inout_string = inout_.value.extract_string();
                    out_string = out_.value.extract_string();
                    ret_string = ret_.extract_string();   

                    if (inout_string.equals(in_string)  
                        && out_string.equals(in_string)
                        && ret_string.equals(in_string))                        
                        testOk = true;
                }
            } catch (Exception e) {
                println("Unexpected exception raised!");
            }
            check(testOk);

            println("test_value_type ...");
            testOk = false;
            try {
                if (registerServer != null) {
                    com.acme.BasicDemo.Event in_ = new EventImpl();
                    in_.text = "Hello";        
                    com.acme.BasicDemo.EventHolder inout_ = new com.acme.BasicDemo.EventHolder();
                    com.acme.BasicDemo.EventHolder out_ = new com.acme.BasicDemo.EventHolder();
                    com.acme.BasicDemo.Event ret_;
            
                    ret_ = display.test_event(in_, inout_, out_);

                    if (inout_.value.text.equals(in_.text)  
                        && out_.value.text.equals(in_.text)
                        && ret_.text.equals(in_.text))                        
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
    public com.acme.BasicDemo.Event event() {
        com.acme.BasicDemo.Event event = new EventImpl();
        event.text = this.text;
        return event;
    }

    public void event(com.acme.BasicDemo.Event event) {
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

