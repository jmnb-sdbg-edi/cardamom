/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */

#include <string>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include "testcodec_lib/DataTransfer_impl.hpp"


// define type traits for TestStruct
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(Cdmw::TestCodec::TestStruct);


namespace Cdmw
{
namespace TestCodec
{


DataTransfer_impl::DataTransfer_impl(CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_dataMgr(new DataManager(orb)),
      m_codec(new Cdmw::OrbSupport::Codec<Cdmw::TestCodec::TestStruct> (orb))
{ 
}

DataTransfer_impl::~DataTransfer_impl()
    throw()
{
    delete m_dataMgr;
    delete m_codec;
}


void DataTransfer_impl::send_data (const CORBA::OctetSeq& data)
     throw(CORBA::SystemException, Cdmw::TestCodec::DecodingFailure)                  
{
    TestStruct_var testStruct = new TestStruct();
    
    try {
        m_codec->decode(data, testStruct.inout());
    } catch (const CORBA::Exception& e) {
        std::cerr << "ERROR during decode:\n"
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                  << std::endl;
        throw Cdmw::TestCodec::DecodingFailure();
    }
    
    m_dataMgr->checkTestStruct(testStruct.in());
    
}



}; // End namespace TestCodec
}; // End namespace Cdmw
