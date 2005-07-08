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


#include "DataProvider.hpp"

#include "Value_impl.hpp"


namespace DemoPerf {
    
    
DataProvider::DataProvider() {
    m_short  = (CORBA::Short)  32109;
    m_long   = (CORBA::Long)   2147483602;
    m_double = (CORBA::Double) 88783214.892032943;
    m_string = CORBA::string_dup("This is a string for the Performance Demo.");
    
    m_fixStruct.s = (CORBA::Short)  32266;
    m_fixStruct.l = (CORBA::Long)   2147483629;

    m_varStruct.fStr.s = (CORBA::Short)  32421;
    m_varStruct.fStr.l = (CORBA::Long)   2147482111;
    m_varStruct.s =  CORBA::string_dup("This is a string for a variable-length struct.");
    
    m_oSeq_size = 500;
    m_oSeq_buffer = new CORBA::Octet[m_oSeq_size];

    m_oSeq256K_size = Benchmark::DECIMAL_256K;
    m_oSeq256K_buffer = new CORBA::Octet[m_oSeq256K_size];

    m_oSeq1M_size = Benchmark::DECIMAL_1M;
    m_oSeq1M_buffer = new CORBA::Octet[m_oSeq1M_size];

    m_sSeq_size = 500;
    m_sSeq_buffer = new CORBA::Short[m_sSeq_size];

    m_sSeq1M_size = Benchmark::DECIMAL_512K;
    m_sSeq1M_buffer = new CORBA::Short[m_sSeq1M_size];

    m_strSeq_size = 100;
    m_strSeq = new Benchmark::StringSeq(m_strSeq_size);
    m_strSeq->length(m_strSeq_size);
    for (unsigned int i=0; i < m_strSeq_size; ++i) {
        m_strSeq[i] = CORBA::string_dup("This is a string for a string sequence.");
    }
    
    m_structSeq_size = 50;
    m_structSeq = new Benchmark::StructSeq(m_structSeq_size);
    m_structSeq->length(m_structSeq_size);
    for (unsigned int i=0; i < m_structSeq_size; ++i) {
        m_structSeq[i].fStr.s = (CORBA::Short)  32123;
        m_structSeq[i].fStr.l = (CORBA::Long)   2147480000;
        m_structSeq[i].s =  CORBA::string_dup("This is a string for a sequence of variable-length structures.");
    }
    
    m_value = new Value_impl();
    Benchmark::FixedStruct fStr;
    fStr.s = (CORBA::Short)  32007;
    fStr.l = (CORBA::Long)   2147483007;
    m_value->fStr(fStr);
    m_value->s(CORBA::string_dup("This is a string for a valuetype"));
    
}    
    

    
DataProvider::~DataProvider() {
    delete[] m_oSeq_buffer;
    delete[] m_oSeq256K_buffer;
    delete[] m_oSeq1M_buffer;
    delete[] m_sSeq_buffer;
    delete[] m_sSeq1M_buffer;   
}    
    
    

CORBA::Short   
DataProvider::get_Short()
{
    return m_short;
}

CORBA::Long    
DataProvider::get_Long()
{
    return m_long;
}

CORBA::Double  
DataProvider::get_Double()
{
    return m_double;
}

char*          
DataProvider::get_String()
{
    return CORBA::string_dup(m_string);
}


Benchmark::FixedStruct     
DataProvider::get_FixedStruct()
{
    return m_fixStruct;
}

Benchmark::VariableStruct*  
DataProvider::get_VariableStruct()
{
    Benchmark::VariableStruct* vStruct = new Benchmark::VariableStruct;
    vStruct->fStr = m_varStruct.fStr;
    vStruct->s = CORBA::string_dup(m_varStruct.s);
    return vStruct;
}


Benchmark::OctetSeq*        
DataProvider::get_OctetSeq()
{
    return new Benchmark::OctetSeq(m_oSeq_size, m_oSeq_size, m_oSeq_buffer, 0);
}

Benchmark::OctetSeq_256K*    
DataProvider::get_OctetSeq_256K()
{
    return new Benchmark::OctetSeq_256K(m_oSeq256K_size, m_oSeq256K_buffer, 0);
}

Benchmark::OctetSeq_1M*     
DataProvider::get_OctetSeq_1M()
{
    return new Benchmark::OctetSeq_1M(m_oSeq1M_size, m_oSeq1M_buffer, 0);
}

Benchmark::ShortSeq*        
DataProvider::get_ShortSeq()
{
    return new Benchmark::ShortSeq(m_sSeq_size, m_sSeq_size, m_sSeq_buffer, 0);
}

Benchmark::ShortSeq_1M*     
DataProvider::get_ShortSeq_1M()
{
    return new Benchmark::ShortSeq_1M(m_sSeq1M_size, m_sSeq1M_buffer, 0);
}

Benchmark::StringSeq*       
DataProvider::get_StringSeq()
{
    Benchmark::StringSeq_var result(m_strSeq);
    return result._retn();
}

Benchmark::StructSeq*       
DataProvider::get_StructSeq()
{
    Benchmark::StructSeq_var result(m_structSeq);
    return result._retn();
}


Benchmark::Value* 
DataProvider::get_Value()
{
    Benchmark::Value_var result(m_value);
    return result._retn();
}
    
    
}; // End of namespace DemoPerf



