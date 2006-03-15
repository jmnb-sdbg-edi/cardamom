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

#include "test3/TestPerformanceFile.hpp"
#include "test3/TargetThread.hpp"


namespace Cdmw
{
namespace Performance
{

/**
*Implementation notes:
*<p>   none
*Portability issues: 
*<p>   none
*/


TestPerformanceFile::TestPerformanceFile(const std::string &logFileName,
				                         bool logCreationSuccess,
										 int max_target)
    throw()
	    :Testable("Cdmw::Performance::LogFile"),
	     m_logFileName(logFileName),
	     m_logCreationSuccess(logCreationSuccess),
		 m_maxTarget(max_target)
			
{
}


TestPerformanceFile::~TestPerformanceFile()
    throw()
{
    // does nothing
}


TestPerformanceFile::TestPerformanceFile(const TestPerformanceFile& rhs)
    throw()
    :Testable("Cdmw::Performance::LogFile")
{
    // does nothing
}
		
TestPerformanceFile&
TestPerformanceFile::operator=(const TestPerformanceFile& rhs)
    throw()
{
    // does nothing
    return *this;
}


void TestPerformanceFile::test_log_file()
{
    TEST_INFO("Test Performance library Log File");

	TEST_CHECK (m_logCreationSuccess == true);
	
	// if log file creation successfull by ProbeCollector
	if (m_logCreationSuccess)
	{
	  Cdmw::Performance::TargetThread target;
	     
	  const int MAX_LINE = 200;
	  char line[MAX_LINE+1];
		   
	  std::ifstream result(m_logFileName.c_str(), std::ios::in);

	  if (!result)
	  {
		TEST_FAILED();
		get_resultStream() << "cannot open file : " << m_logFileName << std::endl;
	  }
	  else
	  {
	    while (result.getline (line,MAX_LINE))
	    {
		  char *p_api_name = &line[0];
															  	  
		  char *p_min_time = NULL;
		  char *p_max_time = NULL;
		  char *p_sum_time = NULL;
		  char *p_hit_count = NULL;
																			  	  
	  	  int seq = 1;
		  int end_loop = 0;
		  int i;
																						  	  
		  for (i=0 ; i < MAX_LINE && end_loop == 0 ; i++)
		  {
		    if (line[i] == ';')
		    {
			  line[i] = '\0';

              if (seq == 1)
			  {
		 	    p_min_time = &line[i+1];
			  }	
		      else if (seq == 2)
			  {
			    p_max_time = &line[i+1];
			  }
		  	  else if (seq == 3)
			  {
		        p_sum_time = &line[i+1];
			  }
			  else if (seq == 4)
			  {
			    p_hit_count = &line[i+1];
			  }	

			  seq++;
		    }

		    else if (line[i] == '\0' && seq == 5)
		    {
		      end_loop = 1;
		    }
		  }
			
		  std::string api_name_str = p_api_name;
		  int api_index = target.getApiIndex(api_name_str);

		  if (api_index == -1)
		  {
		    TEST_FAILED();
		    get_resultStream() << "API name not found : " << p_api_name << std::endl;		
		  }
		  else
		  {
			int min_time = atoi(p_min_time);
			int max_time = atoi(p_max_time);
			int sum_time = atoi(p_sum_time);
			int hit_count = atoi(p_hit_count);

			// sleep time is returned in milli sec and is converted in micro sec
			int targ_time = target.getApiSleepTime(api_index) * 1000;
			  
			int targ_hit_count = m_maxTarget * target.getApiCount(api_index);
			int targ_sum_time = targ_time * targ_hit_count;

			if (min_time < targ_time || min_time > (targ_time + 200000000))
            {
			  TEST_FAILED();
	  		  get_resultStream() << "error for api " << p_api_name 
						         << "in min time : " << min_time << std::endl;
	  		}
			else
			{
		      TEST_SUCCEED();
			}
			
			if (max_time < targ_time || max_time > (targ_time + 200000000)
			   || max_time < min_time)
  	 		{
			  TEST_FAILED();
	  		  get_resultStream() << "error for api " << p_api_name 
					             << "in max time : " << max_time << std::endl;	  
	  		}			  
			else
			{
		      TEST_SUCCEED();
			}

			if (sum_time < targ_sum_time || sum_time > (targ_sum_time + 1000000000))
			{
			  TEST_FAILED();
			  get_resultStream() << "error for api " << p_api_name 
					             << "in sum time : " << sum_time << std::endl;		  
			}			  
			else
			{
		      TEST_SUCCEED();
			}

			if (hit_count != targ_hit_count)
			{
			  TEST_FAILED();
			  get_resultStream() << "error for api " << p_api_name 
					             << "in hit count : " << hit_count << std::endl;	
	  		}			  
			else
			{
		      TEST_SUCCEED();
			}
		  }
	    }

	    result.close();
	  }
    }
}

void TestPerformanceFile::do_tests()		
{
    test_log_file();
}
		
}; // End namespace NamingInterface
}; // End namespace Cdmw

