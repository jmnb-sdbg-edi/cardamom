/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#ifndef _TEST_Q_100_HPP_
#define _TEST_Q_100_HPP_

/**
  * @author Fabrizio Morciano <fmorciano@amjsv.it>
  * @author Raffaele Mele <rmele@progesi.it>
  */

#include <cppunit/extensions/HelperMacros.h>
#include "Time/clockservice/CdmwTime.stub.hpp"

/**
 * @brief Class to test ControlledClock method.
 */
class Test_Q_100 
:   public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Test_Q_100 );
        CPPUNIT_TEST( resolution );
        CPPUNIT_TEST( precision );
        CPPUNIT_TEST( readout_bits );
        CPPUNIT_TEST( coordination_status );
        CPPUNIT_TEST( coordination_stability );
    CPPUNIT_TEST_SUITE_END( );

protected:

    CORBA::Object_ptr 
        m_obj;
        
    CosClockService::ControlledClock_var 
        m_controlledClock1,
        m_controlledClock2;
        
    CosClockService::ClockCatalog_var 
        m_clockCatalog;
        
    CosClockService::Clock_var 
        m_localClock;

    std::string 
        m_alias_name;
        
    std::string  
        m_clockType,
        m_clockType1,
        m_clockType2; 

public:
    Test_Q_100();
    
    void setUp();
    void tearDown();
    
    void resolution ();
    void precision ();
    void readout_bits ();
    void coordination_status ();
    void coordination_stability ();    
};



#endif /*_TEST_Q_100_HPP_*/
