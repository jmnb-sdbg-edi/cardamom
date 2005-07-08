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


package cdmw.ccm.cif.test;

//
// IDL:thalesgroup.com/BasicDemo/CCM_Display:1.0
//
public class CCMDisplayImpl extends org.omg.CORBA.LocalObject
    implements com.acme.BasicDemo.CCM_Display {
    
    public CCMDisplayImpl() {}

    //
    // IDL:thalesgroup.com/BasicDemo/Display/print:1.0
    //
    public void print(String text) {
        // print the message on the default output
        System.out.println(text);
    }

    //
    // IDL:thalesgroup.com/BasicDemo/Display/print_event:1.0
    //
    public void print_event(com.acme.BasicDemo.Event event) {
        // print the message on the default output
        System.out.println(event.text);
    }
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public short test_short(short p1,
                            org.omg.CORBA.ShortHolder p2,
                            org.omg.CORBA.ShortHolder p3)
    {
        p2.value = (short) (p1 * 2);
        p3.value = (short) (p1 * 3);
        return (short) (p1 * 4);
    }

    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public int test_long(int p1,
                  org.omg.CORBA.IntHolder p2,
                  org.omg.CORBA.IntHolder p3)
    {
        p2.value = p1 * 2;
        p3.value = p1 * 3;
        return p1 * 4;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public long test_long_long(long p1,
                        org.omg.CORBA.LongHolder p2,
                        org.omg.CORBA.LongHolder p3)
    {
        p2.value = p1 * 2;
        p3.value = p1 * 3;
        return p1 * 4;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public short test_unsigned_short(short p1,
                              org.omg.CORBA.ShortHolder p2,
                              org.omg.CORBA.ShortHolder p3)
    {
        p2.value = (short) (p1 * 2);
        p3.value = (short) (p1 * 3);
        return (short) (p1 * 4);
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public int test_unsigned_long(int p1,
                           org.omg.CORBA.IntHolder p2,
                           org.omg.CORBA.IntHolder p3)
    {
        p2.value = p1 * 2;
        p3.value = p1 * 3;
        return p1 * 4;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public long test_unsigned_long_long(long p1,
                                 org.omg.CORBA.LongHolder p2,
                                 org.omg.CORBA.LongHolder p3)
    {
        p2.value = p1 * 2;
        p3.value = p1 * 3;
        return p1 * 4;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public float test_float(float p1,
                     org.omg.CORBA.FloatHolder p2,
                     org.omg.CORBA.FloatHolder p3)
    {
        p2.value = p1 * 2;
        p3.value = p1 * 3;
        return p1 * 4;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public double test_double(double p1,
                       org.omg.CORBA.DoubleHolder p2,
                       org.omg.CORBA.DoubleHolder p3)
    {
        p2.value = p1 * 2;
        p3.value = p1 * 3;
        return p1 * 4;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public boolean test_boolean(boolean p1,
                         org.omg.CORBA.BooleanHolder p2,
                         org.omg.CORBA.BooleanHolder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public char test_char(char p1,
                   org.omg.CORBA.CharHolder p2,
                   org.omg.CORBA.CharHolder p3)
    {
        p2.value = (char)p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public char test_wchar(char p1,
                    org.omg.CORBA.CharHolder p2,
                    org.omg.CORBA.CharHolder p3)
    {
        p2.value = (char)p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public byte test_octet(byte p1,
                    org.omg.CORBA.ByteHolder p2,
                    org.omg.CORBA.ByteHolder p3)
    {
        p2.value = (byte)p1;
        p3.value = (byte)p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public com.acme.BasicDemo.Display test_function(com.acme.BasicDemo.Display p1, 
                                                    com.acme.BasicDemo.DisplayHolder p2, 
                                                    com.acme.BasicDemo.DisplayHolder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public com.acme.BasicDemo.s1 test_fixed_struct(com.acme.BasicDemo.s1 p1,
                                                   com.acme.BasicDemo.s1Holder p2,
                                                   com.acme.BasicDemo.s1Holder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public com.acme.BasicDemo.s2 test_var_struct(com.acme.BasicDemo.s2 p1,
                                                 com.acme.BasicDemo.s2Holder p2,
                                                 com.acme.BasicDemo.s2Holder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public com.acme.BasicDemo.DisplayPackage.union1 test_fixed_union(com.acme.BasicDemo.DisplayPackage.union1 p1,
                                                                     com.acme.BasicDemo.DisplayPackage.union1Holder p2,
                                                                     com.acme.BasicDemo.DisplayPackage.union1Holder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public com.acme.BasicDemo.DisplayPackage.union2 test_var_union(com.acme.BasicDemo.DisplayPackage.union2 p1,
                                                                   com.acme.BasicDemo.DisplayPackage.union2Holder p2,
                                                                   com.acme.BasicDemo.DisplayPackage.union2Holder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public com.acme.BasicDemo.s1[] test_seq_struct(com.acme.BasicDemo.s1[] p1,
                                                   com.acme.BasicDemo.s1sHolder p2,
                                                   com.acme.BasicDemo.s1sHolder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public java.lang.String test_string(java.lang.String p1,
                                        org.omg.CORBA.StringHolder p2,
                                        org.omg.CORBA.StringHolder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public java.lang.String test_wstring(java.lang.String p1,
                                         org.omg.CORBA.StringHolder p2,
                                         org.omg.CORBA.StringHolder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public int[] test_fixed_array(int[] p1,
                                  com.acme.BasicDemo.DisplayPackage.array1Holder p2,
                                  com.acme.BasicDemo.DisplayPackage.array1Holder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public java.lang.String[] test_var_array(java.lang.String[] p1,
                                             com.acme.BasicDemo.DisplayPackage.array2Holder p2,
                                             com.acme.BasicDemo.DisplayPackage.array2Holder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public org.omg.CORBA.Any test_any(org.omg.CORBA.Any p1,
                                      org.omg.CORBA.AnyHolder p2,
                                      org.omg.CORBA.AnyHolder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;
    }
    //
    // IDL:thalesgroup.com/BasicDemo/Display/:1.0
    //    
	public com.acme.BasicDemo.Event test_event(com.acme.BasicDemo.Event p1,
                                               com.acme.BasicDemo.EventHolder p2,
                                               com.acme.BasicDemo.EventHolder p3)
    {
        p2.value = p1;
        p3.value = p1;
        return p1;

    }
    
    public void set(int l)
    {
        val = l;
    }
    
    public int get()
    {
        return val;
    }

    private int val;

}

