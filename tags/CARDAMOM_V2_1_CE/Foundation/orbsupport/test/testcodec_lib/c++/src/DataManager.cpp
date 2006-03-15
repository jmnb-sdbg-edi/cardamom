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

#include <iostream>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include "testcodec_lib/DataManager.hpp"


namespace
{
    const char*             TEST_UNION_STR_VALUE
        = "String for 'str' member of TestUnion";
    const CORBA::Short      TEST_UNION_S_VALUE
        = -123;
    const CORBA::ULongLong  TEST_UNION_ULL_VALUE
        = 123456;
    
    const CORBA::UShort     TEST_STR_SEQ_LENGTH
        = 2;
    const char*             TEST_STR_SEQ_0_VALUE
        = "String for member number 0 of TestStrSeq";
    const char*             TEST_STR_SEQ_1_VALUE
        = "String for member number 1 of TestStrSeq";
    
    const char*             TEST_ANY_VALUE
        = "String for 'a' member of TestStruct";
        
    
}


namespace Cdmw
{
namespace TestCodec
{


DataManager::DataManager(CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
{ 
}

DataManager::~DataManager()
    throw()
{
}



TestStruct*
DataManager::createTestStruct(TestEnum e)
{
    TestStruct* testStruct = new TestStruct();
    
    // fill u member
    testStruct->u._d(e);
    switch (e) {
        case VAL1:
            testStruct->u.str(TEST_UNION_STR_VALUE);
            break;
            
        case VAL2:   
            testStruct->u.s(TEST_UNION_S_VALUE);
            break;

        case VAL3:   
            testStruct->u.ull(TEST_UNION_ULL_VALUE);
            break;
    }
    
    
    // fill seq member
    testStruct->seq.length(TEST_STR_SEQ_LENGTH);
    testStruct->seq[0] = TEST_STR_SEQ_0_VALUE;
    testStruct->seq[1] = TEST_STR_SEQ_1_VALUE;
    
    // fill a member
    testStruct->a <<= TEST_ANY_VALUE;
    
    return testStruct;
}



void
DataManager::checkTestStruct(const TestStruct& testStruct)
    throw (DecodingFailure)
{
    try {   
    
        // check u member
        switch (testStruct.u._d()) {
            case VAL1:
                if (strcmp(testStruct.u.str(), TEST_UNION_STR_VALUE) != 0) {
                    std::cerr << "Unexpected value of TestStruct.u.str : " 
                              << testStruct.u.str()
                              << std::endl;
                    throw DecodingFailure();
                }
                break;
                
            case VAL2:   
                if (testStruct.u.s() != TEST_UNION_S_VALUE) {
                    std::cerr << "Unexpected value of TestStruct.u.s : " 
                              << testStruct.u.s()
                              << std::endl;
                    throw DecodingFailure();
                }
                break;
    
            case VAL3:   
                if (testStruct.u.ull() != TEST_UNION_ULL_VALUE) {
                    std::cerr << "Unexpected value of TestStruct.u.ull : " 
                              << testStruct.u.ull()
                              << std::endl;
                    throw DecodingFailure();
                }
                break;
                
            default:
                std::cerr << "Unexpected value of TestStruct.u._d : " 
                          << testStruct.u._d()
                          << std::endl;
                throw DecodingFailure();
                break;
        }
        
        // check seq member
        if (testStruct.seq.length() != TEST_STR_SEQ_LENGTH) {
            std::cerr << "Unexpected length for TestStruct.seq : " 
                      << testStruct.seq.length()
                      << std::endl;
            throw DecodingFailure();
        }
        if (strcmp(testStruct.seq[0], TEST_STR_SEQ_0_VALUE) != 0) {
            std::cerr << "Unexpected value of TestStruct.seq[0] : " 
                      << testStruct.seq[0]
                      << std::endl;
            throw DecodingFailure();
        }
        if (strcmp(testStruct.seq[1], TEST_STR_SEQ_1_VALUE) != 0) {
            std::cerr << "Unexpected value of TestStruct.seq[1] : " 
                      << testStruct.seq[1]
                      << std::endl;
            throw DecodingFailure();
        }
    
        // check a member
        const char* aValue;
        if (! (testStruct.a >>= aValue)) {
            std::cerr << "Unexpected type in TestStruct.a : " 
                      << std::endl;
            CORBA::TypeCode_var tc = testStruct.a.type();
            std::cerr << tc->name() << std::endl;
            throw DecodingFailure();
        }
        if (strcmp(aValue, TEST_ANY_VALUE) != 0) {
            std::cerr << "Unexpected value of content of TestStruct.a : " 
                      << aValue
                      << std::endl;
            throw DecodingFailure();
        }
    
    
    } catch (DecodingFailure) {
        throw;
    } catch (const CORBA::Exception& e) {
        std::cerr << "ERROR while checking TestStruct value:\n"
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                  << std::endl;
        throw DecodingFailure();
    } catch (...) {
        std::cerr << "ERROR while checking TestStruct value:\n"
                  << "Unknown exception !!"
                  << std::endl;
        throw DecodingFailure();
    }
}




}   // TestCodec

}   // Cdmw
