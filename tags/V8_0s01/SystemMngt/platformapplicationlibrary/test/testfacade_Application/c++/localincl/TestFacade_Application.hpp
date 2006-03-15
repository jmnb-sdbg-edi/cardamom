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

//@author Fabrizio Lorenna <f.lorenna@vitrociset.it>

#ifndef INCL_PLATFORMMNGT_TESTFACADE_APPLICATION_HPP 
#define INCL_PLATFORMMNGT_TESTFACADE_APPLICATION_HPP 

#include "Foundation/testutils/TestManager.hpp"
#include "Foundation/common/System.hpp"
#include <Foundation/orbsupport/CORBA.hpp>

namespace Cdmw
{
namespace PlatformMngt
{

/**
*Purpose:
*<p>   
*
*Features:
*<p> Thread safe
*
*
*/
class TestFacade_Application : public CppUnit::TestFixture
{

    public:
        CORBA::ORB_var m_orb;

        /**
        * Purpose:
        * <p> Constructor
        * 
        */ 
        TestFacade_Application() ;



        /**
        * Purpose:
        * <p> Destructor 
        * 
        */ 
        virtual 
        ~TestFacade_Application() ;


        
        /**
        * Purpose:
        * <p> Call from the testable object 
        * 
        */ 
        virtual 
        void
        do_tests() ;




    protected:


    private:

        /**
        * Purpose:
        * <p> Assignment operator
        * 
        */ 
        TestFacade_Application& operator=(const TestFacade_Application& rhs) throw();
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        TestFacade_Application(const TestFacade_Application& rhs) throw();
        

CPPUNIT_TEST_SUITE(TestFacade_Application);
CPPUNIT_TEST( do_tests );
CPPUNIT_TEST_SUITE_END();

};

} // End namespace PlatformMngt
} // End namespace Cdmw


#endif 

