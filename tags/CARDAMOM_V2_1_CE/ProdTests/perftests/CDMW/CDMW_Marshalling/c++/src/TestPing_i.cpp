/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */


#include <TestPing_i.h>

namespace Marshalling {

  TestPing_i::TestPing_i(){
  }

  TestPing_i::~TestPing_i(){
  }

  //syncronous test
  void TestPing_i::long_ping(const Marshalling::LongSeq & l_seq) throw(CORBA::SystemException){}
  void TestPing_i::float_ping(const Marshalling::FloatSeq & f_seq) throw(CORBA::SystemException){}
  void TestPing_i::any_ping(const CORBA::Any& a) throw(CORBA::SystemException){}
  void TestPing_i::record_ping(const Marshalling::RecordSeq& r_seq) throw(CORBA::SystemException){}
  void TestPing_i::invocation_ping() throw(CORBA::SystemException){}

  //asyncronous test
  void TestPing_i::ow_long_ping(const Marshalling::LongSeq & l_seq) throw(CORBA::SystemException){}
  void TestPing_i::ow_float_ping(const Marshalling::FloatSeq & f_seq) throw(CORBA::SystemException){}
  void TestPing_i::ow_any_ping(const CORBA::Any& a) throw(CORBA::SystemException){}
  void TestPing_i::ow_record_ping(const Marshalling::RecordSeq& r_seq) throw(CORBA::SystemException){}
  void TestPing_i::ow_invocation_ping() throw(CORBA::SystemException){}

}
