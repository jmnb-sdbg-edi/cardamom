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


package cdmw.demoperf;



class TesterImpl extends com.thalesgroup.Benchmark.TesterPOA {
    
    private DataProvider m_dataProvider;
    
    private com.thalesgroup.Benchmark.I m_i;
    
    private org.omg.CORBA.ORB m_orb;
    
    
    
    public TesterImpl(com.thalesgroup.Benchmark.I i,
                      org.omg.CORBA.ORB orb) 
    {
        m_dataProvider = new DataProvider();
        m_i = i;
        m_orb = orb;
    }
    
    
    
    //
    // IDL:thalesgroup.com/Benchmark/Tester/ping:1.0
    //
    public void
    ping()
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_in:1.0
    //
    public void
    short_in(short s)
    {
        cdmw.common.Assert.check(s == m_dataProvider.get_Short());
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_out:1.0
    //
    public void
    short_out(org.omg.CORBA.ShortHolder s)
    {
        s.value = m_dataProvider.get_Short();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_inout:1.0
    //
    public void
    short_inout(org.omg.CORBA.ShortHolder s)
    {
         cdmw.common.Assert.check(s.value == m_dataProvider.get_Short());
         s.value = m_dataProvider.get_Short();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_ret:1.0
    //
    public short
    short_ret()
    {
        return m_dataProvider.get_Short();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/long_in:1.0
    //
    public void
    long_in(int l)
    {
        cdmw.common.Assert.check(l == m_dataProvider.get_Long());
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/long_out:1.0
    //
    public void
    long_out(org.omg.CORBA.IntHolder l)
    {
        l.value = m_dataProvider.get_Long();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/long_inout:1.0
    //
    public void
    long_inout(org.omg.CORBA.IntHolder l)
    {
        cdmw.common.Assert.check(l.value == m_dataProvider.get_Long());
        l.value = m_dataProvider.get_Long();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/long_ret:1.0
    //
    public int
    long_ret()
    {
        return m_dataProvider.get_Long();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/double_in:1.0
    //
    public void
    double_in(double d)
    {
        cdmw.common.Assert.check(d == m_dataProvider.get_Double());
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/double_out:1.0
    //
    public void
    double_out(org.omg.CORBA.DoubleHolder d)
    {
        d.value = m_dataProvider.get_Double();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/double_inout:1.0
    //
    public void
    double_inout(org.omg.CORBA.DoubleHolder d)
    {
        cdmw.common.Assert.check(d.value == m_dataProvider.get_Double());
        d.value = m_dataProvider.get_Double();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/double_ret:1.0
    //
    public double
    double_ret()
    {
        return m_dataProvider.get_Double();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/string_in:1.0
    //
    public void
    string_in(String s)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/string_out:1.0
    //
    public void
    string_out(org.omg.CORBA.StringHolder s)
    {
        s.value = m_dataProvider.get_String();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/string_inout:1.0
    //
    public void
    string_inout(org.omg.CORBA.StringHolder s)
    {
        s.value = m_dataProvider.get_String();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/string_ret:1.0
    //
    public String
    string_ret()
    {
        return m_dataProvider.get_String();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/any_in:1.0
    //
    public void
    any_in(org.omg.CORBA.Any a)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/any_out:1.0
    //
    public void
    any_out(org.omg.CORBA.AnyHolder a)
    {
        a.value = m_orb.create_any();
        a.value.insert_string(m_dataProvider.get_String());
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/any_inout:1.0
    //
    public void
    any_inout(org.omg.CORBA.AnyHolder a)
    {
        a.value.insert_string(m_dataProvider.get_String());
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/any_ret:1.0
    //
    public org.omg.CORBA.Any
    any_ret()
    {
        org.omg.CORBA.Any any = m_orb.create_any();
        any.insert_string(m_dataProvider.get_String());
        return any;
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/interface_in:1.0
    //
    public void
    interface_in(com.thalesgroup.Benchmark.I interf)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/interface_out:1.0
    //
    public void
    interface_out(com.thalesgroup.Benchmark.IHolder interf)
    {
        interf.value = m_i;
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/interface_inout:1.0
    //
    public void
    interface_inout(com.thalesgroup.Benchmark.IHolder interf)
    {
        interf.value = m_i;
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/interface_ret:1.0
    //
    public com.thalesgroup.Benchmark.I
    interface_ret()
    {
        return m_i;
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/fixStruct_in:1.0
    //
    public void
    fixStruct_in(com.thalesgroup.Benchmark.FixedStruct fStruct)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/fixStruct_out:1.0
    //
    public void
    fixStruct_out(com.thalesgroup.Benchmark.FixedStructHolder fStruct)
    {
        fStruct.value = m_dataProvider.get_FixedStruct();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/fixStruct_inout:1.0
    //
    public void
    fixStruct_inout(com.thalesgroup.Benchmark.FixedStructHolder fStruct)
    {
        fStruct.value = m_dataProvider.get_FixedStruct();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/fixStruct_ret:1.0
    //
    public com.thalesgroup.Benchmark.FixedStruct
    fixStruct_ret()
    {
        return m_dataProvider.get_FixedStruct();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/varStruct_in:1.0
    //
    public void
    varStruct_in(com.thalesgroup.Benchmark.VariableStruct vStruct)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/varStruct_out:1.0
    //
    public void
    varStruct_out(com.thalesgroup.Benchmark.VariableStructHolder vStruct)
    {
        vStruct.value = m_dataProvider.get_VariableStruct(); 
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/varStruct_inout:1.0
    //
    public void
    varStruct_inout(com.thalesgroup.Benchmark.VariableStructHolder vStruct)
    {
        vStruct.value = m_dataProvider.get_VariableStruct(); 
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/varStruct_ret:1.0
    //
    public com.thalesgroup.Benchmark.VariableStruct
    varStruct_ret()
    {
        return m_dataProvider.get_VariableStruct(); 
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_in:1.0
    //
    public void
    octet_seq_in(byte[] o_seq)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_out:1.0
    //
    public void
    octet_seq_out(com.thalesgroup.Benchmark.OctetSeqHolder o_seq)
    {
        o_seq.value = m_dataProvider.get_OctetSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_inout:1.0
    //
    public void
    octet_seq_inout(com.thalesgroup.Benchmark.OctetSeqHolder o_seq)
    {
        o_seq.value = m_dataProvider.get_OctetSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_ret:1.0
    //
    public byte[]
    octet_seq_ret()
    {
        return m_dataProvider.get_OctetSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_256K_in:1.0
    //
    public void
    octet_seq_256K_in(byte[] o_seq)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_256K_out:1.0
    //
    public void
    octet_seq_256K_out(com.thalesgroup.Benchmark.OctetSeq_256KHolder o_seq)
    {
        o_seq.value = m_dataProvider.get_OctetSeq_256K();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_256K_inout:1.0
    //
    public void
    octet_seq_256K_inout(com.thalesgroup.Benchmark.OctetSeq_256KHolder o_seq)
    {
        o_seq.value = m_dataProvider.get_OctetSeq_256K();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_256K_ret:1.0
    //
    public byte[]
    octet_seq_256K_ret()
    {
        return m_dataProvider.get_OctetSeq_256K();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_1M_in:1.0
    //
    public void
    octet_seq_1M_in(byte[] o_seq)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_1M_out:1.0
    //
    public void
    octet_seq_1M_out(com.thalesgroup.Benchmark.OctetSeq_1MHolder o_seq)
    {
        o_seq.value = m_dataProvider.get_OctetSeq_1M();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_1M_inout:1.0
    //
    public void
    octet_seq_1M_inout(com.thalesgroup.Benchmark.OctetSeq_1MHolder o_seq)
    {
        o_seq.value = m_dataProvider.get_OctetSeq_1M();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/octet_seq_1M_ret:1.0
    //
    public byte[]
    octet_seq_1M_ret()
    {
        return m_dataProvider.get_OctetSeq_1M();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_seq_in:1.0
    //
    public void
    short_seq_in(short[] s_seq)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_seq_out:1.0
    //
    public void
    short_seq_out(com.thalesgroup.Benchmark.ShortSeqHolder s_seq)
    {
        s_seq.value = m_dataProvider.get_ShortSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_seq_inout:1.0
    //
    public void
    short_seq_inout(com.thalesgroup.Benchmark.ShortSeqHolder s_seq)
    {
        s_seq.value = m_dataProvider.get_ShortSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_seq_ret:1.0
    //
    public short[]
    short_seq_ret()
    {
        return m_dataProvider.get_ShortSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_seq_1M_in:1.0
    //
    public void
    short_seq_1M_in(short[] s_seq)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_seq_1M_out:1.0
    //
    public void
    short_seq_1M_out(com.thalesgroup.Benchmark.ShortSeq_1MHolder s_seq)
    {
        s_seq.value = m_dataProvider.get_ShortSeq_1M();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_seq_1M_inout:1.0
    //
    public void
    short_seq_1M_inout(com.thalesgroup.Benchmark.ShortSeq_1MHolder s_seq)
    {
        s_seq.value = m_dataProvider.get_ShortSeq_1M();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/short_seq_1M_ret:1.0
    //
    public short[]
    short_seq_1M_ret()
    {
        return m_dataProvider.get_ShortSeq_1M();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/string_seq_in:1.0
    //
    public void
    string_seq_in(String[] str_seq)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/string_seq_out:1.0
    //
    public void
    string_seq_out(com.thalesgroup.Benchmark.StringSeqHolder str_seq)
    {
        str_seq.value = m_dataProvider.get_StringSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/string_seq_inout:1.0
    //
    public void
    string_seq_inout(com.thalesgroup.Benchmark.StringSeqHolder str_seq)
    {
        str_seq.value = m_dataProvider.get_StringSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/string_seq_ret:1.0
    //
    public String[]
    string_seq_ret()
    {
        return m_dataProvider.get_StringSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/object_seq_in:1.0
    //
    public void
    object_seq_in(com.thalesgroup.Benchmark.I[] obj_seq)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/object_seq_out:1.0
    //
    public void
    object_seq_out(com.thalesgroup.Benchmark.ObjectSeqHolder obj_seq)
    {
        obj_seq.value = new com.thalesgroup.Benchmark.I[20];
        for (int i=0; i < 20; ++i) {
            obj_seq.value[i] =  m_i;
        }
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/object_seq_inout:1.0
    //
    public void
    object_seq_inout(com.thalesgroup.Benchmark.ObjectSeqHolder obj_seq)
    {
        obj_seq.value = new com.thalesgroup.Benchmark.I[20];
        for (int i=0; i < 20; ++i) {
            obj_seq.value[i] =  m_i;
        }
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/object_seq_ret:1.0
    //
    public com.thalesgroup.Benchmark.I[]
    object_seq_ret()
    {
        com.thalesgroup.Benchmark.I[] result = new com.thalesgroup.Benchmark.I[20];
        for (int i=0; i < 20; ++i) {
            result[i] =  m_i;
        }
        return result;
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/struct_seq_in:1.0
    //
    public void
    struct_seq_in(com.thalesgroup.Benchmark.VariableStruct[] struct_seq)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/struct_seq_out:1.0
    //
    public void
    struct_seq_out(com.thalesgroup.Benchmark.StructSeqHolder struct_seq)
    {
        struct_seq.value = m_dataProvider.get_StructSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/struct_seq_inout:1.0
    //
    public void
    struct_seq_inout(com.thalesgroup.Benchmark.StructSeqHolder struct_seq)
    {
        struct_seq.value = m_dataProvider.get_StructSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/struct_seq_ret:1.0
    //
    public com.thalesgroup.Benchmark.VariableStruct[]
    struct_seq_ret()
    {
        return m_dataProvider.get_StructSeq();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/valuetype_in:1.0
    //
    public void
    valuetype_in(com.thalesgroup.Benchmark.Value v)
    {
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/valuetype_out:1.0
    //
    public void
    valuetype_out(com.thalesgroup.Benchmark.ValueHolder v)
    {
        v.value = m_dataProvider.get_Value();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/valuetype_inout:1.0
    //
    public void
    valuetype_inout(com.thalesgroup.Benchmark.ValueHolder v)
    {
        v.value = m_dataProvider.get_Value();
    }

    //
    // IDL:thalesgroup.com/Benchmark/Tester/valuetype_ret:1.0
    //
    public com.thalesgroup.Benchmark.Value
    valuetype_ret()
    {
        return m_dataProvider.get_Value();
    }
    
}






