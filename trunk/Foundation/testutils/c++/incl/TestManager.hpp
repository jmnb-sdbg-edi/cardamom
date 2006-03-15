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

#ifndef TEST_MANAGER_HPP
#define TEST_MANAGER_HPP

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResultCollector.h>

#include <iostream>
#include <string>
#include <typeinfo>
#include <stdexcept>

namespace Cdmw
{
  namespace TestUtils
  {
    /**
     * Return the multiplier coefficient of time
     */
    int get_timescale();

    
    /**
     * Class to send parameter to test case.
     */
    class TestManager 
    {
    public:
      TestManager();
      virtual ~TestManager();

      /**
       */
      int 
      init(int     argc,
           char*** argv);

      /**
       * Start Tests of registered test suite.
       */
      int
      run_tests();
      
      static  TestManager* 
      instance();
    
      void
      use_registration(bool registration=true)
      {
          m_is_registration_used = registration;
      }

      bool
      is_registration_used() const
      {
         return m_is_registration_used;
      }
 
      bool
      was_successful() const
      {
          return m_was_successful;
      }

      void
      set_aux_info (const std::string & info);

      std::string
      get_aux_info () const;

      void 
      summary (std::ostream& os=std::cout);
 
    private:
        
        CppUnit::TestResultCollector 
            m_result;
  
      std::string 
            m_aux_info,
            m_base_test_program_string;
	
        std::vector<std::string> 
            m_test_list;     

    protected:
      bool 
        m_is_initialized,
        m_is_registration_used,
        m_was_successful;
    };
  } // end of TestUtils namespace
} // end of Cdmw


#define TEST_INFO(str) std::cerr<<typeid(*this).name()<<": "<<str<<std::endl;

#endif // TEST_MANAGER_HPP

