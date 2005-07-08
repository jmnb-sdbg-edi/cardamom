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


#include "BenchmarkThread.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "TraceAndPerf/performancelibrary/Probe.hpp"

#include <iostream>


namespace DemoPerf {
    
BenchmarkThread::BenchmarkThread(Benchmark::Tester_ptr tester,
                                 Benchmark::I_ptr i):
    m_tester(Benchmark::Tester::_duplicate(tester)),
    m_i(Benchmark::I::_duplicate(i)),
    m_stopped(false)
{
    m_nb_ping_test           = 50;
    m_nb_short_test          = 50;
    m_nb_long_test           = 50;
    m_nb_double_test         = 50;
    m_nb_string_test         = 50;
    m_nb_any_test            = 50;
    m_nb_interface_test      = 50;
    m_nb_fixStruct_test      = 50;
    m_nb_varStruct_test      = 50;
    m_nb_octet_seq_test      = 50;
    m_nb_octet_seq_256K_test = 50;
    m_nb_octet_seq_1M_test   = 50;
    m_nb_short_seq_test      = 50;
    m_nb_short_seq_1M_test   = 50;
    m_nb_string_seq_test     = 50;
    m_nb_object_seq_test     = 50;
    m_nb_struct_seq_test     = 50;
    m_nb_valuetype_test      = 50;
}

        
        
void BenchmarkThread::run() 
    throw()
{
    std::cout << "--- Start of Benchmark ---" << std::endl;

    try {
        Cdmw::Performance::Probe total("Total time");
        
        // First connection to server.
        {
            Cdmw::Performance::Probe p("First Connection");
            m_tester->ping();
        }
    
        int i;
        std::cout << "  Test performances without arguments " << m_nb_ping_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_ping_test; ++i) {
            test_ping();
        }
        std::cout << "  Test performances with Short " << m_nb_short_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_short_test; ++i) {
            test_short();
        }
        std::cout << "  Test performances with Long " << m_nb_long_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_long_test; ++i) {
            test_long();
        }
        std::cout << "  Test performances with Double " << m_nb_double_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_double_test; ++i) {
            test_double();
        }
        std::cout << "  Test performances with String " << m_nb_string_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_string_test; ++i) {
            test_string();
        }
        std::cout << "  Test performances with Any " << m_nb_any_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_any_test; ++i) {
            test_any();
        }
        std::cout << "  Test performances with Interface " << m_nb_interface_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_interface_test; ++i) {
            test_interface();
        }
        std::cout << "  Test performances with FixedStruct " << m_nb_fixStruct_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_fixStruct_test; ++i) {
            test_fixStruct();
        }
        std::cout << "  Test performances with VarStruct " << m_nb_varStruct_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_varStruct_test; ++i) {
            test_varStruct();
        }
        std::cout << "  Test performances with OctetSeq " << m_nb_octet_seq_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_octet_seq_test; ++i) {
            test_octet_seq();
        }
        std::cout << "  Test performances with OctetSeq_256K " << m_nb_octet_seq_256K_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_octet_seq_256K_test; ++i) {
            test_octet_seq_256K();
        }
        std::cout << "  Test performances with OctetSeq_1M " << m_nb_octet_seq_1M_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_octet_seq_1M_test; ++i) {
            test_octet_seq_1M();
        }
        std::cout << "  Test performances with ShortSeq " << m_nb_short_seq_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_short_seq_test; ++i) {
            test_short_seq();
        }
        std::cout << "  Test performances with ShortSeq_1M " << m_nb_short_seq_1M_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_short_seq_1M_test; ++i) {
            test_short_seq_1M();
        }
        std::cout << "  Test performances with StringSeq " << m_nb_string_seq_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_string_seq_test; ++i) {
            test_string_seq();
        }
        std::cout << "  Test performances with ObjectSeq " << m_nb_object_seq_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_object_seq_test; ++i) {
            test_object_seq();
        }
        std::cout << "  Test performances with StructSeq " << m_nb_struct_seq_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_struct_seq_test; ++i) {
            test_struct_seq();
        }
        std::cout << "  Test performances with Valuetype " << m_nb_valuetype_test << " times." << std::endl;
        for (i = 0; !m_stopped && i < m_nb_valuetype_test; ++i) {
            test_valuetype();
        }

    }
    catch (const CORBA::SystemException& e) 
    {
        std::cerr << "SystemException: " 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                  << std::endl;
    }
        
    if (m_stopped) 
        std::cout << "--- Benchmark stopped ---" << std::endl;
    else
        std::cout << "---- End of Benchmark ----" << std::endl;
}


void BenchmarkThread::test_ping ()
    throw (CORBA::SystemException)
{
    Cdmw::Performance::Probe p("Without any argument");
    m_tester->ping();
}

void BenchmarkThread::test_short ()
    throw (CORBA::SystemException)
{
    CORBA::Short s = m_dataProvider.get_Short();
    {
        Cdmw::Performance::Probe p("Short as in argument");
        m_tester->short_in(s);
    }
    {
        Cdmw::Performance::Probe p("Short as out argument");
        m_tester->short_out(s);
    }
    {
        Cdmw::Performance::Probe p("Short as inout argument");
        m_tester->short_inout(s);
    }
    {
        Cdmw::Performance::Probe p("Short as return argument");
        CORBA::Short result = m_tester->short_ret();
        CDMW_ASSERT(result == s);
    }
}

void BenchmarkThread::test_long   ()
    throw (CORBA::SystemException)
{
    CORBA::Long l = m_dataProvider.get_Long();
    {
        Cdmw::Performance::Probe p("Long as in argument");
        m_tester->long_in(l);
    }
    {
        Cdmw::Performance::Probe p("Long as out argument");
        m_tester->long_out(l);
    }
    {
        Cdmw::Performance::Probe p("Long as inout argument");
        m_tester->long_inout(l);
    }
    {
        Cdmw::Performance::Probe p("Long as return argument");
        CORBA::Long result = m_tester->long_ret();
        CDMW_ASSERT(result == l);
    }
}

void BenchmarkThread::test_double ()
    throw (CORBA::SystemException)
{
    CORBA::Double d = m_dataProvider.get_Double();
    {
        Cdmw::Performance::Probe p("Double as in argument");
        m_tester->double_in(d);
    }
    {
        Cdmw::Performance::Probe p("Double as out argument");
        m_tester->double_out(d);
    }
    {
        Cdmw::Performance::Probe p("Double as inout argument");
        m_tester->double_inout(d);
    }
    {
        Cdmw::Performance::Probe p("Double as return argument");
        CORBA::Double result = m_tester->double_ret();
        CDMW_ASSERT(result == d);
    }
}

void BenchmarkThread::test_string ()
    throw (CORBA::SystemException)
{
    CORBA::String_var str = m_dataProvider.get_String();
    {
        Cdmw::Performance::Probe p("String as in argument");
        m_tester->string_in(str.in());
    }
    {
        Cdmw::Performance::Probe p("String as out argument");
        m_tester->string_out(str.out());
    }
    {
        Cdmw::Performance::Probe p("String as inout argument");
        m_tester->string_inout(str.inout());
    }
    {
        Cdmw::Performance::Probe p("String as return argument");
        CORBA::String_var result = m_tester->string_ret();
    }
}

void BenchmarkThread::test_any ()
    throw (CORBA::SystemException)
{
    CORBA::String_var str = m_dataProvider.get_String();
    CORBA::Any_var any(new CORBA::Any());
    any <<= str.in();
    {
        Cdmw::Performance::Probe p("Any as in argument");
        m_tester->any_in(any.in());
    }
    {
        Cdmw::Performance::Probe p("Any as out argument");
        m_tester->any_out(any.out());
    }
    {
        Cdmw::Performance::Probe p("Any as inout argument");
        m_tester->any_inout(any.inout());
    }
    {
        Cdmw::Performance::Probe p("Any as return argument");
        CORBA::Any_var result = m_tester->any_ret();
    }
}

void BenchmarkThread::test_interface ()
    throw (CORBA::SystemException)
{
    Benchmark::I_var obj = m_i;
    {
        Cdmw::Performance::Probe p("Interface as in argument");
        m_tester->interface_in(obj.in());
    }
    {
        Cdmw::Performance::Probe p("Interface as out argument");
        m_tester->interface_out(obj.out());
    }
    {
        Cdmw::Performance::Probe p("Interface as inout argument");
        m_tester->interface_inout(obj.inout());
    }
    {
        Cdmw::Performance::Probe p("Interface as return argument");
        Benchmark::I_var result = m_tester->interface_ret();
    }
}
        
void BenchmarkThread::test_fixStruct ()
    throw (CORBA::SystemException)
{
    Benchmark::FixedStruct_var fStr = m_dataProvider.get_FixedStruct();
    {
        Cdmw::Performance::Probe p("FixedStruct as in argument");
        m_tester->fixStruct_in(fStr.in());
    }
    {
        Cdmw::Performance::Probe p("FixedStruct as out argument");
        m_tester->fixStruct_out(fStr.out());
    }
    {
        Cdmw::Performance::Probe p("FixedStruct as inout argument");
        m_tester->fixStruct_inout(fStr.inout());
    }
    {
        Cdmw::Performance::Probe p("FixedStruct as return argument");
        Benchmark::FixedStruct_var result = m_tester->fixStruct_ret();
    }
}

void BenchmarkThread::test_varStruct ()
    throw (CORBA::SystemException)
{
    Benchmark::VariableStruct_var vStr = m_dataProvider.get_VariableStruct();
    {
        Cdmw::Performance::Probe p("VariableStruct as in argument");
        m_tester->varStruct_in(vStr.in());
    }
    {
        Cdmw::Performance::Probe p("VariableStruct as out argument");
        m_tester->varStruct_out(vStr.out());
    }
    {
        Cdmw::Performance::Probe p("VariableStruct as inout argument");
        m_tester->varStruct_inout(vStr.inout());
    }
    {
        Cdmw::Performance::Probe p("VariableStruct as return argument");
        Benchmark::VariableStruct_var result = m_tester->varStruct_ret();
    }
}

void BenchmarkThread::test_octet_seq ()
    throw (CORBA::SystemException)
{
    Benchmark::OctetSeq_var oSeq = m_dataProvider.get_OctetSeq();
    {
        Cdmw::Performance::Probe p("OctetSeq as in argument");
        m_tester->octet_seq_in(oSeq.in());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq as out argument");
        m_tester->octet_seq_out(oSeq.out());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq as inout argument");
        m_tester->octet_seq_inout(oSeq.inout());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq as return argument");
        Benchmark::OctetSeq_var result = m_tester->octet_seq_ret();
    }
}

void BenchmarkThread::test_octet_seq_256K ()
    throw (CORBA::SystemException)
{
    Benchmark::OctetSeq_256K_var oSeq = m_dataProvider.get_OctetSeq_256K();
    {
        Cdmw::Performance::Probe p("OctetSeq_256K as in argument");
        m_tester->octet_seq_256K_in(oSeq.in());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq_256K as out argument");
        m_tester->octet_seq_256K_out(oSeq.out());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq_256K as inout argument");
        m_tester->octet_seq_256K_inout(oSeq.inout());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq_256K as return argument");
        Benchmark::OctetSeq_256K_var result = m_tester->octet_seq_256K_ret();
    }
}

void BenchmarkThread::test_octet_seq_1M ()
    throw (CORBA::SystemException)
{
    Benchmark::OctetSeq_1M_var oSeq = m_dataProvider.get_OctetSeq_1M();
    {
        Cdmw::Performance::Probe p("OctetSeq_1M as in argument");
        m_tester->octet_seq_1M_in(oSeq.in());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq_1M as out argument");
        m_tester->octet_seq_1M_out(oSeq.out());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq_1M as inout argument");
        m_tester->octet_seq_1M_inout(oSeq.inout());
    }
    {
        Cdmw::Performance::Probe p("OctetSeq_1M as return argument");
        Benchmark::OctetSeq_1M_var result = m_tester->octet_seq_1M_ret();
    }
}

void BenchmarkThread::test_short_seq ()
    throw (CORBA::SystemException)
{
    Benchmark::ShortSeq_var sSeq = m_dataProvider.get_ShortSeq();
    {
        Cdmw::Performance::Probe p("ShortSeq as in argument");
        m_tester->short_seq_in(sSeq.in());
    }
    {
        Cdmw::Performance::Probe p("ShortSeq as out argument");
        m_tester->short_seq_out(sSeq.out());
    }
    {
        Cdmw::Performance::Probe p("ShortSeq as inout argument");
        m_tester->short_seq_inout(sSeq.inout());
    }
    {
        Cdmw::Performance::Probe p("ShortSeq as return argument");
        Benchmark::ShortSeq_var result = m_tester->short_seq_ret();
    }
}

void BenchmarkThread::test_short_seq_1M ()
    throw (CORBA::SystemException)
{
    Benchmark::ShortSeq_1M_var sSeq = m_dataProvider.get_ShortSeq_1M();
    {
        Cdmw::Performance::Probe p("ShortSeq_1M as in argument");
        m_tester->short_seq_1M_in(sSeq.in());
    }
    {
        Cdmw::Performance::Probe p("ShortSeq_1M as out argument");
        m_tester->short_seq_1M_out(sSeq.out());
    }
    {
        Cdmw::Performance::Probe p("ShortSeq_1M as inout argument");
        m_tester->short_seq_1M_inout(sSeq.inout());
    }
    {
        Cdmw::Performance::Probe p("ShortSeq_1M as return argument");
        Benchmark::ShortSeq_1M_var result = m_tester->short_seq_1M_ret();
    }
}

void BenchmarkThread::test_string_seq ()
    throw (CORBA::SystemException)
{
    Benchmark::StringSeq_var strSeq = m_dataProvider.get_StringSeq();
    {
        Cdmw::Performance::Probe p("StringSeq as in argument");
        m_tester->string_seq_in(strSeq.in());
    }
    {
        Cdmw::Performance::Probe p("StringSeq as out argument");
        m_tester->string_seq_out(strSeq.out());
    }
    {
        Cdmw::Performance::Probe p("StringSeq as inout argument");
        m_tester->string_seq_inout(strSeq.inout());
    }
    {
        Cdmw::Performance::Probe p("StringSeq as return argument");
        Benchmark::StringSeq_var result = m_tester->string_seq_ret();
    }
}

void BenchmarkThread::test_object_seq ()
    throw (CORBA::SystemException)
{
    Benchmark::ObjectSeq_var objSeq = new Benchmark::ObjectSeq(20);
    objSeq->length(20);
    for (unsigned int i=0; i<20; ++i) {
        objSeq[i] = Benchmark::I::_duplicate(m_i.in());
    }
    
    {
        Cdmw::Performance::Probe p("ObjectSeq as in argument");
        m_tester->object_seq_in(objSeq.in());
    }
    {
        Cdmw::Performance::Probe p("ObjectSeq as out argument");
        m_tester->object_seq_out(objSeq.out());
    }
    {
        Cdmw::Performance::Probe p("ObjectSeq as inout argument");
        m_tester->object_seq_inout(objSeq.inout());
    }
    {
        Cdmw::Performance::Probe p("ObjectSeq as return argument");
        Benchmark::ObjectSeq_var result = m_tester->object_seq_ret();
    }
}

void BenchmarkThread::test_struct_seq ()
    throw (CORBA::SystemException)
{
    Benchmark::StructSeq_var strSeq = m_dataProvider.get_StructSeq();
    {
        Cdmw::Performance::Probe p("StructSeq as in argument");
        m_tester->struct_seq_in(strSeq.in());
    }
    {
        Cdmw::Performance::Probe p("StructSeq as out argument");
        m_tester->struct_seq_out(strSeq.out());
    }
    {
        Cdmw::Performance::Probe p("StructSeq as inout argument");
        m_tester->struct_seq_inout(strSeq.inout());
    }
    {
        Cdmw::Performance::Probe p("StructSeq as return argument");
        Benchmark::StructSeq_var result = m_tester->struct_seq_ret();
    }
}

void BenchmarkThread::test_valuetype ()
    throw (CORBA::SystemException)
{
    Benchmark::Value_var val = m_dataProvider.get_Value();
    {
        Cdmw::Performance::Probe p("Value as in argument");
        m_tester->valuetype_in(val.in());
    }
    {
        Cdmw::Performance::Probe p("Value as out argument");
        m_tester->valuetype_out(val.out());
    }
    {
        Cdmw::Performance::Probe p("Value as inout argument");
        m_tester->valuetype_inout(val.inout());
    }
    {
        Cdmw::Performance::Probe p("Value as return argument");
        Benchmark::Value_var result = m_tester->valuetype_ret();
    }
}

};  // End of namespace DemoPerf

