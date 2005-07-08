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

#ifndef ___MARSHALLING_TESTPING_FACET_EXECUTOR_IMPL_HPP__
#define ___MARSHALLING_TESTPING_FACET_EXECUTOR_IMPL_HPP__

// CARDAMOM include
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

// Std ISO C++ include
#include <iostream>

// PerfTest include
#include "TestCcmMarshalling_cif.skel.hpp"


namespace Marshalling
{

//
// IDL:thalesgroup.com/Marshalling/CCM_TestPing:1.0
//
class TestPingFacetExecutor_impl : virtual public CCM_TestPing,
                                  virtual public Cdmw::OrbSupport::RefCountLocalObject
{
       
  public:

    TestPingFacetExecutor_impl();
    ~TestPingFacetExecutor_impl();

    //syncronous test
    virtual void long_ping(const LongSeq & d_seq) throw(CORBA::SystemException);
    virtual void float_ping(const FloatSeq & f_seq) throw(CORBA::SystemException);
    virtual void any_ping(const CORBA::Any& a) throw(CORBA::SystemException);
    virtual void record_ping(const RecordSeq& r_seq) throw(CORBA::SystemException);
    virtual void invocation_ping() throw(CORBA::SystemException);

    //asyncronous test
    virtual void ow_long_ping(const LongSeq & d_seq) throw(CORBA::SystemException);
    virtual void ow_float_ping(const FloatSeq & f_seq) throw(CORBA::SystemException);
    virtual void ow_any_ping(const CORBA::Any& a) throw(CORBA::SystemException);
    virtual void ow_record_ping(const RecordSeq & r_seq) throw(CORBA::SystemException);
    virtual void ow_invocation_ping() throw(CORBA::SystemException);
    

   
  private:
  
    TestPingFacetExecutor_impl(const TestPingFacetExecutor_impl&);
    void operator=(const TestPingFacetExecutor_impl&);

  
  
};

}; // End of namespace Marshalling

#endif // ___MARSHALLING_TESTPING_FACET_EXECUTOR_IMPL_HPP__
