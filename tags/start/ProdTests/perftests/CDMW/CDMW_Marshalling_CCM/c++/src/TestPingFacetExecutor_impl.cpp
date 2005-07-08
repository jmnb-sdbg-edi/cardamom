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


#include "TestPingFacetExecutor_impl.hpp"


namespace Marshalling
{

//
// IDL:thalesgroup.com/Marshalling/CCM_TestPing:1.0
//
TestPingFacetExecutor_impl::TestPingFacetExecutor_impl()
{
}

TestPingFacetExecutor_impl::~TestPingFacetExecutor_impl()
{
    std::cout << "===> TestPingFacetExecutor_impl::~TestPingFacetExecutor_impl called!" << std::endl;
}

  //syncronous test
  void TestPingFacetExecutor_impl::long_ping(const Marshalling::LongSeq & l_seq) throw(CORBA::SystemException){}
  void TestPingFacetExecutor_impl::float_ping(const Marshalling::FloatSeq & f_seq) throw(CORBA::SystemException){}
  void TestPingFacetExecutor_impl::any_ping(const CORBA::Any& a) throw(CORBA::SystemException){}
  void TestPingFacetExecutor_impl::record_ping(const Marshalling::RecordSeq& r_seq) throw(CORBA::SystemException){}
  void TestPingFacetExecutor_impl::invocation_ping() throw(CORBA::SystemException){}  

  //asyncronous test
  void TestPingFacetExecutor_impl::ow_long_ping(const Marshalling::LongSeq & l_seq) throw(CORBA::SystemException){}
  void TestPingFacetExecutor_impl::ow_float_ping(const Marshalling::FloatSeq & f_seq) throw(CORBA::SystemException){}
  void TestPingFacetExecutor_impl::ow_any_ping(const CORBA::Any& a) throw(CORBA::SystemException){}
  void TestPingFacetExecutor_impl::ow_record_ping(const Marshalling::RecordSeq& r_seq) throw(CORBA::SystemException){}
  void TestPingFacetExecutor_impl::ow_invocation_ping() throw(CORBA::SystemException){}  


}; // End of namespace Marshalling

