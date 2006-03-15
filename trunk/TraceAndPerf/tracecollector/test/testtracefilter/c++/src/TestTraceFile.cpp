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


#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>

#include "testtracefilter/TestTraceFile.hpp"


namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes:
*<p>   none
*Portability issues: 
*<p>   none
*/


TestTraceFile::TestTraceFile(const std::string &logFileName,
				             bool logCreationSuccess)
    throw()
	    :Testable("Cdmw::Trace::LogFile"),
	     m_logFileName(logFileName),
	     m_logCreationSuccess(logCreationSuccess)
{
}


TestTraceFile::~TestTraceFile()
    throw()
{
    // does nothing
}


TestTraceFile::TestTraceFile(const TestTraceFile& rhs)
    throw()
    :Testable("Cdmw::Trace::LogFile")
{
    // does nothing
}
		
TestTraceFile&
TestTraceFile::operator=(const TestTraceFile& rhs)
    throw()
{
    // does nothing
    return *this;
}


void TestTraceFile::test_log_file()
{
    // minimum number of lines in trace file corresponding to header
    int min_counter;
    
    TEST_INFO("Test Trace library Log File");

	TEST_CHECK (m_logCreationSuccess == true);
	
	// if log file creation successfull by TraceCollector
	if (m_logCreationSuccess)
	{
	  const int MAX_LINE = 512;
	  char line[MAX_LINE+1];
		   
	  std::ifstream result(m_logFileName.c_str(), std::ios::in);

	  if (!result)
	  {
		TEST_FAILED();
		get_resultStream() << "cannot open file : " << m_logFileName << std::endl;
	  }
	  else
	  {
	    // file is opened
	    TEST_SUCCEED();
	    
	    // number of lines
        int counter = 0;
        
        // header size   
        min_counter = 4;
        
        // list of trace messages
        std::string trace_mesg0 = "trace_1_A_1";
        std::string trace_mesg1 = "trace_2_A_2";
        std::string trace_mesg2 = "trace_3_A_3";
        std::string trace_mesg3 = "trace_4_A_4";
        std::string trace_mesg4 = "trace_5_A_5";
        
        // max number of trace messages
        const int max_trace_nbr = 5;
        
        std::string* trace_table[max_trace_nbr];
        
        trace_table[0] = &trace_mesg0;
        trace_table[1] = &trace_mesg1;
        trace_table[2] = &trace_mesg2;
        trace_table[3] = &trace_mesg3;
        trace_table[4] = &trace_mesg4;
            
        // message header
        std::string mesg_hdr = "==>";
         
        // for each line   
        while (result.getline (line,MAX_LINE))
	    {
            counter ++;

            // skip the header
            if (counter > min_counter)
            {
                // search the message in trace
                char *p_mesg = strstr (line, mesg_hdr.c_str());
                p_mesg += mesg_hdr.size();
                    
                int found = 0;
                int i = counter - min_counter - 1;

                if (i < max_trace_nbr)
                {
                    std::string* trace_cur = trace_table[i];
                    
                    if (!strncmp (p_mesg, trace_cur->c_str(), trace_cur->size()))
                    {
                        found = 1;
                    }

			        if (!found)
                    {
			            TEST_FAILED();
	  		            get_resultStream() << "error in trace message" << std::endl 
						                   << "read     : " <<  line << std::endl;
			        }
			        else
                    {
		                TEST_SUCCEED();
			        }
			    }
			}
	    }
			
	    if (counter <= min_counter)
        {
		    TEST_FAILED();
	  	    get_resultStream() << "error in trace message " << std::endl
						       << "expected : line counter > " << min_counter << std::endl
						       << "read     : " <<  counter << std::endl;
        }
		else
		{
	        TEST_SUCCEED();
		}
   
	    result.close();
	  }
    }
}

void TestTraceFile::do_tests()		
{
    // set number of requested successfull tests
    
    set_nbOfRequestedTestOK (8);

    test_log_file();
}
		
}; // End namespace Trace
}; // End namespace Cdmw

