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


#include "Tester_impl.hpp"

#include "Foundation/common/Assert.hpp"



namespace DemoPerf {
    
//
// IDL:thalestgroup.com/Benchmark/Tester/ping:1.0
//
void Tester_impl::ping()
    throw(CORBA::SystemException)
{
}


//
// IDL:thalestgroup.com/Benchmark/Tester/short_in:1.0
//
void Tester_impl::short_in(CORBA::Short s)
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(s == m_dataProvider.get_Short());
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_out:1.0
//
void Tester_impl::short_out(CORBA::Short_out s)
    throw(CORBA::SystemException)
{
    s = m_dataProvider.get_Short();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_inout:1.0
//
void Tester_impl::short_inout(CORBA::Short& s)
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(s == m_dataProvider.get_Short());
    s = m_dataProvider.get_Short();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_ret:1.0
//
CORBA::Short Tester_impl::short_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_Short();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/long_in:1.0
//
void Tester_impl::long_in(CORBA::Long l)
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(l == m_dataProvider.get_Long());
}

//
// IDL:thalestgroup.com/Benchmark/Tester/long_out:1.0
//
void Tester_impl::long_out(CORBA::Long_out l)
    throw(CORBA::SystemException)
{
    l = m_dataProvider.get_Long();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/long_inout:1.0
//
void Tester_impl::long_inout(CORBA::Long& l)
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(l == m_dataProvider.get_Long());    
    l = m_dataProvider.get_Long();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/long_ret:1.0
//
CORBA::Long Tester_impl::long_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_Long();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/double_in:1.0
//
void Tester_impl::double_in(CORBA::Double d)
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(d == m_dataProvider.get_Double());
}

//
// IDL:thalestgroup.com/Benchmark/Tester/double_out:1.0
//
void Tester_impl::double_out(CORBA::Double_out d)
    throw(CORBA::SystemException)
{
    d = m_dataProvider.get_Double();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/double_inout:1.0
//
void Tester_impl::double_inout(CORBA::Double& d)
    throw(CORBA::SystemException)
{
    CDMW_ASSERT(d == m_dataProvider.get_Double());
    d = m_dataProvider.get_Double();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/double_ret:1.0
//
CORBA::Double Tester_impl::double_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_Double();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/string_in:1.0
//
void Tester_impl::string_in(const char* s)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/string_out:1.0
//
void Tester_impl::string_out(CORBA::String_out s)
    throw(CORBA::SystemException)
{
    s = m_dataProvider.get_String();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/string_inout:1.0
//
void Tester_impl::string_inout(char*& s)
    throw(CORBA::SystemException)
{
    CORBA::string_free(s);
    s = m_dataProvider.get_String();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/string_ret:1.0
//
char* Tester_impl::string_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_String();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/any_in:1.0
//
void Tester_impl::any_in(const CORBA::Any& a)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/any_out:1.0
//
void Tester_impl::any_out(CORBA::Any_out a)
    throw(CORBA::SystemException)
{
    CORBA::Any_var any = new CORBA::Any;
    CORBA::String_var s(m_dataProvider.get_String());
    any.inout() <<= s.in();
    a = any._retn();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/any_inout:1.0
//
void Tester_impl::any_inout(CORBA::Any& a)
    throw(CORBA::SystemException)
{
    CORBA::String_var s(m_dataProvider.get_String());
    a <<= s.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/any_ret:1.0
//
CORBA::Any* Tester_impl::any_ret()
    throw(CORBA::SystemException)
{
    CORBA::String_var s(m_dataProvider.get_String());
    CORBA::Any_var a(new CORBA::Any());
    a <<= s.in();
    return a._retn();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/interface_in:1.0
//
void Tester_impl::interface_in(Benchmark::I_ptr interf)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/interface_out:1.0
//
void Tester_impl::interface_out(Benchmark::I_out interf)
    throw(CORBA::SystemException)
{
    interf = Benchmark::I::_duplicate(m_i.in());
}

//
// IDL:thalestgroup.com/Benchmark/Tester/interface_inout:1.0
//
void Tester_impl::interface_inout(Benchmark::I_ptr& interf)
    throw(CORBA::SystemException)
{
    CORBA::release(interf);
    interf = Benchmark::I::_duplicate(m_i.in());
}

//
// IDL:thalestgroup.com/Benchmark/Tester/interface_ret:1.0
//
Benchmark::I_ptr Tester_impl::interface_ret()
    throw(CORBA::SystemException)
{
    return Benchmark::I::_duplicate(m_i.in());
}

//
// IDL:thalestgroup.com/Benchmark/Tester/fixStruct_in:1.0
//
void Tester_impl::fixStruct_in(const Benchmark::FixedStruct& fStruct)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/fixStruct_out:1.0
//
void Tester_impl::fixStruct_out(Benchmark::FixedStruct_out fStruct)
    throw(CORBA::SystemException)
{
    fStruct = m_dataProvider.get_FixedStruct();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/fixStruct_inout:1.0
//
void Tester_impl::fixStruct_inout(Benchmark::FixedStruct& fStruct)
    throw(CORBA::SystemException)
{
    fStruct = m_dataProvider.get_FixedStruct();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/fixStruct_ret:1.0
//
Benchmark::FixedStruct Tester_impl::fixStruct_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_FixedStruct();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/varStruct_in:1.0
//
void Tester_impl::varStruct_in(const Benchmark::VariableStruct& vStruct)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/varStruct_out:1.0
//
void Tester_impl::varStruct_out(Benchmark::VariableStruct_out vStruct)
    throw(CORBA::SystemException)
{
    vStruct = m_dataProvider.get_VariableStruct();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/varStruct_inout:1.0
//
void Tester_impl::varStruct_inout(Benchmark::VariableStruct& vStruct)
    throw(CORBA::SystemException)
{
    Benchmark::VariableStruct_var tmp(m_dataProvider.get_VariableStruct());
    vStruct = tmp.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/varStruct_ret:1.0
//
Benchmark::VariableStruct* Tester_impl::varStruct_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_VariableStruct();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_in:1.0
//
void Tester_impl::octet_seq_in(const Benchmark::OctetSeq& o_seq)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_out:1.0
//
void Tester_impl::octet_seq_out(Benchmark::OctetSeq_out o_seq)
    throw(CORBA::SystemException)
{
    o_seq = m_dataProvider.get_OctetSeq();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_inout:1.0
//
void Tester_impl::octet_seq_inout(Benchmark::OctetSeq& o_seq)
    throw(CORBA::SystemException)
{
    Benchmark::OctetSeq_var tmp(m_dataProvider.get_OctetSeq());
    o_seq = tmp.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_ret:1.0
//
Benchmark::OctetSeq* Tester_impl::octet_seq_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_OctetSeq();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_256K_in:1.0
//
void Tester_impl::octet_seq_256K_in(const Benchmark::OctetSeq_256K& o_seq)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_256K_out:1.0
//
void Tester_impl::octet_seq_256K_out(Benchmark::OctetSeq_256K_out o_seq)
    throw(CORBA::SystemException)
{
    o_seq = m_dataProvider.get_OctetSeq_256K();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_256K_inout:1.0
//
void Tester_impl::octet_seq_256K_inout(Benchmark::OctetSeq_256K& o_seq)
    throw(CORBA::SystemException)
{
    Benchmark::OctetSeq_256K_var tmp(m_dataProvider.get_OctetSeq_256K());
    o_seq = tmp.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_256K_seq_ret:1.0
//
Benchmark::OctetSeq_256K* Tester_impl::octet_seq_256K_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_OctetSeq_256K();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_1M_in:1.0
//
void Tester_impl::octet_seq_1M_in(const Benchmark::OctetSeq_1M& o_seq)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_1M_out:1.0
//
void Tester_impl::octet_seq_1M_out(Benchmark::OctetSeq_1M_out o_seq)
    throw(CORBA::SystemException)
{
    o_seq = m_dataProvider.get_OctetSeq_1M();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_1M_inout:1.0
//
void Tester_impl::octet_seq_1M_inout(Benchmark::OctetSeq_1M& o_seq)
    throw(CORBA::SystemException)
{
    Benchmark::OctetSeq_1M_var tmp(m_dataProvider.get_OctetSeq_1M());
    o_seq = tmp.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/octet_seq_1M_ret:1.0
//
Benchmark::OctetSeq_1M* Tester_impl::octet_seq_1M_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_OctetSeq_1M();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_seq_in:1.0
//
void Tester_impl::short_seq_in(const Benchmark::ShortSeq& s_seq)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_seq_out:1.0
//
void Tester_impl::short_seq_out(Benchmark::ShortSeq_out s_seq)
    throw(CORBA::SystemException)
{
    s_seq = m_dataProvider.get_ShortSeq();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_seq_inout:1.0
//
void Tester_impl::short_seq_inout(Benchmark::ShortSeq& s_seq)
    throw(CORBA::SystemException)
{
    Benchmark::ShortSeq_var tmp(m_dataProvider.get_ShortSeq());
    s_seq = tmp.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_seq_ret:1.0
//
Benchmark::ShortSeq* Tester_impl::short_seq_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_ShortSeq();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_seq_1M_in:1.0
//
void Tester_impl::short_seq_1M_in(const Benchmark::ShortSeq_1M& s_seq)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_seq_1M_out:1.0
//
void Tester_impl::short_seq_1M_out(Benchmark::ShortSeq_1M_out s_seq)
    throw(CORBA::SystemException)
{
    s_seq = m_dataProvider.get_ShortSeq_1M();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_seq_1M_inout:1.0
//
void Tester_impl::short_seq_1M_inout(Benchmark::ShortSeq_1M& s_seq)
    throw(CORBA::SystemException)
{
    Benchmark::ShortSeq_1M_var tmp(m_dataProvider.get_ShortSeq_1M());
    s_seq = tmp.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/short_seq_1M_ret:1.0
//
Benchmark::ShortSeq_1M* Tester_impl::short_seq_1M_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_ShortSeq_1M();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/string_seq_in:1.0
//
void Tester_impl::string_seq_in(const Benchmark::StringSeq& str_seq)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/string_seq_out:1.0
//
void Tester_impl::string_seq_out(Benchmark::StringSeq_out str_seq)
    throw(CORBA::SystemException)
{
    str_seq = m_dataProvider.get_StringSeq();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/string_seq_inout:1.0
//
void Tester_impl::string_seq_inout(Benchmark::StringSeq& str_seq)
    throw(CORBA::SystemException)
{
    Benchmark::StringSeq_var tmp(m_dataProvider.get_StringSeq());
    str_seq = tmp.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/string_seq_ret:1.0
//
Benchmark::StringSeq* Tester_impl::string_seq_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_StringSeq();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/object_seq_in:1.0
//
void Tester_impl::object_seq_in(const Benchmark::ObjectSeq& obj_seq)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/object_seq_out:1.0
//
void Tester_impl::object_seq_out(Benchmark::ObjectSeq_out obj_seq)
    throw(CORBA::SystemException)
{
    obj_seq = new Benchmark::ObjectSeq(20);
    obj_seq->length(20);
    for (unsigned int i=0; i < 20; ++i) {
        obj_seq[i] =  Benchmark::I::_duplicate(m_i.in());
    }
}

//
// IDL:thalestgroup.com/Benchmark/Tester/object_seq_inout:1.0
//
void Tester_impl::object_seq_inout(Benchmark::ObjectSeq& obj_seq)
    throw(CORBA::SystemException)
{
    obj_seq.length(20);
    for (int i=0; i < 20; ++i) {
        obj_seq[i] =  Benchmark::I::_duplicate(m_i.in());
    }
}

//
// IDL:thalestgroup.com/Benchmark/Tester/object_seq_ret:1.0
//
Benchmark::ObjectSeq* Tester_impl::object_seq_ret()
    throw(CORBA::SystemException)
{
    Benchmark::ObjectSeq_var result = new Benchmark::ObjectSeq(20);
    result->length(20);
    for (int i=0; i < 20; ++i) {
        (result.in())[i] =  Benchmark::I::_duplicate(m_i.in());
    }
    return result._retn();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/struct_seq_in:1.0
//
void Tester_impl::struct_seq_in(const Benchmark::StructSeq& struct_seq)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/struct_seq_out:1.0
//
void Tester_impl::struct_seq_out(Benchmark::StructSeq_out struct_seq)
    throw(CORBA::SystemException)
{
    struct_seq = m_dataProvider.get_StructSeq();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/struct_seq_inout:1.0
//
void Tester_impl::struct_seq_inout(Benchmark::StructSeq& struct_seq)
    throw(CORBA::SystemException)
{
    Benchmark::StructSeq_var tmp(m_dataProvider.get_StructSeq());
    struct_seq = tmp.in();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/struct_seq_ret:1.0
//
Benchmark::StructSeq* Tester_impl::struct_seq_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_StructSeq();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/valuetype_in:1.0
//
void Tester_impl::valuetype_in(Benchmark::Value* v)
    throw(CORBA::SystemException)
{
}

//
// IDL:thalestgroup.com/Benchmark/Tester/valuetype_out:1.0
//
void Tester_impl::valuetype_out(Benchmark::Value_out v)
    throw(CORBA::SystemException)
{
    v = m_dataProvider.get_Value();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/valuetype_inout:1.0
//
void Tester_impl::valuetype_inout(Benchmark::Value*& v)
    throw(CORBA::SystemException)
{
    CORBA::remove_ref(v);
    v = m_dataProvider.get_Value();
}

//
// IDL:thalestgroup.com/Benchmark/Tester/valuetype_ret:1.0
//
Benchmark::Value* Tester_impl::valuetype_ret()
    throw(CORBA::SystemException)
{
    return m_dataProvider.get_Value();
}
    
    
} // End of namespace DemoPerf



