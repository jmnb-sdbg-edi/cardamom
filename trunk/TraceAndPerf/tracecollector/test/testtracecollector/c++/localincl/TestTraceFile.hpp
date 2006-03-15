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


#ifndef INCL_TRACE_TESTTRACEFILE_HPP 
#define INCL_TRACE_TESTTRACEFILE_HPP 

#include <string>

#include "Foundation/testutils/Testable.hpp"

namespace Cdmw
{
namespace Trace 
{

/**
*Purpose:
*<p>   This class tests the Performance Log file written by the Probe Collector
*      when Performance service is clean up.
*
*Features:
*<p>   none
*
*@see 
*@link 
*
*/
class TestTraceFile : public TestUtils::Testable
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param
        *@exception
        */ 
        TestTraceFile(const std::string &logFileName,bool logCreationSuccess,
                      bool horizontalFormat)
                throw();

        /**
        * Purpose:
        * <p>  Copy constructor 
		*
        *@param rhs A TestTraceFile object to assign to current object.
        *@exception
        */ 
        TestTraceFile(const TestTraceFile& rhs)
                throw();


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~TestTraceFile()
                throw();


        /**
        * Purpose:
        * <p> Assignement operator
        * 
        *@param rhs A TestPerformanceFile object to assign to current object.
        *@return A reference to the current object.
        *@exception
        */ 
        TestTraceFile&
        operator=(const TestTraceFile& rhs)
                throw();

    protected:

	    virtual void do_tests();

    private:
        void test_log_file();
		
		std::string m_logFileName;
		
		bool m_logCreationSuccess;
		
		bool m_horizontalFormat;


}; // End class TestTraceFile 

}; // End namespace Trace
}; // End namespace Cdmw
#endif // INCL_TRACE_TESTTRACEFILE_HPP

