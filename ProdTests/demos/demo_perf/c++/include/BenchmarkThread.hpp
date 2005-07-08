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


#ifndef INCL_BENCHMARK_THREAD_HPP
#define INCL_BENCHMARK_THREAD_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include "Benchmark.stub.hpp"
#include "DataProvider.hpp"


namespace DemoPerf {
    
class BenchmarkThread: public virtual Cdmw::OsSupport::Thread
{

    public:
    
        BenchmarkThread(Benchmark::Tester_ptr tester,
                        Benchmark::I_ptr i);
        
        virtual ~BenchmarkThread() throw() {};
        
        void stop() {
            m_stopped = true;
        }
    
    protected:

        void run() throw();
    
    
    private:

        void test_ping  ()
            throw (CORBA::SystemException);

        // Basic types
        void test_short  ()
            throw (CORBA::SystemException);
        void test_long   ()
            throw (CORBA::SystemException);
        void test_double ()
            throw (CORBA::SystemException);
        void test_string ()
            throw (CORBA::SystemException);

        void test_any ()
            throw (CORBA::SystemException);
        void test_interface ()
            throw (CORBA::SystemException);        
        
        // Structures
        void test_fixStruct ()
            throw (CORBA::SystemException);
        void test_varStruct ()
            throw (CORBA::SystemException);
        
        // Sequences
        void test_octet_seq     ()
            throw (CORBA::SystemException);
        void test_octet_seq_256K ()
            throw (CORBA::SystemException);
        void test_octet_seq_1M  ()
            throw (CORBA::SystemException);
        void test_short_seq     ()
            throw (CORBA::SystemException);
        void test_short_seq_1M  ()
            throw (CORBA::SystemException);
        void test_string_seq    ()
            throw (CORBA::SystemException);
        void test_object_seq    ()
            throw (CORBA::SystemException);
        void test_struct_seq    ()
            throw (CORBA::SystemException);
        
        // Valuetype
        void test_valuetype ()
            throw (CORBA::SystemException);
    
    
        /**
        * Data Provider
        */
        DataProvider m_dataProvider;
    
        /**
        * Tester reference
        */
        Benchmark::Tester_var m_tester;
        
        /**
        * I reference
        */
        Benchmark::I_var m_i;

        /**
        * thread stop flag
        */
        bool m_stopped;
        
        /**
        * Number of benchmark call for each type 
        */
        int m_nb_ping_test;
        int m_nb_short_test;
        int m_nb_long_test;
        int m_nb_double_test;
        int m_nb_string_test;
        int m_nb_any_test;
        int m_nb_interface_test;
        int m_nb_fixStruct_test;
        int m_nb_varStruct_test;
        int m_nb_octet_seq_test;
        int m_nb_octet_seq_256K_test;
        int m_nb_octet_seq_1M_test;
        int m_nb_short_seq_test;
        int m_nb_short_seq_1M_test;
        int m_nb_string_seq_test;
        int m_nb_object_seq_test;
        int m_nb_struct_seq_test;
        int m_nb_valuetype_test;
        
        
}; // End of class BenchmarkThread

}; // End of namespace DemoPerf

#endif  // INCL_BENCHMARK_THREAD_HPP

