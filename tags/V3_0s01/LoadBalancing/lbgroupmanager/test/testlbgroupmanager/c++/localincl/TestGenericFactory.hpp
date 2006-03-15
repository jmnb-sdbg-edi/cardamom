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

#ifndef INCL_TEST_GENERIC_FACTORY_HPP
#define INCL_TEST_GENERIC_FACTORY_HPP

/**
 * @file
 * @brief Class to test GenericFactory methods.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <Foundation/orbsupport/CORBA.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include "idllib/CdmwLBGroupManager.stub.hpp"

class TestGenericFactory : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TestGenericFactory );
    CPPUNIT_TEST( create_object );
    CPPUNIT_TEST( delete_object );
    CPPUNIT_TEST_SUITE_END( );

    
    static CdmwLB::LBGroupManager_var m_gm;
    static CdmwLB::LBGroupManager_var M_gm;
    static CORBA::ORB_var M_orb;


public:

    void  setUp();
    void  tearDown() ;
 
    /**
     * @brief test create object group operation.
     *
     */
    virtual void create_object();

    /**
     * @brief test delete object group operation.
     *
     */
    virtual void delete_object();
    
};
#endif //INCL_TEST_GENERIC_FACTORY_HPP

