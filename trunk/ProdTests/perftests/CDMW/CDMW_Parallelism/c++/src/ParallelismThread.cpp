/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ========================================================================= */

#include <fstream>
#include "ParallelismThread.hpp"

namespace Parallelism {
  
  ParallelismThread::ParallelismThread():
    m_tester(NULL),
    m_stopped(false)
  {
  }
  
  ParallelismThread::ParallelismThread(Parallelism::Tester_ptr tester,
				       int niter, 
				       int nstart):
    m_tester(Parallelism::Tester::_duplicate(tester)),
    m_stopped(false)
  {
    m_nb_ping_test           = niter;
    m_nb_ping_start           = nstart;
  }
  
  void ParallelismThread::setNIter(int niter) {
    m_nb_ping_test           = niter;
  } 
  
  void ParallelismThread::setNStart(int nstart) {
    m_nb_ping_start           = nstart;
  } 
  
  void ParallelismThread::setTester(Parallelism::Tester_ptr tester)
  {
    m_tester = Parallelism::Tester::_duplicate(tester);
  } 
  
  long double ParallelismThread::getTimeOfTest() {
    return m_timeval2 - m_timeval1;
  }

  long double ParallelismThread::getStartTime() {
    return m_timeval1;
  }

  long double ParallelismThread::getEndTime() {
    return m_timeval2;
  }
  
  void ParallelismThread::run() 
    throw()
  {
    //    std::cout << "--- Start of Parallelism ---" << std::endl;
    
    try {
      int i;
      // Startup samples
      for (i = 0; !m_stopped && i < m_nb_ping_start; i++) {
	test_ping();
	//	std::cout << "warm= " << i << std::endl;         
      }
      
      //      std::cout << "  Test performances without arguments " << m_nb_ping_test << " times." << std::endl;
      // Test samples
      m_timeval1 = ctools::time::HighResClock::getTime().toMicrosec();
      //     std::cout << "--- Start of Parallelism ---" << std::endl;       
      for (i = 0; !m_stopped && i < m_nb_ping_test; i++) {
	test_ping();
	//	std::cout << "iter= " << i << std::endl;         
      }
      m_timeval2 = ctools::time::HighResClock::getTime().toMicrosec();
    }
    catch (const CORBA::SystemException& e) 
      {
        std::cerr << "SystemException: " 
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                  << std::endl;
      }
    
    //    if (m_stopped) 
    //  std::cout << "--- Parallelism stopped ---" << std::endl;
    //else
      //      std::cout << "---- End of Parallelism ----" << std::endl;
  }
  
  
  void ParallelismThread::test_ping ()
    throw (CORBA::SystemException)
  {
    m_tester->ping();
  }
  
};  // End of namespace Parallelism

