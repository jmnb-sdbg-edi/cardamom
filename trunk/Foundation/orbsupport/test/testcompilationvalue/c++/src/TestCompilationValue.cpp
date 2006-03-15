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

#include "testcompilationvalue/TestCompilationValue.hpp"

#if CDMW_ORB_VDR == tao

#if (CDMW_ORB_VER >= 14)
#include <tao/Valuetype/ValueBase.h>
#include <tao/Valuetype/ValueFactory.h>
#else
#include <tao/ValueBase.h>
#include <tao/ValueFactory.h>
#endif // less than 14

#endif //CDMW_ORB_VDR == tao

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        TestCompilationValue::TestCompilationValue(const std::string& name)
                : Testable(name)
        {
            // Nothing to do
        }



        TestCompilationValue::~TestCompilationValue()
        throw()
        {
        }


        void
        TestCompilationValue::do_tests()
        throw()
        {
            // set number of requested successfull tests
            set_nbOfRequestedTestOK (2);
            
            TEST_INFO("Test if the ORB implement ORBA::ValueBase");
            CORBA::ValueBase_var value1;
            TEST_SUCCEED();

            TEST_INFO("Test if the ORB implement ORBA::ValueFactoryBase");            
            CORBA::ValueFactoryBase_var value2;            
            TEST_SUCCEED();
        }

    } // End namespace OrbSupport
} // End namespace Cdmw

