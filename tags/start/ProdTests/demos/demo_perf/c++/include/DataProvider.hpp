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


#ifndef INCL_DATA_PROVIDER_HPP
#define INCL_DATA_PROVIDER_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Benchmark.stub.hpp"




namespace DemoPerf {
    
class DataProvider {

    public :
    
        DataProvider();
        ~DataProvider();
        
        CORBA::Short   get_Short();
        CORBA::Long    get_Long();
        CORBA::Double  get_Double();
        char*          get_String();
        
        Benchmark::FixedStruct      get_FixedStruct();
        Benchmark::VariableStruct*  get_VariableStruct();
        
        Benchmark::OctetSeq*        get_OctetSeq();
        Benchmark::OctetSeq_256K*    get_OctetSeq_256K();
        Benchmark::OctetSeq_1M*     get_OctetSeq_1M();
        Benchmark::ShortSeq*        get_ShortSeq();
        Benchmark::ShortSeq_1M*     get_ShortSeq_1M();
        Benchmark::StringSeq*       get_StringSeq();
        Benchmark::StructSeq*       get_StructSeq();
    
        Benchmark::Value*    get_Value();

        
    private:
        CORBA::Short        m_short;
        CORBA::Long         m_long;
        CORBA::Double       m_double;
        CORBA::String_var   m_string;
        
        Benchmark::FixedStruct         m_fixStruct;
        Benchmark::VariableStruct      m_varStruct;
        
        
        unsigned int   m_oSeq_size;
        CORBA::Octet*  m_oSeq_buffer;
        unsigned int   m_oSeq256K_size;
        CORBA::Octet*  m_oSeq256K_buffer;
        unsigned int   m_oSeq1M_size;
        CORBA::Octet*  m_oSeq1M_buffer;
        unsigned int   m_sSeq_size;
        CORBA::Short*  m_sSeq_buffer;
        unsigned int   m_sSeq1M_size;
        CORBA::Short*  m_sSeq1M_buffer;
        unsigned int              m_strSeq_size;
        Benchmark::StringSeq_var  m_strSeq;
        unsigned int              m_structSeq_size;
        Benchmark::StructSeq_var  m_structSeq;

        Benchmark::Value_var    m_value;
    

}; // End of class DataProvider
    
}; // End of namespace DemoPerf


#endif // INCL_DATA_PROVIDER_HPP

