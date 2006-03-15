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


#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/Codec.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


#include "testcodec/EncoderProcessControl.hpp"
#include "testcodec/DataManager.hpp"


// define type traits for TestStruct
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(Cdmw::TestCodec::TestStruct);


namespace Cdmw
{
namespace TestCodec
{


EncoderProcessControl::EncoderProcessControl (CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : Testable("Test Codec"),
      m_orb (CORBA::ORB::_duplicate(orb))
{
}

EncoderProcessControl::~EncoderProcessControl()
    throw()
{
}
  


// process initialisation called by platformmngt     
void EncoderProcessControl::on_initialise (
                  const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    // do nothing            
}
  
// process to run called by platformmngt    
void EncoderProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
          CORBA::SystemException)
{
    // 
    // Retrive DataTransfer
    //
    std::cout << "Retrive DataTransfer object from Repository" << std::endl;
    
    // Get the naming and repository from Repository interface
    CdmwNamingAndRepository::Repository_var repository =
        CdmwNamingAndRepository::Repository::_nil();  
    repository = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
     
    // Get Naming Interface of dom1 name domain
    Cdmw::CommonSvcs::Naming::NamingInterface ni =
        Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface (
                        "dom1");
         
    // Get the DataTransfer object from repository
    CORBA::Object_var objref = ni.resolve ("DataTransfer");

    // get the DataTransfer proxy
    m_dataTransfer = Cdmw::TestCodec::DataTransfer::_narrow(objref.in());

    if (CORBA::is_nil(m_dataTransfer.in()))
    {
        std::cout << "File : " << __FILE__ << " Line : " << __LINE__
                  << "could not _narrow object to type DataTransfer" << std::endl;
        throw CdmwPlatformMngt::ProcessDelegate::NotReadyToRun();
    }

    // 
    // start tests
    //
    start();
}


// process to stop called by platformmngt
void EncoderProcessControl::on_stop()
    throw(CORBA::SystemException)
{
}



void EncoderProcessControl::do_tests()
{
    set_nbOfRequestedTestOK(3);
    
    DataManager dataMgr(m_orb.in());
    /* PCR-0049
    Cdmw::OrbSupport::Codec<Cdmw::TestCodec::TestStruct> codec(m_orb.in());
    */
    Cdmw::OrbSupport::Codec<Cdmw::TestCodec::TestStruct> codec;
    
    try {
        TEST_INFO("Sending encoded data with enum set to VAL1");
        
        // construct data
        TestStruct_var testStruct = dataMgr.createTestStruct(VAL1);
        
        // encode data
        CORBA::OctetSeq_var encoded_data = codec.encode(testStruct.in());
        
        // send data
        m_dataTransfer->send_data(encoded_data.in());
        
        TEST_SUCCEED();
        
     } catch (Cdmw::TestCodec::DecodingFailure& e) {
         std::cerr << "DecodingFailure exception send by decoder" << std::endl;
         TEST_FAILED();
     } catch (const Cdmw::Exception& e) {
        std::cerr << "Cdmw::Exception :\n" << e.what() << std::endl;
        TEST_FAILED();
     } catch (const CORBA::Exception& e) {
        std::cerr << "CORBA::Exception :\n" 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                  << std::endl;
        TEST_FAILED();
     }
    
    try {
        TEST_INFO("Sending encoded data with enum set to VAL2");
        
        // construct data
        TestStruct_var testStruct = dataMgr.createTestStruct(VAL2);
        
        // encode data
        CORBA::OctetSeq_var encoded_data = codec.encode(testStruct.in());
        
        // send data
        m_dataTransfer->send_data(encoded_data.in());
        
        TEST_SUCCEED();
        
     } catch (Cdmw::TestCodec::DecodingFailure& e) {
         std::cerr << "DecodingFailure exception send by decoder" << std::endl;
         TEST_FAILED();
     } catch (const Cdmw::Exception& e) {
        std::cerr << "Cdmw::Exception :\n" << e.what() << std::endl;
        TEST_FAILED();
     } catch (const CORBA::Exception& e) {
        std::cerr << "CORBA::Exception :\n" 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                  << std::endl;
        TEST_FAILED();
     }
    
    try {
        TEST_INFO("Sending encoded data with enum set to VAL3");
        
        // construct data
        TestStruct_var testStruct = dataMgr.createTestStruct(VAL3);
        
        // encode data
        CORBA::OctetSeq_var encoded_data = codec.encode(testStruct.in());
        
        // send data
        m_dataTransfer->send_data(encoded_data.in());
        
        TEST_SUCCEED();
        
     } catch (Cdmw::TestCodec::DecodingFailure& e) {
         std::cerr << "DecodingFailure exception send by decoder" << std::endl;
         TEST_FAILED();
     } catch (const Cdmw::Exception& e) {
        std::cerr << "Cdmw::Exception :\n" << e.what() << std::endl;
        TEST_FAILED();
     } catch (const CORBA::Exception& e) {
        std::cerr << "CORBA::Exception :\n" 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                  << std::endl;
        TEST_FAILED();
     }
    
}





}; // End namespace TestCodec
}; // End namespace Cdmw
