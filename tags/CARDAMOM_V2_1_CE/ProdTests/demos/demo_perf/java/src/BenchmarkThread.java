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


class BenchmarkThread extends Thread {
    
    private DataProvider m_dataProvider;
    private com.thalesgroup.Benchmark.Tester m_tester;
    private com.thalesgroup.Benchmark.I m_i;
    private org.omg.CORBA.ORB m_orb;
    private boolean m_stopped;
    
    private int m_nb_ping_test           = 50;
    private int m_nb_short_test          = 50;
    private int m_nb_long_test           = 50;
    private int m_nb_double_test         = 50;
    private int m_nb_string_test         = 50;
    private int m_nb_any_test            = 50;
    private int m_nb_interface_test      = 50;
    private int m_nb_fixStruct_test      = 50;
    private int m_nb_varStruct_test      = 50;
    private int m_nb_octet_seq_test      = 50;
    private int m_nb_octet_seq_256K_test = 50;
    private int m_nb_octet_seq_1M_test   = 50;
    private int m_nb_short_seq_test      = 50;
    private int m_nb_short_seq_1M_test   = 50;
    private int m_nb_string_seq_test     = 50;
    private int m_nb_object_seq_test     = 50;
    private int m_nb_struct_seq_test     = 50;
    private int m_nb_valuetype_test      = 50;    
    
    
    public BenchmarkThread(com.thalesgroup.Benchmark.Tester tester,
                           com.thalesgroup.Benchmark.I i,
                           org.omg.CORBA.ORB orb)
    {
        m_dataProvider = new DataProvider();
        m_tester = tester;
        m_i = i;
        m_orb = orb;
        m_stopped = false;
    }
    
    
    public void run() {
        System.out.println("--- Start of Benchmark ---");
    
        try {
            cdmw.performance.Probe total = null;
            try {
                total = new cdmw.performance.Probe("Total time");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            
            // First connection to server.
            {
                cdmw.performance.Probe p = null;
                try {
                    p = new cdmw.performance.Probe("First Connection");
                } catch (cdmw.common.BadParameterException e) {
                    // shouldn't happen since probe name is not null
                    cdmw.common.Assert.check(false);
                }
                m_tester.ping();
                
                // because the garbage collector calls p.finalize() immediatly,
                // p.stop() should be called explicitly
                p.stop();
            }
        
            int i;
            System.out.println("  Test performances without arguments " + m_nb_ping_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_ping_test; ++i) {
                test_ping();
            }
            System.out.println("  Test performances with Short " + m_nb_short_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_short_test; ++i) {
                test_short();
            }
            System.out.println("  Test performances with Long " + m_nb_long_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_long_test; ++i) {
                test_long();
            }
            System.out.println("  Test performances with Double " + m_nb_double_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_double_test; ++i) {
                test_double();
            }
            System.out.println("  Test performances with String " + m_nb_string_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_string_test; ++i) {
                test_string();
            }
            System.out.println("  Test performances with Any " + m_nb_any_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_any_test; ++i) {
                test_any();
            }
            System.out.println("  Test performances with Interface " + m_nb_interface_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_interface_test; ++i) {
                test_interface();
            }
            System.out.println("  Test performances with FixedStruct " + m_nb_fixStruct_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_fixStruct_test; ++i) {
                test_fixStruct();
            }
            System.out.println("  Test performances with VarStruct " + m_nb_varStruct_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_varStruct_test; ++i) {
                test_varStruct();
            }
            System.out.println("  Test performances with OctetSeq " + m_nb_octet_seq_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_octet_seq_test; ++i) {
                test_octet_seq();
            }
            System.out.println("  Test performances with OctetSeq_256K " + m_nb_octet_seq_256K_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_octet_seq_256K_test; ++i) {
                test_octet_seq_256K();
            }
            System.out.println("  Test performances with OctetSeq_1M " + m_nb_octet_seq_1M_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_octet_seq_1M_test; ++i) {
                test_octet_seq_1M();
            }
            System.out.println("  Test performances with ShortSeq " + m_nb_short_seq_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_short_seq_test; ++i) {
                test_short_seq();
            }
            System.out.println("  Test performances with ShortSeq_1M " + m_nb_short_seq_1M_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_short_seq_1M_test; ++i) {
                test_short_seq_1M();
            }
            System.out.println("  Test performances with StringSeq " + m_nb_string_seq_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_string_seq_test; ++i) {
                test_string_seq();
            }
            System.out.println("  Test performances with ObjectSeq " + m_nb_object_seq_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_object_seq_test; ++i) {
                test_object_seq();
            }
            System.out.println("  Test performances with StructSeq " + m_nb_struct_seq_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_struct_seq_test; ++i) {
                test_struct_seq();
            }
            System.out.println("  Test performances with Valuetype " + m_nb_valuetype_test + " times.");
            for (i = 0; !m_stopped && i < m_nb_valuetype_test; ++i) {
                test_valuetype();
            }
    
            // because the garbage collector calls p.finalize() immediatly,
            // total.stop() should be called explicitly
            total.stop();
        }
        catch (org.omg.CORBA.SystemException e) 
        {
            System.err.println("SystemException: " +
                               cdmw.orbsupport.ORBSupport.exceptionToString(e));
        }
            
        if (m_stopped) 
            System.out.println("--- Benchmark stopped ---");
        else
            System.out.println("---- End of Benchmark ----");
    }

    
    private void test_ping ()
        throws org.omg.CORBA.SystemException
    {
        cdmw.performance.Probe p = null;
        try {
            p = new cdmw.performance.Probe("Without any argument");
        } catch (cdmw.common.BadParameterException e) {
            // shouldn't happen since probe name is not null
            cdmw.common.Assert.check(false);
        }
        m_tester.ping();
        p.stop();
    }
    
    private void test_short ()
        throws org.omg.CORBA.SystemException
    {
        short s = m_dataProvider.get_Short();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Short as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.short_in(s);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Short as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.ShortHolder sHolder = new org.omg.CORBA.ShortHolder(s);
            m_tester.short_out(sHolder);
            s = sHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Short as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.ShortHolder sHolder = new org.omg.CORBA.ShortHolder(s);
            m_tester.short_inout(sHolder);
            s = sHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Short as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            short result = m_tester.short_ret();
            cdmw.common.Assert.check(result == s);
            p.stop();
        }
    }
    
    private void test_long   ()
        throws org.omg.CORBA.SystemException
    {
        int l = m_dataProvider.get_Long();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Long as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.long_in(l);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Long as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.IntHolder iHolder = new org.omg.CORBA.IntHolder(l);
            m_tester.long_out(iHolder);
            l = iHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Long as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.IntHolder iHolder = new org.omg.CORBA.IntHolder(l);
            m_tester.long_inout(iHolder);
            l = iHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Long as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            long result = m_tester.long_ret();
            cdmw.common.Assert.check(result == l);
            p.stop();
        }
    }
    
    private void test_double ()
        throws org.omg.CORBA.SystemException
    {
        double d = m_dataProvider.get_Double();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Double as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.double_in(d);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Double as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.DoubleHolder dHolder = new org.omg.CORBA.DoubleHolder(d);
            m_tester.double_out(dHolder);
            d = dHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Double as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.DoubleHolder dHolder = new org.omg.CORBA.DoubleHolder(d);
            m_tester.double_inout(dHolder);
            d = dHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Double as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            double result = m_tester.double_ret();
            cdmw.common.Assert.check(result == d);
            p.stop();
        }
    }
    
    private void test_string ()
        throws org.omg.CORBA.SystemException
    {
        String str = m_dataProvider.get_String();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("String as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.string_in(str);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("String as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.StringHolder strHolder = new org.omg.CORBA.StringHolder(str);
            m_tester.string_out(strHolder);
            str = strHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("String as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.StringHolder strHolder = new org.omg.CORBA.StringHolder(str);
            m_tester.string_inout(strHolder);
            str = strHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("String as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            String result = m_tester.string_ret();
            p.stop();
        }
    }
    
    private void test_any ()
        throws org.omg.CORBA.SystemException
    {
        String str = m_dataProvider.get_String();
        org.omg.CORBA.Any any = m_orb.create_any();
        any.insert_string(str);
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Any as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.any_in(any);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Any as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.AnyHolder anyHolder = new org.omg.CORBA.AnyHolder(any);
            m_tester.any_out(anyHolder);
            any = anyHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Any as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.AnyHolder anyHolder = new org.omg.CORBA.AnyHolder(any);
            m_tester.any_inout(anyHolder);
            any = anyHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Any as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            org.omg.CORBA.Any result = m_tester.any_ret();
            p.stop();
        }
    }
    
    private void test_interface ()
        throws org.omg.CORBA.SystemException
    {
        com.thalesgroup.Benchmark.I obj = m_i;
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Interface as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.interface_in(obj);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Interface as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.IHolder objHolder = new com.thalesgroup.Benchmark.IHolder(obj);
            m_tester.interface_out(objHolder);
            obj = objHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Interface as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.IHolder objHolder = new com.thalesgroup.Benchmark.IHolder(obj);
            m_tester.interface_inout(objHolder);
            obj = objHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Interface as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.I result = m_tester.interface_ret();
            p.stop();
        }
    }
            
    private void test_fixStruct ()
        throws org.omg.CORBA.SystemException
    {
        com.thalesgroup.Benchmark.FixedStruct fStr = m_dataProvider.get_FixedStruct();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("FixedStruct as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.fixStruct_in(fStr);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("FixedStruct as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.FixedStructHolder fStrHolder = new com.thalesgroup.Benchmark.FixedStructHolder(fStr);
            m_tester.fixStruct_out(fStrHolder);
            fStr = fStrHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("FixedStruct as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.FixedStructHolder fStrHolder = new com.thalesgroup.Benchmark.FixedStructHolder(fStr);
            m_tester.fixStruct_inout(fStrHolder);
            fStr = fStrHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("FixedStruct as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.FixedStruct result = m_tester.fixStruct_ret();
            p.stop();
        }
    }
    
    private void test_varStruct ()
        throws org.omg.CORBA.SystemException
    {
        com.thalesgroup.Benchmark.VariableStruct vStr = m_dataProvider.get_VariableStruct();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("VariableStruct as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.varStruct_in(vStr);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("VariableStruct as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.VariableStructHolder vStrHolder = new com.thalesgroup.Benchmark.VariableStructHolder(vStr);
            m_tester.varStruct_out(vStrHolder);
            vStr = vStrHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("VariableStruct as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.VariableStructHolder vStrHolder = new com.thalesgroup.Benchmark.VariableStructHolder(vStr);
            m_tester.varStruct_inout(vStrHolder);
            vStr = vStrHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("VariableStruct as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.VariableStruct result = m_tester.varStruct_ret();
            p.stop();
        }
    }
    
    private void test_octet_seq ()
        throws org.omg.CORBA.SystemException
    {
        byte[] oSeq = m_dataProvider.get_OctetSeq();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.octet_seq_in(oSeq);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.OctetSeqHolder oSeqHolder = new com.thalesgroup.Benchmark.OctetSeqHolder(oSeq);
            m_tester.octet_seq_out(oSeqHolder);
            oSeq = oSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.OctetSeqHolder oSeqHolder = new com.thalesgroup.Benchmark.OctetSeqHolder(oSeq);
            m_tester.octet_seq_inout(oSeqHolder);
            oSeq = oSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            byte[] result = m_tester.octet_seq_ret();
            p.stop();
        }
    }
    
    private void test_octet_seq_256K ()
        throws org.omg.CORBA.SystemException
    {
        byte[] oSeq = m_dataProvider.get_OctetSeq_256K();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq_256K as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.octet_seq_256K_in(oSeq);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq_256K as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.OctetSeq_256KHolder oSeqHolder = new com.thalesgroup.Benchmark.OctetSeq_256KHolder(oSeq);
            m_tester.octet_seq_256K_out(oSeqHolder);
            oSeq = oSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq_256K as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.OctetSeq_256KHolder oSeqHolder = new com.thalesgroup.Benchmark.OctetSeq_256KHolder(oSeq);
            m_tester.octet_seq_256K_inout(oSeqHolder);
            oSeq = oSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq_256K as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            byte[] result = m_tester.octet_seq_256K_ret();
            p.stop();
        }
    }
    
    private void test_octet_seq_1M ()
        throws org.omg.CORBA.SystemException
    {
        byte[] oSeq = m_dataProvider.get_OctetSeq_1M();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq_1M as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.octet_seq_1M_in(oSeq);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq_1M as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.OctetSeq_1MHolder oSeqHolder = new com.thalesgroup.Benchmark.OctetSeq_1MHolder(oSeq);
            m_tester.octet_seq_1M_out(oSeqHolder);
            oSeq = oSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq_1M as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.OctetSeq_1MHolder oSeqHolder = new com.thalesgroup.Benchmark.OctetSeq_1MHolder(oSeq);
            m_tester.octet_seq_1M_inout(oSeqHolder);
            oSeq = oSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("OctetSeq_1M as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            byte[] result = m_tester.octet_seq_1M_ret();
            p.stop();
        }
    }
    
    private void test_short_seq ()
        throws org.omg.CORBA.SystemException
    {
        short[] sSeq = m_dataProvider.get_ShortSeq();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ShortSeq as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.short_seq_in(sSeq);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ShortSeq as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.ShortSeqHolder sSeqHolder = new com.thalesgroup.Benchmark.ShortSeqHolder(sSeq);
            m_tester.short_seq_out(sSeqHolder);
            sSeq = sSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ShortSeq as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.ShortSeqHolder sSeqHolder = new com.thalesgroup.Benchmark.ShortSeqHolder(sSeq);
            m_tester.short_seq_inout(sSeqHolder);
            sSeq = sSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ShortSeq as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            short[] result = m_tester.short_seq_ret();
            p.stop();
        }
    }
    
    private void test_short_seq_1M ()
        throws org.omg.CORBA.SystemException
    {
        short[] sSeq = m_dataProvider.get_ShortSeq_1M();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ShortSeq_1M as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.short_seq_1M_in(sSeq);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ShortSeq_1M as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.ShortSeq_1MHolder sSeqHolder = new com.thalesgroup.Benchmark.ShortSeq_1MHolder(sSeq);
            m_tester.short_seq_1M_out(sSeqHolder);
            sSeq = sSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ShortSeq_1M as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.ShortSeq_1MHolder sSeqHolder = new com.thalesgroup.Benchmark.ShortSeq_1MHolder(sSeq);
            m_tester.short_seq_1M_inout(sSeqHolder);
            sSeq = sSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ShortSeq_1M as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            short[] result = m_tester.short_seq_1M_ret();
            p.stop();
        }
    }
    
    private void test_string_seq ()
        throws org.omg.CORBA.SystemException
    {
        String[] strSeq = m_dataProvider.get_StringSeq();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("StringSeq as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.string_seq_in(strSeq);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("StringSeq as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.StringSeqHolder strSeqHolder = new com.thalesgroup.Benchmark.StringSeqHolder(strSeq);
            m_tester.string_seq_out(strSeqHolder);
            strSeq = strSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("StringSeq as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.StringSeqHolder strSeqHolder = new com.thalesgroup.Benchmark.StringSeqHolder(strSeq);
            m_tester.string_seq_inout(strSeqHolder);
            strSeq = strSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("StringSeq as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            String[] result = m_tester.string_seq_ret();
            p.stop();
        }
    }
    
    private void test_object_seq ()
        throws org.omg.CORBA.SystemException
    {
        com.thalesgroup.Benchmark.I[] objSeq = new com.thalesgroup.Benchmark.I[20];
        for (int i=0; i<20; ++i) {
            objSeq[i] = m_i;
        }
        
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ObjectSeq as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.object_seq_in(objSeq);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ObjectSeq as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.ObjectSeqHolder objSeqHolder = new com.thalesgroup.Benchmark.ObjectSeqHolder(objSeq);
            m_tester.object_seq_out(objSeqHolder);
            objSeq = objSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ObjectSeq as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.ObjectSeqHolder objSeqHolder = new com.thalesgroup.Benchmark.ObjectSeqHolder(objSeq);
            m_tester.object_seq_inout(objSeqHolder);
            objSeq = objSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("ObjectSeq as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.I[] result = m_tester.object_seq_ret();
            p.stop();
        }
    }
    
    private void test_struct_seq ()
        throws org.omg.CORBA.SystemException
    {
        com.thalesgroup.Benchmark.VariableStruct[] strSeq = m_dataProvider.get_StructSeq();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("StructSeq as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.struct_seq_in(strSeq);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("StructSeq as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.StructSeqHolder strSeqHolder = new com.thalesgroup.Benchmark.StructSeqHolder(strSeq);
            m_tester.struct_seq_out(strSeqHolder);
            strSeq = strSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("StructSeq as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.StructSeqHolder strSeqHolder = new com.thalesgroup.Benchmark.StructSeqHolder(strSeq);
            m_tester.struct_seq_inout(strSeqHolder);
            strSeq = strSeqHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("StructSeq as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.VariableStruct[] result = m_tester.struct_seq_ret();
            p.stop();
        }
    }
    
    private void test_valuetype ()
        throws org.omg.CORBA.SystemException
    {
        com.thalesgroup.Benchmark.Value val = m_dataProvider.get_Value();
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Value as in argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            m_tester.valuetype_in(val);
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Value as out argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.ValueHolder valHolder = new com.thalesgroup.Benchmark.ValueHolder(val);
            m_tester.valuetype_out(valHolder);
            val = valHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Value as inout argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.ValueHolder valHolder = new com.thalesgroup.Benchmark.ValueHolder(val);
            m_tester.valuetype_inout(valHolder);
            val = valHolder.value;
            p.stop();
        }
        {
            cdmw.performance.Probe p = null;
            try {
                p = new cdmw.performance.Probe("Value as return argument");
            } catch (cdmw.common.BadParameterException e) {
                // shouldn't happen since probe name is not null
                cdmw.common.Assert.check(false);
            }
            com.thalesgroup.Benchmark.Value result = m_tester.valuetype_ret();
            p.stop();
        }
    }
        
    
}

