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

#include "testtracecollector/TestTraceFile.hpp"


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
				             bool logCreationSuccess,
				             bool horizontalFormat)
    throw()
	    :Testable("Cdmw::Trace::LogFile"),
	     m_logFileName(logFileName),
	     m_logCreationSuccess(logCreationSuccess),
	     m_horizontalFormat(horizontalFormat)
			
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
    
    // maximum number of lines in trace file
    int max_counter;

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
        
        // max number of trace message type
        const int max_trace_type = 3;
        
        std::string* trace_table[max_trace_type];

		std::string trace_mesg0 = "start target thread";
		std::string trace_mesg1 = "target loop cycle";
		std::string trace_mesg2 = "end target thread";
		
		trace_table[0] = &trace_mesg0;
        trace_table[1] = &trace_mesg1;
        trace_table[2] = &trace_mesg2;
        
        if (m_horizontalFormat)
        {
            min_counter = 4;
            max_counter = 29;
            
            std::string mesg_hdr = "==>";
            
            while (result.getline (line,MAX_LINE))
	        {
                counter ++;

                // if not the header and not the last line
                if (counter > min_counter && counter < max_counter)
                {
                    // search the message in trace
                    char *p_mesg = strstr (line, mesg_hdr.c_str());
                    p_mesg += mesg_hdr.size();
                    
                    int found = 0;

		            for (int i=0 ; !found && i < max_trace_type ; i++)
                    {
		                std::string* trace_cur = trace_table[i];

                        if (!strncmp (p_mesg, trace_cur->c_str(), trace_cur->size()))
                        {
                            found = 1;
                        }
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
			
			if (counter <= min_counter)
            {
		        TEST_FAILED();
	  	        get_resultStream() << "error in trace message " << std::endl
						           << "expected : line counter > " << min_counter << std::endl
						           << "read     : " <<  counter << std::endl;
            }
		    else if (counter != max_counter)
            {
		        TEST_FAILED();
	  	        get_resultStream() << "error in trace message " << std::endl
						           << "expected : line counter = " << max_counter << std::endl
				                   << "read     : " <<  counter << std::endl;
            }
		    else
		    {
	            TEST_SUCCEED();
		    }

        }
        
        else     // vertical format
        {
            min_counter = 4;
            max_counter = 365; // ECR-0123
    
            // current index of trace message in one sequence
            int sequence = 0;
        
		    bool fail_seq = false;
		    int i; 

		    // max index of trace message in sequence
            const int msg_seq_inx = 13;
		    
		    std::string* label_table[msg_seq_inx];

		    std::string label_seq0  = "Date";
		    std::string label_seq1  = "Time Stamp";
		    std::string label_seq2  = "Host Name";
		    std::string label_seq2a = "Application Name";
		    std::string label_seq3  = "Process Name";
		    std::string label_seq3a = "Process Id";
            // ECR-0123
            std::string label_seq3b = "Component Name";
		    std::string label_seq4  = "Domain";
		    std::string label_seq5  = "Level";
		    std::string label_seq6  = "Thread Id";
		    std::string label_seq7  = "File Name";
		    std::string label_seq8  = "Line";
		    std::string label_seq9  = "==>";

            label_table[0] = &label_seq0;
            label_table[1] = &label_seq1;
            label_table[2] = &label_seq2;
            label_table[3] = &label_seq2a;
            label_table[4] = &label_seq3;
            label_table[5] = &label_seq3a;
            // ECR-0123
            label_table[6] = &label_seq3b;
            label_table[7] = &label_seq4;
            label_table[8] = &label_seq5;
            label_table[9] = &label_seq6;
            label_table[10] = &label_seq7;
            label_table[11] = &label_seq8;
            label_table[12] = &label_seq9;

	        while (result.getline (line,MAX_LINE))
	        {
                counter ++;

                // if not the header and not the last line
                if (counter > min_counter && counter < max_counter)
                {
                    if (sequence < msg_seq_inx)
                    {
		                std::string* label_cur = label_table[sequence];
                        if (strncmp (line, label_cur->c_str(), label_cur->size()))
                        {
		                    fail_seq = true;
	  		                get_resultStream() << "error in message sequence " << sequence << std::endl
						                       << "expected : " <<  label_cur->c_str() << std::endl
						                       << "read     : " <<  line << std::endl;
			            }

			            // if last label of sequence
                        if (sequence == (msg_seq_inx-1))
                        {
		                    if (fail_seq)
                            {
			                    TEST_FAILED();
                            }
				            else
				            {
		                        TEST_SUCCEED();
				            }
                        }

                        sequence++;
			        }
			        else if (sequence == msg_seq_inx)
                    {
                        int found = 0;

		                for (i=0 ; !found && i < max_trace_type ; i++)
                        {
		                    std::string* trace_cur = trace_table[i];

                            if (!strncmp (line, trace_cur->c_str(), trace_cur->size()))
                            {
                                found = 1;
                            }
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

                        sequence++;
			        }
			        else
                    {
                        if (line[0] != '\0')
                        {
			                TEST_FAILED();
	  		                get_resultStream() << "error in message sequence " << sequence << std::endl
						                       << "expected : line feed" << std::endl
						                       << "read     : " <<  line << std::endl;
			            }
			            else
                        {
		                    TEST_SUCCEED();
			            }

			            // restart sequence
                        sequence = 0;
		                fail_seq = false;
			        }
		        }
		    }
		    
		    if (counter <= min_counter)
            {
		        TEST_FAILED();
	  	        get_resultStream() << "error in trace message " << sequence << std::endl
						           << "expected : line counter > " << min_counter << std::endl
						           << "read     : " <<  counter << std::endl;
            }
		    else if (counter != max_counter)
            {
		        TEST_FAILED();
	  	        get_resultStream() << "error in trace message " << sequence << std::endl
						           << "expected : line counter = " << max_counter << std::endl
				                   << "read     : " <<  counter << std::endl;
            }
		    else
		    {
	            TEST_SUCCEED();
		    }
		    
		}
		    
	    result.close();
	  }
    }
}

void TestTraceFile::do_tests()		
{
    // set number of requested successfull tests
    
    if (m_horizontalFormat)
    {
        set_nbOfRequestedTestOK (27);
    }
    else  // vertical format
    {
        set_nbOfRequestedTestOK (75);
    }
    
    test_log_file();
}
		
}; // End namespace Trace
}; // End namespace Cdmw

