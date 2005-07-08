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


#ifndef INCL_TESTER_IMPL_HPP
#define INCL_TESTER_IMPL_HPP


#include "Foundation/orbsupport/CORBA.hpp"

#include "Benchmark.skel.hpp"
#include "DataProvider.hpp"


namespace DemoPerf {

class Tester_impl: public virtual POA_Benchmark::Tester
{
    
    public: 

        Tester_impl(Benchmark::I_ptr i):
            m_i(Benchmark::I::_duplicate(i))
        {};
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/ping:1.0
        //
        virtual void ping()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_in:1.0
        //
        virtual void short_in(CORBA::Short s)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_out:1.0
        //
        virtual void short_out(CORBA::Short_out s)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_inout:1.0
        //
        virtual void short_inout(CORBA::Short& s)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_ret:1.0
        //
        virtual CORBA::Short short_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/long_in:1.0
        //
        virtual void long_in(CORBA::Long l)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/long_out:1.0
        //
        virtual void long_out(CORBA::Long_out l)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/long_inout:1.0
        //
        virtual void long_inout(CORBA::Long& l)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/long_ret:1.0
        //
        virtual CORBA::Long long_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/double_in:1.0
        //
        virtual void double_in(CORBA::Double d)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/double_out:1.0
        //
        virtual void double_out(CORBA::Double_out d)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/double_inout:1.0
        //
        virtual void double_inout(CORBA::Double& d)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/double_ret:1.0
        //
        virtual CORBA::Double double_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/string_in:1.0
        //
        virtual void string_in(const char* s)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/string_out:1.0
        //
        virtual void string_out(CORBA::String_out s)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/string_inout:1.0
        //
        virtual void string_inout(char*& s)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/string_ret:1.0
        //
        virtual char* string_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/any_in:1.0
        //
        virtual void any_in(const CORBA::Any& a)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/any_out:1.0
        //
        virtual void any_out(CORBA::Any_out a)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/any_inout:1.0
        //
        virtual void any_inout(CORBA::Any& a)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/any_ret:1.0
        //
        virtual CORBA::Any* any_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/interface_in:1.0
        //
        virtual void interface_in(Benchmark::I_ptr interf)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/interface_out:1.0
        //
        virtual void interface_out(Benchmark::I_out interf)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/interface_inout:1.0
        //
        virtual void interface_inout(Benchmark::I_ptr& interf)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/interface_ret:1.0
        //
        virtual Benchmark::I_ptr interface_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/fixStruct_in:1.0
        //
        virtual void fixStruct_in(const Benchmark::FixedStruct& fStruct)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/fixStruct_out:1.0
        //
        virtual void fixStruct_out(Benchmark::FixedStruct_out fStruct)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/fixStruct_inout:1.0
        //
        virtual void fixStruct_inout(Benchmark::FixedStruct& fStruct)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/fixStruct_ret:1.0
        //
        virtual Benchmark::FixedStruct fixStruct_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/varStruct_in:1.0
        //
        virtual void varStruct_in(const Benchmark::VariableStruct& vStruct)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/varStruct_out:1.0
        //
        virtual void varStruct_out(Benchmark::VariableStruct_out vStruct)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/varStruct_inout:1.0
        //
        virtual void varStruct_inout(Benchmark::VariableStruct& vStruct)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/varStruct_ret:1.0
        //
        virtual Benchmark::VariableStruct* varStruct_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_in:1.0
        //
        virtual void octet_seq_in(const Benchmark::OctetSeq& o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_out:1.0
        //
        virtual void octet_seq_out(Benchmark::OctetSeq_out o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_inout:1.0
        //
        virtual void octet_seq_inout(Benchmark::OctetSeq& o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_ret:1.0
        //
        virtual Benchmark::OctetSeq* octet_seq_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_256K_in:1.0
        //
        virtual void octet_seq_256K_in(const Benchmark::OctetSeq_256K& o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_256K_out:1.0
        //
        virtual void octet_seq_256K_out(Benchmark::OctetSeq_256K_out o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_256K_inout:1.0
        //
        virtual void octet_seq_256K_inout(Benchmark::OctetSeq_256K& o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_256K_seq_ret:1.0
        //
        virtual Benchmark::OctetSeq_256K* octet_seq_256K_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_1M_in:1.0
        //
        virtual void octet_seq_1M_in(const Benchmark::OctetSeq_1M& o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_1M_out:1.0
        //
        virtual void octet_seq_1M_out(Benchmark::OctetSeq_1M_out o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_1M_inout:1.0
        //
        virtual void octet_seq_1M_inout(Benchmark::OctetSeq_1M& o_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/octet_seq_1M_ret:1.0
        //
        virtual Benchmark::OctetSeq_1M* octet_seq_1M_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_seq_in:1.0
        //
        virtual void short_seq_in(const Benchmark::ShortSeq& s_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_seq_out:1.0
        //
        virtual void short_seq_out(Benchmark::ShortSeq_out s_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_seq_inout:1.0
        //
        virtual void short_seq_inout(Benchmark::ShortSeq& s_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_seq_ret:1.0
        //
        virtual Benchmark::ShortSeq* short_seq_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_seq_1M_in:1.0
        //
        virtual void short_seq_1M_in(const Benchmark::ShortSeq_1M& s_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_seq_1M_out:1.0
        //
        virtual void short_seq_1M_out(Benchmark::ShortSeq_1M_out s_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_seq_1M_inout:1.0
        //
        virtual void short_seq_1M_inout(Benchmark::ShortSeq_1M& s_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/short_seq_1M_ret:1.0
        //
        virtual Benchmark::ShortSeq_1M* short_seq_1M_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/string_seq_in:1.0
        //
        virtual void string_seq_in(const Benchmark::StringSeq& str_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/string_seq_out:1.0
        //
        virtual void string_seq_out(Benchmark::StringSeq_out str_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/string_seq_inout:1.0
        //
        virtual void string_seq_inout(Benchmark::StringSeq& str_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/string_seq_ret:1.0
        //
        virtual Benchmark::StringSeq* string_seq_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/object_seq_in:1.0
        //
        virtual void object_seq_in(const Benchmark::ObjectSeq& obj_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/object_seq_out:1.0
        //
        virtual void object_seq_out(Benchmark::ObjectSeq_out obj_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/object_seq_inout:1.0
        //
        virtual void object_seq_inout(Benchmark::ObjectSeq& obj_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/object_seq_ret:1.0
        //
        virtual Benchmark::ObjectSeq* object_seq_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/struct_seq_in:1.0
        //
        virtual void struct_seq_in(const Benchmark::StructSeq& struct_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/struct_seq_out:1.0
        //
        virtual void struct_seq_out(Benchmark::StructSeq_out struct_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/struct_seq_inout:1.0
        //
        virtual void struct_seq_inout(Benchmark::StructSeq& struct_seq)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/struct_seq_ret:1.0
        //
        virtual Benchmark::StructSeq* struct_seq_ret()
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/valuetype_in:1.0
        //
        virtual void valuetype_in(Benchmark::Value* v)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/valuetype_out:1.0
        //
        virtual void valuetype_out(Benchmark::Value_out v)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/valuetype_inout:1.0
        //
        virtual void valuetype_inout(Benchmark::Value*& v)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalestgroup.com/Benchmark/Tester/valuetype_ret:1.0
        //
        virtual Benchmark::Value* valuetype_ret()
            throw(CORBA::SystemException);

            
    private:
    
        DataProvider m_dataProvider;
        
        Benchmark::I_var m_i;
    
        
}; // End class Tester_impl    
    
}; // End namespace DemoPerf

#endif // INCL_TESTER_IMPL_HPP

