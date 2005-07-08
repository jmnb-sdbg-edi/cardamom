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


class DataProvider {

    private short        m_short;
    private int          m_long;
    private double       m_double;
    private String       m_string;
    
    private com.thalesgroup.Benchmark.FixedStruct         m_fixStruct;
    private com.thalesgroup.Benchmark.VariableStruct      m_varStruct;
    
    
    private int      m_oSeq_size;
    private byte[]   m_oSeq_buffer;
    private int      m_oSeq256K_size;
    private byte[]   m_oSeq256K_buffer;
    private int      m_oSeq1M_size;
    private byte[]   m_oSeq1M_buffer;
    private int      m_sSeq_size;
    private short[]  m_sSeq_buffer;
    private int      m_sSeq1M_size;
    private short[]  m_sSeq1M_buffer;
    private int      m_strSeq_size;
    private String[] m_strSeq;
    private int                                        m_structSeq_size;
    private com.thalesgroup.Benchmark.VariableStruct[] m_structSeq;

    private com.thalesgroup.Benchmark.Value  m_value;
    

    
    public DataProvider() {
        m_short  = 32109;
        m_long   = 2147483602;
        m_double = 88783214.892032943;
        m_string = "This is a string for the Performance Demo.";
        
        m_fixStruct = new com.thalesgroup.Benchmark.FixedStruct();
        m_fixStruct.s = 32266;
        m_fixStruct.l = 2147483629;
    
        m_varStruct = new com.thalesgroup.Benchmark.VariableStruct();
        m_varStruct.fStr = new com.thalesgroup.Benchmark.FixedStruct();
        m_varStruct.fStr.s = 32421;
        m_varStruct.fStr.l = 2147482111;
        m_varStruct.s = "This is a string for a variable-length struct.";
        
        m_oSeq_size = 500;
        m_oSeq_buffer = new byte[m_oSeq_size];
    
        m_oSeq256K_size = com.thalesgroup.Benchmark.DECIMAL_256K.value;
        m_oSeq256K_buffer = new byte[m_oSeq256K_size];
    
        m_oSeq1M_size = com.thalesgroup.Benchmark.DECIMAL_1M.value;
        m_oSeq1M_buffer = new byte[m_oSeq1M_size];
    
        m_sSeq_size = 500;
        m_sSeq_buffer = new short[m_sSeq_size];
    
        m_sSeq1M_size = com.thalesgroup.Benchmark.DECIMAL_512K.value;
        m_sSeq1M_buffer = new short[m_sSeq1M_size];
    
        m_strSeq_size = 100;
        m_strSeq = new String[m_strSeq_size];
        for (int i=0; i < m_strSeq_size; ++i) {
            m_strSeq[i] = new String("This is a string for a string sequence.");
        }
        
        m_structSeq_size = 50;
        m_structSeq = new com.thalesgroup.Benchmark.VariableStruct[m_structSeq_size];
        for (int i=0; i < m_structSeq_size; ++i) {
            m_structSeq[i] = new com.thalesgroup.Benchmark.VariableStruct();
            m_structSeq[i].fStr = new com.thalesgroup.Benchmark.FixedStruct();
            m_structSeq[i].fStr.s = 32123;
            m_structSeq[i].fStr.l = 2147480000;
            m_structSeq[i].s =  new String("This is a string for a sequence of variable-length structures.");
        }
        
        m_value = new ValueImpl();
        com.thalesgroup.Benchmark.FixedStruct fStr = new com.thalesgroup.Benchmark.FixedStruct();
        fStr.s = 32007;
        fStr.l = 2147483007;
        m_value.fStr = fStr;
        m_value.s = new String("This is a string for a valuetype");
        
    }        
        
    
    
    public short 
    get_Short()
    {
        return m_short;
    }
    
    public int 
    get_Long()
    {
        return m_long;
    }
    
    public double 
    get_Double()
    {
        return m_double;
    }
    
    public String 
    get_String()
    {
        return m_string;
    }

    public com.thalesgroup.Benchmark.FixedStruct     
    get_FixedStruct()
    {
        return m_fixStruct;
    }
    
    public com.thalesgroup.Benchmark.VariableStruct  
    get_VariableStruct()
    {
        return m_varStruct;
    }
    
    public byte[]    
    get_OctetSeq()
    {
        return m_oSeq_buffer;
    }
    
    public byte[]    
    get_OctetSeq_256K()
    {
        return m_oSeq256K_buffer;
    }
    
    public byte[]    
    get_OctetSeq_1M()
    {
        return m_oSeq1M_buffer;
    }
    
    public short[]   
    get_ShortSeq()
    {
        return m_sSeq_buffer;
    }
    
    public short[]   
    get_ShortSeq_1M()
    {
        return m_sSeq1M_buffer;
    }
    
    public String[]  
    get_StringSeq()
    {
        return m_strSeq;
    }
    
    public com.thalesgroup.Benchmark.VariableStruct[] 
    get_StructSeq()
    {
        return m_structSeq;
    }
    
    public com.thalesgroup.Benchmark.Value 
    get_Value()
    {
        return m_value;
    }


}


